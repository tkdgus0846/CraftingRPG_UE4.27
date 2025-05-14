
#pragma once

#include "CoreMinimal.h"
#include "StateEffect.h"
#include "IceStateEffect.generated.h"


UCLASS()
class MYRPG_API AIceStateEffect : public AStateEffect
{
	GENERATED_BODY()

private:
	virtual void StartState() override;	
};
