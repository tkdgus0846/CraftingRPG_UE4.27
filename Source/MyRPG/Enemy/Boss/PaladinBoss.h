// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Enemy.h"
#include "PaladinBoss.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API APaladinBoss : public AEnemy
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Damaged(struct FDamageInfo* DamageInfo) override;
	virtual void SetDead() override;

	virtual bool IsSkillAvailable(int SkillNum) override;
	virtual bool Skill(int SkillNum) override;
	virtual void ActionNotify_Implementation(int ActionNum, float Duration) override;
	virtual void AnimEventNotify_Implementation(int AnimNum) override;

protected:
	// for debug
	UPROPERTY(EditInstanceOnly)
	bool bOnlyUseOneSkill = false;
	UPROPERTY(EditInstanceOnly, Meta = (EditCondition = "bOnlyUseOneSkill == true", EditConditionHides))
	int OnlySkillNum = 0;

private:
	int Phase = 1;
	const float Phase2HPPercent = 0.5f;

	void WalkToCenter(TFunction<void()> Func);
	FTimerHandle WalkToCenterTimer;
	const FVector CenterPos = { -41839.367188f, 65.956238f, 9045.164062f };

	/*
		Phase2 의 홀리엔젤 소환과 관련된 프로퍼티들
	*/
	void SpawnAngel(float DelayTime);
	bool bSpawnAngelFinished = false;
	FTimerHandle SpawnAngelTimer;
	FVector SpawnAngelPos = FVector::ZeroVector;
	const float SpawnAngelDistance = 400.f;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* SpawnAngelMontage;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHolyAngel> AngelClass;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* SpawnAngelPredictFX;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TArray<USceneComponent*> AngelPoints;
	UPROPERTY()
	TWeakObjectPtr<class AHolyAngel> Angel;

	UPROPERTY(EditInstanceOnly)
	TWeakObjectPtr<class ABGMTriggerVolume> BGMTrigger;
	

#pragma region Skill1
	/* Skill 1 */
	bool CanDownThrust();
	void DownThrust();
	FTimerHandle SpawnDownThrustAttackTimer;
	UFUNCTION()
	void SpawnDownThrustAttack(float Angle);
	const float SpawnDownThurstAttackRate = 0.1f;
	const float SpawnDownThurstAttackRadius = 200.f;
	const float SpawnDownThurstAttackMiddleDistance = 500.f;
	const float SpawnDownThurstAttackMiddle2ForwardDistance = 300.f;
	const float SpawnDownThurstAttackMiddle2SideDistance = 250.f;
	const float SpawnDownThurstAttackEndDistance = 2500.f;
	const float SpawnDownThurstAttackInitAngle = 15.f;
	const float SpawnDownThrustAttackActionTime = 2.f;
	const float SpawnDownThurstAttackAngleInterval = 5.f;
#pragma endregion

#pragma region Skill2
	/* Skill 2 */
	bool CanDashChargeAttack();
	void DashChargeAttack();
	void ActionDashCharge(float Duration);
	void ActionDashChargeDash(float Duration);

	const float DashChargeWaitTime = 0.5f;
	FVector DashChargeStepBackLocation;

	FTimerHandle SpawnDashChargeAttackTimer;
	UFUNCTION()
	void SpawnDashChargeAttack(float TotalTime);
	const float SpawnDashChargeAttackRate = 0.15f;
	const float SpawnDashChargeAttackActionTime = 3.f;
	const float SpawnDashChargeAttackSideInitDistance = 50.f;
	const float SpawnDashChargeAttackSideTargetDistance = 2000.f;

	float SpawnDashChargeAttackDuration;
#pragma endregion

#pragma region Skill3
	/* Skill 3 */
	bool CanJumpAttack();
	void JumpAttack();
	void ActionJump(float Duration);

	const float JumpAttackWaitTime = 0.5f;
	FVector DownAttackLocation;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* DownAttackPredictFX;
#pragma endregion

#pragma region Skill4
	/* Skill 4 */
	bool CanSideShock();
	void SideShock();
	
	UFUNCTION()
	void SpawnSideShockAttack(float Angle);

	FVector SpawnSideShockAttackPoint[2];
	const float SpawnSideShockAttackRate = 0.2f;
	FTimerHandle SpawnSideShockAttackTimer;
	const float SpawnSideShockAttackAngleInterval = 15.f;
	const float SpawnSideShockAttackActionTime = 1.8f;
	const float SpawnSideShockAttackRadius = 100.f;
	const float SpawnSideShockAttackActionDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* SideShockPredictFX;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* SideShockSpawnFX;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* SideShockSpawnSound;

#pragma endregion
	
#pragma region Skill5
	/* Skill 5 */
	bool CanVerticalSlash();
	void VerticalSlash();
	void SpawnVerticalSlash();
	UFUNCTION()
	void SpawnSecondVerticalSlash();

	const float SpawnVerticalSlashAttackAngle = 15.f;
	const float SpawnVerticalSlashAttackStartDistance = 200.f;
	const float SpawnVerticalSlashAttackDistance = 1500.f;
	const float SpawnVerticalSlashAttackMiddleDistance = SpawnVerticalSlashAttackDistance * 0.6666f;
	const float SpawnVerticalSlashAttackActionTime = 1.3f;

	FVector VerticalSlashAttackFinishPoint[2]; // 0: 오른쪽 1: 왼쪽
	TWeakObjectPtr<class AActionActor> VerticalSlashAttackArray[2];

	FTimerHandle SpawnSecondVerticalSlashAttackTimer;
	
	const float SpawnSecondVerticalSlashAttackSideFarDistance = 500.f;
	const float SpawnSecondVerticalSlashAttackSideNearDistance = 150.f;
	const float SpawnSecondVerticalSlashAttackDistance = 2000.f;
	const float SpawnSecondVerticalSlashAttackActionTime = 2.6f;
	FVector VerticalSlashAttackMiddlePoint;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* VerticalSlashEndFX;
#pragma endregion

#pragma region Skill6
	/* Skill 6 */
	bool CanKneel();
	void Kneel();

	UFUNCTION()
	void SpawnKneelAttack(int Count);

	FTimerHandle SpawnKneelAttackTimer;
	const float SpawnKneelAttackFirstDelay = 2.f;
	const float SpawnKneelAttackSeconds = 8.f;
	const float SpawnKneelAttackDistance = 2700.f;
	const float SpawnKneelAttackInterval = 3.f;


#pragma endregion

#pragma region Skill7
	/* Skill 7 */
	bool CanChargeSwing();
	void ChargeSwing();
	UFUNCTION()
	void SpawnChargeSwingAttack();
	UFUNCTION()
	void PlayChargeSwingAttackMontage();

	FTimerHandle SpawnChargeSwingAttackTimer;
	const float SpawnChargeSwingAttackDistance = 1500.f;
	const float SpawnChargeSwingAttackDuration = 2.f;
#pragma endregion
};
