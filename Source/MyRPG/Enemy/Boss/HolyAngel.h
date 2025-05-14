
#pragma once

#include "CoreMinimal.h"
#include "../Enemy.h"
#include "HolyAngel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectileAttackDelegate);

UCLASS()
class MYRPG_API AHolyAngel : public AEnemy
{
	GENERATED_BODY()

public:
	AHolyAngel();

	void SetAngelPoints(TArray<USceneComponent*>& InPoints);
	virtual void BeginPlay() override;
	virtual void SetDead() override;

	virtual bool IsSkillAvailable(int SkillNum) override;
	virtual bool Skill(int SkillNum) override;
	virtual void ActionNotify_Implementation(int ActionNum, float Duration) override;
	virtual void AnimEventNotify_Implementation(int AnimNum) override;

	/* Ret = 팔라딘이 공격을 시작하는 시점, 팔라딘에 신호를 보낼때까지 팔라딘은 기다리게 하기 */
	FProjectileAttackDelegate& PlayProjectileAttack();

protected:

private:
	UPROPERTY()
	TWeakObjectPtr<class APaladinBoss> Paladin;

	FProjectileAttackDelegate ProjectileAttackDelegate;
	FTimerHandle ProjectileAttackTimer;
	bool bCanProjectileAttack = false;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAttackActor> ProjectileAttackClass;
	UFUNCTION()
	void SpawnProjectileAttack();

	const FVector CenterPos = { -41839.367188f, 65.956238f, 9045.164062f };

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* SpawnMontage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* SpawnProjectileMontage;

	void FadeOut(float FadeOutTime, float Delay = -1.f);
	void FadeIn(float FadeInTime, float Delay = -1.f);

	FTimerHandle FadeTimer;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* FadeFXMaterial;
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* FadeWingsFXMaterial;

	UPROPERTY()
	TArray<TWeakObjectPtr<USceneComponent>> AngelPoints;
	
	FVector GetRandomAngelPoint();
	FVector GetToPlayerDirection(const FVector& PointLocation, bool bOnlyXY = true);
	const FVector RandomPosInArena();

#pragma region Skill1
	void Dash();
	void DashAction(float Duration);
	const float DashDistance = 1500.f;
	const float DashSpawnRadius = 500.f;
#pragma endregion

#pragma region Skill2
	void Meteor();
	void SpawnMeteorAttack();
	UFUNCTION()
	void SpawnMeteorAttackTimerFunc(int Count);
	void MeteorAction(float Duration);

	const int SpawnMeteorNum = 30;
	FTimerHandle SpawnMeteorAttackTimer;
	FTimerHandle ShowMeteorAttackVFXTimer;
	TArray<FTimerHandle> MeteorAttackEachTimer;

#pragma endregion

public:
	void SetPaladin(class APaladinBoss* InPaladin);
	FORCEINLINE TSubclassOf<class AAttackActor> GetProjectileAttackActorClass() { return ProjectileAttackClass; }

};
