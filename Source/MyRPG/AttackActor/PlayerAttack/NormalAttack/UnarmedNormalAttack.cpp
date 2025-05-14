
#include "UnarmedNormalAttack.h"
#include "Components/StaticMeshComponent.h"
#include "MyRPG/BaseClasses/CreatureActor.h"
#include "MyRPG/Interfaces/Creature.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/Effect/ImpactEffect.h"
#include "DrawDebugHelpers.h"

#include "MyRPG/Command/CommandPoolManager.h"
#include "MyRPG/Command/DamageActionCommand/CameraShakeCommand.h"
#include "MyRPG/Command/DamageActionCommand/DamageOnceCommand.h"
#include "MyRPG/Command/DamageActionCommand/SpawnImpactEffectCommand.h"

void AUnarmedNormalAttack::OnOverlapBegin_Implementation(AActor* OtherActor)
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

void AUnarmedNormalAttack::OnOverlapEnd_Implementation(AActor* OtherActor)
{
	
}

void AUnarmedNormalAttack::OnOverlapping_Implementation(float DeltaTime)
{

}
