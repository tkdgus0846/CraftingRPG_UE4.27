
#pragma once

#include "CoreMinimal.h"
#include "MyRPG/Item/Equipment/Equipment.h"
#include "MyRPG/Types/WeaponType.h"
#include "MyRPG/Data/ItemDataTable.h"
#include "Weapon.generated.h"


UCLASS()
class MYRPG_API AWeapon : public AEquipment
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PopFromInventory(const FVector& PopPos, float PopRadius = 100.f) override;

	virtual bool NormalAttackPressed();
	virtual bool NormalAttackPressing(float DeltaTime);
	virtual bool NormalAttackReleased();

	virtual void QSkillPressed() {}
	virtual void QSkillPressing(float DeltaTime) {}
	virtual void QSkillReleased() {}

	virtual void ESkillPressed() {}
	virtual void ESkillPressing(float DeltaTime) {}
	virtual void ESkillReleased() {}

	void BindNormalAttackMontage(class UAnimMontage* Montage);
	void BindSkillMontage(class UAnimMontage* Montage);
	void BindQSkillActor(TSubclassOf<class AAttackActor> AttackActor);
	void BindESkillActor(TSubclassOf<class AAttackActor> AttackActor);
	
	void ComboFinished();
	void Reset();

	void SetMeshComponentScale(float Scale);
	class APlayerCharacter* GetPlayer();

protected:
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess = true))
	class USkeletalMesh* Mesh = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	class USkeletalMeshComponent* MeshComponent;

	virtual void BringDataTableDatas() override;

	UPROPERTY()
	class UAnimMontage* NormalAttackMontage = nullptr;
	UPROPERTY()
	class UAnimMontage* SkillMontage = nullptr;

	UPROPERTY()
	TSubclassOf<class AAttackActor> QSkillActor;

	UPROPERTY()
	TSubclassOf<class AAttackActor> ESkillActor;

	class UAnimInstance* GetPlayerAnimInstance();

	bool BeginCombo();
	void EndCombo();

	void SetMaxCombo(uint8 ComboNum);
	uint8 GetCurrentCombo() const;

	void SetCanMoveWhenAttack(bool bCanMove = true);
	void SetAttackWalkSpeed(float WalkSpeed);

	void UpdateAttackDamage(class AAttackActor* AttackActor, float DamageMultiplier = 1.f, int DamageAdder = 0);

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	EWeaponType WeaponType;

	int8 CurrentCombo = -1;
	uint8 MaxCombo = 0;
	bool bComboFinished = true;

	bool IsComboFinished();
	void ResetCombo();

	UPROPERTY(EditDefaultsOnly)
	bool bCanMoveWhenAttack = false;

	UPROPERTY(EditDefaultsOnly)
	float AttackWalkSpeed = 300.f;

	UPROPERTY()
	FDamageRange AttackDamage;

public:
	EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE bool CanMoveWhenAttack() const { return bCanMoveWhenAttack; }
	FORCEINLINE float GetAttackWalkSpeed() const { return AttackWalkSpeed; }
	FORCEINLINE class USkeletalMesh* GetSkeletalMesh() const { return Mesh; }
	void SetSkeletalMesh(class USkeletalMesh* NewMesh);
	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComponent() const { return MeshComponent; }


	FORCEINLINE class UAnimMontage* GetSkillMontage() { return SkillMontage; }
	FORCEINLINE class UAnimMontage* GetNormalAttackMontage() { return NormalAttackMontage; }

	const FDamageRange GetAttackDamageRange() const;
};
