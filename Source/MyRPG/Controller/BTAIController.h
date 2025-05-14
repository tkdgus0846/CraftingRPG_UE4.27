
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BTAIController.generated.h"

UCLASS()
class MYRPG_API ABTAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABTAIController();
	virtual void OnPossess(APawn* InPawn) override;

	void RunAI();
	void StopAI();

protected:

private:
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* BTAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UBlackboardData* BBAsset = nullptr;

public:
	class UBehaviorTree* GetBTAsset() { return BTAsset; }
	
};
