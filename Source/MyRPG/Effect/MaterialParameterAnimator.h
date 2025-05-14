
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MyRPG/Types/CurveValueType.h"
#include "MyRPG/Data/PlayCharacterVFXInfo.h"
#include "MaterialParameterAnimator.generated.h"



UCLASS()
class MYRPG_API AMaterialParameterAnimator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMaterialParameterAnimator();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Initialize(const TArray<FMaterialParameterAnimationInfo>& NewAnimationInfo);
	void ClearToDefaultValue();

protected:
	

private:
	UPROPERTY()
	TArray<FTimeline> Timelines;
	UPROPERTY()
	TArray<FMaterialParameterAnimationInfo> AnimationInfo;
	void TickTimelines(float DeltaTime);

	void BindScalarInfo(const FMaterialParameterAnimationInfo& Info, FTimeline& Timeline);
	void BindLinearColorInfo(const FMaterialParameterAnimationInfo& Info, FTimeline& Timeline);

	UFUNCTION()
	void CurveFloatFunc(float Value);
	UFUNCTION()
	void CurveLinearColorFunc(FLinearColor Color);
	UFUNCTION()
	void CurveFinished();

	int CurrentIndex = 0;
	int TimelineFinishedCount = 0;


};
