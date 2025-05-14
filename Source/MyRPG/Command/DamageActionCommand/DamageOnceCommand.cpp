// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageOnceCommand.h"
#include <MyRPG/Enemy/Enemy.h>

void UDamageOnceCommand::Execute(UObject* WorldContext)
{
	if (Creature == nullptr || Creature->GetDead())
	{
		return;
	}

	if (IsValid(Character))
	{
		Character->SpawnDamageUI(Damage, bIsEnemy);
	}

	Creature->GainHP(-Damage);
	Creature->GetDamagedDelegate().Broadcast();

	if (Creature->GetHP() <= 0)
	{
		Creature->GainHP(-Creature->GetHP());
		Creature->SetDead();
	}
}

void UDamageOnceCommand::Reset()
{
	Creature = nullptr;
	Character = nullptr;
}

EDamageActionCommandTag UDamageOnceCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_Damage;
}

void UDamageOnceCommand::Initialize(ICreature* NewCreature, int NewDamage)
{
	Creature = NewCreature;
	Damage = NewDamage;

	Character = Cast<ACreatureCharacter>(Creature);
	bIsEnemy = Cast<AEnemy>(Creature) ? true : false;
}
