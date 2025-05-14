#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MyRPG/Types/DamageActionCommandTag.h"
#include "Command.generated.h"

UINTERFACE(MinimalAPI)
class UCommand : public UInterface
{
	GENERATED_BODY()
};

class MYRPG_API ICommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UObject* WorldContext) = 0;
	virtual void Reset() = 0;
	virtual EDamageActionCommandTag GetTag() = 0;
	virtual const uint32 GetPriority() const { return 0; }
	virtual const bool IsInProgress() { return false; }
};
