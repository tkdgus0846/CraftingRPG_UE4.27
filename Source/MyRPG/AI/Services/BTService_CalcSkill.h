
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CalcSkill.generated.h"

/* �ϴ� �� ���� �������. Skill Task���� �� ������ ó���ϰ�����. */
UCLASS()
class MYRPG_API UBTService_CalcSkill : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CalcSkill();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
