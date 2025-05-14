
#include "EnemyAnimInstance.h"
#include "MyRPG/Enemy/Enemy.h"



void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EnemyOwner = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (EnemyOwner == nullptr) return;

	Speed = EnemyOwner->GetVelocity().Size2D();
	bDead = EnemyOwner->GetDead();
	bIsWalking = (EnemyOwner->GetMaxWalkSpeed() == EnemyOwner->GetWalkSpeed());
	bIsRunning = (EnemyOwner->GetMaxWalkSpeed() == EnemyOwner->GetRunSpeed());
}

//void UEnemyAnimInstance::AnimNotify_EnableCollision()
//{
//	if (EnemyOwner == nullptr) return;
//
//	EnemyOwner->NormalAttackCollisionEnable();
//}
//
//void UEnemyAnimInstance::AnimNotify_DisableCollision()
//{
//	if (EnemyOwner == nullptr) return;
//
//	EnemyOwner->NormalAttackCollisionDisable();
//}
