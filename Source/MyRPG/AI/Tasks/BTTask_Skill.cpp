
#include "BTTask_Skill.h"
#include "MyRPG/Controller/BTAIController.h"
#include "MyRPG/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Skill::UBTTask_Skill()
{
	NodeName = TEXT("Skill");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Skill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* Enemy = Cast<AEnemy>(ControllingPawn);

	if (BlackBoard && Enemy)
	{
		// ��ų�� �������� �ϴ°� ������ �Ʒ��� RandomChoiceSkill �κи� ���ǹ����� �ٲ��ָ�ȴ�.
		int SkillNum = Enemy->RandomChoiceSkill();
		Enemy->SetValueAsInt(FName("SkillNum"), SkillNum);

		if (Enemy->GetValueAsBool("CanSkill") == false || SkillNum == INDEX_NONE)
		{
			return EBTNodeResult::Type::Failed;
		} 

		BlackBoard->SetValueAsBool("IsSkillFinished", false);
		Enemy->Skill(SkillNum);
		return EBTNodeResult::Type::InProgress;
	}

	return EBTNodeResult::Type::Failed;
}

void UBTTask_Skill::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();

	if (BlackBoard)
	{
		bool bSkillFinished = BlackBoard->GetValueAsBool("IsSkillFinished");
		if (bSkillFinished)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Type::Succeeded);
			return;
		}
	}
	
}

void UBTTask_Skill::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	//UE_LOG(LogTemp, Warning, TEXT("OnTaskFinished"));
	if (BlackBoard)
	{
		BlackBoard->SetValueAsBool("IsSkillFinished", false);
	}

}
