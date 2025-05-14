
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StateEffect.generated.h"

UCLASS()
class MYRPG_API AStateEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	AStateEffect();

	void Initialize(const FName& NewEffectName, float NewDuration);

	void Play();
	void Stop();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ReserveEndMaterialAnim(float ReserveDuration);

	UPROPERTY()
	class AVFXCreatureCharacter* Character = nullptr;
	UPROPERTY()
	class UMyGameInstance* GameInstance = nullptr;

	/*UPROPERTY()
	FTimerHandle StateTimer;*/
	UPROPERTY()
	FTimerHandle StateMaterialAnimTimer;

	
	UPROPERTY()
	FName StateName;
	UPROPERTY()
	float StateDuration;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* StateMaterial;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AImpactEffect> StateImpactEffectClass;

	virtual void StartState() {}

private:
	UFUNCTION()
	void EndMaterialAnim();

};
