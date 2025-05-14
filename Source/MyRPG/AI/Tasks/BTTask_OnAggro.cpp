
#include "BTTask_OnAggro.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_OnAggro::UBTTask_OnAggro()
{
	NodeName = TEXT("OnAggro");
}

EBTNodeResult::Type UBTTask_OnAggro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (Enemy && BlackBoard)
	{
		auto Player = Cast<AActor>(BlackBoard->GetValueAsObject("Target"));
		if (IsValid(Player))
		{
			Enemy->SetInterpRotationTarget(Player->GetActorLocation());

			Enemy->OnAggro();
			Enemy->Ready();

			if (bChangeStateToRun)
				Enemy->StateToRun();
			else
				Enemy->StateToWalk();

			return EBTNodeResult::Type::Succeeded;
		}
	}
 
	return EBTNodeResult::Type::Failed;
}
