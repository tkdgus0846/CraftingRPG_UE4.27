
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMTriggerVolume.generated.h"

class USoundWave;

UENUM(BlueprintType)
enum class EBGMTriggerTiming : uint8
{
	EBTT_None UMETA(DisplayName = "None"),
	EBTT_Start UMETA(DisplayName = "Start"),
	EBTT_Middle UMETA(DisplayName = "Middle"),
	EBTT_End UMETA(DisplayName = "End")
};

UCLASS()
class MYRPG_API ABGMTriggerVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ABGMTriggerVolume();

	UFUNCTION()
	void PlayEndTriggerSound();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditInstanceOnly)
	bool bProcedural = false;

	UPROPERTY(EditInstanceOnly, Meta = (EditCondition = "bProcedural == false", EditConditionHides))
	USoundWave* Sound;
	UPROPERTY(EditInstanceOnly, Meta = (EditCondition = "bProcedural == false", EditConditionHides))
	float Volume;

	UPROPERTY(EditInstanceOnly, Meta = (EditCondition = "bProcedural == true", EditConditionHides))
	TMap<EBGMTriggerTiming, USoundWave*> SoundMap;
	UPROPERTY(EditInstanceOnly, Meta = (EditCondition = "bProcedural == true", EditConditionHides))
	TMap<EBGMTriggerTiming, float> SoundFadeInDelayTimeMap;
	UPROPERTY(EditInstanceOnly, Meta = (EditCondition = "bProcedural == true", EditConditionHides))
	TMap<EBGMTriggerTiming, float> SoundVolmueMap;

	UPROPERTY(EditInstanceOnly)
	float FadeInTime = 1.f;
	UPROPERTY(EditInstanceOnly)
	float FadeOutTime = 1.f;

	UPROPERTY()
	TWeakObjectPtr<class UAudioComponent> CurAudioComponent;
	//UPROPERTY()
	//TWeakObjectPtr<class UAudioComponent> NextAudioComponent;
	//
	//UPROPERTY()
	//EBGMTriggerTiming CurrentBGMTiming;

	FTimerHandle SpawnPreceduralSoundTimer;
	UFUNCTION()
	void SpawnPreceduralSound(EBGMTriggerTiming BGMTiming);

	UFUNCTION()
	void CollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
