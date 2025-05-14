
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include "MaceESkill.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AMaceESkill : public AAttackActor
{
	GENERATED_BODY()

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;
	
};
