
#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include <Kismet/GameplayStatics.h>
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAnimInstance.h"
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/KismetArrayLibrary.h"
#include "MyRPG/Types/ChannelType.h"
#include "MyRPG/Data/DamageInfo.h"
#include "MyRPG/Item/Item.h"

AEnemy::AEnemy()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TargetDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetection"));
	if (TargetDetectionSphere)
	{
		RootComponent = Cast<USceneComponent>(GetCapsuleComponent());

		TargetDetectionSphere->SetupAttachment(RootComponent);
	}

	NormalAttackDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("NormalAttackDetection"));
	if (NormalAttackDetectionSphere)
	{
		NormalAttackDetectionSphere->SetupAttachment(RootComponent);
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> EnemyDataTableFind(TEXT("/Game/Blueprints/DataTable/EnemyDataTable.EnemyDataTable"));
	if (EnemyDataTableFind.Succeeded())
	{
		EnemyDataTable = EnemyDataTableFind.Object;
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (TargetDetectionSphere)
	{
		TargetDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::TargetDetectionOverlapBegin);
		TargetDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::TargetDetectionOverlapEnd);
		TargetDetectionSphere->SetCollisionProfileName("CreatureDetection");
	}

	if (NormalAttackDetectionSphere)
	{
		NormalAttackDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::NormalAttackDetectionOverlapBegin);
		NormalAttackDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::NormalAttackDetectionOverlapEnd);
		NormalAttackDetectionSphere->SetCollisionProfileName("CreatureDetection");
	}

	StateToWalk();

	float AccWeight = 0.f;
	for (int i = 0; i < SkillCoolTimeArray.Num(); i++)
	{
		OnCooldownFinishedArray.Add(true);
		SkillCoolTimeTimerArray.Add(FTimerHandle());

		if (i < SkillWeight.Num())
		{
			AccWeight += SkillWeight[i];
			SkillWeight[i] = AccWeight;
		}
	}

	NormalAttackActors.SetNum(NormalAttackClasses.Num());
	SkillAttackActors.SetNum(SkillAttackClasses.Num());

	SetValueAsBool("CanSkill", false);
	SetValueAsInt("SkillNum", INDEX_NONE);
	AnimInstance = GetMesh()->GetAnimInstance();
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(TargetDetectionSphere))
	{
		TargetDetectionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy::TargetDetectionOverlapBegin);
		TargetDetectionSphere->OnComponentEndOverlap.RemoveDynamic(this, &AEnemy::TargetDetectionOverlapEnd);
	}

	if (IsValid(NormalAttackDetectionSphere))
	{
		NormalAttackDetectionSphere->OnComponentBeginOverlap.RemoveDynamic(this, &AEnemy::NormalAttackDetectionOverlapBegin);
		NormalAttackDetectionSphere->OnComponentEndOverlap.RemoveDynamic(this, &AEnemy::NormalAttackDetectionOverlapEnd);
	}
}

void AEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	bUseControllerRotationYaw = false;
	BringDataTableDatas();
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyTrace, ECollisionResponse::ECR_Block);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetDead()) return;

	CheckIsNormalAttacking();
	AutoInterpRotationToTarget();

}

void AEnemy::Damaged(FDamageInfo* DamageInfo)
{
	Super::Damaged(DamageInfo);
	//EndNormalAttack(CurrentNormalAttackIndex);
	
	/*if (IsValid(DamageInfo.DamageCauser) && IsValid(Cast<APlayerCharacter>(DamageInfo.DamageCauser)))
	{
		SetValueAsObject("Target", DamageInfo.DamageCauser);
	}*/

	if (GetDead())
	{
		return;
	}

	HitReact();
}

void AEnemy::SetDead()
{
	StopBehaviorTree();
	IsSkillFinishedToTrue();

	DropItem();

	if (DeathMontage)
	{
		
		if (AnimInstance)
		{
			GetWorldTimerManager().ClearAllTimersForObject(this);
			GetWorldTimerManager().ClearTimer(InterpRotationForciblyTimer);
			GetWorldTimerManager().ClearTimer(PlaySkillMontageTimer);

			ClearAction();
			AnimInstance->StopAllMontages(0.1f);
			float AnimTime = AnimInstance->Montage_Play(DeathMontage, 1.f);
			/*FTimerHandle DeadTimer;
			GetMesh()->bPauseAnims = true;
			GetWorld()->GetTimerManager().SetTimer(DeadTimer, this, &AEnemy::PauseAnim, 0.001f, false, AnimTime);*/
		}
	}
	Super::SetDead();
}

void AEnemy::GainHP(int HPAmount)
{
	ReserveShowHPBar(7.f);

	Super::GainHP(HPAmount);
}

void AEnemy::NormalAttack_Implementation()
{
	if (bNormalAttackEnable == false) return;

	if (AnimInstance == nullptr) return;
	if (AnimInstance->IsAnyMontagePlaying()) return;

	FString SectionStr = FString("Combo");
	int SectionNum = NormalAttackSectionNum();
	SectionNum = FMath::Rand() % SectionNum + 1;
	SectionStr += FString::FromInt(SectionNum);

	AnimInstance->Montage_Play(NormalAttackMontage);
	AnimInstance->Montage_JumpToSection(FName(*SectionStr), NormalAttackMontage);
	SetNormalAttackTimer(FName(*SectionStr));
}

void AEnemy::SetNormalAttackTimer(const FName& SectionName)
{
	if (NormalAttackMontage == nullptr) return;

	float AnimTime = NormalAttackMontage->GetSectionLength(NormalAttackMontage->GetSectionIndex(SectionName));
	bNormalAttackEnable = false;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemy::NormalAttackTimer, 0.0001f, false, AnimTime);
}

void AEnemy::NormalAttackTimer()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemy::EnableNormalAttack, 0.0001f, false, NormalAttackCoolTime);
}

void AEnemy::OnAggro_Implementation()
{
	if (AnimInstance == nullptr || OnAggroMontage == nullptr) return;
	if (AnimInstance->Montage_IsPlaying(OnAggroMontage)) return;

	float MontageLength = AnimInstance->Montage_Play(OnAggroMontage);

	PauseBehaviorTree();
	FTimerHandle Handle1, Handle2, Handle3;
	GetWorld()->GetTimerManager().SetTimer(Handle1, this, &AEnemy::ResumeBehaviorTree, 0.0001f, false, MontageLength);
	GetWorld()->GetTimerManager().SetTimer(Handle2, this, &AEnemy::AggroAnimationFinished, 0.0001f, false, MontageLength);
	GetWorld()->GetTimerManager().SetTimer(Handle3, this, &AEnemy::CanSkillToTrue, 0.0001f, false, MontageLength + CanSkillInterval);
}

void AEnemy::TargetDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		SetValueAsObject("Target", PlayerCharacter);
	}
}

void AEnemy::TargetDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		AAIController* AIController = Cast<AAIController>(Controller);
		if (AIController && AIController->GetBlackboardComponent())
		{
			AIController->GetBlackboardComponent()->SetValueAsObject("Player", nullptr);
		}
	}*/
}

void AEnemy::NormalAttackDetectionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString PlainName = FName(OtherComp->GetName()).GetPlainNameString();
	if (PlainName.Equals("Shield"))
	{
		return;
	}
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		SetValueAsBool("CanAttack", true);
	}
}

void AEnemy::NormalAttackDetectionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString PlainName = FName(OtherComp->GetName()).GetPlainNameString();
	if (PlainName.Equals("Shield"))
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		SetValueAsBool("CanAttack", false);
	}
}

void AEnemy::EnableNormalAttack()
{
	bNormalAttackEnable = true;
}

int AEnemy::NormalAttackSectionNum()
{
	if (NormalAttackMontage)
	{
		return NormalAttackMontage->CompositeSections.Num();
	}
	return 0;
}

void AEnemy::AggroAnimationFinished()
{
	SetValueAsBool("IsAggroAnimationFinished", true);
}

bool AEnemy::CanInterpRotation()
{
	return (Super::CanInterpRotation() && bSkillInProgress == false) || bForceInterpRotation;
}

void AEnemy::AutoInterpRotationToTarget()
{
	if (bAutoInterpRotationToTarget == false) return;

	AActor* Target = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Target) == false) return;

	SetInterpRotationTarget(Target->GetActorLocation());
}

void AEnemy::SetValueAsBool(FName KeyName, bool bState)
{
	AAIController* AIController = Cast<AAIController>(Controller);
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(KeyName, bState);
	}
}

void AEnemy::SetValueAsObject(FName KeyName, UObject* Object)
{
	AAIController* AIController = Cast<AAIController>(Controller);
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsObject(KeyName, Object);
	}
	
}

void AEnemy::SetValueAsInt(FName KeyName, int Num)
{
	AAIController* AIController = Cast<AAIController>(Controller);
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsInt(KeyName, Num);
	}
}

bool AEnemy::GetValueAsBool(FName KeyName)
{
	AAIController* AIController = Cast<AAIController>(Controller);
	if (AIController && AIController->GetBlackboardComponent())
	{
		return AIController->GetBlackboardComponent()->GetValueAsBool(KeyName);
	}

	return false;
}

UObject* AEnemy::GetValueAsObject(FName KeyName)
{
	AAIController* AIController = Cast<AAIController>(Controller);
	if (AIController && AIController->GetBlackboardComponent())
	{
		return AIController->GetBlackboardComponent()->GetValueAsObject(KeyName);
	}

	return nullptr;
}

int AEnemy::GetValueAsInt(FName KeyName)
{
	AAIController* AIController = Cast<AAIController>(Controller);
	if (AIController && AIController->GetBlackboardComponent())
	{
		return AIController->GetBlackboardComponent()->GetValueAsInt(KeyName);
	}

	return -1;
}

float AEnemy::GetMaxWalkSpeed() const
{
	if (GetCharacterMovement())
		return GetCharacterMovement()->MaxWalkSpeed;
	return 0.f;
}

const FVector AEnemy::GetCurrentTargetLocation()
{
	FVector Result = FVector::ZeroVector;

	AActor* Player = Cast<AActor>(GetValueAsObject("Target"));
	if (IsValid(Player))
	{
		Result = Player->GetActorLocation();
	}
	return Result;
}

#pragma region LegacyNormalAttackNotify(NowChanged)
//void AEnemy::NormalAttackCollisionEnable_Implementation()
//{
//	FActorSpawnParameters SpawnParam;
//	SpawnParam.Owner = this;
//
//	NormalAttackActor = GetWorld()->SpawnActor<AAttackActor>(NormalAttackClass, SpawnParam);
//}
//
//void AEnemy::NormalAttackCollisionDisable_Implementation()
//{
//	if (NormalAttackActor)
//	{
//		NormalAttackActor->Destroy();
//	}
//}
//
//void AEnemy::NormalAttackComboFinished_Implementation()
//{
//}
#pragma endregion

void AEnemy::BeginNormalAttack_Implementation(int AttackActorIndex)
{
	CurrentNormalAttackIndex = AttackActorIndex;
	if (CurrentNormalAttackIndex >= NormalAttackClasses.Num() || CurrentNormalAttackIndex < 0) return;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	
	NormalAttackActors[AttackActorIndex] = GetWorld()->SpawnActor<AAttackActor>(NormalAttackClasses[AttackActorIndex], SpawnParam);
}

void AEnemy::EndNormalAttack_Implementation(int AttackActorIndex)
{
	if (AttackActorIndex >= NormalAttackActors.Num() || AttackActorIndex < 0) return;

	if (IsValid(NormalAttackActors[AttackActorIndex]))
	{
		NormalAttackActors[AttackActorIndex]->Destroy();
	}
}

void AEnemy::BeginSkillAttack_Implementation(int SkillActorIndex, float NewLifeSpan, FName AttachSocketName)
{
	if (SkillActorIndex >= SkillAttackClasses.Num() || SkillActorIndex < 0) return;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	SkillAttackActors[SkillActorIndex] = GetWorld()->SpawnActorDeferred<AAttackActor>(SkillAttackClasses[SkillActorIndex], FTransform::Identity, this);
	if (IsValid(SkillAttackActors[SkillActorIndex]) == false) return;

	SkillAttackActors[SkillActorIndex]->Initialize(AttachSocketName);

	UGameplayStatics::FinishSpawningActor(SkillAttackActors[SkillActorIndex], FTransform::Identity);

	if (NewLifeSpan != -1.f)
		SkillAttackActors[SkillActorIndex]->SetLifeSpan(NewLifeSpan);
}

void AEnemy::EndSkillAttack_Implementation(int SkillActorIndex)
{
	if (SkillActorIndex >= SkillAttackActors.Num() || SkillActorIndex < 0) return;

	if (IsValid(SkillAttackActors[SkillActorIndex]))
		SkillAttackActors[SkillActorIndex]->Destroy();

}

void AEnemy::PauseBehaviorTree()
{
	auto AIController = Cast<AAIController>(Controller);
	if (AIController)
	{
		AIController->GetBrainComponent()->PauseLogic("");
	}
}

void AEnemy::ResumeBehaviorTree()
{
	if (GetDead()) return;

	auto AIController = Cast<AAIController>(Controller);
	if (AIController)
	{
		AIController->GetBrainComponent()->ResumeLogic("");
	}
}

void AEnemy::StopBehaviorTree(float StopDuration)
{
	auto AIController = Cast<ABTAIController>(Controller);
	if (AIController)
	{
		AIController->StopAI();			
	}

	if (StopDuration != -1.f && GetWorld()->GetTimerManager().IsTimerActive(DamagedStopBehaviorTimer) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(DamagedStopBehaviorTimer, this, &AEnemy::StartBehaviorTree, 0.001f, false, StopDuration);
	}
}

void AEnemy::StartBehaviorTree()
{
	if (GetDead()) return;

	auto AIController = Cast<ABTAIController>(Controller);
	if (AIController)
	{
		AIController->RunAI();
	}
}

void AEnemy::StateToWalk()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void AEnemy::StateToRun()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void AEnemy::Ready()
{
	if (AnimInstance)
	{
		auto Instance = Cast<UEnemyAnimInstance>(AnimInstance);
		if (Instance)
		{
			Instance->SetReady();
		}
	}
}

void AEnemy::AddMovePointToAction(FActionInfo& ActionInfo, const FVector& Location, ESplinePointType::Type PointType, bool bIsLocal)
{
	if (ActionComponent)
	{
		ActionComponent->AddMovePointToAction(ActionInfo, Location, PointType, bIsLocal);
	}
}

bool AEnemy::Skill(int SkillNum)
{
	if (GetValueAsBool("CanSkill") == false) return false;

	ActiveSuperArmor();
	bSkillInProgress = true;

	SetValueAsBool("CanSkill", false);
	//CurrentSkillNum = SkillNum;
	OnCooldownFinishedArray[SkillNum] = false;

	FTimerDelegate TimerDele;
	TimerDele.BindUFunction(this, "SkillCoolTimeFinished", SkillNum);
	GetWorld()->GetTimerManager().SetTimer(SkillCoolTimeTimerArray[SkillNum], TimerDele, SkillCoolTimeArray[SkillNum], false);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AEnemy::CanSkillToTrue, 0.0001f, false, CanSkillInterval);

	Skill_FromBP(SkillNum);

	return true;
}

bool AEnemy::IsSkillAvailable(int SkillNum)
{
	bool bAvailable = false;
	IsSkillAvailable_FromBP(SkillNum, bAvailable);

	return bAvailable;
}

int AEnemy::RandomChoiceSkill()
{
	int ResultChoiceNum = INDEX_NONE;

	TArray<int> RandArr;
	for (int SkillNum = 0; SkillNum < SkillCoolTimeArray.Num(); SkillNum++)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(SkillCoolTimeTimerArray[SkillNum]) || OnCooldownFinishedArray[SkillNum] == false)
			continue;

		bool bSkillAvailable = IsSkillAvailable(SkillNum);
		

		if (bSkillAvailable == false)
			continue;

		RandArr.Add(SkillNum);
	}

	if (RandArr.Num() == 0) return INDEX_NONE;

	if (SkillWeight.Num() > 0)
	{
		float RandFloat = FMath::FRandRange(0.f, *SkillWeight.end());

		for (int Index : RandArr)
		{
			float PrevWeight = (Index - 1 >= 0) ? SkillWeight[Index - 1] : 0.f;
			if (SkillWeight[Index] >= RandFloat && PrevWeight < RandFloat)
			{
				ResultChoiceNum = Index;
				break;
			}
		}
	}
	else
	{
		ResultChoiceNum = RandArr[FMath::Rand() % RandArr.Num()];
	}
	
	
	return ResultChoiceNum;
}

void AEnemy::SkillCoolTimeFinished(int SkillNum)
{
	OnCooldownFinishedArray[SkillNum] = true;
}

void AEnemy::CanSkillToTrue()
{
	SetValueAsBool("CanSkill", true);
}

int AEnemy::SkillSectionNum()
{
	if (SkillMontage)
	{
		return SkillMontage->CompositeSections.Num();
	}
	return 0;
}

bool AEnemy::IsAllSkillInCooltime()
{
	for (int i = 0; i < OnCooldownFinishedArray.Num(); i++)
	{
		if (OnCooldownFinishedArray[i] == true)
			return false;
	}
	return true;
}

bool AEnemy::IsSomeSkillOnCondition()
{
	bool bResult = false;
	
	for (int32 SkillNum = 0; SkillNum < SkillCoolTimeArray.Num(); SkillNum++)
	{
		bResult = IsSkillAvailable(SkillNum);

		if (bResult == true)
			break;
	}
		
	return bResult;
}

// 스킬과 관련없는 다른 태스크노드에서 스킬을 쓰기위해 태스크를 멈출수 있나에 대햐여 쓰이는 함수다.
bool AEnemy::CanStopTaskToUseSkill()
{
	// 스킬을 사용가능하고 모든 스킬이 쿨타임이 아닌경우 true, 
	return GetValueAsBool("CanSkill") && !IsAllSkillInCooltime() && IsSomeSkillOnCondition();
}

void AEnemy::StopAllMontages(float BlendOut)
{
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(BlendOut);
	}
}

void AEnemy::AnimEventNotify_Implementation(int AnimNum)
{
}

void AEnemy::ReserveSkillFinish(float DelayTime)
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AEnemy::IsSkillFinishedToTrue, DelayTime, false);

}

void AEnemy::IsSkillFinishedToTrue()
{
	SetRotationCurrentToInterp();
	SetValueAsBool("IsSkillFinished", true);

	bSkillInProgress = false;
	InactiveSuperArmor();
}

void AEnemy::PlaySkillMontageAfterInterpRotationForcibly(const FName& SectionName, float WaitTime, TFunction<void()> Func)
{
	ACharacter* Player = Cast<ACharacter>(GetValueAsObject("Target"));
	if (IsValid(Player) == false) return;

	FVector ToPlayerDirection = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	SetInterpRotationTarget(ToPlayerDirection.Rotation());
	SetInterpRotationForcibly(true);
	SetAutoInterpRotationToTarget(true);

	FTimerDelegate TimerDele;
	TimerDele.BindLambda
	(
		[this, Func, SectionName]()
		{
			SetInterpRotationForcibly(false);
			SetAutoInterpRotationToTarget(false);

			Func();

			GetAnimInstance()->Montage_Play(GetSkillMontage());
			GetAnimInstance()->Montage_JumpToSection(SectionName, GetSkillMontage());
		}
	);


	GetWorld()->GetTimerManager().SetTimer(InterpRotationForciblyTimer, TimerDele, WaitTime, false);
}

void AEnemy::PlaySkillMontage(const FName& SectionName, float WaitTime)
{
	FTimerDelegate TimerDele;
	TimerDele.BindLambda
	(
		[=]()
		{
			GetAnimInstance()->Montage_Play(GetSkillMontage());
			GetAnimInstance()->Montage_JumpToSection(SectionName, GetSkillMontage());
		}
	);
	
	GetWorld()->GetTimerManager().SetTimer(PlaySkillMontageTimer, TimerDele, WaitTime, false);
}

void AEnemy::ReserveMontage(UAnimMontage* Montage, const FName& SectionName, float WaitTime)
{
	FTimerDelegate TimerDele;
	TimerDele.BindLambda
	(
		[=]()
		{
			GetAnimInstance()->Montage_Play(Montage);
			GetAnimInstance()->Montage_JumpToSection(SectionName, GetSkillMontage());
		}
	);

	GetWorld()->GetTimerManager().SetTimer(PlaySkillMontageTimer, TimerDele, WaitTime, false);
}

FName AEnemy::SkillSectionName(int SkillNum, FString AppendStr)
{
	return FName(*(FString("Skill") + FString::FromInt(SkillNum + 1) + AppendStr));
}

void AEnemy::CheckIsNormalAttacking()
{
	if (AnimInstance && NormalAttackMontage)
	{
		bool bPlaying = AnimInstance->Montage_IsPlaying(NormalAttackMontage);
		SetValueAsBool("IsNormalAttacking", bPlaying);
	}
	
}

void AEnemy::HitReact_Implementation()
{
	if (bCanHitReact == false)
	{
		return;
	}
	/*
		맞았을때의 HitReact 몽타주 실행시키고 BehaviorTree를 몽타주 시간동안 Stop,
	*/
	if (HitReactMontage && AnimInstance && IsActivedSuperArmor() == false)
	{
		AnimInstance->Montage_Play(HitReactMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, true);
		float TimerLength = HitReactMontage->GetPlayLength();

		StopBehaviorTree(TimerLength);
		//GetWorld()->GetTimerManager().SetTimer(DamagedStopBehaviorTimer, this, &AEnemy::StartBehaviorTree, 0.001f, false, TimerLength);

		//SetValueAsBool("IsSkillFinished", false);

		ActiveSuperArmor();
		GetWorld()->GetTimerManager().SetTimer(HitReactCoolTimeTimer, this, &AEnemy::InactiveSuperArmor, HitReactCoolTime, false);
	}
	


	// 위의 방식에서 Delay 시간만 늘려주면 스턴 구현 가능할듯함.
}

const bool AEnemy::CanInactiveSuperArmor() const
{
	return !bSkillInProgress;
}

AAttackActor* AEnemy::GetSkillAttackActor(int Index)
{
	return SkillAttackActors[Index];
}

void AEnemy::SetSkillAttackActor(int Index, AAttackActor* SkillActor)
{
	SkillAttackActors[Index] = SkillActor;
}

TSubclassOf<class AAttackActor> AEnemy::GetSkillAttackClass(int Index)
{
	return SkillAttackClasses[Index];
}

void AEnemy::BringDataTableDatas()
{
	if (FindEnemyDataRowName.IsNone()) return;
	if (EnemyDataTable == nullptr) return;

	FEnemyDataTable* DataRow = EnemyDataTable->FindRow<FEnemyDataTable>(FindEnemyDataRowName, FString("EnemyDataTable"));
	if (DataRow == nullptr) return;

	SetLevel(DataRow->Level);
	EnemyName = DataRow->Name;
	DropItems = DataRow->DropItems;
}

void AEnemy::DropItem()
{
	if (DropItems.Num() == 0)
	{
		return;
	}

	
	for (const FDropItemInfo& ItemInfo : DropItems)
	{
		float RandomPercent = FMath::FRand();
		if (ItemInfo.DropProbability < RandomPercent) continue;

		int DropCount = FMath::RandRange(ItemInfo.DropCountRange.DropCountMin, ItemInfo.DropCountRange.DropCountMax);
		AItem* DropItem = GetWorld()->SpawnActor<AItem>(ItemInfo.ItemClass);

		if (IsValid(DropItem))
		{
			DropItem->SetItemCount(DropCount);
			DropItem->PopFromInventory(GetActorLocation());
		}
	}
}
