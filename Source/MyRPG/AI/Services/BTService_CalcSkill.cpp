
#include "BTService_CalcSkill.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CalcSkill::UBTService_CalcSkill()
{
	NodeName = "Decide SkillNum";
	Interval = 0.1f;
	RandomDeviation = 0.00f;
}

void UBTService_CalcSkill::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (Enemy && BlackBoard)
	{
		bool bCanSkill = BlackBoard->GetValueAsBool(FName("CanSkill"));
		if (bCanSkill == false)
		{
			BlackBoard->SetValueAsInt(FName("SkillNum"), INDEX_NONE);
			return;
		}

		int SkillNum = Enemy->RandomChoiceSkill();

		BlackBoard->SetValueAsInt(FName("SkillNum"), SkillNum);
	}
}
