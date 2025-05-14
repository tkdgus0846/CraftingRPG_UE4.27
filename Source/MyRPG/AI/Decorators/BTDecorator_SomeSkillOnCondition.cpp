
#include "BTDecorator_SomeSkillOnCondition.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_SomeSkillOnCondition::UBTDecorator_SomeSkillOnCondition()
{
	NodeName = "Some Skill On Condition";
}

bool UBTDecorator_SomeSkillOnCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bResult == false)
		return bResult;

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (Enemy)
	{
		bResult = Enemy->IsSomeSkillOnCondition();
		// UE_LOG(LogTemp, Warning, TEXT("Skill: %d"), bResult);
	}
	return bResult;
}
