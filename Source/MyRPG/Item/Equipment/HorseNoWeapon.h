// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HorseNoWeapon.generated.h"

UCLASS()
class MYRPG_API AHorseNoWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	virtual bool NormalAttackPressed() override;
	virtual bool NormalAttackPressing(float DeltaTime) override;
	virtual bool NormalAttackReleased() override;
};
