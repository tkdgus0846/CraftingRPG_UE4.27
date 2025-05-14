
#pragma once

#include "CoreMinimal.h"
#include "AttackActor.h"
#include "ProjectileActor.generated.h"

UCLASS()
class MYRPG_API AProjectileActor : public AAttackActor
{
	GENERATED_BODY()

public:
	AProjectileActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UProjectileMovementComponent* ProjectileMovementComponent;

public:
	
};
