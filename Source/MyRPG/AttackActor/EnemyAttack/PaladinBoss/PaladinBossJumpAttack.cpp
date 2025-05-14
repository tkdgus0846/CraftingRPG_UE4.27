// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinBossJumpAttack.h"
#include "MyRPG/Effect/ImpactEffect.h"
#include "NiagaraComponent.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/Interfaces/Creature.h>

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void APaladinBossJumpAttack::BeginPlay()
{
	SetSphereRadius(420.f);

	Super::BeginPlay();

	GetWorld()->SpawnActor<AImpactEffect>(SpawnEffectClass, FTransform(GetActorLocation()));
}

void APaladinBossJumpAttack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APaladinBossJumpAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);
	}
}

void APaladinBossJumpAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void APaladinBossJumpAttack::OnOverlapping_Implementation(float DeltaTime)
{
}
