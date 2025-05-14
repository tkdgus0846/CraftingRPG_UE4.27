
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/ActionActor.h"
#include "PaladinBossVerticalSlashAttack.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API APaladinBossVerticalSlashAttack : public AActionActor
{
	GENERATED_BODY()

public:
	APaladinBossVerticalSlashAttack();

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;

private:
	void InterpRotation(float DeltaTime);
	FRotator TargetRotation;
	FVector PrevPos;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* SpinSound;
	UPROPERTY()
	TWeakObjectPtr<class UAudioComponent> SpinSoundComponent;

	/*UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* VerticalSlashEndFX;*/
	//bool bFirstTick = true;
};
