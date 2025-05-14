// Fill out your copyright notice in the Description page of Project Settings.


#include "TickSpawnImpactEffectCommand.h"
#include "MyRPG/Interfaces/Creature.h"
#include <MyRPG/Effect/ImpactEffect.h>

void UTickSpawnImpactEffectCommand::Execute(UObject* WorldContext)
{
	if (Creature == nullptr) return;

	UScheduledCommand::Execute(WorldContext);
}

EDamageActionCommandTag UTickSpawnImpactEffectCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_SpawnEffect;
}

void UTickSpawnImpactEffectCommand::Reset()
{
	UScheduledCommand::Reset();
	Actor = nullptr;
	Creature = nullptr;
	ImpactEffectClass = nullptr;
}

void UTickSpawnImpactEffectCommand::Initialize(UWorld* World, UClass* Class, AActor* InActor, float InRate, float InDuration, bool bInLoop)
{
	CurWorld = World;

	ImpactEffectClass = Class;

	Rate = InRate;
	FirstDelay = 0.f;
	Duration = InDuration;
	bLoop = bInLoop;

	Actor = InActor;
	Creature = Cast<ICreature>(Actor);

	TimerDele.BindUFunction(this, "TickSpawnImpactEffect");
}

void UTickSpawnImpactEffectCommand::TickSpawnImpactEffect()
{
	if (Creature == nullptr || Creature->GetDead() == true || IsValid(Actor) == false)
	{
		Clear();
		return;
	}

	if (IsValid(CurWorld))
	{
		CurWorld->SpawnActor<AImpactEffect>(ImpactEffectClass, FTransform(Actor->GetActorForwardVector().Rotation(), Actor->GetActorLocation()));
	}
}
