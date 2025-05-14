 
#include "BTTask_RotateTo.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RotateTo::UBTTask_RotateTo()
{
	NodeName = TEXT("RotateTo");
}

void UBTTask_RotateTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

EBTNodeResult::Type UBTTask_RotateTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (Result != EBTNodeResult::Type::Succeeded)
		return Result;
	
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (Enemy && BlackBoard)
	{
		AActor* TargetActor = Cast<AActor>(BlackBoard->GetValueAsObject(GetSelectedBlackboardKey()));
		if (TargetActor)
		{
			Enemy->SetInterpRotationTarget(TargetActor->GetActorLocation());
		}
		return EBTNodeResult::Type::Succeeded;
	}
	return EBTNodeResult::Type::Failed;
}
