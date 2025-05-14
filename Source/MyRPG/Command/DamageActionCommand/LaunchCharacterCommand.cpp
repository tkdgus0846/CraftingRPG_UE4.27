
#include "LaunchCharacterCommand.h"
#include "MyRPG/BaseClasses/CreatureCharacter.h"
#include "MyRPG/Types/DamageActionCommandTag.h"

void ULaunchCharacterCommand::Execute(UObject* WorldContext)
{
	if (IsValid(LaunchCharacter) == false || LaunchCharacter->GetDead())
	{
		return;
	}

	LaunchCharacter->LaunchCharacter(LaunchVelocity, bLaunchXYOverride, bLaunchZOverride);
}

void ULaunchCharacterCommand::Reset()
{
	LaunchCharacter = nullptr;
}

EDamageActionCommandTag ULaunchCharacterCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_CrowdControl;
}

void ULaunchCharacterCommand::Initialize(AActor* Character, FVector Velocity, bool bXYOverride, bool bZOverride)
{
	LaunchCharacter = Cast<ACreatureCharacter>(Character);
	LaunchVelocity = Velocity;
	bLaunchXYOverride = bXYOverride;
	bLaunchZOverride = bZOverride;	
}
