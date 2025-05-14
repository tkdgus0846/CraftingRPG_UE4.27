#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Sword.generated.h"

UCLASS()
class MYRPG_API ASword : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual bool NormalAttackPressed() override;
	virtual bool NormalAttackPressing(float DeltaTime) override;
	virtual bool NormalAttackReleased() override;

	virtual void QSkillPressed() override;
	virtual void QSkillPressing(float DeltaTime) override;
	virtual void QSkillReleased() override;

	virtual void ESkillPressed() override;
	virtual void ESkillPressing(float DeltaTime) override;
	virtual void ESkillReleased() override;

protected:

private:
	FVector QSkillSize = { 400.f,80.f,30.f };

	void SpawnQSkillActor_Timer();

	const float QSkillSpawnLatent = 0.85f;
	const float ESkillWalkSpeed = 200.f;

public:
	FORCEINLINE const float GetESkillWalkSpeed() const { return ESkillWalkSpeed; }
};
