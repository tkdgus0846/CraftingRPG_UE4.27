
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/ActionActor.h"
#include "PaladinBossKneelAttack.generated.h"

UCLASS()
class MYRPG_API APaladinBossKneelAttack : public AActionActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;

private:
	const float DamageInterval = 0.2f;
	float CalcDamageTime = 0.f;

	float WaitTime = 2.f;
	FTimerHandle VFXWaitTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* WaitVFX;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* AttackVFX;

	UPROPERTY()
	TWeakObjectPtr<class UNiagaraComponent> WaitVFXComponent;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* LoopSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundCue* WaitFinishSound;

public:
	FORCEINLINE void SetWaitTime(float InWaitTime) { WaitTime = InWaitTime; }
};
