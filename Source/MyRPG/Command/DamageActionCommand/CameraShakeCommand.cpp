
#include "CameraShakeCommand.h"
#include "MyRPG/GameInstance/MyGameInstance.h"
#include "MyRPG/Types/DamageActionCommandTag.h"

void UCameraShakeCommand::Execute(UObject* WorldContext)
{
	if (IsValid(WorldContext) == false)
	{
		return;
	}

	Cast<UMyGameInstance>(WorldContext->GetWorld()->GetGameInstance())->ClientPlayCameraShake(CameraShakeType, CameraShakeScale);
}

void UCameraShakeCommand::Reset()
{
}

EDamageActionCommandTag UCameraShakeCommand::GetTag()
{
	return EDamageActionCommandTag::EDACT_CameraShake;
}

void UCameraShakeCommand::Initialize(ECameraShake ShakeType, float Scale)
{
	CameraShakeType = ShakeType;
	CameraShakeScale = Scale;
}
