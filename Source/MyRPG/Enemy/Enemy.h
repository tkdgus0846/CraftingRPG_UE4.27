// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/BaseClasses/VFXCreatureCharacter.h"
#include "MyRPG/Data/EnemyDataTable.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AEnemy : public AVFXCreatureCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

	virtual void Damaged(struct FDamageInfo* DamageInfo) override;
	virtual void SetDead() override;

	virtual void GainHP(int HPAmount) override;

	UFUNCTION(BlueprintNativeEvent)
	void NormalAttack();

	UFUNCTION()
	void NormalAttack_Implementation();

	// �ִϸ��̼��� ������ �ð��� ���缭 �븻���� ��Ÿ���� ����������.
	UFUNCTION(BlueprintCallable)
	void SetNormalAttackTimer(const FName& SectionName);

	UFUNCTION(BlueprintNativeEvent)
	void OnAggro();

	UFUNCTION()
	void OnAggro_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAppear();

	UFUNCTION()
	void OnAppear_Implementation() {}

#pragma region LegacyNormalAttackNotify(NowChanged)
	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void NormalAttackCollisionEnable();

	UFUNCTION()
	void NormalAttackCollisionEnable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void NormalAttackCollisionDisable();

	UFUNCTION()
	void NormalAttackCollisionDisable_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void NormalAttackComboFinished();

	UFUNCTION()
	void NormalAttackComboFinished_Implementation();*/
#pragma endregion

	/* ��Ÿ������ �浹 ON/OFF */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeginNormalAttack(int AttackActorIndex);
	UFUNCTION()
	void BeginNormalAttack_Implementation(int AttackActorIndex);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndNormalAttack(int AttackActorIndex);
	UFUNCTION()
	void EndNormalAttack_Implementation(int AttackActorIndex);

	/* ��ų������ ���� Spawn/Destroy */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeginSkillAttack(int SkillActorIndex, float NewLifeSpan = -1.f, FName AttachSocketName = NAME_None);
	UFUNCTION()
	void BeginSkillAttack_Implementation(int SkillActorIndex, float NewLifeSpan = -1.f, FName AttachSocketName = NAME_None);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndSkillAttack(int SkillActorIndex);
	UFUNCTION()
	void EndSkillAttack_Implementation(int SkillActorIndex);

	UFUNCTION(BlueprintCallable)
	void PauseBehaviorTree();

	UFUNCTION(BlueprintCallable)
	void ResumeBehaviorTree();

	UFUNCTION(BlueprintCallable)
	void StopBehaviorTree(float StopDuration = -1.f);

	UFUNCTION(BlueprintCallable)
	void StartBehaviorTree();

	UFUNCTION(BlueprintCallable)
	void StateToWalk();

	UFUNCTION(BlueprintCallable)
	void StateToRun();

	void Ready();

	UFUNCTION(BlueprintCallable)
	void AddMovePointToAction(UPARAM(ref) FActionInfo& ActionInfo, const FVector& Location, ESplinePointType::Type PointType = ESplinePointType::Curve, bool bIsLocal = false);

	/*
		Skill ���õ� ������Ƽ��
	*/
	UFUNCTION(BlueprintCallable)
	virtual bool Skill(int SkillNum);

	UFUNCTION(BlueprintImplementableEvent)
	void Skill_FromBP(int SkillNum);

	UFUNCTION(BlueprintImplementableEvent)
	void IsSkillAvailable_FromBP(int SkillNum, bool& bRet);
	virtual bool IsSkillAvailable(int SkillNum);

	// ��ų�� ���õ� ������Ƽ��, ���� INDEX_NONE (-1) �̸� ������
	int RandomChoiceSkill();
	bool IsAllSkillInCooltime();
	// bResetArray if true -> OnConditionSkillArray Recalculate
	bool IsSomeSkillOnCondition();
	bool CanStopTaskToUseSkill();

	// Montages
	void StopAllMontages(float BlendOut);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AnimEventNotify(int AnimNum);
	virtual void AnimEventNotify_Implementation(int AnimNum);

protected:
	void CheckIsNormalAttacking();

	virtual void HitReact_Implementation() override;
	virtual const bool CanInactiveSuperArmor() const;

	UFUNCTION(BlueprintCallable)
	FName SkillSectionName(int SkillNum, FString AppendStr = "");

	UFUNCTION(BlueprintCallable)
	void ReserveSkillFinish(float DelayTime);

	/* ��ų�� ������ ȣ���ϸ� �Ǵ� �޼ҵ� */
	void IsSkillFinishedToTrue();
	void CanSkillToTrue();

	/* ȸ�� ���� */
	void PlaySkillMontageAfterInterpRotationForcibly(const FName& SectionName, float WaitTime, TFunction<void()> Func = []() {});

	FORCEINLINE void SetInterpRotationForcibly(bool bNewState) { bForceInterpRotation = bNewState; }
	FORCEINLINE void SetAutoInterpRotationToTarget(bool bNewState) { bAutoInterpRotationToTarget = bNewState; }

	/**/
	void PlaySkillMontage(const FName& SectionName, float WaitTime = 0.0001f);
	void ReserveMontage(class UAnimMontage* Montage, const FName& SectionName, float WaitTime = 0.0001f);

	/**/
	FORCEINLINE class UAnimMontage* GetSkillMontage() const { return SkillMontage; }
	FORCEINLINE class UAnimInstance* GetAnimInstance() const { return AnimInstance; }
	FORCEINLINE class TSubclassOf<class ADecalActor> GetDecalClass(int Index) const { return DecalClasses[Index]; }

	/* */
	class AAttackActor* GetSkillAttackActor(int Index); 
	void SetSkillAttackActor(int Index, class AAttackActor* SkillActor);
	TSubclassOf<class AAttackActor> GetSkillAttackClass(int Index);

private:
	/* ������ ������ ���̺�� �ű⼭ �ҷ��� �����͵� */
	UPROPERTY(EditDefaultsOnly, Category = Data)
	FName FindEnemyDataRowName;
	UPROPERTY(EditDefaultsOnly, Category = Data)
	class UDataTable* EnemyDataTable;

	void BringDataTableDatas();

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FName EnemyName;

	UPROPERTY()
	TArray<FDropItemInfo> DropItems;
	void DropItem();

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UAnimInstance* AnimInstance = nullptr;

	/* ��Ÿ ���� Ŭ������ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<class AAttackActor>> NormalAttackClasses;
	/* ��ų ���� Ŭ������ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<class AAttackActor>> SkillAttackClasses;
	/* ���ؼ� ����� ��Į Ŭ������ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<class ADecalActor>> DecalClasses;

	/* Enemy �⺻ ���� ��Ÿ�ֵ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* NormalAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* OnAggroMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* SkillMontage;

	UPROPERTY()
	TArray<class AAttackActor*> NormalAttackActors;
	UPROPERTY()
	TArray<class AAttackActor*> SkillAttackActors;

	/* Ÿ���� �����ϴ� Sphere */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* TargetDetectionSphere;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* NormalAttackDetectionSphere;

	UFUNCTION()
	void TargetDetectionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TargetDetectionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void NormalAttackDetectionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void NormalAttackDetectionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTimerHandle DamagedStopBehaviorTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float HitReactCoolTime = 3.f;

	FTimerHandle HitReactCoolTimeTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float NormalAttackCoolTime = 2.f;

	FTimerHandle NormalAttackCoolTimeTimer;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bNormalAttackEnable = true;

	UFUNCTION()
	void NormalAttackTimer();

	UFUNCTION()
	void EnableNormalAttack();
	int NormalAttackSectionNum();

	void AggroAnimationFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float WalkSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float RunSpeed = 400.f;

	/* ȸ������ �����ϴ°� */
	virtual bool CanInterpRotation() override;
	bool bForceInterpRotation = false;
	bool bAutoInterpRotationToTarget = false;
	void AutoInterpRotationToTarget();
	FTimerHandle  InterpRotationForciblyTimer;

	/* ��ų ��Ÿ�� ���� */
	FTimerHandle PlaySkillMontageTimer;

	/* ��ų�� ���� ������ */
	UPROPERTY(EditDefaultsOnly)
	TArray<float> SkillCoolTimeArray;
	TArray<bool> OnCooldownFinishedArray;
	TArray<FTimerHandle> SkillCoolTimeTimerArray;

	/* Set To None, No Weight */
	UPROPERTY(EditDefaultsOnly)
	TArray<float> SkillWeight;
	
	
	bool bSkillInProgress = false;
	//int CurrentSkillNum;
	UFUNCTION()
	void SkillCoolTimeFinished(int SkillNum);
	
	int SkillSectionNum();

	UPROPERTY(EditDefaultsOnly)
	float CanSkillInterval = 10.f;

	int CurrentNormalAttackIndex = -1;

	/*
		HitReact�� �����ʴ� ��찡 �������̴�. (Boss�������)	
	*/
	bool bCanHitReact = true;

public:
	UFUNCTION(BlueprintCallable)
	void SetValueAsBool(FName KeyName, bool bState);

	UFUNCTION(BlueprintCallable)
	void SetValueAsObject(FName KeyName, UObject* Object);

	UFUNCTION(BlueprintCallable)
	void SetValueAsInt(FName KeyName, int Num);

	UFUNCTION(BlueprintCallable)
	bool GetValueAsBool(FName KeyName);

	UFUNCTION(BlueprintCallable)
	UObject* GetValueAsObject(FName KeyName);

	UFUNCTION(BlueprintCallable)
	int GetValueAsInt(FName KeyName);

	UFUNCTION(BlueprintCallable)
	float GetMaxWalkSpeed() const;

	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const { return RunSpeed; }
	FORCEINLINE void SetCanHitReact(bool bNewCanHitReact) { bCanHitReact = bNewCanHitReact; }
	FORCEINLINE const float GetCanSkillInterval() const { return CanSkillInterval; }

	const FVector GetCurrentTargetLocation();
};
