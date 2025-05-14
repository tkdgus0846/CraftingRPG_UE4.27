// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordESkill.h"
#include "MyRPG/Interfaces/Creature.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/Effect/ImpactEffect.h"

void ASwordESkill::BeginPlay()
{
	Super::BeginPlay();
}

void ASwordESkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (Player)
	{
		bool bSkillPlaying = Player->GetCombatComponent()->IsSkillMontagePlaying(EWeaponType::EWT_Sword);

		if (bSkillPlaying == false)
		{
			Destroy();
			Player->WalkSpeedToOrigin();
		}
	}
}

void ASwordESkill::OnOverlapBegin_Implementation(AActor* OtherActor)
{
}

void ASwordESkill::OnOverlapEnd_Implementation(AActor* OtherActor)
{
}

void ASwordESkill::OnOverlapping_Implementation(float DeltaTime)
{
	//ElapsedTime += DeltaTime;
	//if (HitTiming <= ElapsedTime)
	//{
	//	ElapsedTime = 0.f;
	//	for (int i = 0; i < OverlappingCreatures.Num(); i++)
	//	{
	//		ICreature* Creature = Cast<ICreature>(OverlappingCreatures[i]);

	//		if (Creature == nullptr) continue;

	//		
	//		Cast<UMyGameInstance>(GetGameInstance())->ClientPlayCameraShake(ECameraShake::ECS_AttackActor, 0.6f);

	//		FVector Location = OverlappingCreatures[i]->GetActorLocation();
	//		//DrawDebugSphere(GetWorld(), Location, 40.f, 50, FColor::Red, true);

	//		/*AImpactEffect* Impact = GetWorld()->SpawnActorDeferred<AImpactEffect>(ImpactEffectClass.Get(), FTransform(Location), this);

	//		Impact->SetHitResult();*/

	//		GetWorld()->SpawnActor<AImpactEffect>(ImpactEffectClass.Get(), FTransform(Location));

	//		FDamageInfo DamageInfo = FDamageInfo(GetOwner());
	//		Creature->Damaged(GetDamage(), DamageInfo);
	//	}


	//}
}
