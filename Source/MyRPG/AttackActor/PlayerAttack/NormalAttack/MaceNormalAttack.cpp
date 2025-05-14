
#include "MaceNormalAttack.h"
#include "MyRPG/BaseClasses/CreatureActor.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/Enemy/Enemy.h"
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/LaunchCharacterCommand.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"


void AMaceNormalAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());
		
		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());

		FVector ForwardVector = GetOwner()->GetActorForwardVector() * 200.f + FVector::UpVector;
		AddDamageCommand(ULaunchCharacterCommand, OtherActor, ForwardVector, true, true);
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);
	}
}

void AMaceNormalAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AMaceNormalAttack::OnOverlapping_Implementation(float DeltaTime)
{
}
