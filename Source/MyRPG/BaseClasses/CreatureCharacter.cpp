

#include "CreatureCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "MyRPG/Types/ChannelType.h"
#include "MyRPG/UI/LevelHpStateWidget.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "MyRPG/Interfaces/Command.h"
#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Data/DamageInfo.h"
#include "MyRPG/UI/FloatingTextWidget.h"

#include <Engine/DataTable.h>
#include <Kismet/KismetMathLibrary.h>



ACreatureCharacter::ACreatureCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));

	LevelHpStateComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LevelHpStateComponent"));
	if (LevelHpStateComponent)
	{
		LevelHpStateComponent->SetupAttachment(RootComponent);
		static ConstructorHelpers::FClassFinder<UUserWidget> LevelHpBarFind(TEXT("/Game/Blueprints/UI/CreatureUI/WBP_LevelHPState.WBP_LevelHPState_C"));
		if (LevelHpBarFind.Succeeded())
		{
			LevelHpStateComponent->SetWidgetClass(LevelHpBarFind.Class);
			LevelHpStateComponent->SetWidgetSpace(EWidgetSpace::Screen);
			LevelHpStateComponent->SetDrawSize(FVector2D(120.f, 80.f));
		}
	}	

	

	static ConstructorHelpers::FClassFinder<UUserWidget> FloatingTextWidgetFind(TEXT("/Game/Blueprints/UI/CreatureUI/WBP_FloatingTextWidget.WBP_FloatingTextWidget_C"));
	if (FloatingTextWidgetFind.Succeeded())
	{
		FloatingTextWidgetClass = FloatingTextWidgetFind.Class;
	}
}

void ACreatureCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACreatureCharacter::CreatureCharacterOverlapBegin);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ACreatureCharacter::CreatureCharacterOverlapEnd);
	}


	if (LevelHpStateComponent)
	{
		ULevelHPStateWidget* HpBarStateWidget = Cast<ULevelHPStateWidget>(LevelHpStateComponent->GetWidget());

		if (HpBarStateWidget)
			HpBarStateWidget->UpdateCreatureCharacter(this);

		LevelHpStateComponent->SetRelativeLocation(FVector(0.f, 0.f, GetDefaultHalfHeight() + HpBarZOffset));

		ShowHpBar(false);
	}

	ShowHpBarTimerDele.BindUFunction(this, "ShowHpBar", false);
	GetDeadDelegate().AddDynamic(this, &ACreatureCharacter::HideHpBar);
}

void ACreatureCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ACreatureCharacter::CreatureCharacterOverlapBegin);
		GetCapsuleComponent()->OnComponentEndOverlap.RemoveDynamic(this, &ACreatureCharacter::CreatureCharacterOverlapEnd);

		//InterpRotationFinishedDele.RemoveAll(this);
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ACreatureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetDead()) return;

	CreatureCharacterPushEachOther(DeltaTime);
	InterpRotationToTarget(DeltaTime);
}

void ACreatureCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetMesh())
	{
		GetMesh()->bReceivesDecals = false;
	}
}

void ACreatureCharacter::Damaged(FDamageInfo* DamageInfo)
{
	if (GetDead() || DamageInfo == nullptr) return;

	TSortedMap<EDamageActionCommandTag, FCommandArray>& Commands = DamageInfo->GetDamageActionCommands();

	for (TSortedMap<EDamageActionCommandTag, FCommandArray>::TIterator It(Commands); It; ++It)
	{
		bool bIgnoreCommand = false;

		int CurBit = 1 << (int)It.Key();
		if ((CurBit & DamageActionCommandIgnoreBit) == CurBit)
		{
			bIgnoreCommand = true;
		}

		FCommandArray& CommandArray = It.Value();
		for (ICommand* Command : CommandArray.Get())
		{
			if (bIgnoreCommand == false)
			{
				Command->Execute(this);
			}
			UCommandPoolManager::ReturnCommand(Command);
		}
	}	
}

FDamagedDelegate& ACreatureCharacter::GetDamagedDelegate()
{
	return OnDamaged;
}

FDeadDelegate& ACreatureCharacter::GetDeadDelegate()
{
	return OnDead;
}

FIKFootTraceResult ACreatureCharacter::IKFootTrace(const FName& SocketName, float Distance, bool bDebugLine)
{
	if (IsValid(GetMesh()) == false)
	{
		return FIKFootTraceResult();
	}

	FVector TraceStart = GetMesh()->GetSocketLocation(SocketName);
	TraceStart.Z = GetActorLocation().Z;
	FVector TraceEnd = FVector(TraceStart.X, TraceStart.Y, TraceStart.Z - Distance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FVector HitLocation = FVector::ZeroVector;
	float FootZOffset = 0.f;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams);
	if (bHit)
	{
		if (bDebugLine)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green);
		}

		
		FVector Diff = HitResult.Location - GetMesh()->GetComponentLocation();
		//FVector Diff = HitResult.Location - (GetCapsuleComponent()->GetComponentLocation() - FVector(0.f, 0.f, GetDefaultHalfHeight()));

		FootZOffset = Diff.Z - IKHipOffset;
		HitLocation = HitResult.Location;
	}
	else
	{
		if (bDebugLine)
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red);
		}
	}

	return FIKFootTraceResult(FootZOffset, HitLocation);
}

void ACreatureCharacter::ReserveShowHPBar(float Duration)
{
	ShowHpBar(true);
	GetWorld()->GetTimerManager().SetTimer(ShowHpBarTimer, ShowHpBarTimerDele, Duration, false);
}

void ACreatureCharacter::SpawnDamageUI(int Damage, bool bIsEnemy)
{
	UWidgetComponent* WidgetComponent = NewObject<UWidgetComponent>(this);
	WidgetComponent->SetWidgetClass(FloatingTextWidgetClass);
	WidgetComponent->SetDrawSize(FVector2D(300, 300));
	WidgetComponent->RegisterComponent();
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WidgetComponent->SetWorldLocation(GetActorLocation());

	UFloatingTextWidget* Widget = Cast<UFloatingTextWidget>(WidgetComponent->GetWidget());
	if (IsValid(Widget))
	{
		FLinearColor DamageColor;

		if (bIsEnemy)
		{
			if (Damage >= 10)
			{
				DamageColor = FLinearColor::Yellow;
			}
			else
			{
				DamageColor = FLinearColor::Gray;
			}
		}
		else
		{
			DamageColor = FLinearColor::Red;
		}

		Widget->SetText(FString::FromInt(Damage), DamageColor);
	}

	
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(
		Timer, 
		FTimerDelegate::CreateLambda([WidgetComponent]()
		{
			if (IsValid(WidgetComponent))
			{
				WidgetComponent->DestroyComponent();
			}
		}),
		1.5f,
		false
	);
}

void ACreatureCharacter::ShowHpBar(bool bState)
{
	if (IsValid(LevelHpStateComponent) == false || GetIsUseHPBar() == false) return;

	ULevelHPStateWidget* HpBarStateWidget = Cast<ULevelHPStateWidget>(LevelHpStateComponent->GetWidget());
	if (HpBarStateWidget)
	{
		HpBarStateWidget->SetVisibility((bState) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	
}

void ACreatureCharacter::HideHpBar()
{
	UE_LOG(LogTemp, Warning, TEXT("HideHpBar %s"), *GetFName().ToString());
	ShowHpBar(false);
}

void ACreatureCharacter::SetHPBarZOffset(float NewZOffset)
{
	if (LevelHpStateComponent)
	{
		LevelHpStateComponent->SetRelativeLocation(FVector(0.f, 0.f, GetDefaultHalfHeight() + NewZOffset));
	}
}

void ACreatureCharacter::SetHPBarZOffsetToOrigin()
{
	if (LevelHpStateComponent)
	{
		LevelHpStateComponent->SetRelativeLocation(FVector(0.f, 0.f, GetDefaultHalfHeight() + HpBarZOffset));
	}
}

float ACreatureCharacter::GetOriginHpBarZOffset() const
{
	return HpBarZOffset;
}

void ACreatureCharacter::SetInterpRotationTarget(const FVector& Location)
{
	bInterpRotation = true;
	TargetInterpRotation = (Location - GetActorLocation()).Rotation();
	TargetInterpRotation.Pitch = 0.f;
	TargetInterpRotation.Roll = 0.f;
	InterpingRotation = GetActorRotation();
}

void ACreatureCharacter::SetInterpRotationTarget(const FRotator& Rotation)
{
	bInterpRotation = true;
	TargetInterpRotation = Rotation;
	InterpingRotation = GetActorRotation();
}

void ACreatureCharacter::SetIgnoreCommandState(EDamageActionCommandTag IgnoreCommand, bool bState)
{
	int IgnoreBit = (1 << (int)IgnoreCommand);
	if (bState)
	{
		DamageActionCommandIgnoreBit |= IgnoreBit;
	}
	else
	{
		DamageActionCommandIgnoreBit &= (~IgnoreBit);
	}
	
}

void ACreatureCharacter::SetAllIgnoreCommandState(bool bState)
{
	for (int i = 0; i < (int)EDamageActionCommandTag::EDACT_MAX; ++i)
	{
		SetIgnoreCommandState((EDamageActionCommandTag)i, bState);
	}
}

void ACreatureCharacter::SetIgnoreDamage(bool bNewState)
{
	if (IsValid(GetCapsuleComponent()))
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_CreatureDetection, ((bNewState == false) ? ECollisionResponse::ECR_Overlap : ECollisionResponse::ECR_Ignore));
	}

}

//void ACreatureCharacter::ActiveSuperArmorForDuration(float Duration)
//{
//	ActiveSuperArmor();
//	GetWorld()->GetTimerManager().SetTimer(HitReactCoolTimeTimer, this, &AEnemy::InactiveSuperArmor, 0.001f, false, HitReactCoolTime);
//}

void ACreatureCharacter::SetDead_Implementation()
{
	bDead = true;

	if (bAutoDestroy)
		SetLifeSpan(DestroyTime);

	for (int i = 0; i < OverlappingCreatures.Num(); i++)
	{
		auto Character = OverlappingCreatures[i];
		Character->OverlappingCreatures.Remove(this);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_CreatureDetection, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Creature, ECollisionResponse::ECR_Ignore);
	}

	if (GetCharacterMovement())
	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
		

	if (GetMesh())
	{
		GetMesh()->SetCollisionResponseToChannel(ECC_CreatureDetection, ECollisionResponse::ECR_Ignore);
		//GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	OnDead.Broadcast();
}

void ACreatureCharacter::ActiveSuperArmor()
{
	if (CanActiveSuperArmor() == false) return;
	bSuperArmored = true;
}

void ACreatureCharacter::InactiveSuperArmor()
{
	if (CanInactiveSuperArmor() == false) return;
	bSuperArmored = false; 
}

void ACreatureCharacter::CreatureCharacterOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetDead()) return;

	auto Character = Cast<ACreatureCharacter>(OtherActor);
	auto MyCapsule = Cast<UCapsuleComponent>(OverlappedComp);
	auto OtherCapsule = Cast<UCapsuleComponent>(OtherComp);

	if (Character && MyCapsule && OtherCapsule)
	{
		if (Character->GetDead() == false)
			OverlappingCreatures.Add(Character);
	}
}

void ACreatureCharacter::CreatureCharacterOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetDead()) return;

	auto Character = Cast<ACreatureCharacter>(OtherActor);
	auto MyCapsule = Cast<UCapsuleComponent>(OverlappedComp);
	auto OtherCapsule = Cast<UCapsuleComponent>(OtherComp);

	if (Character && MyCapsule && OtherCapsule)
	{
		if (OverlappingCreatures.Find(Character) != INDEX_NONE)
		{
			OverlappingCreatures.Remove(Character);
		}
	}
}

void ACreatureCharacter::CreatureCharacterPushEachOther(float DeltaTime)
{
	for (int i = 0; i < OverlappingCreatures.Num(); i++)
	{
		auto Character = OverlappingCreatures[i];
		
		FVector MyLocation = GetActorLocation();
		FVector OtherLocation = Character->GetActorLocation();

		auto MyCapsule = GetCapsuleComponent();
		auto OtherCapsule = Character->GetCapsuleComponent();

		float MyRadius = MyCapsule->GetScaledCapsuleRadius();
		float OtherRadius = OtherCapsule->GetScaledCapsuleRadius();

		
		FVector Direction = OtherLocation - MyLocation;
		Direction.Z = 0.f;
		float PushAmount = (MyRadius + OtherRadius) - Direction.Size();
		Direction.Normalize();

		Character->SetActorLocation(OtherLocation + (Direction * DeltaTime * PushAmount * 0.5f * 2.f), true);
		SetActorLocation(MyLocation + (-Direction * DeltaTime * PushAmount * 0.5f * 2.f), true);
		//Character->LaunchCharacter(Direction * 100.f, false, true);
	}
	
}

void ACreatureCharacter::RemoveDeadCreatures()
{
	if (GetDead()) OverlappingCreatures.Empty();

	OverlappingCreatures.RemoveAll
	(
		[](ACreatureCharacter* CreatureCharacter)->bool
		{
			if (CreatureCharacter)
			{
				return CreatureCharacter->GetDead();
			}
			return false;
		}
	);
}

void ACreatureCharacter::InterpRotationToTarget(float DeltaTime)
{
	if (CanInterpRotation() == false) return;

	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetInterpRotation, InterpingRotation);
	if (DeltaRotation.IsNearlyZero(1.f))
	{
		bInterpRotation = false;
	}
	
	InterpingRotation = FMath::RInterpTo(InterpingRotation, TargetInterpRotation, DeltaTime, InterpRotationSpeed);
	SetActorRotation(InterpingRotation);
}

void ACreatureCharacter::PreventFallLedge(float DeltaTime)
{
	// ¾×¼ÇÀÌ ½ÇÇàÁßÀÏ¶© ¾×¼Ç¿¡ ¸ÂÃç¼­ ¶³¾îÁü¹æÁöÃ³¸®
	if (IsActionPlaying())
	{

	}
	else
	{
		FVector Velocity = GetLastMovementInputVector();
		Velocity.Normalize();

		FVector StartLocation = GetActorLocation() + Velocity * GetCapsuleComponent()->GetScaledCapsuleRadius();
		FVector EndLocation = StartLocation - FVector(0.f, 0.f, GetDefaultHalfHeight() + 50.f);
		
		/*DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 100.f, 5.f);*/
		FHitResult HitResult;
		bool bTraceHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

		if (bTraceHit == false)
		{
			StartLocation.Z -= (GetDefaultHalfHeight() + 20.f);
			EndLocation = StartLocation - (Velocity * GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.f);

			FHitResult NormalHitResult;
			if (GetWorld()->LineTraceSingleByChannel(NormalHitResult, StartLocation, EndLocation, ECC_Visibility))
			{
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 100.f, 5.f);
				FVector NewVelocity = GetMovementComponent()->ComputeSlideVector(GetLastMovementInputVector(), 1.f, -1.f * NormalHitResult.ImpactNormal, NormalHitResult);

				DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + NewVelocity * 100.f, FColor::Blue, false, -1.f, 100.f, 5.f);
				
				
				GetMovementComponent()->Velocity = NewVelocity * 300.f;
				SetActorLocation(GetActorLocation() + (-1.f * NormalHitResult.ImpactNormal * DeltaTime * NormalHitResult.Distance), true);
			}
		}

		
	}
}

bool ACreatureCharacter::CanInterpRotation()
{
	return bInterpRotation == true && IsActionPlaying() == false;
}

void ACreatureCharacter::SetRotationCurrentToInterp()
{
	InterpingRotation = GetActorRotation();
}

//void ACreatureCharacter::BindInterpRotationFinished(void(*Func)())
//{
//	InterpRotationFinishedDele.AddDynamic(this, Func);
//}

bool ACreatureCharacter::GetDead()
{
	return bDead;
}

int ACreatureCharacter::GetHP() const
{
	return HP;
}

void ACreatureCharacter::GainHP(int HPAmount)
{
	HP += HPAmount;
	if (HP > MaxHP)
		HP = MaxHP;
}

void ACreatureCharacter::SetMaxHP(int NewMaxHP, bool bHPUpdate)
{
	MaxHP = NewMaxHP;
	if (bHPUpdate)
		HP = MaxHP;
}

int ACreatureCharacter::GetMaxHP() const
{
	return MaxHP;
}

void ACreatureCharacter::SetHP(int NewHP)
{
	HP = NewHP;
	if (HP > MaxHP)
	{
		HP = MaxHP;
	}
	else if (HP < 0)
	{
		HP = 0;
	}
}

FFootStepResult ACreatureCharacter::FootStep(const FName& BoneName)
{
	FFootStepResult Ret;

	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f, 0.f, -1000.f) };
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = 1;

	bool bTraceSucceed = GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (bTraceSucceed)
	{
		Ret.Surface = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
		if (GetMesh())
		{
			Ret.SpawnLocation = GetMesh()->GetSocketLocation(BoneName);
		}
	}
	Ret.bSucceed = bTraceSucceed;

	return Ret;
}

float ACreatureCharacter::AddAction(const FActionInfo& ActionInfo, bool bForceAdd)
{
	if (ActionComponent)
	{
		return ActionComponent->AddAction(ActionInfo, bForceAdd);
	}
	return 0.f;
}

void ACreatureCharacter::PlayAction()
{
	if (ActionComponent)
	{
		//UE_LOG(LogTemp, Error, TEXT("%s"), *GetActorLocation().ToString());
		ActionComponent->PlayAction();
	}
}

void ACreatureCharacter::PauseAction()
{
	if (ActionComponent)
	{
		ActionComponent->PauseAction();
	}
}

void ACreatureCharacter::ResumeAction()
{
	if (ActionComponent)
	{
		ActionComponent->ResumeAction();
	}
}

void ACreatureCharacter::ResetAction()
{
	if (ActionComponent)
	{
		ActionComponent->ResetAction();
	}
}

void ACreatureCharacter::ClearAction()
{
	if (ActionComponent)
	{
		ActionComponent->ClearAction();
	}
}

const bool ACreatureCharacter::IsActionPlaying()
{
	if (ActionComponent)
	{
		return ActionComponent->IsActionPlaying();
	}
	return false;
}
