// Fill out your copyright notice in the Description page of Project Settings.

#include "KnightNormalAttack.h"
#include "MyRPG/BaseClasses/CreatureActor.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/Enemy/Enemy.h"
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void AKnightNormalAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);
	}
}

void AKnightNormalAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AKnightNormalAttack::OnOverlapping_Implementation(float DeltaTime)
{
}
