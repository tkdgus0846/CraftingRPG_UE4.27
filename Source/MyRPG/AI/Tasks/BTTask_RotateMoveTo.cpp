#include "BTTask_RotateMoveTo.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RotateMoveTo::UBTTask_RotateMoveTo()
{
	NodeName = TEXT("RotateMoveTo");
}

EBTNodeResult::Type UBTTask_RotateMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (Result != EBTNodeResult::Type::Succeeded)
		return Result;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);

	
	return EBTNodeResult::Type::Succeeded;
}

void UBTTask_RotateMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	FName SelectedKeyName = GetSelectedBlackboardKey();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);
	if (Enemy && BlackBoard)
	{
		// 스킬을 사용하기위해 태스크를 멈출수 있는가?
		if (Enemy->CanStopTaskToUseSkill())
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
			OwnerComp.GetAIOwner()->StopMovement();
			return;
		}

		AActor* TargetActor = Cast<AActor>(BlackBoard->GetValueAsObject(SelectedKeyName));
		if (TargetActor && IsValid(TargetActor))
		{
			Enemy->SetInterpRotationTarget(TargetActor->GetActorLocation());
		}
	}	
}