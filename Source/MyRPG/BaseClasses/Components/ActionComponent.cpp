
#include "ActionComponent.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/BaseClasses/CreatureCharacter.h"
#include "MyRPG/SplineActor/SplineActor.h"
#include "DrawDebugHelpers.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"


UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<ASplineActor> ClassFind(TEXT("Blueprint'/Game/Blueprints/Splines/BP_SplineActor.BP_SplineActor_C'"));
	if (ClassFind.Succeeded())
	{
		SplineActorClass = ClassFind.Class;
	}

}


void UActionComponent::BeginPlay()
{
	Super::BeginPlay();

	MyActor = GetOwner();
	Character = Cast<ACreatureCharacter>(GetOwner());
}

void UActionComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (FActionInfo& ActionInfo : ActionQueue)
	{
		ASplineActor* SplineActor = ActionInfo.GetSplineActor();
		if (IsValid(SplineActor))
		{
			SplineActor->OnUpdateSpline.Unbind();
			SplineActor->OnFinishedSpline.Unbind();
		}
		
	}
}


void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UActionComponent::AddAction(const FActionInfo& ActionInfoParam, bool bForceAdd)
{
	if (IsActionPlaying() && bForceAdd == false)
	{
		return 0.f;
	}	
	else if (bForceAdd == true)
	{
		ClearAction();
	}


	CurIndex = 0;	
	FActionInfo ActionInfo = ActionInfoParam;

	auto GameInstance = GetWorld()->GetGameInstance<UMyGameInstance>();
	if (GameInstance == nullptr) return 0.f;

	UCurveFloat* CurveFloat = GameInstance->GetCurveFloat(ActionInfo.CurveType);

	float Time;
	// ActionTime을 설정하지 않은 경우에
	if (ActionInfo.ActionTime == -1.f && ActionInfo.AnimMontage)
	{
		Time = (ActionInfo.AnimMontage->GetSectionLength(ActionInfo.AnimMontage->GetSectionIndex(ActionInfo.SectionName)) * (1.f/FMath::Abs(ActionInfo.MontagePlayRate))) - ActionInfo.FirstDelay - ActionInfo.PreActionMontagePlayTime;
		Time = (Time < 0.f) ? 0.f : Time;
	}
	else
	{
		Time = ActionInfo.ActionTime - ActionInfo.FirstDelay;
	}
	
	FVector StartLocation = ActionInfo.SplinePoint[0].PointLocation; 
	ActionInfo.SplineActor = GetWorld()->SpawnActorDeferred<ASplineActor>(SplineActorClass, FTransform(StartLocation));

	for (int i = 0; i < ActionInfo.SplinePoint.Num(); i++)
	{
		ActionInfo.SplineActor->AddPoint(i, ActionInfo.SplinePoint[i].PointLocation, ActionInfo.SplinePoint[i].PointType, ActionInfo.SplinePoint[i].bIsLocal);
	}
		
	ActionInfo.SplineActor->OnUpdateSpline.BindUFunction(this, FName("ActionFunc"));
	ActionInfo.SplineActor->OnFinishedSpline.BindUFunction(this, FName("NextAction"));
	ActionInfo.SplineActor->BindInfoTimeline(CurveFloat, Time, true);
	

	ActionQueue.Add(ActionInfo);

	// 본 액션시간과 딜레이를 합쳐야 총 액션시간이라는 논리
	return Time + ActionInfo.FirstDelay;
}

void UActionComponent::AddMovePointToAction(FActionInfo& ActionInfo, const FVector& Location, ESplinePointType::Type PointType, bool bIsLocal)
{
	ActionInfo.AddMovePoint(Location, PointType, bIsLocal);
}

void UActionComponent::PlayAction()
{
	if (MyActor == nullptr || IsActionPlaying()) return;

	bEnableActionStop = false;
	bFirstTick = true;
	bEvenOneTickBlocked = false;
	//bMontagePlayed = false;
	FActionInfo& ActionInfo = GetCurActionInfo();
	PlayActionAnimation(ActionInfo);

	if (ActionInfo.OriginRotation.IsZero() == false)
		MyActor->SetActorRotation(ActionInfo.OriginRotation);

	if (GetCurActionInfo().FirstDelay == 0.f)
		PlayFromStart();
	else
		GetWorld()->GetTimerManager().SetTimer(FirstDelayTimer, this, &UActionComponent::PlayFromStart, 0.0001f, false, GetCurActionInfo().FirstDelay);
}

void UActionComponent::PauseAction()
{

}

void UActionComponent::ResetAction()
{
	CurIndex = 0;
}

void UActionComponent::ClearAction()
{
	//bMontagePlayed = false;

	CurIndex = 0;
	ClearActionQueue();
}

void UActionComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	ClearActionQueue();
}

void UActionComponent::EnableActionStop()
{
	bEnableActionStop = true;
}

void UActionComponent::ResumeAction()
{

}

void UActionComponent::NextAction()
{
	if (ActionQueue.Num() > CurIndex + 1)
	{
		CurIndex += 1;
		PlayAction();
	}
	else
	{
		ClearAction();
	}
}

void UActionComponent::ActionFunc()
{
	if (MyActor == nullptr || bEvenOneTickBlocked == true) return;
	if (bFirstTick == true)
	{
		bFirstTick = false;
		return;
	}

	const FActionInfo& ActionInfo = GetCurActionInfo();
	if (IsValid(ActionInfo.SplineActor) == false || ActionInfo.SplineActor == nullptr) return;

	FVector Pos;
	bool bSweep = ActionInfo.bSweep;
	
	Pos = ActionInfo.SplineActor->GetCurrentPos();

	
#pragma region PrePlayNextAnimation
	FActionInfo* NextActionInfo = GetNextActionInfo();
	if (NextActionInfo && NextActionInfo->PreActionMontagePlayTime > 0.f && 
		(NextActionInfo->PreActionMontagePlayTime >= ActionInfo.SplineActor->GetRemainedTime()))
	{
		PlayActionAnimation(*NextActionInfo);
	}
#pragma endregion	

	if (ActionInfo.bAttachToGround && IsValid(Character))
	{
		AttachToGround(Pos, bSweep);
	}
	
	if (ActionInfo.TeleportType == ETeleportType::TeleportPhysics && IsValid(ActionInfo.GetSplineActor()))
	{
		
		FRotator Rotation = FMath::RInterpTo(MyActor->GetActorRotation(), ActionInfo.GetSplineActor()->GetCurrentTangent().Rotation(), GetWorld()->GetDeltaSeconds(), 10.f);
		MyActor->SetActorRotation(Rotation);

		if (IsValid(Character))
		{
			Character->AddMovementInput(FVector(1.f, 0.f, 0.f));
		}
	}
	MyActor->SetActorLocation(Pos, bSweep, nullptr, ActionInfo.TeleportType);
}

void UActionComponent::PlayFromStart()
{
	if (IsCurIndexValid() == false) return;

	//UE_LOG(LogTemp, Warning, TEXT("Action Start!!"));
	FVector StartLocation = GetCurActionInfo().SplinePoint[0].PointLocation;
	UGameplayStatics::FinishSpawningActor(GetCurActionInfo().SplineActor, FTransform(StartLocation));
}

FActionInfo& UActionComponent::GetCurActionInfo()
{
	return ActionQueue[CurIndex];
}

FActionInfo* UActionComponent::GetNextActionInfo()
{
	if (ActionQueue.IsValidIndex(CurIndex + 1))
	{
		return &ActionQueue[CurIndex + 1];
	}
	else
	{
		return nullptr;
	}
}

const bool UActionComponent::IsCurIndexValid()
{
	return CurIndex < ActionQueue.Num();
}

const bool UActionComponent::PlayActionAnimation(FActionInfo& ActionInfo)
{
	// 애니메이션 처리부분
	// 애니메이션이 덜끝난 상태에서 액션이 끝

	/*UAnimMontage* AnimMontage = ActionInfo.AnimMontage;
	if (Character && AnimMontage && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(AnimMontage);

		if (bMontagePlayed && CurrentSectionName.IsNone())
		{
		}
		else if (AnimInstance->Montage_IsPlaying(AnimMontage) == false ||
			CurrentSectionName.IsEqual(ActionInfo.SectionName) == false)
		{
			bMontagePlayed = true;
			AnimInstance->Montage_Play(AnimMontage);
			AnimInstance->Montage_SetPlayRate(AnimMontage, ActionInfo.MontagePlayRate);

			if (ActionInfo.SectionName.IsEqual(DEFAULT_NAME) == false)
				AnimInstance->Montage_JumpToSection(ActionInfo.SectionName, AnimMontage);

			return true;
		}

	}
	return false;*/

	if (ActionInfo.bMontagePlayed == true)
		return false;

	UAnimMontage* AnimMontage = ActionInfo.AnimMontage;
	if (Character && AnimMontage && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

		AnimInstance->Montage_Play(AnimMontage);
		AnimInstance->Montage_SetPlayRate(AnimMontage, ActionInfo.MontagePlayRate);
		AnimInstance->Montage_JumpToSection(ActionInfo.SectionName, AnimMontage);

		ActionInfo.bMontagePlayed = true;
		return true;
	}
	return false;
}

void UActionComponent::ClearActionQueue()
{
	for (int i = 0; i < ActionQueue.Num(); i++)
	{
		if (IsValid(ActionQueue[i].SplineActor) == false || ActionQueue[i].SplineActor == nullptr) continue;
		ActionQueue[i].SplineActor->Destroy();
	}
	ActionQueue.Empty();
}

void UActionComponent::AttachToGround(FVector& Pos, bool& bSweep)
{
	// Pos = 앞으로 갈 위치
	Pos.Z = Character->GetActorLocation().Z;
	FVector TargetPos = Pos - FVector(0.f, 0.f, Character->GetDefaultHalfHeight() + 50.f);
	FHitResult HitResult;
	bool bTrace = GetWorld()->LineTraceSingleByChannel(HitResult, Pos, TargetPos, ECC_Visibility);
	bool bCanMoveAngle = false;

	// 0~0.5 이면 0에서 45도까지 체크
	// 법선벡터와 z 성분을 제거한 벡
	if (bTrace)
	{
		FVector NormalWithoutZ = FVector(FVector2D(HitResult.ImpactNormal), 0.f);

		if (NormalWithoutZ == FVector::ZeroVector)
		{
			bCanMoveAngle = true;
		}
		else
		{
			float Angle = FVector::DotProduct(NormalWithoutZ, HitResult.ImpactNormal);
			bCanMoveAngle = (Angle > 0.f && Angle < 0.5f) ? true : false;
		}
	}

	if (bTrace == false || bCanMoveAngle == false)
	{
		bEvenOneTickBlocked = true;
		return;
	}
	else
	{
		float CharacterBaseHeight = Pos.Z - Character->GetDefaultHalfHeight();

		// 올라가는경우
		if (CharacterBaseHeight + MaxActionStepHeight >= HitResult.ImpactPoint.Z && CharacterBaseHeight + MinActionStepHeight <= HitResult.ImpactPoint.Z)
		{
			bSweep = false;
		}
		// 내려가는경우
		else
		{
			bSweep = true;
		}

		Pos = HitResult.ImpactPoint + FVector(0.f, 0.f, Character->GetDefaultHalfHeight());
	}
}

const bool UActionComponent::IsActionPlaying()
{
	return (IsCurIndexValid() && GetCurActionInfo().SplineActor->IsPlaying()) 
		|| GetWorld()->GetTimerManager().IsTimerActive(FirstDelayTimer);
}
