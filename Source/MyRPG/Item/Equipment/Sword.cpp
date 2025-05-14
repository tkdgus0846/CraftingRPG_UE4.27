
#include "Sword.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include "MyRPG/Player/Components/MouseComponent.h"
#include <MyRPG/GameInstance/MyGameInstance.h>

void ASword::BeginPlay()
{
	Super::BeginPlay();

	SetMaxCombo(2);
	SetCanMoveWhenAttack(true);
	SetAttackWalkSpeed(300.f);
}

bool ASword::NormalAttackPressed()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	bool bBegin = BeginCombo();
	if (bBegin == false) return false;

	GetPlayerAnimInstance()->Montage_Play(NormalAttackMontage);
	UE_LOG(LogTemp, Warning, TEXT("Pressed Play"));
	float Size = GetPlayer()->GetVelocity().Size();

	switch (GetCurrentCombo())
	{
	case 0:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo1", NormalAttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("StandingCombo1 Pressed"));
		break;
	case 1:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo2", NormalAttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("StandingCombo2 Pressed"));
		break;
	case 2:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo3", NormalAttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("StandingCombo3 Pressed"));
		break;

	}


	EndCombo();
	return true;
}

bool ASword::NormalAttackPressing(float DeltaTime)
{
	if (Super::NormalAttackPressed() == false)
		return false;

	bool bBegin = BeginCombo();
	if (bBegin == false) return false;

	GetPlayerAnimInstance()->Montage_Play(NormalAttackMontage);
	UE_LOG(LogTemp, Warning, TEXT("Pressing Play"));
	float Size = GetPlayer()->GetVelocity().Size();

	switch (GetCurrentCombo())
	{
	case 0:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo1", NormalAttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("StandingCombo1 Pressing"));
		break;
	case 1:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo2", NormalAttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("StandingCombo2 Pressing"));

		break;
	case 2:
		GetPlayerAnimInstance()->Montage_JumpToSection("StandingCombo3", NormalAttackMontage);
		UE_LOG(LogTemp, Warning, TEXT("StandingCombo3 Pressing"));

		break;

	}


	EndCombo();
	return true;
}

bool ASword::NormalAttackReleased()
{
	if (Super::NormalAttackPressed() == false)
		return false;

	return true;
}

void ASword::QSkillPressed()
{
	APlayerCharacter* Player = GetPlayer();

	if (Player == nullptr || Player->GetMouseComponent() == nullptr) return;

	Player->GetMouseComponent()->TurnOnAimPreview(EAimPreview::EAP_Line, QSkillSize, QSkillSize.X);
}

void ASword::QSkillPressing(float DeltaTime)
{
}

void ASword::QSkillReleased()
{
	if (GetWorld() == nullptr) return;
	APlayerCharacter* Player = GetPlayer();

	if (Player == nullptr || Player->GetMouseComponent() == nullptr) return;

	Player->GetMouseComponent()->TurnOffAimPreview();
	Player->GetMouseComponent()->RotateByAimPreview();

	
	if (SkillMontage)
	{
		FActionInfo ActionQ;

		ActionQ.AnimMontage = SkillMontage;
		ActionQ.SectionName = "QSkill";
		ActionQ.bSweep = true;
		ActionQ.AddMovePoint(Player->GetActorLocation());

		Player->AddAction(ActionQ);
		Player->PlayAction();
	}

	

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASword::SpawnQSkillActor_Timer, 0.0001f, false, QSkillSpawnLatent);
	
	if (Player->GetCombatComponent())
	{
		Player->GetCombatComponent()->OnQSkill.Broadcast();
	}
}

void ASword::ESkillPressed()
{
	if (SkillMontage)
	{
		GetPlayerAnimInstance()->Montage_Play(SkillMontage);
		GetPlayerAnimInstance()->Montage_JumpToSection("ESkill", SkillMontage);

		APlayerCharacter* Player = GetPlayer();
		if (Player == nullptr) return;

		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = Player;

		GetWorld()->SpawnActor<AAttackActor>(ESkillActor, SpawnParam);
		Player->SetMaxWalkSpeed(GetESkillWalkSpeed());

		if (Player->GetCombatComponent())
		{
			Player->GetCombatComponent()->OnESkill.Broadcast();
		}
	}
}

void ASword::ESkillPressing(float DeltaTime)
{
}

void ASword::ESkillReleased()
{
	//GetGameInstance<UMyGameInstance>()->SlowMotion(ECurveFloat::ECF_TestSlowMotion, 1.f);
}

void ASword::SpawnQSkillActor_Timer()
{
	APlayerCharacter* Player = GetPlayer();
	if (Player == nullptr) return;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;

	GetWorld()->SpawnActor<AAttackActor>(QSkillActor, Player->GetActorLocation() + Player->GetActorForwardVector() * 10.f, Player->GetActorForwardVector().Rotation(), SpawnParam);
}
