
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "ScheduledCommand.generated.h"

/*
	When declare "Initialize" Func in child, essentially use parameter UWorld*  
 */
UCLASS()
class MYRPG_API UScheduledCommand : public UObject, public ICommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UObject* WorldContext) override;
	virtual void Reset() override;
	virtual EDamageActionCommandTag GetTag() { return EDamageActionCommandTag(); }
	virtual const bool IsInProgress() override;

protected:
	UWorld* CurWorld = nullptr;
	FTimerDelegate TimerDele;
	
	float Rate;
	float FirstDelay;
	bool bLoop;
	float Duration;

	UFUNCTION()
	void Clear();

private:
	FTimerHandle TimerHandle;
	FTimerHandle ClearTimerHandle;
	
	
};
