
#include "MaceQSkill.h"
#include "MyRPG/Enemy/Enemy.h"
#include "MyRPG/BaseClasses/CreatureActor.h"
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/LaunchCharacterCommand.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void AMaceQSkill::BeginPlay()
{
	Super::BeginPlay();

	Cast<UMyGameInstance>(GetGameInstance())->ClientPlayCameraShake(ECameraShake::ECS_Explosion, 0.2f);

	FVector Location = GetActorLocation() - FVector(0.f,0.f,70.f);
	GetWorld()->SpawnActor<AImpactEffect>(SpawnEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), Location));
}

void AMaceQSkill::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());
		
		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());
		AddDamageCommand(ULaunchCharacterCommand, OtherActor, FVector(0.f, 0.f, 500.f), true, true);
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);
	}
}

void AMaceQSkill::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AMaceQSkill::OnOverlapping_Implementation(float DeltaTime)
{
}
