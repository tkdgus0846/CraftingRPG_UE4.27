// Fill out your copyright notice in the Description page of Project Settings.


#include "HorseJumpCaster.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


void AHorseJumpCaster::SkillPressed()
{
	if (IsValid(PlayerCharacter) == false || IsValid(PlayerCharacter->GetRiding()) == false || IsValid(PlayerCharacter->GetCombatComponent()) == false || PlayerCharacter->GetRiding()->GetCharacterMovement()->IsFalling()) return;

	PlayerCharacter->GetRiding()->Jump();
	PlayerCharacter->GetCombatComponent()->OnSpaceSkill.Broadcast();
}

void AHorseJumpCaster::SkillPressing(float DeltaTime)
{
}

void AHorseJumpCaster::SkillReleased()
{
}
