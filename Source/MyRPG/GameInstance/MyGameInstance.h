
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyRPG/Types/CameraShakeType.h"
#include "MyRPG/Types/MaterialParameterCollectionType.h"
#include "MyRPG/Types/PostProcessAnimationType.h"
#include "MyRPG/Types/CurveValueType.h"
#include "MyRPG/Types/StateMaterialType.h"
#include "MyRPG/Data/PlayCharacterVFXInfo.h"
#include "Components/TimelineComponent.h"
#include "MyGameInstance.generated.h"

class UCameraShakeBase;
class UCurveFloat;


UCLASS()
class MYRPG_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void ClientPlayCameraShake(ECameraShake WhichShake, float Scale = 1.f);
	UCurveFloat* GetCurveFloat(ECurveFloat Type);
	
	UFUNCTION(BlueprintCallable)
	void SlowMotion(ECurveFloat Type, float Duration, float FirstDelay = -1.f);

	UFUNCTION(BlueprintCallable)
	void StopSlowMotion();

	UFUNCTION(BlueprintCallable)
	void TimeDilation(float Dilation);

	/*UFUNCTION(BlueprintCallable)
	void SetMaterialScalarParameter(FName ParameterName, float ParameterValue, EMaterialParameterCollectionType Type = EMaterialParameterCollectionType::EMPC_PostProcess);*/

	UFUNCTION(BlueprintCallable)
	void Play_PostProcessAnimation(EPostProcessAnimationType Type, float Duration);

	// If Use LinearColorCurve. ScaleValue, ValueOffset will be ignored
	UFUNCTION(BlueprintCallable)
	void Play_MaterialParamAnimation(const FName& Name, class USceneComponent* Component, TArray<FPlayMaterialParameterAnimationInfo>& PlayInfo);

	void Clear_MaterialParamAnimation(class USceneComponent* Key, const FName& Name);

	UFUNCTION()
	class ADecalActor* SpawnDecal(TSubclassOf<ADecalActor> Decal, const FTransform& Transform, const FVector DecalSize = FVector::ZeroVector);

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ECameraShake, TSubclassOf<UCameraShakeBase>> CS_Map;

	TSubclassOf<UCameraShakeBase> GetCameraShake(ECameraShake WhichShake);

	UPROPERTY(EditDefaultsOnly)
	TMap<ECurveFloat, UCurveFloat*> CurveFloat_Map;

	FTimeline SlowMotionTimeline;
	FTimerHandle SlowMotionTimer;

	UFUNCTION()
	void CurveFloatSlowMotion(float Value);

	void PlaySlowMotionTimeline();
	void TickSlowMotionTimeline();

	float SlowmotionTickDeltaTime;

	UPROPERTY(EditDefaultsOnly)
	TMap<EMaterialParameterCollectionType, UMaterialParameterCollection*> MaterialParameterCollection_Map;

	UPROPERTY(EditDefaultsOnly)
	TMap<EPostProcessAnimationType, TSubclassOf<class APostProcessAnimator>> PostProcessAnimator_Map;

	/*
		Material�� ��Į�� �ִϸ��̼��� ���������� �����ϰ� ���ϰ� ���Ҽ��ְ� �ϱ�����
		�Ʒ��� �ڵ���� �ۼ�.
	*/
	UPROPERTY()
	TMap<TWeakObjectPtr<USceneComponent>, FMaterialParameterAnimatorMap> MaterialAnimationInfo;

	FTimerHandle MaterialAnimationInfo_CleanUpTimer;

	UFUNCTION()
	void CleanUp_MaterialParamAnimation();

public:

	
};
