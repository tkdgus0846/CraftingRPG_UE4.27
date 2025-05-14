// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseNoWeapon.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"


void AHorseNoWeapon::BeginPlay()
{
	Super::BeginPlay();
}

bool AHorseNoWeapon::NormalAttackPressed()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	APlayerCharacter* Player = GetPlayer();

	if (Player && Player->GetCombatComponent())
		Player->GetCombatComponent()->OnNormalAttack.Broadcast();

	return true;
}

bool AHorseNoWeapon::NormalAttackPressing(float DeltaTime)
{
	return true;
}

bool AHorseNoWeapon::NormalAttackReleased()
{
	return true;
}
