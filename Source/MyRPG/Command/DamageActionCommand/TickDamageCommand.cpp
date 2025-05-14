// Fill out your copyright notice in the Description page of Project Settings.


#include "TickDamageCommand.h"
#include "MyRPG/Command/CommandPoolManager.h"
#include <MyRPG/Enemy/Enemy.h>

EDamageActionCommandTag UTickDamageCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_Damage;
}

void UTickDamageCommand::Reset()
{
	UScheduledCommand::Reset();
	Creature = nullptr;
	Character = nullptr;
}

void UTickDamageCommand::Initialize(UWorld* World, ICreature* InCreature, float InRate, float InDuration, int InDamage)
{
	CurWorld = World;

	Rate = InRate;
	FirstDelay = 0.f;
	Duration = InDuration;
	bLoop = true;
	Damage = InDamage;
	Creature = InCreature;

	Character = Cast<ACreatureCharacter>(Creature);
	bIsEnemy = Cast<AEnemy>(Creature) ? true : false;
	
	TimerDele.BindUFunction(this, "TickDamage");
}

void UTickDamageCommand::TickDamage()
{
	if (Creature == nullptr || Creature->GetDead() == true)
	{
		Clear();
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
