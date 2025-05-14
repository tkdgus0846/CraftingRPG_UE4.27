
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/BaseClasses/FadeOutActor.h"
#include "MyRPG/Interfaces/Creature.h"
#include <MyRPG/Data/ItemDataTable.h>
#include "CreatureActor.generated.h"

UCLASS()
class MYRPG_API ACreatureActor : public AFadeOutActor, public ICreature
{
	GENERATED_BODY()
	
public:	
	ACreatureActor();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Damaged(struct FDamageInfo* DamageInfo) override;
	virtual FDamagedDelegate& GetDamagedDelegate() final;
	virtual FDeadDelegate& GetDeadDelegate() final;

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

	/* �������� �޾����� Broadcast.. */
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable)
	FDamagedDelegate OnDamaged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDeadDelegate OnDead;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void SetDead() override;

	UFUNCTION()
	void SetDead_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void OnDamage();

	UFUNCTION()
	void OnDamage_Implementation();

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditDefaultsOnly)
	int HP;
	UPROPERTY(EditDefaultsOnly)
	int MaxHP;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	float DestroyTime;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	bool bAutoDestroy;

	bool bDead = false;

	void WhitenessInterping();

	bool bIsDamaged = false;
	
	UPROPERTY(EditDefaultsOnly)
	float WhitenessDownSpeed = 25.f;
	UPROPERTY(EditDefaultsOnly)
	float WhitenessUpSpeed = 20.f;
	UPROPERTY(EditDefaultsOnly)
	float MaxWhiteness = 0.4f;

	float InterpWhiteness = 0.f;
	float TargetWhiteness = 0.f;

	FTimerHandle InterpingWhitenessTimer;

	/* ��Ḧ �����Ҷ� ������ ������ ����. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<class UWidgetComponent*> GatherWidgetComponent;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	TArray<int> StackGatherPoints = { 0,0 };

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	int CurWidgetIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int32 GatherPointAvg = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	int32 GatherPointTolerance = 2;

	/* ������ �����ų �� */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Drop, Meta = (AllowPrivateAccess = true))
	TArray<FDropItemInfo> DropItems;

	UPROPERTY(EditDefaultsOnly, Category = Drop)
	FName FindDropItemName;

	UPROPERTY(EditDefaultsOnly, Category = Drop)
	class UDataTable* DropItemDataTable;

	void BringDataTableDatas();

#pragma region FX
	/* ���̾ư��� VFX ���õ� ������.. */
	UPROPERTY(EditDefaultsOnly, Category = FX)
	class UNiagaraSystem* HitVFX;
	UPROPERTY(EditDefaultsOnly, Category = FX)
	class UNiagaraSystem* DeathVFX;

	/* SFX ���õ� ������.. */
	UPROPERTY(EditDefaultsOnly, Category = FX)
	class USoundCue* HitSFX;
	UPROPERTY(EditDefaultsOnly, Category = FX)
	class USoundCue* DeathSFX;

	UPROPERTY(EditDefaultsOnly, Category = FX)
	FVector HitFXOffset{ 0.f,0.f,0.f };
	UPROPERTY(EditDefaultsOnly, Category = FX)
	FVector DeathFXOffset{ 0.f,0.f,0.f };

	/* FX Spawn �Լ��� */
	void SpawnHitFX();
	void SpawnDeathFX();
#pragma endregion

	/* �������� ���� ĳ���͸� �����ؼ� ó���ϱ�����. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	AActor* LastHitCharacter = nullptr;
};
