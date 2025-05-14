

#include "MaceESkill.h"
#include "MyRPG/Enemy/Enemy.h"
#include "MyRPG/BaseClasses/CreatureActor.h"
#include "MyRPG/Effect/ImpactEffect.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/LaunchCharacterCommand.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void AMaceESkill::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	ICreature* Creature = Cast<ICreature>(OtherActor);
	if (Creature)
	{
		SettingDamageInfo(GetOwner());

		AddDamageCommand(UCameraShakeCommand, ECameraShake::ECS_AttackActor, 1.f);
		AddDamageCommand(UDamageOnceCommand, Creature, GetDamage());

		FVector ForwardVector = GetOwner()->GetActorForwardVector() * 1000.f + FVector::UpVector;
		AddDamageCommand(ULaunchCharacterCommand, OtherActor, ForwardVector, true, true);
		AddDamageCommand(USpawnImpactEffectCommand, ImpactEffectClass.Get(), FTransform(GetActorForwardVector().Rotation(), OtherActor->GetActorLocation()));

		Damage(Creature);
	}
}

void AMaceESkill::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void AMaceESkill::OnOverlapping_Implementation(float DeltaTime)
{
	/*for (AActor* OtherActor : OverlappingCreatures)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			FVector ForwardVector = GetOwner()->GetActorForwardVector();
			Enemy->LaunchCharacter(ForwardVector * 50.f, false, false);
		}
	}*/
	
}
