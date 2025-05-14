
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "kismet/GameplayStatics.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/CombatComponent.h"
#include "MyRPG/AttackActor/AttackActor.h"

AWeapon::AWeapon()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");

	if (MeshComponent)
	{
		MeshComponent->SetupAttachment(RootComponent);
	}

	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComponent)
		MeshComponent->SetVisibility(false);

	if (DroppedMeshComponent)
		DroppedMeshComponent->SetVisibility(true);
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (DroppedMeshComponent)
		DroppedMeshComponent->SetVisibility(false);

	if (MeshComponent)
		MeshComponent->SetVisibility(true);
}

bool AWeapon::NormalAttackPressed()
{
	if (GetPlayer()->GetCombatComponent())
	{
		return GetPlayer()->GetCombatComponent()->CanNormalAttack();
	}
	return true;
}

bool AWeapon::NormalAttackPressing(float DeltaTime)
{
	if (GetPlayer()->GetCombatComponent())
	{
		return GetPlayer()->GetCombatComponent()->CanNormalAttack();
	}
	return true;
}

bool AWeapon::NormalAttackReleased()
{
	if (GetPlayer()->GetCombatComponent())
	{
		return GetPlayer()->GetCombatComponent()->CanNormalAttack();
	}
	return true;
}

void AWeapon::BindNormalAttackMontage(UAnimMontage* Montage)
{
	NormalAttackMontage = Montage;
}

void AWeapon::BindSkillMontage(UAnimMontage* Montage)
{
	SkillMontage = Montage;
}

void AWeapon::BindQSkillActor(TSubclassOf<class AAttackActor> AttackActor)
{
	QSkillActor = AttackActor;
}

void AWeapon::BindESkillActor(TSubclassOf<class AAttackActor> AttackActor)
{
	ESkillActor = AttackActor;
}

void AWeapon::ComboFinished()
{
	bComboFinished = true;

	if (bCanMoveWhenAttack == true)
	{
		APlayerCharacter* Player = GetPlayer();
		if (Player)
		{
			Player->WalkSpeedToOrigin();
		}

	}
}

void AWeapon::Reset()
{
	ResetCombo();
	APlayerCharacter* Player = GetPlayer();
	if (Player)
	{
		Player->WalkSpeedToOrigin();
	}
}

void AWeapon::SetMeshComponentScale(float Scale)
{
	MeshComponent->SetRelativeScale3D({ Scale,Scale,Scale });
}

void AWeapon::BringDataTableDatas()
{
	Super::BringDataTableDatas();

	if (DataTable == nullptr) return;

	FWeaponDataTable* DataRow = DataTable->FindRow<FWeaponDataTable>(RowName, FString("WeaponDataTable"));

	if (DataRow == nullptr) return;

	Mesh = DataRow->Mesh;
	
	if (MeshComponent && Mesh)
	{
		MeshComponent->SetSkeletalMesh(Mesh);
	}

	AttackDamage = DataRow->AttackDamage;
}

UAnimInstance* AWeapon::GetPlayerAnimInstance()
{
	ACharacter* Player = GetPlayer();

	if (Player == nullptr) 
		return nullptr;

	if (Player->GetMesh())
		return Player->GetMesh()->GetAnimInstance();
	else
		return nullptr;
}

APlayerCharacter* AWeapon::GetPlayer()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);

	return Cast<APlayerCharacter>(Player);
}

void AWeapon::PopFromInventory(const FVector& PopPos, float PopRadius)
{
	Super::PopFromInventory(PopPos);

	if (MeshComponent)
		MeshComponent->SetVisibility(false);
}

bool AWeapon::BeginCombo()
{
	if (IsComboFinished())
	{
		if (CurrentCombo >= MaxCombo)
		{
			ResetCombo();
		}
		CurrentCombo += 1;

		if (bCanMoveWhenAttack == true)
		{
			APlayerCharacter* Player = GetPlayer();
			if (Player)
			{
				Player->SetMaxWalkSpeed(AttackWalkSpeed);
			}
			
		}
		return true;
	}
		
	return false;
}

void AWeapon::EndCombo()
{
	bComboFinished = false;
}

bool AWeapon::IsComboFinished()
{
	return bComboFinished;
}

void AWeapon::ResetCombo()
{
	CurrentCombo = -1;
	bComboFinished = true;
}

void AWeapon::UpdateAttackDamage(AAttackActor* AttackActor, float DamageMultiplier, int DamageAdder)
{
	if (IsValid(AttackActor) == false)
	{
		return;
	}

	APlayerCharacter* Player = GetPlayer();
	if (Player)
	{
		if (Player->GetCombatComponent())
		{
			AttackActor->SetDamage(Player->GetCombatComponent()->GetWeaponDamage() * DamageMultiplier + DamageAdder);
		}
	}
}

void AWeapon::SetSkeletalMesh(USkeletalMesh* NewMesh)
{
	MeshComponent->SetSkeletalMesh(NewMesh);
}

const FDamageRange AWeapon::GetAttackDamageRange() const
{
	return AttackDamage;
}

void AWeapon::SetCanMoveWhenAttack(bool bCanMove)
{
	bCanMoveWhenAttack = bCanMove;
}

void AWeapon::SetAttackWalkSpeed(float WalkSpeed)
{
	AttackWalkSpeed = WalkSpeed;
}

void AWeapon::SetMaxCombo(uint8 ComboNum)
{
	MaxCombo = ComboNum;
}

uint8 AWeapon::GetCurrentCombo() const
{
	return CurrentCombo;
}
