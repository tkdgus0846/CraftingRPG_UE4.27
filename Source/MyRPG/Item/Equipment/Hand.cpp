// Fill out your copyright notice in the Description page of Project Settings.

#include "Hand.h"


void AHand::BeginPlay()
{
	Super::BeginPlay();

	SetMaxCombo(2);
	SetCanMoveWhenAttack(true);
	SetAttackWalkSpeed(300.f);
}

bool AHand::NormalAttackPressed()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	bool bBegin = BeginCombo();
	if (bBegin == false) return false;

	

	GetPlayerAnimInstance()->Montage_Play(NormalAttackMontage);

	switch (GetCurrentCombo())
	{
	case 0:
		GetPlayerAnimInstance()->Montage_JumpToSection("Combo1", NormalAttackMontage);
		break;
	case 1:
		GetPlayerAnimInstance()->Montage_JumpToSection("Combo2", NormalAttackMontage);
		break;
	case 2:
		GetPlayerAnimInstance()->Montage_JumpToSection("Combo3", NormalAttackMontage);
		break;

	}

	EndCombo();
	return true;
}

bool AHand::NormalAttackPressing(float DeltaTime)
{
	if (Super::NormalAttackPressed() == false)
		return false;

	bool bBegin = BeginCombo();
	if (bBegin == false) return false;

	GetPlayerAnimInstance()->Montage_Play(NormalAttackMontage);

	switch (GetCurrentCombo())
	{
	case 0:
		GetPlayerAnimInstance()->Montage_JumpToSection("Combo1", NormalAttackMontage);
		break;
	case 1:
		GetPlayerAnimInstance()->Montage_JumpToSection("Combo2", NormalAttackMontage);
		break;
	case 2:
		GetPlayerAnimInstance()->Montage_JumpToSection("Combo3", NormalAttackMontage);
		break;

	}

	EndCombo();
	return true;
}

bool AHand::NormalAttackReleased()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	return true;
}