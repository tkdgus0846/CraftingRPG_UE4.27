
#pragma once

#include "CoreMinimal.h"
#include "../ScheduledCommand.h"
#include "TickSpawnImpactEffectCommand.generated.h"

class ICreature;

UCLASS()
class MYRPG_API UTickSpawnImpactEffectCommand : public UScheduledCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UObject* WorldContext) override;
	virtual EDamageActionCommandTag GetTag() override;
	virtual void Reset() override;

	void Initialize(UWorld* World, UClass* Class, AActor* InActor, float InRate, float InDuration, bool bInLoop);

private:
	UFUNCTION()
	void TickSpawnImpactEffect();

	class ICreature* Creature = nullptr;
	UPROPERTY()
	AActor* Actor = nullptr;
	
	UPROPERTY()
	UClass* ImpactEffectClass = nullptr;
	
};
