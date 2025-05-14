// Fill out your copyright notice in the Description page of Project Settings.


#include "HolyAngelMeteorAttack.h"
#include <NiagaraFunctionLibrary.h>
#include <MyRPG/Interfaces/Creature.h>
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void AHolyAngelMeteorAttack::BeginPlay()
{
	Super::BeginPlay();

	UNiagaraFunctionLibrary::SpawnSystemAttached(TrailVFX, RootScene, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
}

void AHolyAngelMeteorAttack::PlayAttackVFX()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackVFX, GetActorLocation(), GetActorForwardVector().Rotation() + FRotator(0.f, 90.f, 0.f));
}

void AHolyAngelMeteorAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
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

void AHolyAngelMeteorAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AHolyAngelMeteorAttack::OnOverlapping_Implementation(float DeltaTime)
{
}