
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyRPG/Types/WeaponType.h"
#include "MyRPG/Types/OtherSkillsType.h"
#include "MyRPG/Data/ItemDataTable.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastZeroParamDelegate);

USTRUCT(Atomic, BlueprintType)
struct FWeaponSkillTexture
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* NormalAttackTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* QSkillTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* ESkillTexture;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	void AttackPressed();
	void AttackReleased();
	void AimPressed();
	void AimReleased();
	void DashPressed();
	void DashReleased();
	void LeapPressed();

	void QPressed();
	void QReleased();
	void EPressed();
	void EReleased();
	void RPressed();
	void RReleased();
	void CPressed();
	void CReleased();
	void TPressed();
	void TReleased();

	/*
		���⸦ �ٲ����ÿ�, �κ��丮���� �ڵ����� ȣ����.
		NewWeaponType = �ٲٷ��� ����Ÿ��
		bPlayEquipMontage = ����ϴ� ��Ÿ�ָ� �����Ұ���.
		bSaveToSpare = CurrentWeaponType�� ���� �ٲٴ°� �ƴ� SpareWeaponType�� �����س�����. ���⸦ �ٲٸ� �ȵǴ� ��Ȳ�� ���⸦ �ٲ����� �����.
	*/ 
	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(EWeaponType NewWeaponType, bool bPlayEquipMontage = true, bool bSaveToSpare = false);
	/*
		������ ������ ȣ���ϸ��. ����� �����س��� ����� �ٲٴ¸޼ҵ���.
	*/
	UFUNCTION(BlueprintCallable)
	void ChangeToSpareWeapon(bool bPlayEquipMontage = true);
	UFUNCTION(BlueprintCallable)
	void SaveCurrentWeaponToSpare();

	// ��ų�� �ٲܽÿ� ȣ��
	UFUNCTION(BlueprintCallable)
	void ChangeRSkill(ERSkillType NewSkillType);
	UFUNCTION(BlueprintCallable)
	void ChangeCSkill(ECSkillType NewSkillType);
	UFUNCTION(BlueprintCallable)
	void ChangeTSkill(ETSkillType NewSkillType);
	UFUNCTION(BlueprintCallable)
	void ChangeDashSkill(EDashSkillType NewSkillType);

	UFUNCTION(BlueprintCallable)
	void EnableCollision();

	UFUNCTION(BlueprintCallable)
	void DisableCollision();

	UFUNCTION(BlueprintCallable)
	void ComboFinished();

	bool CanNormalAttack();
	void NormalAttackCancel();

	/*
		OnChangeWeapon: �� ��������Ʈ�� ȣ��Ǹ� ��ų���̾ƿ��̶� ���� �޽��� �ٲ�.
	*/
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnChangeWeapon;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnChangeRSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnChangeCSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnChangeTSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnChangeDashSkill;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnNormalAttack;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnQSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnESkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnSpaceSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnRSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnCSkill;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDynamicMulticastZeroParamDelegate OnTSkill;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	/* ���� ������ ������ ���̷�Ż�޽� */
	UPROPERTY()
	class USkeletalMesh* CurrentWeaponSkeletalMesh;
	
	/* ��ų ��Ÿ�ֵ� */
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, class UAnimMontage*> NormalAttackMontages;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, class UAnimMontage*> WeaponSkillMontages;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = RSkill)
	TMap<ERSkillType, class UAnimMontage*> RSkillMontages;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = CSkill)
	TMap<ECSkillType, class UAnimMontage*> CSkillMontages;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = TSkill)
	TMap<ETSkillType, class UAnimMontage*> TSkillMontages;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = DashSkill)
	TMap<EDashSkillType, class UAnimMontage*> DashSkillMontages;

	/* ��ų �ؽ��ĵ� */
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), BlueprintReadOnly, Category = Weapon)
	TMap<EWeaponType, FWeaponSkillTexture> WeaponSkillTextures;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), BlueprintReadOnly, Category = RSkill)
	TMap<ERSkillType, class UTexture2D*> RSkillTextures;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), BlueprintReadOnly, Category = CSkill)
	TMap<ECSkillType, class UTexture2D*> CSkillTextures;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), BlueprintReadOnly, Category = TSkill)
	TMap<ETSkillType, class UTexture2D*> TSkillTextures;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), BlueprintReadOnly, Category = DashSkill)
	TMap<EDashSkillType, class UTexture2D*> DashSkillTextures;

	// �����̳� ��ų ��Ÿ�ְ� ����ǰ� �ִ��� �˻�
	bool IsCombatMontagePlaying();

	// �غ񿡼� ���̵�, ���̵鿡�� �غ�, ���⸦ �����ϴ� �ִϸ��̼� 
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, class UAnimMontage*> ReadyIdleEquipMontages;

	void PlayReadyIdleEquipMontage(FName SectionName);

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Leap)
	class UAnimMontage* LeapMontage;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, TSubclassOf<class AWeapon>> WeaponClasses;

	AWeapon* Weapons[(int)EWeaponType::EWT_MAX];
	AWeapon* GetWeapon(EWeaponType WeaponType);

	void CreateWeapon(EWeaponType WeaponType);

	/*
		CurrentWeaponType: �� ������ �ٲ����� ������ ��Ÿ�� ��ų�� ���� �ٲ�Ե�
	*/
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	EWeaponType CurrentWeaponType;
	EWeaponType SpareWeaponType;
	
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, TSubclassOf<class AAttackActor>> NormalAttackActorClasses;

	class AAttackActor* NormalAttackActor[(int)EWeaponType::EWT_MAX];
	class AAttackActor* GetNormalAttackActor(EWeaponType WeaponType);

	void CreateNormalAttackActor(EWeaponType WeaponType);

	/* ��ų ĳ���� R,C,T,Space ���� ��ų�� */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	ERSkillType CurrentRSkillType;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	ECSkillType CurrentCSkillType;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	ETSkillType CurrentTSkillType;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	EDashSkillType CurrentDashSkillType;

	UPROPERTY()
	class ASkillCaster* RSkillCaster = nullptr;
	UPROPERTY()
	class ASkillCaster* CSkillCaster = nullptr;
	UPROPERTY()
	class ASkillCaster* TSkillCaster = nullptr;
	UPROPERTY()
	class ASkillCaster* DashSkillCaster = nullptr;

	

	/* ��ų ���� ���͵� */
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, TSubclassOf<class AAttackActor>> QSkillActorClasses;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, TSubclassOf<class AAttackActor>> ESkillActorClasses;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = RSkill)
	TMap<ERSkillType, TSubclassOf<class AAttackActor>> RSkillActorClasses;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = CSkill)
	TMap<ECSkillType, TSubclassOf<class AAttackActor>> CSkillActorClasses;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = TSkill)
	TMap<ETSkillType, TSubclassOf<class AAttackActor>> TSkillActorClasses;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true), Category = DashSkill)
	TMap<EDashSkillType, TSubclassOf<class AAttackActor>> DashSkillActorClasses;

	void BindInfoToWeapon(EWeaponType WeaponType);
	bool BindInfoToSkillCaster(class UAnimMontage** FindMontage, TSubclassOf<class AAttackActor>* FindAttackActor, class ASkillCaster* SkillCaster);
	class ASkillCaster* GetRSkillCaster(ERSkillType NewSkillType);
	class ASkillCaster* GetCSkillCaster(ECSkillType NewSkillType);
	class ASkillCaster* GetTSkillCaster(ETSkillType NewSkillType);
	class ASkillCaster* GetDashSkillCaster(EDashSkillType NewSkillType);

	
// ��Ÿ, ��ų�� �����ؼ� ��Ÿ�� ���� ��Ÿ����� ó���Ѵ�.
#pragma region Attack,Skill Properties
	bool bAttackPressing = false;
	float AttackPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanNormalAttack = true;
	void CalcAttackPressingTime(float DeltaTime);
	// CanNormalAttack() �޼ҵ�� public�ǿ� ����.

	bool bQSkillPressing = false;
	float QSkillPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanQSkill = true;
	void CalcQSkillPressingTime(float DeltaTime);
	bool CanQSkill();
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float QSkillCooltimeRemained = 0.f;
	void SpawnQSkillCooltimeUI();

	bool bESkillPressing = false;
	float ESkillPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanESkill = true;
	void CalcESkillPressingTime(float DeltaTime);
	bool CanESkill();
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float ESkillCooltimeRemained = 0.f;
	void SpawnESkillCooltimeUI();

	bool bRSkillPressing = false;
	float RSkillPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanRSkill = true;
	void CalcRSkillPressingTime(float DeltaTime);
	bool CanRSkill();
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float RSkillCooltimeRemained = 0.f;
	void SpawnRSkillCooltimeUI();

	bool bCSkillPressing = false;
	float CSkillPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanCSkill = true;
	void CalcCSkillPressingTime(float DeltaTime);
	bool CanCSkill();
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float CSkillCooltimeRemained = 0.f;
	void SpawnCSkillCooltimeUI();

	bool bTSkillPressing = false;
	float TSkillPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanTSkill = true;
	void CalcTSkillPressingTime(float DeltaTime);
	bool CanTSkill();
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float TSkillCooltimeRemained = 0.f;
	void SpawnTSkillCooltimeUI();

	bool bDashSkillPressing = false;
	float DashSkillPressingTime = 0.f;
	/* boolean by cooltime */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bCanDashSkill = true;
	void CalcDashSkillPressingTime(float DeltaTime);
	bool CanDashSkill();
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float DashSkillCooltimeRemained = 0.f;
	void SpawnDashSkillCooltimeUI();
#pragma endregion

	/* ��Ÿ�ӵ� */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, float> NormalAttackCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, float> QSkillCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = Weapon)
	TMap<EWeaponType, float> ESkillCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = RSkill)
	TMap<ERSkillType, float> RSkillCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = CSkill)
	TMap<ECSkillType, float> CSkillCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = TSkill)
	TMap<ETSkillType, float> TSkillCoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = DashSkill)
	TMap<EDashSkillType, float> DashSkillCoolTime;	

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	float TravelModeTransTime = 0.5f;
	float TravelModeElapsedTime = TravelModeTransTime;
	bool bTravelMode = true;

	void CalcTravelModeTime(float DeltaTime);

	bool bAimPressing = false;
	float AimPressingTime = 0.f;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	float AimCameraBlendTime = 0.5f;

	void CalcAimPressingTime(float DeltaTime); 

	UPROPERTY()
	FDamageRange CurWeaponDamageRange;

public:
	const int GetWeaponDamage() const;

	bool IsTravelMode() const;
	FORCEINLINE float GetAttackPressingTime() const { return AttackPressingTime; }
	FORCEINLINE bool IsAiming() const { return bAimPressing; }
	FORCEINLINE bool IsAttacking() const { return bAttackPressing; }
	FORCEINLINE class USkeletalMesh* GetCurrentWeaponSkeletalMesh() const { return CurrentWeaponSkeletalMesh; }
	FORCEINLINE const EWeaponType GetCurrentWeaponType() const { return CurrentWeaponType; }

	bool CanMoveWhenAttack();
	float GetAttackWalkSpeed();
	
	/* �Ű������� �ƹ��͵� �ȳ����� �˾Ƽ� ���� ����Ÿ�Կ� �°� ���ư�. */
	bool IsSkillMontagePlaying(EWeaponType WeaponType = EWeaponType::EWT_None);
	bool IsNormalAttackMontagePlaying(EWeaponType WeaponType = EWeaponType::EWT_None);
		
};
