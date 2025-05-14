
#include "PostProcessAnimator.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"

APostProcessAnimator::APostProcessAnimator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APostProcessAnimator::Initialize(float Duration)
{
	AnimationDuration = Duration;
}

void APostProcessAnimator::SetPlayRateByPercent(UTimelineComponent* Timeline, float Percent)
{
	if (IsValid(Timeline) == false) return;

	Timeline->SetPlayRate(1.f / (AnimationDuration * Percent));
}

void APostProcessAnimator::BeginPlay()
{
	Super::BeginPlay();
	
	Play();
	SetLifeSpan(AnimationDuration + 0.1f);
}

void APostProcessAnimator::Play_Implementation()
{
}
