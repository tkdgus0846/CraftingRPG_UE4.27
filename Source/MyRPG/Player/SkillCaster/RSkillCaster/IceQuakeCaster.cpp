// Fill out your copyright notice in the Description page of Project Settings.


#include "IceQuakeCaster.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include "MyRPG/Player/Components/MouseComponent.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include <Kismet/GameplayStatics.h>

void AIceQuakeCaster::SkillPressed()
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;

	PlayerCharacter->GetMouseComponent()->TurnOnAimPreview(EAimPreview::EAP_Sector, SkillSize, SkillSize.X);
}

void AIceQuakeCaster::SkillPressing(float DeltaTime)
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;
}

void AIceQuakeCaster::SkillReleased()
{
	if (IsValid(PlayerCharacter) == false || IsValid(SkillMontage) == false) return;

	PlayerCharacter->GetMouseComponent()->TurnOffAimPreview();
	PlayerCharacter->GetMouseComponent()->RotateByAimPreview();

	FActionInfo Action;
	Action.AnimMontage = SkillMontage;
	Action.AddMovePoint(PlayerCharacter->GetActorLocation());
	PlayerCharacter->AddAction(Action);
	PlayerCharacter->PlayAction();

	//PlayerCharacter->PlayAnimMontage(SkillMontage);
	PlayerCharacter->GetCombatComponent()->OnRSkill.Broadcast();

	SkillTransfrom = FTransform(PlayerCharacter->GetActorRotation(), PlayerCharacter->GetActorLocation());
	RerserveSpawnSkillActor(1.f);

	
}

void AIceQuakeCaster::SpawnSkillActor()
{
	if (IsValid(SkillActorClass) == false) return;
	AAttackActor* SkillActor = GetWorld()->SpawnActorDeferred<AAttackActor>(SkillActorClass, SkillTransfrom, GetPlayer());
	if (IsValid(SkillActor) == false) return;
	UGameplayStatics::FinishSpawningActor(SkillActor, SkillTransfrom);
}
