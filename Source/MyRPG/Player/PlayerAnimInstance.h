// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyRPG/Types/WeaponType.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AnimNotify_ComboFinished();
	UFUNCTION()
	void AnimNotify_EnableCollision();
	UFUNCTION()
	void AnimNotify_DisableCollision();
	UFUNCTION()
	void AnimNotify_EnableActionStop();

protected:
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);

private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bIsMoving;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bTravelMode = true;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float MoveAngle;

	void CalcMoveAngle(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float YawDelta;

	FRotator RotationLastTick;
	//FVector TargetInterpRotation;
	void CalcYawDelta(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float AO_Pitch;

	void CalcAOYaw(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bIsAttacking;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	EWeaponType CurrentWeaponType;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float OriginWalkSpeed;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bCanMoveWhenAttack = false;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float AttackWalkSpeed = 300.f;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bIsMounted = false;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FTransform MountLeftHandTransform;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FTransform MountRightHandTransform;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FVector IKLeftFootEffector = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FVector IKRightFootEffector = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float IKHipOffset = 0.f;

	void CalcIKFootEffector();

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bPlayMontageOnly = false;

public:
	FORCEINLINE void SetPlayMontageOnly(bool bState) { bPlayMontageOnly = bState; }
};
