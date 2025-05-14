// Fill out your copyright notice in the Description page of Project Settings.


#include "IceQuakeSkill.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/BaseClasses/VFXCreatureCharacter.h"
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/ChangeStateCommand.h"
#include "MyRPG/Command/DamageActionCommand/TickDamageCommand.h"
#include "MyRPG/Command/DamageActionCommand/TickSpawnImpactEffectCommand.h"

void AIceQuakeSkill::BeginPlay()
{
	Super::BeginPlay();
}

void AIceQuakeSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIceQuakeSkill::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UChangeStateCommand, Creature, EStateMaterialType::ESMT_Ice, 5.f);
		AddDamageCommand(UTickDamageCommand, GetWorld(), Creature, 0.2f, 5.f, GetDamage());
		AddDamageCommand(UTickSpawnImpactEffectCommand, GetWorld(), ImpactEffectClass.Get(), OtherActor, 0.2f, 5.f, true);
		
		Damage(Creature);
	}
}

void AIceQuakeSkill::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AIceQuakeSkill::OnOverlapping_Implementation(float DeltaTime)
{
}
