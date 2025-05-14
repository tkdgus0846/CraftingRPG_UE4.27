
#include "SpawnImpactEffectCommand.h"
#include "MyRPG/Effect/ImpactEffect.h"

void USpawnImpactEffectCommand::Execute(UObject* WorldContext)
{
	if (IsValid(WorldContext) == false || IsValid(ImpactEffectClass) == false)
	{
		return;
	}
	WorldContext->GetWorld()->SpawnActor<AImpactEffect>(ImpactEffectClass, ImpactEffectTransform);
}

void USpawnImpactEffectCommand::Reset()
{
	ImpactEffectClass = nullptr;
}

EDamageActionCommandTag USpawnImpactEffectCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_SpawnEffect;
}

void USpawnImpactEffectCommand::Initialize(UClass* Class, FTransform Transform)
{
	ImpactEffectClass = Class;
	ImpactEffectTransform = Transform;
}