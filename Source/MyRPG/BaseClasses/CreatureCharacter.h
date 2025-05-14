
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyRPG/Interfaces/Creature.h"
#include "MyRPG/BaseClasses/Components/ActionComponent.h"
#include <MyRPG/Data/ItemDataTable.h>
#include "CreatureCharacter.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInterpRotationFinishedDelegate);
enum class EDamageActionCommandTag : uint8;

USTRUCT(BlueprintType)
struct FFootStepResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> Surface;

	UPROPERTY(BlueprintReadOnly)
	FVector SpawnLocation;

	UPROPERTY(BlueprintReadOnly)
	bool bSucceed;
};

USTRUCT()
struct FIKFootTraceResult
{
	GENERATED_BODY()

	FIKFootTraceResult() {}
	FIKFootTraceResult(float InFootZOffset, const FVector& InHitLocation) : 
		FootZOffset(InFootZOffset),
		HitLocation(InHitLocation)
	{}

	float FootZOffset = 0.f;
	FVector HitLocation = FVector::ZeroVector;
};

UCLASS()
class MYRPG_API ACreatureCharacter : public ACharacter, public ICreature
{
	GENERATED_BODY()

public:
	ACreatureCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Damaged(struct FDamageInfo* DamageInfo) override;
	virtual FDamagedDelegate& GetDamagedDelegate() final;
	virtual FDeadDelegate& GetDeadDelegate() final;

	void SpawnDamageUI(int Damage, bool bIsEnemy);

	UFUNCTION(BlueprintCallable)
	bool GetDead() override;

	UFUNCTION(BlueprintCallable)
	int GetHP() const override;

	UFUNCTION(BlueprintCallable)
	void GainHP(int HPAmount) override;

	UFUNCTION(BlueprintCallable)
	void SetMaxHP(int NewMaxHP, bool bHPUpdate) override;

	UFUNCTION(BlueprintCallable)
	int GetMaxHP() const override;

	UFUNCTION(BlueprintCallable)
	void SetHP(int NewHP) final;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDamagedDelegate OnDamaged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDeadDelegate OnDead;

	UFUNCTION(BlueprintCallable)
	FFootStepResult FootStep(const FName& BoneName);

	// bForceAdd가 true이면 진행하던 액션을 지워버리고 새로 실행 Ret: Action Total Length
	UFUNCTION(BlueprintCallable)
	float AddAction(const FActionInfo& ActionInfoParam, bool bForceAdd = false);
	UFUNCTION(BlueprintCallable)
	void PlayAction();
	void PauseAction();
	void ResumeAction();
	// 액션을 맨처음 인덱스로 되돌린다.
	void ResetAction();
	// 액션 리스트들을 아예 비워버린다
	void ClearAction();

	UFUNCTION(BlueprintPure)
	const bool IsActionPlaying();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionNotify(int ActionNum, float Duration);
	UFUNCTION()
	virtual void ActionNotify_Implementation(int ActionNum, float Duration) {}

	UFUNCTION(BlueprintCallable)
	void ShowHpBar(bool bState);
	UFUNCTION(BlueprintCallable)
	void HideHpBar();
	UFUNCTION(BlueprintCallable)
	void SetHPBarZOffset(float NewZOffset);
	UFUNCTION(BlueprintCallable)
	void SetHPBarZOffsetToOrigin();
	UFUNCTION(BlueprintCallable)
	float GetOriginHpBarZOffset() const;

	/*
			회전 보간 관련 메소드들
	*/
	void SetInterpRotationTarget(const FVector& Location);
	void SetInterpRotationTarget(const FRotator& Rotation);
	// 현재 Rotation을 InterpRotation값으로 맞춰주기 (스킬쓰고나서 InterpRotation 계산안되는것때문에)
	void SetRotationCurrentToInterp();

	/*
			DamageActionCommand를 무시하는 비트플래그 설정
	*/
	UFUNCTION()
	void SetIgnoreCommandState(EDamageActionCommandTag IgnoreCommand, bool bState);
	UFUNCTION()
	void SetAllIgnoreCommandState(bool bState);

	///*
	//	슈퍼 아머를 일정 초만큼
	//*/
	//UFUNCTION(BlueprintCallable)
	//void ActiveSuperArmorForDuration(float Duration);

	void SetIgnoreDamage(bool bNewState);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void SetDead() override;

	UFUNCTION()
	void SetDead_Implementation();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* DeathMontage;

	void ActiveSuperArmor();
	virtual const bool CanActiveSuperArmor() const { return true; }
	void InactiveSuperArmor();
	virtual const bool CanInactiveSuperArmor() const { return true; }
	const bool IsActivedSuperArmor() const { return bSuperArmored; }

	void AutoDestroy() { bAutoDestroy = true; }
	void PreventFallLedge(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UActionComponent* ActionComponent;

	FORCEINLINE void SetLevel(int NewLevel) { Level = NewLevel; }

	UFUNCTION(BlueprintNativeEvent)
	void HitReact();
	UFUNCTION()
	virtual void HitReact_Implementation() {}

	/*
		Rotation Interp Methods
	*/
	virtual bool CanInterpRotation();

	/*UPROPERTY()
	FInterpRotationFinishedDelegate InterpRotationFinishedDele;*/

	/*void BindInterpRotationFinished(void (*Func)());*/

	const TSubclassOf<class UFloatingTextWidget> GetFloatingTextWidgetClass() const { return FloatingTextWidgetClass; }

	/*
		Foot IK. Ret: Effector의 Z오프셋
	*/
	FIKFootTraceResult IKFootTrace(const FName& SocketName, float Distance, bool bDebugLine = false);

	/*
		IK Properties
	*/
	float IKHipOffset = 0.f;

	/*
		몇초동안 HpBar를 보이게 했다가 사라지게
	*/
	void ReserveShowHPBar(float Duration);

private:
	UPROPERTY(EditAnywhere)
	int HP;
	UPROPERTY(EditAnywhere)
	int MaxHP;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	float DestroyTime;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	bool bAutoDestroy = true;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bSuperArmored;
	
	bool bDead = false;

	/*
		Level HP 위젯 관련 프로퍼티
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int Level;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UWidgetComponent* LevelHpStateComponent;
	UPROPERTY(EditDefaultsOnly)
	float HpBarZOffset = 40.f;

	/*
		Damage UI나 Skill Cooltime UI같이 잠깐 띄웠다가 사라지는 위젯의 프로퍼티
	*/
	UPROPERTY()
	TSubclassOf<class UFloatingTextWidget> FloatingTextWidgetClass;

	UFUNCTION()
	void CreatureCharacterOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CreatureCharacterOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<ACreatureCharacter*> OverlappingCreatures;

	void CreatureCharacterPushEachOther(float DeltaTime);
	void RemoveDeadCreatures();

	/*
			회전 보간 변수
	*/
	bool bInterpRotation = false;
	FRotator TargetInterpRotation;
	FRotator InterpingRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float InterpRotationSpeed = 6.f;

	void InterpRotationToTarget(float DeltaTime);

	/*
		DamageActionCommand 별로 무시할지 안할지를 체크하는 비트플래그
	*/
	int DamageActionCommandIgnoreBit = 0;

	/*
		HPBar를 사용할지안할지...
	*/
	bool bIsUseHpBar = true;


	// 체력바 관련 변수
	FTimerHandle ShowHpBarTimer;
	FTimerDelegate ShowHpBarTimerDele;

public:
	class UAnimMontage* GetHitReactMontage() { return HitReactMontage; }
	FORCEINLINE const int GetLevel() const { return Level; }

	FORCEINLINE const float GetIKHipOffset() const { return IKHipOffset; }
	FORCEINLINE const bool GetIsUseHPBar() const { return bIsUseHpBar; }
	FORCEINLINE void SetIsUseHPBar(bool bInState) { bIsUseHpBar = bInState; }
	FORCEINLINE class UWidgetComponent* GetLevelHPStateComponent() { return LevelHpStateComponent; }
	
};
