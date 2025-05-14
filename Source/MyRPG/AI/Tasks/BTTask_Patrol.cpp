
#include "BTTask_Patrol.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyRPG/SplineActor/SplineActor.h"


UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);

	if (BlackBoard && Enemy)
	{
		ASplineActor* SplineActor = Cast<ASplineActor>(BlackBoard->GetValueAsObject("PatrolSpline"));
		if (SplineActor)
		{
			SplineActor->InitializePatrol();
			return EBTNodeResult::Type::InProgress;
		}
	}

	return EBTNodeResult::Type::Failed;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);

	if (BlackBoard && Enemy)
	{
		ASplineActor* SplineActor = Cast<ASplineActor>(BlackBoard->GetValueAsObject("PatrolSpline"));

		if (SplineActor == nullptr) return;

		/*if (BlackBoard->GetValueAsObject("Player"))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
			return;
		}*/
		if (SplineActor->IsFinishedPatrolOneRound())
		{
			SplineActor->Stop();
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Failed);
			return;
		}
		
		FVector UpToGround = SplineActor->GetCurrentPos() + FVector{ 0.f,0.f,Enemy->GetDefaultHalfHeight() };
		Enemy->AddMovementInput(UpToGround);
		Enemy->SetActorLocation(UpToGround, false, nullptr, ETeleportType::TeleportPhysics);
		FRotator Rotator = SplineActor->GetCurrentTangent().Rotation();
		Rotator.Pitch = 0.f;
		Rotator.Roll = 0.f;
		Enemy->SetActorRotation(Rotator);
	}
	
}
