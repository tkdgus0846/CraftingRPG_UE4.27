
#include "MaterialParameterAnimator.h"
#include "Curves/CurveLinearColor.h"

AMaterialParameterAnimator::AMaterialParameterAnimator()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMaterialParameterAnimator::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < AnimationInfo.Num(); ++i) 
	{
		if (AnimationInfo[i].bPlayReverse)
			Timelines[i].ReverseFromEnd();
		else
			Timelines[i].PlayFromStart();
	}
	
}

void AMaterialParameterAnimator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickTimelines(DeltaTime);
}

void AMaterialParameterAnimator::Initialize(const TArray<FMaterialParameterAnimationInfo>& NewAnimationInfo)
{
	AnimationInfo = NewAnimationInfo;

	Timelines.SetNum(AnimationInfo.Num());

	for (int i = 0; i < AnimationInfo.Num(); i++)
	{
		auto& Info = AnimationInfo[i];
		switch (Info.ParameterMode)
		{
		case ECurveValueType::ECVT_Scalar:
		{
			BindScalarInfo(Info, Timelines[i]);
			break;
		}
		case ECurveValueType::ECVT_LinearColor:
		{
			BindLinearColorInfo(Info, Timelines[i]);
			break;
		}
		default:
			break;
		}

	}
	
}

void AMaterialParameterAnimator::ClearToDefaultValue()
{
	for (auto& Info : AnimationInfo)
	{
		if (Info.bValueToDefaultWhenEndVFX == false) continue;

		switch (Info.ParameterMode)
		{
		case ECurveValueType::ECVT_Scalar:
			Info.Instance->SetScalarParameterValue(Info.ParameterName, Info.DefaultScalarValue);
			break;
		case ECurveValueType::ECVT_LinearColor:
			Info.Instance->SetVectorParameterValue(Info.ParameterName, Info.DefaultColorValue);
			break;
		default:
			break;
		}

	}

	Destroy();
}

void AMaterialParameterAnimator::TickTimelines(float DeltaTime)
{
	for (int i = 0; i < Timelines.Num(); i++)
	{
		CurrentIndex = i;
		Timelines[i].TickTimeline(DeltaTime);
	}
}

void AMaterialParameterAnimator::BindScalarInfo(const FMaterialParameterAnimationInfo& Info, FTimeline& Timeline)
{
	FOnTimelineFloat CurveFloatCallback;
	CurveFloatCallback.BindUFunction(this, FName("CurveFloatFunc"));
	Timeline.AddInterpFloat(Info.CurveFloat, CurveFloatCallback);

	FOnTimelineEvent TimelineFinishedCallback;
	TimelineFinishedCallback.BindUFunction(this, FName("CurveFinished"));
	Timeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	Timeline.SetPlayRate(1.f / Info.Duration);
}

void AMaterialParameterAnimator::BindLinearColorInfo(const FMaterialParameterAnimationInfo& Info, FTimeline& Timeline)
{
	FOnTimelineLinearColor CurveLinearColorCallback;
	CurveLinearColorCallback.BindUFunction(this, FName("CurveLinearColorFunc"));
	Timeline.AddInterpLinearColor(Info.CurveLinearColor, CurveLinearColorCallback);

	FOnTimelineEvent TimelineFinishedCallback;
	TimelineFinishedCallback.BindUFunction(this, FName("CurveFinished"));
	Timeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	Timeline.SetPlayRate(1.f / Info.Duration);
}

void AMaterialParameterAnimator::CurveFloatFunc(float Value)
{
	FMaterialParameterAnimationInfo& Info = AnimationInfo[CurrentIndex];

	Info.Instance->SetScalarParameterValue(Info.ParameterName, Value * Info.ScaleValue + Info.ValueOffset);
}

void AMaterialParameterAnimator::CurveLinearColorFunc(FLinearColor Color)
{
	FMaterialParameterAnimationInfo& Info = AnimationInfo[CurrentIndex];

	Info.Instance->SetVectorParameterValue(Info.ParameterName, Color);
}

void AMaterialParameterAnimator::CurveFinished()
{
	TimelineFinishedCount += 1;

	if (TimelineFinishedCount == AnimationInfo.Num())
		ClearToDefaultValue();
}

