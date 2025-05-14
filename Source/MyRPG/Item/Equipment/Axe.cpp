// Fill out your copyright notice in the Description page of Project Settings.


#include "Axe.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/MouseComponent.h"
#include "MyRPG/Player/Components/CombatComponent.h"

#include "MyRPG/AttackActor/PlayerAttack/SkillAttack/AxeQSkill.h"
#include "MyRPG/AttackActor/PlayerAttack/SkillAttack/AxeESkill.h"
#include <Kismet/GameplayStatics.h>


void AAxe::BeginPlay()
{
    Super::BeginPlay();

    SetMaxCombo(2);
    SetCanMoveWhenAttack(true);
    SetAttackWalkSpeed(300.f);
}

bool AAxe::NormalAttackPressed()
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

bool AAxe::NormalAttackPressing(float DeltaTime)
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

bool AAxe::NormalAttackReleased()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	return true;
}

void AAxe::QSkillPressed()
{
	APlayerCharacter* Player = GetPlayer();
	if (Player == nullptr || Player->IsActionPlaying()) return;


	Player->GetMouseComponent()->RotatePlayerByMouseDirection();

	FActionInfo Action;

	Action.bSweep = true;
	Action.AnimMontage = SkillMontage;
	Action.SectionName = "QSkill";
	Action.bAttachToGround = true;
	Action.ActionTime = 0.5f;
	//Action.MontagePlayRate = 10.5f;
	//Action.FirstDelay = 0.2f;
	Action.CurveType = ECurveFloat::ECF_EaseOut;

	Action.AddMovePoint(Player->GetActorLocation());
	Action.AddMovePoint(Player->GetActorLocation() + Player->GetActorForwardVector() * 400.f);

	Player->AddAction(Action);

	FActionInfo Action2;

	Action2.bSweep = true;
	Action2.ActionTime = 0.2f;
	Action2.bAttachToGround = true;
	Action2.AddMovePoint(Player->GetActorLocation() + Player->GetActorForwardVector() * 400.f);

	Player->AddAction(Action2);
	Player->PlayAction();
	

	GetPlayerAnimInstance()->Montage_Play(SkillMontage);
	GetPlayerAnimInstance()->Montage_JumpToSection("QSkill", SkillMontage);

	if (Player->GetCombatComponent())
	{
		Player->GetCombatComponent()->OnQSkill.Broadcast();
	}
}

void AAxe::QSkillPressing(float DeltaTime)
{
}

void AAxe::QSkillReleased()
{
}

void AAxe::ESkillPressed()
{
	APlayerCharacter* Player = GetPlayer();

	if (Player == nullptr || Player->GetMouseComponent() == nullptr) return;

	Player->GetMouseComponent()->TurnOnAimPreview(EAimPreview::EAP_Circle, ESkillSize);
}

void AAxe::ESkillPressing(float DeltaTime)
{
	
}

void AAxe::ESkillReleased()
{
	APlayerCharacter* Player = GetPlayer();
	if (Player == nullptr) return;

	if (Player == nullptr || Player->GetMouseComponent() == nullptr) return;

	Player->GetMouseComponent()->TurnOffAimPreview();

	TargetLocation = Player->GetMouseComponent()->GetAimPreviewPos();
	//Player->GetMouseComponent()->RotateByAimPreview();

	GetPlayerAnimInstance()->Montage_Play(SkillMontage);
	GetPlayerAnimInstance()->Montage_JumpToSection("ESkill", SkillMontage);

	FTimerHandle ESkillTimer;
	GetWorld()->GetTimerManager().SetTimer(ESkillTimer, this, &AAxe::ESkill, 0.0001f, false, 0.5f);

	if (Player->GetCombatComponent())
	{
		Player->GetCombatComponent()->OnESkill.Broadcast();
	}
}

void AAxe::ESkill()
{
	APlayerCharacter* Player = GetPlayer();
	if (Player == nullptr) return;

	FVector PlayerLocation = Player->GetActorLocation();

	AAxeESkill* LeftAxe = GetWorld()->SpawnActorDeferred<AAxeESkill>(ESkillActor, FTransform(PlayerLocation));
	AAxeESkill* RightAxe = GetWorld()->SpawnActorDeferred<AAxeESkill>(ESkillActor, FTransform(PlayerLocation));

	LeftAxe->SetLeftRotateAxe(true);
	RightAxe->SetLeftRotateAxe(false);

	UGameplayStatics::FinishSpawningActor(LeftAxe, FTransform(PlayerLocation));
	UGameplayStatics::FinishSpawningActor(RightAxe, FTransform(PlayerLocation));

	FActionInfo LeftAction1, LeftAction2;

	float Distance = FVector::DistXY(TargetLocation, PlayerLocation);
	FVector ForwardVector = (TargetLocation - PlayerLocation).GetSafeNormal2D();
	FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);

	FVector StartLocation = PlayerLocation - (RightVector * 50.f) + (ForwardVector * 50.f);
	FVector MiddleLocation = PlayerLocation - (RightVector * 200.f) + (ForwardVector * (Distance / 2.f));
	FVector EndLocation = PlayerLocation - (RightVector * 50.f) + ForwardVector * Distance;

	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);

	LeftAction1.ActionTime = Distance / 600.f;
	LeftAction1.bSweep = false;
	LeftAction1.AddMovePoint(StartLocation);
	LeftAction1.AddMovePoint(MiddleLocation);
	LeftAction1.AddMovePoint(EndLocation);
	LeftAction1.CurveType = ECurveFloat::ECF_Default;

	LeftAction2.ActionTime = 2.f;
	LeftAction2.bSweep = false;
	LeftAction2.AddMovePoint(EndLocation);

	LeftAxe->AddAction(LeftAction1);
	LeftAxe->AddAction(LeftAction2);

	LeftAxe->SetLifeSpan(LeftAction1.ActionTime + LeftAction2.ActionTime);
	LeftAxe->PlayAction();

	FActionInfo RightAction1, RightAction2;

	StartLocation = PlayerLocation + (RightVector * 50.f) + (ForwardVector * 50.f);
	MiddleLocation = PlayerLocation + (RightVector * 200.f) + (ForwardVector * (Distance / 2.f));
	EndLocation = PlayerLocation + (RightVector * 50.f) + ForwardVector * Distance;

	RightAction1.ActionTime = Distance / 600.f;
	RightAction1.bSweep = false;
	RightAction1.AddMovePoint(StartLocation);
	RightAction1.AddMovePoint(MiddleLocation);
	RightAction1.AddMovePoint(EndLocation);
	RightAction1.CurveType = ECurveFloat::ECF_Default;

	RightAction2.ActionTime = 2.f;
	RightAction2.bSweep = false;
	RightAction2.AddMovePoint(EndLocation);

	RightAxe->AddAction(RightAction1);
	RightAxe->AddAction(RightAction2);

	RightAxe->SetLifeSpan(RightAction1.ActionTime + RightAction2.ActionTime);
	RightAxe->PlayAction();
}
