// Fill out your copyright notice in the Description page of Project Settings.

//
#include "FootmanNormalAttack.h"
#include "MyRPG/Interfaces/Creature.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"

void AFootmanNormalAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());

		Damage(Creature);
		Destroy();
	}
}

void AFootmanNormalAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AFootmanNormalAttack::OnOverlapping_Implementation(float DeltaTime)
{
}
