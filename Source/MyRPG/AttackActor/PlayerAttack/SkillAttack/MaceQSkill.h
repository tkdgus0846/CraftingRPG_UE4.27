
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include "MaceQSkill.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AMaceQSkill : public AAttackActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;
	
};
