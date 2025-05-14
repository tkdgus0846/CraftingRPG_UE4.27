
#pragma once

#include "CoreMinimal.h"
#include "../ScheduledCommand.h"
#include "TickDamageCommand.generated.h"

class ICreature;

UCLASS()
class MYRPG_API UTickDamageCommand : public UScheduledCommand
{
	GENERATED_BODY()
	
public:
	virtual EDamageActionCommandTag GetTag() override;
	virtual void Reset() override;

	void Initialize(UWorld* World, ICreature* InCreature, float InRate, float InDuration, int InDamage);

private:
	UFUNCTION()
	void TickDamage();

	int Damage;

	class ICreature* Creature = nullptr;
	UPROPERTY()
	class ACreatureCharacter* Character = nullptr;
	bool bIsEnemy;
};
