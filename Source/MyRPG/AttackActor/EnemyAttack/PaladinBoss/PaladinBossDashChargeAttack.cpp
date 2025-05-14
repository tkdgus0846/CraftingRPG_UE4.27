// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinBossDashChargeAttack.h"
#include <NiagaraFunctionLibrary.h>
#include <MyRPG/Interfaces/Creature.h>
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void APaladinBossDashChargeAttack::BeginPlay()
{
	Super::BeginPlay();

	UNiagaraFunctionLibrary::SpawnSystemAttached(VFX, RootScene, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
	SetLifeSpan(3.0f);
}

void APaladinBossDashChargeAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);

		Destroy();
	}
}

void APaladinBossDashChargeAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void APaladinBossDashChargeAttack::OnOverlapping_Implementation(float DeltaTime)
{
}
