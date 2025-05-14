
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/*UFUNCTION()
	void AnimNotify_EnableCollision();

	UFUNCTION()
	void AnimNotify_DisableCollision();*/

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class AEnemy* EnemyOwner = nullptr;

private:
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Speed;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bDead;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsWalking;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsRunning;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsReady;

public:
	FORCEINLINE void SetReady() { bIsReady = true; }
};
