// Fill out your copyright notice in the Description page of Project Settings.


#include "Mace.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include "MyRPG/Player/Components/MouseComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyRPG/SplineActor/SplineActor.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/AttackActor/AttackActor.h"

void AMace::BeginPlay()
{
	Super::BeginPlay();

	SetMaxCombo(2);
	SetCanMoveWhenAttack(true);
	SetAttackWalkSpeed(300.f);
}

bool AMace::NormalAttackPressed()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	bool bBegin = BeginCombo();
	if (bBegin == false) return false;

	GetPlayerAnimInstance()->Montage_Play(NormalAttackMontage);
	float Size = GetPlayer()->GetVelocity().Size();

	switch (GetCurrentCombo())
	{
	case 0:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo1", NormalAttackMontage);
		break;
	case 1:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo2", NormalAttackMontage);
		break;
	case 2:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo3", NormalAttackMontage);
		break;

	}
	

	EndCombo();
	return true;
}

bool AMace::NormalAttackPressing(float DeltaTime)
{
	if (Super::NormalAttackPressed() == false)
		return false;

	bool bBegin = BeginCombo();
	if (bBegin == false) return false;

	GetPlayerAnimInstance()->Montage_Play(NormalAttackMontage);
	float Size = GetPlayer()->GetVelocity().Size();

	switch (GetCurrentCombo())
	{
	case 0:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo1", NormalAttackMontage);
		break;
	case 1:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo2", NormalAttackMontage);
		break;
	case 2:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo3", NormalAttackMontage);
		break;

	}


	EndCombo();
	return true;
}

bool AMace::NormalAttackReleased()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	return true;
}

void AMace::QSkillPressed()
{
	APlayerCharacter* Player = GetPlayer();

	if (Player == nullptr || Player->GetMouseComponent() == nullptr) return;

	Player->GetMouseComponent()->TurnOnAimPreview(EAimPreview::EAP_Circle, QSkillSize);
}

void AMace::QSkillPressing(float DeltaTime)
{
}

void AMace::QSkillReleased()
{
	if (GetWorld() == nullptr) return;
	APlayerCharacter* Player = GetPlayer();

	if (Player == nullptr || Player->GetMouseComponent() == nullptr) return;

	Player->GetMouseComponent()->TurnOffAimPreview();

	FActionInfo ActionMaceQ;

	ActionMaceQ.ActionTime = 0.73f;
	ActionMaceQ.bSweep = true;
	ActionMaceQ.SectionName = FName("QSkill");
	ActionMaceQ.AnimMontage = SkillMontage;
	ActionMaceQ.CurveType = ECurveFloat::ECF_Default;
	ActionMaceQ.MontagePlayRate = 1.f;
	ActionMaceQ.FirstDelay = 0.14f;
	
	FVector StartLocation = Player->GetActorLocation();
	FVector TargetLocation = Player->GetMouseComponent()->GetAimPreviewPos();
	FVector MiddleLocation = Player->GetActorLocation();
	MiddleLocation += (TargetLocation - Player->GetActorLocation()) / 2.f;
	MiddleLocation.Z += 300.f;
	TargetLocation.Z += Player->GetDefaultHalfHeight();

	ActionMaceQ.OriginRotation = (TargetLocation - StartLocation).GetSafeNormal2D().Rotation();

	ActionMaceQ.AddMovePoint(StartLocation);
	ActionMaceQ.AddMovePoint(MiddleLocation);
	ActionMaceQ.AddMovePoint(TargetLocation);

	FActionInfo DelayMaceQ;

	DelayMaceQ.ActionTime = 0.47f;
	DelayMaceQ.AddMovePoint(TargetLocation);

	Player->AddAction(ActionMaceQ);
	Player->AddAction(DelayMaceQ);
	Player->PlayAction();

	SplineActor = ActionMaceQ.GetSplineActor();


	GetWorld()->GetTimerManager().SetTimer(RideSplineTimerHandle, this, &AMace::SpawnQSkillActor, 0.0001f, false, 0.9f);

	if (Player->GetCombatComponent())
	{
		Player->GetCombatComponent()->OnQSkill.Broadcast();
	}
}

void AMace::ESkillPressed()
{
	GetPlayerAnimInstance()->Montage_Play(SkillMontage);
	GetPlayerAnimInstance()->Montage_JumpToSection("ESkill");

	APlayerCharacter* Player = GetPlayer();
	if (Player)
	{
		FVector ForwardVec = Player->GetActorForwardVector();
		FActionInfo ActionMaceE;

		ActionMaceE.bSweep = true;
		ActionMaceE.SectionName = FName("ESkill");
		ActionMaceE.AnimMontage = SkillMontage;
		ActionMaceE.ActionTime = 1.4;
		ActionMaceE.AddMovePoint(Player->GetActorLocation());

		Player->AddAction(ActionMaceE);
		Player->PlayAction();

		/*FActorSpawnParameters Params;
		Params.Owner = GetPlayer();
		AAttackActor* ESkill = GetWorld()->SpawnActor<AAttackActor>(ESkillActor, Params);*/
		FTimerHandle ESkillTempTimer;
		GetWorld()->GetTimerManager().SetTimer(ESkillTempTimer, this, &AMace::SpawnESkillActor, 0.0001f, false, 0.4f);
		

		if (Player->GetCombatComponent())
		{
			Player->GetCombatComponent()->OnESkill.Broadcast();
		}
	}
}

void AMace::ESkillPressing(float DeltaTime)
{
}

void AMace::ESkillReleased()
{
}

void AMace::SpawnQSkillActor()
{
	AAttackActor* QSkill = GetWorld()->SpawnActorDeferred<AAttackActor>(QSkillActor, GetPlayer()->GetActorTransform(), GetPlayer());

	UpdateAttackDamage(QSkill, 1.5f);
	QSkill->SetBoxSize(QSkillSize);
	QSkill->SetLifeSpan(0.2f);

	UGameplayStatics::FinishSpawningActor(QSkill, GetPlayer()->GetActorTransform());
}

void AMace::SpawnESkillActor()
{
	AAttackActor* ESkill = GetWorld()->SpawnActorDeferred<AAttackActor>(ESkillActor, FTransform::Identity, GetPlayer());

	UpdateAttackDamage(ESkill, 1.5f);
	if (ESkill)
		ESkill->SetLifeSpan(0.1f);

	UGameplayStatics::FinishSpawningActor(ESkill, FTransform::Identity);
}
