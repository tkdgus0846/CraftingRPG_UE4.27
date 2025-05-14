
#include "BTDecorator_IsAllSkillInCooltime.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsAllSkillInCooltime::UBTDecorator_IsAllSkillInCooltime()
{
	NodeName = "Some Skill Off Cooldown";
}

bool UBTDecorator_IsAllSkillInCooltime::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bSuccess = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bSuccess) return false;

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (Enemy)
	{
		return !Enemy->IsAllSkillInCooltime();
	}
	return true;
}
