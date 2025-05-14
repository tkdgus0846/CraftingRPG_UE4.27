
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillCaster.generated.h"

UCLASS()
class MYRPG_API ASkillCaster : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillCaster();

	virtual void BeginPlay() override;
	virtual void SkillPressed() {}
	virtual void SkillPressing(float DeltaTime) {}
	virtual void SkillReleased() {}

	void BindSkillMontage(class UAnimMontage* Montage);
	void BindSkillActor(TSubclassOf<class AAttackActor> AttackActor);

protected:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter = nullptr;
	class APlayerCharacter* GetPlayer();

	UPROPERTY()
	class UAnimMontage* SkillMontage = nullptr;
	UPROPERTY()
	TSubclassOf<class AAttackActor> SkillActorClass = nullptr;


	void RerserveSpawnSkillActor(float WaitTime);
	virtual void SpawnSkillActor() {}

private:

public:

};
