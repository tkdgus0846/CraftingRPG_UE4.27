// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/ActionActor.h"
#include "PaladinBossDashChargeAttack.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API APaladinBossDashChargeAttack : public AActionActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* VFX;
};
