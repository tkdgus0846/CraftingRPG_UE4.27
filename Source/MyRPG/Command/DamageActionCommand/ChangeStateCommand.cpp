
#include "ChangeStateCommand.h"
#include "MyRPG/BaseClasses/VFXCreatureCharacter.h"

void UChangeStateCommand::Execute(UObject* WorldContext)
{
	if (IsValid(Creature) == false || Creature->GetDead())
	{
		return;
	}

	Creature->ChangeState(State, Duration);
}

void UChangeStateCommand::Reset()
{
	Creature = nullptr;
}

EDamageActionCommandTag UChangeStateCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_ChangeState;
}

void UChangeStateCommand::Initialize(ICreature* InCreature, EStateMaterialType InState, float InDuration)
{
	Creature = Cast<AVFXCreatureCharacter>(InCreature);
	State = InState;
	Duration = InDuration;
}
