
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyRPG/Interfaces/Command.h"
#include "LaunchCharacterCommand.generated.h"

UCLASS()
class MYRPG_API ULaunchCharacterCommand : public UObject, public ICommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UObject* WorldContext) override;
	virtual void Reset() override;
	virtual EDamageActionCommandTag GetTag() override;
	virtual const uint32 GetPriority() const override { return 0; }

	void Initialize(class AActor* Character, FVector Velocity, bool bXYOverride, bool bZOverride);

private:
	UPROPERTY()
	class ACreatureCharacter* LaunchCharacter = nullptr;
	FVector LaunchVelocity;
	bool bLaunchXYOverride;
	bool bLaunchZOverride;
};
