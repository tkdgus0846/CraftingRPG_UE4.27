
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "MyRPG/Types/StateMaterialType.h"
#include "ChangeStateCommand.generated.h"

UCLASS()
class MYRPG_API UChangeStateCommand : public UObject, public ICommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UObject* WorldContext) override;
	virtual void Reset() override;
	virtual EDamageActionCommandTag GetTag() override;
	virtual const uint32 GetPriority() const override { return 0; }

	void Initialize(class ICreature* InCreature, EStateMaterialType InState, float InDuration);

private:
	UPROPERTY()
	class AVFXCreatureCharacter* Creature = nullptr;
	EStateMaterialType State;
	float Duration;
};
