
#include "CombatComponent.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/Inventory.h"
#include "MyRPG/Player/SkillCaster/SkillCaster.h"

#include "MyRPG/Item/Equipment/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include <MyRPG/GameInstance/MyGameInstance.h>
#include <MyRPG/Player/SkillCaster/DashCaster/BasicDashCaster.h>
#include <MyRPG/Player/SkillCaster/DashCaster/HorseJumpCaster.h>
#include <MyRPG/Player/SkillCaster/RSkillCaster/IceQuakeCaster.h>
#include <MyRPG/Player/SkillCaster/CSkillCaster/BarrierCaster.h>



//

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	for (int i = 1; i < (int)EWeaponType::EWT_MAX; i++)
	{
		Weapons[i] = nullptr;
		NormalAttackActor[i] = nullptr;

		CreateWeapon((EWeaponType)i);
		CreateNormalAttackActor((EWeaponType)i);
		BindInfoToWeapon((EWeaponType)i);
	}

	ChangeWeapon(EWeaponType::EWT_Unarmed);
	ChangeDashSkill(EDashSkillType::EDT_BasicDash);

	// 후에 인터페이스 만들면 지우기
	ChangeRSkill(ERSkillType::ERT_IceQuake);
	ChangeCSkill(ECSkillType::ECT_Barrier);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Dashing();

	CalcTravelModeTime(DeltaTime);
	CalcAttackPressingTime(DeltaTime);
	CalcAimPressingTime(DeltaTime);

	CalcQSkillPressingTime(DeltaTime);
	CalcESkillPressingTime(DeltaTime);
	CalcRSkillPressingTime(DeltaTime);
	CalcCSkillPressingTime(DeltaTime);
	CalcTSkillPressingTime(DeltaTime);
	CalcDashSkillPressingTime(DeltaTime);
}

bool UCombatComponent::IsCombatMontagePlaying()
{
	bool bCombatMontagePlaying = false;
	if (PlayerCharacter->GetMesh()->GetAnimInstance())
	{
		UAnimMontage** Montage = NormalAttackMontages.Find(CurrentWeaponType);

		if (Montage)
		{
			bCombatMontagePlaying = PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(*Montage);
		}		
	}

	return bCombatMontagePlaying;
}

void UCombatComponent::PlayReadyIdleEquipMontage(FName SectionName)
{
	UAnimMontage** FindMontage = ReadyIdleEquipMontages.Find(CurrentWeaponType);
	if (FindMontage && PlayerCharacter)
	{
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(*FindMontage);

		// 무기 장착 몽타주 섹션으로
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, *FindMontage);
	}
}

AWeapon* UCombatComponent::GetWeapon(EWeaponType WeaponType)
{
	return Weapons[(int)WeaponType];
}

void UCombatComponent::CreateWeapon(EWeaponType WeaponType)
{
	TSubclassOf<AWeapon>* FindWeapon = WeaponClasses.Find(WeaponType);
	if (FindWeapon == nullptr) return;
	
	Weapons[(int)WeaponType] = GetWorld()->SpawnActor<AWeapon>(FindWeapon->Get());
	if (GetWeapon(WeaponType))
	{
		GetWeapon(WeaponType)->PushInInventory();
		GetWeapon(WeaponType)->GetDroppedMeshComponent()->SetVisibility(false);
	}
}

AAttackActor* UCombatComponent::GetNormalAttackActor(EWeaponType WeaponType)
{
	return NormalAttackActor[(int)WeaponType];
}

void UCombatComponent::CreateNormalAttackActor(EWeaponType WeaponType)
{
	TSubclassOf<AAttackActor>* FindClass = NormalAttackActorClasses.Find(WeaponType);
	if (FindClass == nullptr) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;

	NormalAttackActor[(int)WeaponType] = GetWorld()->SpawnActor<AAttackActor>(FindClass->Get(), SpawnParams);

	if (NormalAttackActor[(int)WeaponType])
	{
		NormalAttackActor[(int)WeaponType]->SetCollsionEnabled(ECollisionEnabled::NoCollision);
	}
}

// 스킬 몽타주를 실행하고 있지않을때
bool UCombatComponent::CanNormalAttack()
{
	return !IsSkillMontagePlaying() && bCanNormalAttack;
}

void UCombatComponent::CalcTravelModeTime(float DeltaTime)
{
	// 나중에 어택뿐만 아니라 QE 스킬에도 적용시켜야함.

	// 어택을 누르고 있지 않을때
	// 공격이나 스킬 몽타주를 실행하고 있지않을때를 기준으로 트래블모드 타임을 재야한다

	if (bAttackPressing == false && IsCombatMontagePlaying() == false)
	{
		
		TravelModeElapsedTime += DeltaTime;

		if (bTravelMode == false && TravelModeElapsedTime >= TravelModeTransTime)
		{
			bTravelMode = true;
			// 레디에서 아이들로 가는 몽타주

			//PlayReadyIdleEquipMontage("ReadyToIdle");

			if (GetWeapon(CurrentWeaponType))
			{
				GetWeapon(CurrentWeaponType)->Reset();
			}
		}
		
	}
	// 어택을 누르고 있을 때
	else
	{
		TravelModeElapsedTime = 0.f;

		if (bTravelMode)
		{
			bTravelMode = false;
			// 아이들에서 레디로 가는 몽타주

			//PlayReadyIdleEquipMontage("IdleToReady");
		}
	}
		
}

void UCombatComponent::CalcAttackPressingTime(float DeltaTime)
{
	if (bAttackPressing == true)
	{
		AttackPressingTime += DeltaTime;
		if (GetWeapon(CurrentWeaponType))
		{
			GetWeapon(CurrentWeaponType)->NormalAttackPressing(DeltaTime);
		}
	}
	else
		AttackPressingTime = 0.f;
}

void UCombatComponent::CalcAimPressingTime(float DeltaTime)
{
	if (bAimPressing == true)
		AimPressingTime += DeltaTime;
	else
		AimPressingTime = 0.f;
}

void UCombatComponent::BindInfoToWeapon(EWeaponType WeaponType)
{
	if (GetWeapon(WeaponType))
	{
		UAnimMontage** AnimMontage = NormalAttackMontages.Find(WeaponType);

		if (AnimMontage != nullptr)
			GetWeapon(WeaponType)->BindNormalAttackMontage(*AnimMontage);

		AnimMontage = WeaponSkillMontages.Find(WeaponType);

		if (AnimMontage != nullptr)
			GetWeapon(WeaponType)->BindSkillMontage(*AnimMontage);

		TSubclassOf<AAttackActor>* AttackActor = QSkillActorClasses.Find(WeaponType);
		if (AttackActor)
		{
			GetWeapon(WeaponType)->BindQSkillActor(*AttackActor);
		}

		AttackActor = ESkillActorClasses.Find(WeaponType);
		if (AttackActor)
		{
			GetWeapon(WeaponType)->BindESkillActor(*AttackActor);
		}
	}
}

bool UCombatComponent::BindInfoToSkillCaster(UAnimMontage** FindMontage, TSubclassOf<AAttackActor>* FindAttackActor, ASkillCaster* SkillCaster)
{
	bool bResult = true;
	if (IsValid(SkillCaster))
	{
		if (FindMontage)
		{
			SkillCaster->BindSkillMontage(*FindMontage);
		}
		if (FindAttackActor)
		{
			SkillCaster->BindSkillActor(*FindAttackActor);
		}
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

ASkillCaster* UCombatComponent::GetRSkillCaster(ERSkillType NewSkillType)
{
	switch (NewSkillType)
	{
	case ERSkillType::ERT_IceQuake:
		return GetWorld()->SpawnActor<AIceQuakeCaster>();
	case ERSkillType::ERT_MAX:
		return nullptr;
	default:
		return nullptr;
	}
}

ASkillCaster* UCombatComponent::GetCSkillCaster(ECSkillType NewSkillType)
{
	switch (NewSkillType)
	{
	case ECSkillType::ECT_Barrier:
		return GetWorld()->SpawnActor<ABarrierCaster>();
	case ECSkillType::ECT_MAX:
		return nullptr;
	default:
		return nullptr;
	}
}

ASkillCaster* UCombatComponent::GetTSkillCaster(ETSkillType NewSkillType)
{
	switch (NewSkillType)
	{
	case ETSkillType::ETT_MAX:
		return nullptr;
	default:
		return nullptr;
	}
}

ASkillCaster* UCombatComponent::GetDashSkillCaster(EDashSkillType NewSkillType)
{
	switch (NewSkillType)
	{
	case EDashSkillType::EDT_BasicDash:
		return GetWorld()->SpawnActor<ABasicDashCaster>();
	case EDashSkillType::EDT_HorseJump:
		return GetWorld()->SpawnActor<AHorseJumpCaster>();
	case EDashSkillType::EDT_MAX:
		return nullptr;
	default:
		return nullptr;
	}
}

void UCombatComponent::CalcQSkillPressingTime(float DeltaTime)
{
	if (bQSkillPressing == true)
	{
		QSkillPressingTime += DeltaTime;
		if (GetWeapon(CurrentWeaponType))
		{
			GetWeapon(CurrentWeaponType)->QSkillPressing(DeltaTime);
		}
	}
	else
		QSkillPressingTime = 0.f;
}

// 쿨타임 같은것들을 집어넣으면 좋을것같다.
bool UCombatComponent::CanQSkill()
{
	return bCanQSkill && !IsSkillMontagePlaying();
}

void UCombatComponent::SpawnQSkillCooltimeUI()
{
	if (bCanQSkill == false && IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnSkillCooltimeUI(QSkillCooltimeRemained);
	}
}

void UCombatComponent::NormalAttackCancel()
{
	DisableCollision();
	if (GetWeapon(CurrentWeaponType))
		GetWeapon(CurrentWeaponType)->Reset();
}

void UCombatComponent::CalcESkillPressingTime(float DeltaTime)
{
	if (bESkillPressing == true)
	{
		ESkillPressingTime += DeltaTime;
		if (GetWeapon(CurrentWeaponType))
		{
			GetWeapon(CurrentWeaponType)->ESkillPressing(DeltaTime);
		}
	}
	else
		ESkillPressingTime = 0.f;
}

bool UCombatComponent::CanESkill()
{
	return bCanESkill && !IsSkillMontagePlaying();
}

void UCombatComponent::SpawnESkillCooltimeUI()
{
	if (bCanESkill == false && IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnSkillCooltimeUI(ESkillCooltimeRemained);
	}
}

void UCombatComponent::CalcRSkillPressingTime(float DeltaTime)
{
	if (bRSkillPressing == true)
	{
		RSkillPressingTime += DeltaTime;
		if (IsValid(RSkillCaster))
		{
			RSkillCaster->SkillPressing(DeltaTime);
		}
	}
	else
		RSkillPressingTime = 0.f;
}

bool UCombatComponent::CanRSkill()
{
	return bCanRSkill && !IsSkillMontagePlaying();
}

void UCombatComponent::SpawnRSkillCooltimeUI()
{
	if (bCanRSkill == false && IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnSkillCooltimeUI(RSkillCooltimeRemained);
	}
}

void UCombatComponent::CalcCSkillPressingTime(float DeltaTime)
{
	if (bCSkillPressing == true)
	{
		CSkillPressingTime += DeltaTime;
		if (IsValid(CSkillCaster))
		{
			CSkillCaster->SkillPressing(DeltaTime);
		}
	}
	else
		CSkillPressingTime = 0.f;
}

bool UCombatComponent::CanCSkill()
{
	return bCanCSkill && !IsSkillMontagePlaying();
}

void UCombatComponent::SpawnCSkillCooltimeUI()
{
	if (bCanCSkill == false && IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnSkillCooltimeUI(CSkillCooltimeRemained);
	}
}

void UCombatComponent::CalcTSkillPressingTime(float DeltaTime)
{
	if (bTSkillPressing == true)
	{
		TSkillPressingTime += DeltaTime;
		if (IsValid(TSkillCaster))
		{
			TSkillCaster->SkillPressing(DeltaTime);
		}
	}
	else
		TSkillPressingTime = 0.f;
}

bool UCombatComponent::CanTSkill()
{
	return bCanTSkill && !IsSkillMontagePlaying();
}

void UCombatComponent::SpawnTSkillCooltimeUI()
{
	if (bCanTSkill == false && IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnSkillCooltimeUI(TSkillCooltimeRemained);
	}
}

void UCombatComponent::CalcDashSkillPressingTime(float DeltaTime)
{
	if (bDashSkillPressing == true)
	{
		DashSkillPressingTime += DeltaTime;
		if (IsValid(DashSkillCaster))
		{
			DashSkillCaster->SkillPressing(DeltaTime);
		}
	}
	else
		DashSkillPressingTime = 0.f;
}

bool UCombatComponent::CanDashSkill()
{
	return bCanDashSkill && !IsSkillMontagePlaying();
}

void UCombatComponent::SpawnDashSkillCooltimeUI()
{
	if (bCanDashSkill == false && IsValid(PlayerCharacter))
	{
		PlayerCharacter->SpawnSkillCooltimeUI(DashSkillCooltimeRemained);
	}
}

const int UCombatComponent::GetWeaponDamage() const
{
	return FMath::RandRange(CurWeaponDamageRange.MinDamage, CurWeaponDamageRange.MaxDamage);
}

bool UCombatComponent::IsTravelMode() const
{
	return bTravelMode;
}

bool UCombatComponent::CanMoveWhenAttack()
{
	if (GetWeapon(CurrentWeaponType))
	{
		return GetWeapon(CurrentWeaponType)->CanMoveWhenAttack();
	}

	return false;
	
}

float UCombatComponent::GetAttackWalkSpeed()
{
	if (GetWeapon(CurrentWeaponType))
	{
		return GetWeapon(CurrentWeaponType)->GetAttackWalkSpeed();
	}
	
	return 0.0f;
}

bool UCombatComponent::IsSkillMontagePlaying(EWeaponType WeaponType)
{
	if (PlayerCharacter->GetMesh() == nullptr || PlayerCharacter->GetMesh()->GetAnimInstance() == nullptr) return false;

	// 어떤 몽타주가 플레이 중인데 노말어택 몽타주가 플레이중이 아니라면 스킬몽타주가 실행중임을 알수있다.
	return !IsNormalAttackMontagePlaying() && PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr);
}

bool UCombatComponent::IsNormalAttackMontagePlaying(EWeaponType WeaponType)
{
	EWeaponType FindWeaponType = WeaponType;
	if (WeaponType == EWeaponType::EWT_None)
		FindWeaponType = CurrentWeaponType;
	
	auto FindPtr = NormalAttackMontages.Find(FindWeaponType);
	if (FindPtr == nullptr || PlayerCharacter->GetMesh() == nullptr || PlayerCharacter->GetMesh()->GetAnimInstance() == nullptr) return false;

	return PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_IsPlaying(*FindPtr);
}

void UCombatComponent::AttackPressed()
{
	//if (CanNormalAttack() == false) return;

	
	bAttackPressing = true;

	if (GetWeapon(CurrentWeaponType))
	{
		
		GetWeapon(CurrentWeaponType)->NormalAttackPressed();
	}
}

void UCombatComponent::AttackReleased()
{
	/*if (CanNormalAttack() == false) return;*/

	bAttackPressing = false;

	if (GetWeapon(CurrentWeaponType))
	{
		GetWeapon(CurrentWeaponType)->NormalAttackReleased();
	}
	
}

void UCombatComponent::AimPressed()
{
	bAimPressing = true;
	if (PlayerCharacter)
	{
		PlayerCharacter->ChangeCamera(ECameraActor::ECA_Aim, AimCameraBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.f, true);
	}
}

void UCombatComponent::AimReleased()
{
	bAimPressing = false;
	if (PlayerCharacter)
	{
		PlayerCharacter->ChangeCamera(ECameraActor::ECA_Main, AimCameraBlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.f, true);
	}
}

void UCombatComponent::ChangeWeapon(EWeaponType NewWeaponType, bool bPlayEquipMontage, bool bSaveToSpare)
{
	if (bSaveToSpare)
	{
		SpareWeaponType = NewWeaponType;
		return;
	}

	CurrentWeaponType = NewWeaponType;

	if (GetWeapon(NewWeaponType))
	{
		GetWeapon(NewWeaponType)->Reset();
	}

	DisableCollision();

	if (NewWeaponType == EWeaponType::EWT_Unarmed || NewWeaponType == EWeaponType::EWT_Horse)
	{
		CurrentWeaponSkeletalMesh = nullptr;
	}
	else if (PlayerCharacter && PlayerCharacter->GetCurrentSelectedWeapon())
	{
		CurrentWeaponSkeletalMesh = PlayerCharacter->GetCurrentSelectedWeapon()->GetSkeletalMesh();
	}

	// 새로운 장비 장착하는 몽타주 실행하기 
	if (bPlayEquipMontage)
		PlayReadyIdleEquipMontage("Equip");

	// 장비에 대한 타입을 조사하고 해당 Weapon에 메쉬가져와서 입히기

	bTravelMode = true;
	TravelModeElapsedTime = TravelModeTransTime;

	if (IsValid(PlayerCharacter))
	{
		if (IsValid(PlayerCharacter->GetInventory()->GetCurrentSelectedWeapon()))
		{
			CurWeaponDamageRange = PlayerCharacter->GetInventory()->GetCurrentSelectedWeapon()->GetAttackDamageRange();
			UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponValid: %d %d %s"), CurWeaponDamageRange.MinDamage, CurWeaponDamageRange.MaxDamage, *PlayerCharacter->GetInventory()->GetCurrentSelectedWeapon()->GetFName().ToString());
		}
		else
		{
			CurWeaponDamageRange.MaxDamage = 10;
			CurWeaponDamageRange.MinDamage = 5;
			UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponNotValid: %d %d"), CurWeaponDamageRange.MinDamage, CurWeaponDamageRange.MaxDamage);
		}
	}

	// 웨폰 메쉬를 현재 선택된 CurrentWeaponSkeletalMesh로 변경, 스킬 레이아웃도 바꿈
	OnChangeWeapon.Broadcast();
}

void UCombatComponent::ChangeToSpareWeapon(bool bPlayEquipMontage)
{
	ChangeWeapon(SpareWeaponType, bPlayEquipMontage);
}

void UCombatComponent::SaveCurrentWeaponToSpare()
{
	SpareWeaponType = CurrentWeaponType;
}

void UCombatComponent::ChangeRSkill(ERSkillType NewSkillType)
{
	if (CurrentRSkillType == NewSkillType) return;

	if (IsValid(RSkillCaster))
		RSkillCaster->Destroy();

	RSkillCaster = GetRSkillCaster(NewSkillType);
	if (BindInfoToSkillCaster(RSkillMontages.Find(NewSkillType), RSkillActorClasses.Find(NewSkillType), RSkillCaster))
	{
		CurrentRSkillType = NewSkillType;
		OnChangeRSkill.Broadcast();
	}
}

void UCombatComponent::ChangeCSkill(ECSkillType NewSkillType)
{
	if (CurrentCSkillType == NewSkillType) return;

	if (IsValid(CSkillCaster))
		CSkillCaster->Destroy();

	CSkillCaster = GetCSkillCaster(NewSkillType);
	if (BindInfoToSkillCaster(CSkillMontages.Find(NewSkillType), CSkillActorClasses.Find(NewSkillType), CSkillCaster))
	{
		CurrentCSkillType = NewSkillType;
		OnChangeCSkill.Broadcast();
	}
}

void UCombatComponent::ChangeTSkill(ETSkillType NewSkillType)
{
	if (CurrentTSkillType == NewSkillType) return;

	if (IsValid(TSkillCaster))
		TSkillCaster->Destroy();

	TSkillCaster = GetTSkillCaster(NewSkillType);
	if (BindInfoToSkillCaster(TSkillMontages.Find(NewSkillType), TSkillActorClasses.Find(NewSkillType), TSkillCaster))
	{
		CurrentTSkillType = NewSkillType;
		OnChangeTSkill.Broadcast();
	}
}

void UCombatComponent::ChangeDashSkill(EDashSkillType NewSkillType)
{
	if (CurrentDashSkillType == NewSkillType) return;

	if (IsValid(DashSkillCaster))
		DashSkillCaster->Destroy();

	DashSkillCaster = GetDashSkillCaster(NewSkillType);
	if (BindInfoToSkillCaster(DashSkillMontages.Find(NewSkillType), DashSkillActorClasses.Find(NewSkillType), DashSkillCaster))
	{
		CurrentDashSkillType = NewSkillType;
		OnChangeDashSkill.Broadcast();
	}
}

void UCombatComponent::EnableCollision()
{
	if (GetNormalAttackActor(CurrentWeaponType))
	{
		
		/*
		무기의 기본공격은 오버랩 이벤트를 한번만 발생시키는 어택액터이다.컴뱃컴포넌트의 BeginPla 에서 최초에 Spawn해놓고 지우지 않기 때문에 임의로 겹친 크리쳐 목록들을 삭제해줘야한다.나중에 프레임 드랍이 생기면 아래코드를 지우고 리팩토링하자.

		리팩토링 아이디어는 원래대로 되돌리는것이다. AttackActor의 OverlapFinishedCreatures를 지우고 그냥 Collide 가능한 시간을 줄이면된다.
		*/

		GetNormalAttackActor(CurrentWeaponType)->SetDamage(GetWeaponDamage());

		GetNormalAttackActor(CurrentWeaponType)->EmptyOverlapFinishedCreatures();
		GetNormalAttackActor(CurrentWeaponType)->SetCollsionEnabled(ECollisionEnabled::QueryOnly);
	}
	
}

void UCombatComponent::DisableCollision()
{
	if (GetNormalAttackActor(CurrentWeaponType))
	{
		GetNormalAttackActor(CurrentWeaponType)->SetCollsionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UCombatComponent::ComboFinished()
{
	if (GetWeapon(CurrentWeaponType))
	{
		GetWeapon(CurrentWeaponType)->ComboFinished();
	}
}

void UCombatComponent::DashPressed()
{
	SpawnDashSkillCooltimeUI();
	if (CanDashSkill() == false || IsValid(DashSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bDashSkillPressing = true;
	DashSkillCaster->SkillPressed();
}

void UCombatComponent::DashReleased()
{
	if (CanDashSkill() == false || bDashSkillPressing == false || IsValid(DashSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bDashSkillPressing = false;
	DashSkillCaster->SkillReleased();
}

void UCombatComponent::LeapPressed()
{

	if (PlayerCharacter == nullptr || LeapMontage == nullptr) return;
	if (PlayerCharacter->GetRiding()) return;

	NormalAttackCancel();

	FVector Velocity = PlayerCharacter->GetActorForwardVector();
	Velocity.Normalize();

	FVector TraceStartLocation = PlayerCharacter->GetActorLocation() + Velocity * 300.f;
	float CanLeapLength = PlayerCharacter->GetDefaultHalfHeight() + 50.f;
	FVector TraceEndLocation = TraceStartLocation - FVector(0.f, 0.f, 10000.f);

	/*DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 100.f, 5.f);*/
	FHitResult HitResult;
	bool bTraceHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECC_Visibility);

	if (bTraceHit == false) return;
	if (HitResult.Distance <= CanLeapLength) return;

	float PlayRate = FMath::GetMappedRangeValueClamped({ CanLeapLength, 2500.f }, { 0.9f, 0.1f }, HitResult.Distance);

	FActionInfo Action1;
	Action1.AnimMontage = LeapMontage;
	Action1.CurveType = ECurveFloat::ECF_Default;
	Action1.SectionName = "Travel";
	Action1.MontagePlayRate = PlayRate;

	FVector StartLocation = PlayerCharacter->GetActorLocation();
	FVector TargetLocation = HitResult.ImpactPoint;
	FVector MiddleLocation = StartLocation;
	MiddleLocation += FVector((FVector2D(TargetLocation) - FVector2D(StartLocation)) / 2.f, 0.f);
	MiddleLocation.Z += 200.f;
	TargetLocation.Z += PlayerCharacter->GetDefaultHalfHeight();

	Action1.AddMovePoint(StartLocation);
	Action1.AddMovePoint(MiddleLocation);
	Action1.AddMovePoint(TargetLocation);

	FActionInfo Action2;
	Action2.AnimMontage = LeapMontage;
	Action2.SectionName = "Landing";
	Action2.MontagePlayRate = 0.5f;
	Action2.PreActionMontagePlayTime = 0.1f;

	Action2.AddMovePoint(TargetLocation);

	PlayerCharacter->AddAction(Action1);
	PlayerCharacter->AddAction(Action2);
	PlayerCharacter->PlayAction();
}

void UCombatComponent::QPressed()
{
	SpawnQSkillCooltimeUI();
	if (CanQSkill() == false) return;

	bQSkillPressing = true;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
		GetWeapon(CurrentWeaponType)->QSkillPressed();
	}
}

void UCombatComponent::QReleased()
{
	if (CanQSkill() == false || bQSkillPressing == false) return;

	bQSkillPressing = false;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
		GetWeapon(CurrentWeaponType)->QSkillReleased();
	}
}

void UCombatComponent::EPressed()
{
	SpawnESkillCooltimeUI();
	if (CanESkill() == false) return;


	bESkillPressing = true;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
		GetWeapon(CurrentWeaponType)->ESkillPressed();
	}
}

void UCombatComponent::EReleased()
{
	if (CanESkill() == false || bESkillPressing == false) return;

	bESkillPressing = false;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
		GetWeapon(CurrentWeaponType)->ESkillReleased();
	}
}

void UCombatComponent::RPressed()
{
	SpawnRSkillCooltimeUI();
	if (CanRSkill() == false || IsValid(RSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bRSkillPressing = true;
	RSkillCaster->SkillPressed();
}

void UCombatComponent::RReleased()
{
	if (CanRSkill() == false || bRSkillPressing == false || IsValid(RSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bRSkillPressing = false;
	RSkillCaster->SkillReleased();
}

void UCombatComponent::CPressed()
{
	SpawnCSkillCooltimeUI();
	if (CanCSkill() == false || IsValid(CSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bCSkillPressing = true;
	CSkillCaster->SkillPressed();
}

void UCombatComponent::CReleased()
{
	if (CanCSkill() == false || bCSkillPressing == false || IsValid(CSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bCSkillPressing = false;
	CSkillCaster->SkillReleased();
}

void UCombatComponent::TPressed()
{
	SpawnTSkillCooltimeUI();
	if (CanTSkill() == false || IsValid(TSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bTSkillPressing = true;
	TSkillCaster->SkillPressed();
}

void UCombatComponent::TReleased()
{
	if (CanTSkill() == false || bTSkillPressing == false || IsValid(TSkillCaster) == false) return;

	if (GetWeapon(CurrentWeaponType))
	{
		NormalAttackCancel();
	}

	bTSkillPressing = false;
	TSkillCaster->SkillReleased();
}




