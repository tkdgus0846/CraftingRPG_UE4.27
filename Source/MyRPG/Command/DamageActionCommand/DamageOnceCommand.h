
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "DamageOnceCommand.generated.h"


UCLASS()
class MYRPG_API UDamageOnceCommand : public UObject, public ICommand
{
	GENERATED_BODY()
	
public:
	virtual void Execute(UObject* WorldContext) override;
	virtual void Reset() override;
	virtual EDamageActionCommandTag GetTag() override;
	virtual const uint32 GetPriority() const override { return 0; }

	void Initialize(class ICreature* NewCreature, int NewDamage);

private:
	int Damage;

	class ICreature* Creature = nullptr;
	UPROPERTY()
	class ACreatureCharacter* Character = nullptr;
	bool bIsEnemy;
	
};
