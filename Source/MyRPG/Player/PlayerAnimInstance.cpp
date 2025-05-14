// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "MyRPG/Controller/MyPlayerController.h"
#include "Components/CombatComponent.h"
#include <MyRPG/GameInstance/MyGameInstance.h>

void UPlayerAnimInstance::AnimNotify_ComboFinished()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->GetCombatComponent() == nullptr) return;

	PlayerCharacter->GetCombatComponent()->ComboFinished();
}

void UPlayerAnimInstance::AnimNotify_EnableCollision()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->GetCombatComponent() == nullptr) return;

	PlayerCharacter->GetCombatComponent()->EnableCollision();
}

void UPlayerAnimInstance::AnimNotify_DisableCollision()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->GetCombatComponent() == nullptr) return;

	PlayerCharacter->GetCombatComponent()->DisableCollision();
}

void UPlayerAnimInstance::AnimNotify_EnableActionStop()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->GetActionComponent() == nullptr) return;

	PlayerCharacter->GetActionComponent()->EnableActionStop();
	UE_LOG(LogTemp, Warning, TEXT("AnimNotify"));
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	if (PlayerCharacter)
	{
		OriginWalkSpeed = PlayerCharacter->GetOriginWalkSpeed();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(PlayerCharacter) == false) return;


	bTravelMode = PlayerCharacter->IsTravelMode();
	
	Speed = PlayerCharacter->GetVelocity().Size2D();
	bIsMoving = Speed > 0.f;
	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();
	bIsAiming = PlayerCharacter->IsAiming();
	bIsAttacking = PlayerCharacter->IsAttacking();
	CurrentWeaponType = PlayerCharacter->GetCurrentSelectedWeaponType();
	bCanMoveWhenAttack = PlayerCharacter->CanMoveWhenAttack();
	AttackWalkSpeed = PlayerCharacter->GetAttackWalkSpeed();
	bIsMounted = PlayerCharacter->GetIsMounted();
	MountLeftHandTransform = PlayerCharacter->GetMountLeftHandTransform();
	MountRightHandTransform = PlayerCharacter->GetMountRightHandTransform();

	CalcIKFootEffector();
	
	// ���� �ö�Ÿ�� ���� ������
	if (bIsMounted == false)
	{
		if (PlayerCharacter->IsActionPlaying())
		{
			AO_Yaw = 0.f;
			return;
		}

		CalcMoveAngle(DeltaSeconds);
		CalcYawDelta(DeltaSeconds);
		CalcAOYaw(DeltaSeconds);
	}
	// ���� �ö�Ÿ�� ������
	else
	{

	}
	

	//UE_LOG(LogTemp, Warning, TEXT("Origin Walk Speed: %f"), OriginWalkSpeed);
	

}

void UPlayerAnimInstance::CalcMoveAngle(float DeltaTime)
{
	FVector ForwardVector;
	if (PlayerCharacter->GetCameraForwardVector(ForwardVector) == false) return;
	ForwardVector.Z = 0.f;
	ForwardVector.Normalize();

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Velocity.Normalize();

	// ���콺 ��ġ���� ȸ�� ������ ���� �����ִ� �κ�
	// ī�޶� ȸ���ϰ������� ĳ���� ȸ�� ���Ƴ����ڵ��̴�.
	/*if (PlayerCharacter->IsCameraRotate() == false)
	{
		TargetInterpRotation = ForwardVector.RotateAngleAxis(PlayerCharacter->GetMouseDirectionAngle(), { 0.f,0.f,1.f });
	}*/
	FVector TargetInterpRotation = ForwardVector.RotateAngleAxis(PlayerCharacter->GetMouseDirectionAngle(), { 0.f,0.f,1.f });

	PlayerCharacter->SetInterpRotationTarget(TargetInterpRotation.Rotation());
	
	if (Velocity.Size() == 0.f) return;

	float AngleInRadians = FMath::Acos(FVector::DotProduct(TargetInterpRotation, Velocity) / (TargetInterpRotation.Size() * Velocity.Size()));
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

	// ������ ���Ϳ� Velocity ���� ����
	FVector CrossVec = UKismetMathLibrary::Cross_VectorVector(TargetInterpRotation, Velocity);
	FVector UpVec = PlayerCharacter->GetActorUpVector();

	if (FVector::DotProduct(CrossVec, UpVec) < 0.f)
	{
		AngleInDegrees = 360.f - AngleInDegrees;
	}

	MoveAngle = AngleInDegrees;
	
}

// ĳ���͸� ȸ����Ű�� �κ�
void UPlayerAnimInstance::CalcYawDelta(float DeltaTime)
{
	//if (PlayerCharacter->GetCombatComponent()->IsNormalAttackMontagePlaying()) return;

	FRotator MyRotation = PlayerCharacter->GetActorRotation();

	/* YawDelta ����ϴ� �κ�*/
	FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(RotationLastTick, MyRotation);

	float TargetYawDelta = RotationDelta.Yaw / DeltaTime / 7.f;
	YawDelta = FMath::FInterpTo(YawDelta, TargetYawDelta, DeltaTime, 6.f);

	YawDelta = FMath::Clamp(YawDelta, -45.f, 45.f);

	RotationLastTick = MyRotation;
}

void UPlayerAnimInstance::CalcAOYaw(float DeltaTime)
{
	/*if (PlayerCharacter->IsCameraRotate() || 
		PlayerCharacter->IsEnabledGameplayMouse() == false ||
		PlayerCharacter->GetCombatComponent()->IsNormalAttackMontagePlaying()) return;*/
	if (PlayerCharacter->IsCameraRotate() ||
		PlayerCharacter->GetCombatComponent()->IsNormalAttackMontagePlaying()) return;

	// ���콺 ��ġ���ٰ� ī�޶� ��ġ�� ���ؾߵ�.
	FVector CameraForwardVector;
	PlayerCharacter->GetCameraForwardVector(CameraForwardVector);
	CameraForwardVector.Z = 0.f;

	FRotator CameraRotation(CameraForwardVector.Rotation());
	FRotator MouseRotation(0.f, PlayerCharacter->GetMouseDirectionAngle(), 0.f);

	FRotator TargetRotation = CameraRotation + MouseRotation;
	
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator
	(
		TargetRotation,
		PlayerCharacter->GetActorRotation()
	);

	float TargetInterpAOYaw = DeltaRotation.Yaw;

	AO_Yaw = FMath::FInterpTo(AO_Yaw, TargetInterpAOYaw, DeltaTime, 10.f);

	/*AO_Yaw = DeltaRotation.Yaw;*/
}

void UPlayerAnimInstance::CalcIKFootEffector()
{
	FVector WorldDirection(0.f, 0.f, 1.f);

	FTransform LeftFootTransform = PlayerCharacter->GetMesh()->GetSocketTransform("Left_Ankle_JNT");
	FVector LeftFootDirection = LeftFootTransform.InverseTransformVectorNoScale(WorldDirection).GetSafeNormal();

	FTransform RightFootTransform = PlayerCharacter->GetMesh()->GetSocketTransform("Right_Ankle_JNT");
	FVector RightFootDirection = RightFootTransform.InverseTransformVectorNoScale(WorldDirection).GetSafeNormal();

	//UE_LOG(LogTemp, Warning, TEXT("LeftDirection: %s, RightDirection: %s => LeftDirection: %s, RightDirection: %s"), *LeftFootDirection.ToString(), *RightFootDirection.ToString(), *LeftFootTransform.TransformVectorNoScale(LeftFootDirection).ToString(), *RightFootTransform.TransformVectorNoScale(RightFootDirection).ToString());

	IKLeftFootEffector = LeftFootDirection * (PlayerCharacter->GetIKLeftFootOffset() * 0.01f);
	IKRightFootEffector = RightFootDirection * (PlayerCharacter->GetIKRightFootOffset() * 0.01f);
	IKHipOffset = PlayerCharacter->GetIKHipOffset();
}


