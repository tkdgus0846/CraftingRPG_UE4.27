
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "MyRPG/Types/CameraShakeType.h"
#include "CameraShakeCommand.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UCameraShakeCommand : public UObject, public ICommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UObject* WorldContext) override;
	virtual void Reset() override;
	virtual EDamageActionCommandTag GetTag() override;
	virtual const uint32 GetPriority() const override { return 0; }

	void Initialize(ECameraShake ShakeType, float Scale);

private:
	ECameraShake CameraShakeType;
	float CameraShakeScale;
};
