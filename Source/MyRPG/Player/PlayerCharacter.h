
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/BaseClasses/VFXCreatureCharacter.h"
#include "MyRPG/Types/WeaponType.h"
#include "PlayerCharacter.generated.h"

UENUM()
enum class ECameraActor : int8
{
	ECA_Main = -1,
	ECA_Aim,

	ECA_MAX
};

UCLASS()
class MYRPG_API APlayerCharacter : public AVFXCreatureCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	void ChangeCamera(ECameraActor CameraType, float BlendTime = 0, EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false);
	virtual void Damaged(struct FDamageInfo* DamageInfo) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponVisiblity(bool bVisibility);

	UFUNCTION(BlueprintCallable)
	void SpawnSkillCooltimeUI(float RemainCooltime);

	/*
		LoadPlayer
	*/
	UFUNCTION()
	void LoadPlayer(class UPlayerSave* PlayerSave);
	void ClearPlayer(const FVector& ClearLocation);

	void FlushBindingInputs();
	

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void HitReact_Implementation() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Zoom(float Value);

	void AttackPressed();
	void AttackReleased();
	void CameraRotatePressed();
	void CameraRotateReleased();
	void CancelPressed();
	void InteractPressed();
	void InteractReleased();
	void InventoryOpenPressed();
	void Press1();
	void Press2();
	void Press3();
	void Press4();
	void Press5();
	void Press6();
	void Press7();
	void Press8();
	void Press9();
	void SpaceBarPressed();
	void SpaceBarReleased();
	void QPressed();
	void QReleased();
	void EPressed();
	void EReleased();
	void RPressed();
	void RReleased();
	void CPressed();
	void CReleased();
	void ShiftPressed();
	void ShiftReleased();

private:
	UPROPERTY()
	class AMyPlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* Aim_SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	AActor* CameraActor[(int8)ECameraActor::ECA_MAX];

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UInventory* Inventory;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* InteractDetectionSphere;

	UPROPERTY(VisibleAnywhere)
	class UMouseComponent* MouseComponent;

	UPROPERTY(VisibleAnywhere)
	class UInteractComponent* InteractComponent;


	/* 오리지널 메쉬들 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* CustomHeadMeshComponent;
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* OriginLeftWeaponComponent;
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* OriginRightWeaponComponent;

	/* VFX 메쉬들 */
	/*UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* VFXMeshComponent;*/
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* VFXHeadMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* VFXRightWeaponComponent;
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* VFXLeftWeaponComponent;
	

	/* 기본 카메라 조작 변수들 */
	const float MaxAngle = -40.f;
	const float MinAngle = -60.f;
	const float MaxZoom = 1000.f;
	const float MinZoom = 700.f;
	const float ZoomSpeed = 200.f;

	float InterpZoomLength;
	float TargetZoomLength;
	void InterpCameraZoom(float DeltaTime);
	void ClearSpringArmSetting();

	/* 이동속도 관련 변수들 */
	float OriginWalkSpeed = 500.f;
	float OriginAcceleration = 0.f;
	bool bRestrictChangeWalkSpeed = false;

	// 플레이어 가리는 액터들 페이드아웃 타이머
	FTimerHandle FadeOutTimer;
	void FadeOutBetweenCamera();
	FVector FadeOutBoxExtent = { 2.f, 15.f, 80.f };

	TArray<TWeakObjectPtr<class UMeshComponent>> PreviousFadeOutArr;
	TArray<TWeakObjectPtr<class UMeshComponent>> CurFadeOutArr;

	/* 무기 바꿀때 VFX 메쉬 교체 */
	UFUNCTION()
	void ChangeWeaponMesh();

	/* 말 라이딩 변수들 */
	bool bIsMounted = false;

	UPROPERTY()
	ACreatureCharacter* Riding = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* MountMontage = nullptr;

	FTimerHandle MountTimerHandle;

	UFUNCTION()
	void MountVarSettings(bool bState, ACreatureCharacter* NewRiding);
	float MountAction(const FVector& RidingPos);
	float UnMountAction(const FVector& RidingPos);

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FTransform MountLeftHandTransform;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FTransform MountRightHandTransform;

	/*
		Rotation Interp Methods	
	*/
	virtual bool CanInterpRotation() override;

	/*
		Escape Widget~!
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEscapeWidget> EscapeWidgetClass;
	UPROPERTY()
	class UEscapeWidget* EscapeWidget = nullptr;
	void OpenAndCloseEscapeWidget();

	/*
		IK Properties
	*/
	float IKLeftFootOffset = 0.f;
	float IKRightFootOffset = 0.f;

	void CalcIKOffsets(float DeltaSeconds);
	
public:
	const bool GetCameraForwardVector(FVector& CameraVector);
	const bool IsTravelMode() const;
	FVector GetCameraLocation();
	FTransform GetCameraTransform();

	UFUNCTION(BlueprintCallable)
	class AMyPlayerController* GetPlayerController();

	AActor* GetCameraActor(ECameraActor CameraActorType) const;
	void SetCameraActor(ECameraActor CameraActorType, AActor* NewCameraActor);
	bool IsAiming() const; 
	bool IsAttacking() const;
	bool IsCameraRotate(); 
	float GetMouseDirectionAngle();
	bool IsEnabledGameplayMouse();

	EWeaponType GetCurrentSelectedWeaponType() const;
	class AWeapon* GetCurrentSelectedWeapon() const;

	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(EWeaponType NewWeaponType, bool bPlayEquipMontage = true, bool bSaveToSpare = false);
	
	bool CanMoveWhenAttack();
	float GetAttackWalkSpeed();

	void SetMaxWalkSpeed(float Speed);
	void WalkSpeedToOrigin();
	void SetMaxAcceleration(float Acc);
	void AccelerationToOrigin();
	FORCEINLINE void RestrictChangeMaxWalkSpeed(bool bState) { bRestrictChangeWalkSpeed = bState; }

	UFUNCTION(BlueprintPure)
	class USpringArmComponent* GetCameraBoom() { return SpringArmComponent; }

	/* */
	UFUNCTION(BlueprintCallable)
	void SetIsMounted(bool bState, ACreatureCharacter* NewRiding = nullptr); 
	UFUNCTION(BlueprintCallable)
	const bool GetIsMounted() const { return bIsMounted; }

	FTransform GetMountLeftHandTransform() const { return MountLeftHandTransform; }
	FTransform GetMountRightHandTransform() const { return MountRightHandTransform; }

	
	const FRotator GetSpringArmRelativeRotation() const;
	void SetSpringArmRelativeRotation(FRotator Rotation);

	FORCEINLINE const float GetInterpZoomLength() const { return InterpZoomLength; }
	FORCEINLINE void SetZoomLength(float ZoomLength) { InterpZoomLength = ZoomLength; TargetZoomLength = ZoomLength; }

	FORCEINLINE float GetOriginWalkSpeed() const { return OriginWalkSpeed; }
	FORCEINLINE class UMouseComponent* GetMouseComponent() { return MouseComponent; }
	FORCEINLINE class UCombatComponent* GetCombatComponent() { return CombatComponent; }
	FORCEINLINE class UActionComponent* GetActionComponent() { return ActionComponent; }
	FORCEINLINE class UInteractComponent* GetInteractComponent() { return InteractComponent; }
	FORCEINLINE class UInventory* GetInventory() { return Inventory; }
	FORCEINLINE class ACreatureCharacter* GetRiding() { return Riding; }

	FORCEINLINE const float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	FORCEINLINE const float GetIKRightFootOffset() const { return IKRightFootOffset; }
};
