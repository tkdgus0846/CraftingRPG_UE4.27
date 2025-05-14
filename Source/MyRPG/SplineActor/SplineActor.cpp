
#include "SplineActor.h"
#include "Components/SplineComponent.h"

ASplineActor::ASplineActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootSceneComponent");
	if (RootSceneComponent)
	{
		SetRootComponent(RootSceneComponent);
	}

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	if (RootSceneComponent && SplineComponent)
	{
		SplineComponent->SetupAttachment(RootSceneComponent);
	}
}

void ASplineActor::BeginPlay()
{
	Super::BeginPlay();	
	
	Play();
}

void ASplineActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SplineTimeline.IsPlaying())
	{
		SplineTimeline.TickTimeline(DeltaTime);
		ElapsedTime += DeltaTime;

		if (bDestroyFinished || bLoop == false) return;

		if (SplineTimeline.GetPlaybackPosition() >= SplineTimeline.GetTimelineLength())
		{
			if (bOneWay)
			{
				SplineTimeline.PlayFromStart();
			}
			else
			{
				SplineTimeline.ReverseFromEnd();
				bReversing = true;
			}
			bFinishedOneRound = true;
		}

		if (bOneWay == false && bReversing && SplineTimeline.GetPlaybackPosition() <= 0.f)
		{
			SplineTimeline.PlayFromStart();
			bReversing = false;
			bFinishedOneRound = true;
		}
	}
}

void ASplineActor::BindInfoTimeline(UCurveFloat* CurveFloat, float CustomTimelineLength, bool bDestroyAfterFinished)
{
	if (CurveFloat == nullptr)
	{
		if (DefaultCurveFloat)
			CurveFloat = DefaultCurveFloat;

		if (CurveFloat == nullptr)
			return;
	}

	FOnTimelineFloat CurveFloatCallback;
	CurveFloatCallback.BindUFunction(this, FName("CurveFloatFunc"));
	SplineTimeline.AddInterpFloat(CurveFloat, CurveFloatCallback);

	if (bDestroyAfterFinished)
	{
		bDestroyFinished = bDestroyAfterFinished;

		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("SplineActorDestroy"));
		SplineTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}

	SplineTimeline.SetPlayRate(1.f / CustomTimelineLength);
	TimelineLength = CustomTimelineLength;
}

void ASplineActor::Play()
{
	//UE_LOG(LogTemp, Warning, TEXT("Spline Start!!"));
	SplineTimeline.PlayFromStart();
}

const bool ASplineActor::IsPlaying() const
{
	return SplineTimeline.IsPlaying();
}

void ASplineActor::Stop()
{
	SplineTimeline.Stop();
}

void ASplineActor::Resume()
{
	if (bReversing)
	{
		SplineTimeline.Reverse();
	}
	else
	{
		SplineTimeline.Play();
	}
}

void ASplineActor::InitializePatrol()
{
	bFinishedOneRound = false;
	SetAttachToGround(true);
	Resume();
}

float ASplineActor::GetRemainedTime()
{
	return TimelineLength - GetElapsedTime();
}

float ASplineActor::GetElapsedTime()
{
	return ElapsedTime;
}

void ASplineActor::SplineActorDestroy()
{
	//Destroy();
	OnFinishedSpline.Execute();
}

void ASplineActor::CurveFloatFunc(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value);

	
	Value = FMath::Clamp(Value, 0.f, 1.f);
	float Distance = FMath::Lerp(0.f, SplineComponent->GetSplineLength(), Value);

	PrevPos = CurrentPos;
	CurrentPos = SplineComponent->GetWorldLocationAtDistanceAlongSpline(Distance);
	CurrentTangent = SplineComponent->GetWorldTangentAtDistanceAlongSpline(Distance);
	CurrentValue = Value;

	if (bAttachToGround)
	{
		FVector TargetPos = CurrentPos + FVector(0.f,0.f,-1000.f);
		FHitResult HitResult;
		bool bTrace = GetWorld()->LineTraceSingleByChannel(HitResult, CurrentPos, TargetPos, ECC_Visibility);
		if (bTrace)
		{
			CurrentPos = HitResult.ImpactPoint;
		}
	}		
	

	if (bReversing)
	{
		CurrentTangent *= -1.f;
	}

	if (OnUpdateSpline.IsBound())
		OnUpdateSpline.Execute();
}

FVector ASplineActor::GetNextTickPos(float TickAmount) const
{
	if (SplineComponent == nullptr) return FVector::ZeroVector;

	if (TickAmount == -1.f)
		TickAmount = GetWorld()->GetDeltaSeconds();

	float NextTickValue = FMath::Clamp(CurrentValue + TickAmount, 0.f, 1.f);
	float NextTickDistance = FMath::Lerp(0.f, SplineComponent->GetSplineLength(), NextTickValue);

	return SplineComponent->GetWorldLocationAtDistanceAlongSpline(NextTickDistance);
}


