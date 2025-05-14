#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Creature.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDamagedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeadDelegate);

UINTERFACE(MinimalAPI)
class UCreature : public UInterface
{
	GENERATED_BODY()
};

class MYRPG_API ICreature
{
	GENERATED_BODY()

public:
	virtual void Damaged(struct FDamageInfo* DamageInfo) = 0;
	virtual bool GetDead() = 0;
	virtual void SetDead() = 0;

	virtual int GetHP() const = 0;
	virtual void GainHP(int HPAmount) = 0;

	virtual void SetMaxHP(int NewMaxHP, bool bHPUpdate = false) = 0;
	virtual int GetMaxHP() const = 0;

	virtual void SetHP(int NewHP) = 0;

	virtual FDamagedDelegate& GetDamagedDelegate() = 0;
	virtual FDeadDelegate& GetDeadDelegate() = 0;
};
