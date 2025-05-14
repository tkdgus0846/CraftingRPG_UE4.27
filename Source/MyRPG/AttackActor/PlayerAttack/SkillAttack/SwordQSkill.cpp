// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordQSkill.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Enemy/Enemy.h"
#include <MyRPG/Item/Equipment/Weapon.h>
#include <MyRPG/GameInstance/MyGameInstance.h>
#include "MyRPG/Effect/ImpactEffect.h"

void ASwordQSkill::BeginPlay()
{
	Super::BeginPlay();

	FActionInfo Action;

	Action.ActionTime = 0.4f;
	Action.bSweep = true;
	Action.CurveType = ECurveFloat::ECF_Default;

	FVector StartLocation = GetActorLocation();
	FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * 690.f;

	Action.AddMovePoint(StartLocation);
	Action.AddMovePoint(TargetLocation);

	AddAction(Action);
	PlayAction();
	
	SetLifeSpan(1.0f);
	//Cast<UMyGameInstance>(GetGameInstance())->ClientPlayCameraShake(ECameraShake::ECS_Explosion, 0.2f);
}

void ASwordQSkill::OnOverlapBegin_Implementation(AActor* OtherActor)
{
	if (bOverlapped) return;

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		Cast<UMyGameInstance>(GetGameInstance())->ClientPlayCameraShake(ECameraShake::ECS_Explosion, 0.2f);
		Action(Enemy);

		bOverlapped = true;
		ClearAction();
	}
}

void ASwordQSkill::OnOverlapEnd_Implementation(AActor* OtherActor)
{
	if (bOverlapped) return;
}

void ASwordQSkill::OnOverlapping_Implementation(float DeltaTime)
{
	if (bOverlapped) return;
}

void ASwordQSkill::Action(AEnemy* Enemy)
{
	const FVector& EnemyPos = EnemyAction(Enemy);
	PlayerAction(EnemyPos);
}

const FVector ASwordQSkill::EnemyAction(AEnemy* Enemy)
{
	FVector EnemyPos = Enemy->GetActorLocation();
	FVector TargetPos = EnemyPos + FVector(0.f, 0.f, 400.f);

	FActionInfo Action1;

	Action1.ActionTime = 0.4f;
	Action1.bSweep = false;
	Action1.CurveType = ECurveFloat::ECF_Default;

	Action1.AddMovePoint(EnemyPos);
	Action1.AddMovePoint(TargetPos);
	Enemy->AddAction(Action1, true);

	FActionInfo Action2;
	Action2.ActionTime = 3.6f;
	Action2.AddMovePoint(TargetPos);
	Action2.bSweep = false;

	Enemy->AddAction(Action2);

	Enemy->PlayAction();

	Enemy->StopBehaviorTree(4.f);
	Enemy->StopAllMontages(0.1f);

	return TargetPos;
}

void ASwordQSkill::PlayerAction(const FVector& EnemyPos)
{
	AWeapon* OwnerWeapon = Cast<AWeapon>(GetOwner());
	if (OwnerWeapon == nullptr) return;

	ACreatureCharacter* Player = OwnerWeapon->GetPlayer();
	if (Player == nullptr) return;

	

	FVector PlayerPos = Player->GetActorLocation();
	FVector TargetDirection = (PlayerPos - EnemyPos).GetSafeNormal2D();

	FVector TargetPos = EnemyPos + TargetDirection * 200.f;
	FVector BackPos = EnemyPos - TargetDirection * 200.f;
	FVector BackStepPos = EnemyPos - TargetDirection * 400.f;

	FActionInfo ActionPrepare;
	ActionPrepare.ActionTime = 0.4f;

	ActionPrepare.AddMovePoint(PlayerPos);
	Player->AddAction(ActionPrepare, true);


	FActionInfo Action1;
	//Action1.ActionTime = 0.8f;
	Action1.AnimMontage = OwnerWeapon->GetSkillMontage();
	Action1.SectionName = "QSkillDash";
	Action1.bSweep = false;
	Action1.CurveType = ECurveFloat::ECF_EaseOut;
	Action1.MontagePlayRate = 1.0f;

	FVector MiddlePos = PlayerPos + (TargetPos-PlayerPos)/2.f;
	MiddlePos.Z = TargetPos.Z;
	
	Action1.AddMovePoint(PlayerPos);
	Action1.AddMovePoint(MiddlePos);
	Action1.AddMovePoint(BackPos);
	Player->AddAction(Action1);

	FActionInfo Action2;
	//Action2.ActionTime = 0.7f;
	Action2.AnimMontage = OwnerWeapon->GetSkillMontage();
	Action2.SectionName = "QSkillAttack";
	Action2.bSweep = false;
	Action2.MontagePlayRate = 1.5f;
	Action2.OriginRotation = (TargetPos - BackPos).Rotation();

	Action2.AddMovePoint(BackPos);
	Player->AddAction(Action2);

	FActionInfo ActionBackStep;
	ActionBackStep.AnimMontage = OwnerWeapon->GetSkillMontage();
	ActionBackStep.SectionName = "QSkillBackStep";
	ActionBackStep.bSweep = false;
	ActionBackStep.MontagePlayRate = 0.5f;
	
	ActionBackStep.AddMovePoint(BackPos);
	ActionBackStep.AddMovePoint(BackStepPos);
	Player->AddAction(ActionBackStep);


	FActionInfo ActionDash;

	ActionDash.AnimMontage = OwnerWeapon->GetSkillMontage();
	ActionDash.SectionName = "QSkillDash2";
	ActionDash.bSweep = false;
	ActionDash.CurveType = ECurveFloat::ECF_EaseOut;
	ActionDash.MontagePlayRate = 1.0f;

	ActionDash.AddMovePoint(BackStepPos);
	ActionDash.AddMovePoint(TargetPos);
	Player->AddAction(ActionDash);

	FActionInfo Action3;
	//Action3.ActionTime = 0.7f;
	Action3.AnimMontage = OwnerWeapon->GetSkillMontage();
	Action3.SectionName = "QSkillAttack";
	Action3.bSweep = false;
	Action3.MontagePlayRate = 1.2f;
	Action3.OriginRotation = (BackPos - TargetPos).Rotation();

	Action3.AddMovePoint(TargetPos);
	Player->AddAction(Action3);


	//FActionInfo Action4;
	////Action4.ActionTime = 0.8f;
	//Action4.AnimMontage = OwnerWeapon->GetSkillMontage();
	//Action4.SectionName = "QSkillDash";
	//Action4.bSweep = false;
	//Action4.CurveType = ECurveFloat::ECF_EaseOut;
	//Action4.MontagePlayRate = 1.2f;

	//Action4.AddMovePoint(BackPos);
	//Action4.AddMovePoint(TargetPos);
	//Player->AddAction(Action4);

	/*FActionInfo Action4;
	Action4.ActionTime = 0.7f;
	Action4.AnimMontage = OwnerWeapon->GetSkillMontage();
	Action4.SectionName = "QSkillAttack";
	Action4.bSweep = false;
	Action4.MontagePlayRate = 1.5f;

	Action4.AddMovePoint(TargetPos + FVector(0.f, 0.f, -200.f));
	Player->AddAction(Action4);*/



	Player->PlayAction();

}
