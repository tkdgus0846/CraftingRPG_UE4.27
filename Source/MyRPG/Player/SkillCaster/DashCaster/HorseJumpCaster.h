
#pragma once

#include "CoreMinimal.h"
#include "../SkillCaster.h"
#include "HorseJumpCaster.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AHorseJumpCaster : public ASkillCaster
{
	GENERATED_BODY()

public:
	// ASkillCaster을(를) 통해 상속됨
	void SkillPressed() override;
	void SkillPressing(float DeltaTime) override;
	void SkillReleased() override;
};
