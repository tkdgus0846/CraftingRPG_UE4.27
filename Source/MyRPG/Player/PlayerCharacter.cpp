
#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Components/CombatComponent.h"
#include "Components/Inventory.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyRPG/Controller/MyPlayerController.h"
#include "Camera/CameraActor.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "Components/MouseComponent.h"
#include "Components/InteractComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MyRPG/Types/ChannelType.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/BaseClasses/FadeOutActor.h>
#include "MyRPG/Interfaces/Interactable.h"
#include "MyRPG/Data/DamageInfo.h"
#include "MyRPG/UI/FloatingTextWidget.h"
#include "MyRPG/UI/EscapeWidget.h"
#include "Components/WidgetComponent.h"
#include "MyRPG/SaveGame/PlayerSave.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	if (SpringArmComponent)
	{
		SpringArmComponent->SetupAttachment(RootComponent);

		SpringArmComponent->bUsePawnControlRotation = false;
		SpringArmComponent->bInheritYaw = false;
	}

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	if (CameraComponent)
	{
		CameraComponent->SetupAttachment(SpringArmComponent);
	}

	Aim_SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("AimCameraBoom");
	if (Aim_SpringArmComponent)
	{
		Aim_SpringArmComponent->SetupAttachment(GetMesh());

		Aim_SpringArmComponent->bUsePawnControlRotation = false;
		Aim_SpringArmComponent->bInheritYaw = false;

		Aim_SpringArmComponent->SetRelativeLocation({ 0.f,0.f,80.f });
		Aim_SpringArmComponent->SetRelativeRotation(FRotator(0.f, 45.f, 0.f));
		Aim_SpringArmComponent->TargetArmLength = 200.f;
	}	

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("Combat");
	if (IsValid(CombatComponent))
	{
		CombatComponent->OnChangeWeapon.AddDynamic(this, &APlayerCharacter::ChangeWeaponMesh);
	}

	Inventory = CreateDefaultSubobject<UInventory>("Inventory");

	InteractDetectionSphere = CreateDefaultSubobject<USphereComponent>("InteractDetection");

	if (InteractDetectionSphere)
	{
		InteractDetectionSphere->SetupAttachment(RootComponent);
	}

	MouseComponent = CreateDefaultSubobject<UMouseComponent>("MouseComponent");

	InteractComponent = CreateDefaultSubobject<UInteractComponent>("InteractComponent");

	/* 오리지널 메쉬들 */
	CustomHeadMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Head");
	if (CustomHeadMeshComponent)
	{
		CustomHeadMeshComponent->SetupAttachment(GetMesh(), "Head_JNT");
	}

	/* VFX 메쉬들 */
	/*VFXMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FXMesh");
	if (VFXMeshComponent)
	{
		VFXMeshComponent->SetupAttachment(RootComponent);
	}*/

	VFXHeadMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HeadFXMesh");
	if (VFXHeadMeshComponent)
	{
		VFXHeadMeshComponent->SetupAttachment(VFXMeshComponent, "Head_JNT");
	}

	OriginLeftWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("OriginLWeapon");
	if (OriginLeftWeaponComponent)
	{
		OriginLeftWeaponComponent->SetupAttachment(GetMesh(), "Weapon02_JNT");
		OriginLeftWeaponComponent->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
	}
	
	OriginRightWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("OriginRWeapon");
	if (OriginRightWeaponComponent)
	{
		OriginRightWeaponComponent->SetupAttachment(GetMesh(), "Weapon01_JNT");
		OriginRightWeaponComponent->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
	}

	VFXLeftWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FXLWeapon");
	if (VFXLeftWeaponComponent)
	{
		VFXLeftWeaponComponent->SetupAttachment(GetMesh(), "Weapon02_JNT");
		VFXLeftWeaponComponent->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
	}

	VFXRightWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("FXRWeapon");
	if (VFXRightWeaponComponent)
	{
		VFXRightWeaponComponent->SetupAttachment(GetMesh(), "Weapon01_JNT");
		VFXRightWeaponComponent->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
	}

	bUseControllerRotationYaw = false;
}

void APlayerCharacter::ChangeCamera(ECameraActor CameraType, float BlendTime, EViewTargetBlendFunction BlendFunc, float BlendExp, bool bLockOutgoing)
{
	if (GetPlayerController() && GetCameraActor(CameraType))
	{
		GetPlayerController()->SetViewTargetWithBlend(GetCameraActor(CameraType), BlendTime, BlendFunc, BlendExp, bLockOutgoing);
	}
}

void APlayerCharacter::Damaged(FDamageInfo* DamageInfo)
{
	Super::Damaged(DamageInfo);

	HitReact();
	
	//if (GetDead()) return;

	//if (CombatComponent)
	//{
	//	// 일정시간동안 기본공격을 못하게 막아버려야함.
	//	CombatComponent->NormalAttackCancel();
	//}
}

void APlayerCharacter::SetWeaponVisiblity(bool bVisibility)
{
	OriginLeftWeaponComponent->SetVisibility(bVisibility);
	OriginRightWeaponComponent->SetVisibility(bVisibility);
	/*VFXLeftWeaponComponent->SetVisibility(bVisibility);
	VFXRightWeaponComponent->SetVisibility(bVisibility);*/
}

void APlayerCharacter::SpawnSkillCooltimeUI(float RemainCooltime)
{
	UWidgetComponent* WidgetComponent = NewObject<UWidgetComponent>(this);
	WidgetComponent->SetWidgetClass(GetFloatingTextWidgetClass());
	WidgetComponent->SetDrawSize(FVector2D(300, 300));
	WidgetComponent->RegisterComponent();
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WidgetComponent->SetWorldLocation(GetActorLocation());

	UFloatingTextWidget* Widget = Cast<UFloatingTextWidget>(WidgetComponent->GetWidget());
	if (IsValid(Widget))
	{
		Widget->SetText(FString::Printf(TEXT("%.1fs"), RemainCooltime), FLinearColor::Red);
	}

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(
		Timer,
		FTimerDelegate::CreateLambda([WidgetComponent]()
			{
				if (IsValid(WidgetComponent))
				{
					WidgetComponent->DestroyComponent();
					UE_LOG(LogTemp, Warning, TEXT("DestoryComponent!"));
				}
			}),
		1.5f,
		false
	);
}

void APlayerCharacter::LoadPlayer(UPlayerSave* PlayerSave)
{
	if (IsValid(PlayerSave) == false)
	{
		return;
	}
	
	SetHP(PlayerSave->HP);
	SetActorTransform(PlayerSave->Transform);
	SetActorLocation(PlayerSave->Transform.GetLocation());

	if (IsValid(Inventory))
	{
		Inventory->ClearItemArray();
		Inventory->SetNewItemArray(PlayerSave->ItemData);
	}

	SetZoomLength(PlayerSave->LastInterpZoomLength);
	SetSpringArmRelativeRotation(PlayerSave->LastSpringArmRelativeRotation);
}

void APlayerCharacter::ClearPlayer(const FVector& ClearLocation)
{
	SetActorLocation(ClearLocation);
	if (IsValid(Inventory))
	{
		Inventory->ClearItemArray();
	}

	ClearSpringArmSetting();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpringArmComponent)
	{
		ClearSpringArmSetting();

		ACameraActor* AimCameraActor = GetWorld()->SpawnActor<ACameraActor>();
		if (AimCameraActor == nullptr) return;

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
		AimCameraActor->AttachToComponent(Aim_SpringArmComponent, AttachmentRules);
		AimCameraActor->SetActorRelativeLocation({0.f,100.f,0.f});
		AimCameraActor->SetActorRotation(FRotator(0.f, 0.f, 0.f));
		AimCameraActor->GetCameraComponent()->bConstrainAspectRatio = false;

		SetCameraActor(ECameraActor::ECA_Aim, AimCameraActor);

	}

	if (InteractDetectionSphere && InteractComponent)
	{
		InteractDetectionSphere->OnComponentBeginOverlap.AddDynamic(InteractComponent, &UInteractComponent::OnOverlapBegin);
		InteractDetectionSphere->OnComponentEndOverlap.AddDynamic(InteractComponent, &UInteractComponent::OnOverlapEnd);

	}

	auto Comp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (Comp)
	{
		OriginWalkSpeed = Comp->MaxWalkSpeed;
		OriginAcceleration = Comp->MaxAcceleration;
	}

	GetWorld()->GetTimerManager().SetTimer(FadeOutTimer, this, &APlayerCharacter::FadeOutBetweenCamera, 0.1f, true);	

	ShowHpBar(true);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/* 오리진 메쉬 */
	RegisterMesh(CustomHeadMeshComponent, true, false);
	RegisterMesh(OriginLeftWeaponComponent, true, false);
	RegisterMesh(OriginRightWeaponComponent, true, false);

	/* FX 메쉬 */
	//RegisterMesh(VFXMeshComponent, false, true);
	RegisterMesh(VFXHeadMeshComponent, false, false);
	RegisterMesh(VFXLeftWeaponComponent, false, false);
	RegisterMesh(VFXRightWeaponComponent, false, false);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PreventFallLedge(DeltaTime);
	InterpCameraZoom(DeltaTime);
	//CalcIKOffsets(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::AttackPressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &APlayerCharacter::AttackReleased);
	PlayerInputComponent->BindAction("CameraRotate", IE_Pressed, this, &APlayerCharacter::CameraRotatePressed);
	PlayerInputComponent->BindAction("CameraRotate", IE_Released, this, &APlayerCharacter::CameraRotateReleased);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::InteractPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &APlayerCharacter::InteractReleased);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &APlayerCharacter::InventoryOpenPressed);
	PlayerInputComponent->BindAction("Cancel", IE_Pressed, this, &APlayerCharacter::CancelPressed);

	PlayerInputComponent->BindAction("Press1", IE_Pressed, this, &APlayerCharacter::Press1);
	PlayerInputComponent->BindAction("Press2", IE_Pressed, this, &APlayerCharacter::Press2);
	PlayerInputComponent->BindAction("Press3", IE_Pressed, this, &APlayerCharacter::Press3);
	PlayerInputComponent->BindAction("Press4", IE_Pressed, this, &APlayerCharacter::Press4);
	PlayerInputComponent->BindAction("Press5", IE_Pressed, this, &APlayerCharacter::Press5);
	PlayerInputComponent->BindAction("Press6", IE_Pressed, this, &APlayerCharacter::Press6);
	PlayerInputComponent->BindAction("Press7", IE_Pressed, this, &APlayerCharacter::Press7);
	PlayerInputComponent->BindAction("Press8", IE_Pressed, this, &APlayerCharacter::Press8);
	PlayerInputComponent->BindAction("Press9", IE_Pressed, this, &APlayerCharacter::Press9);

	PlayerInputComponent->BindAction("SpaceBar", IE_Pressed, this, &APlayerCharacter::SpaceBarPressed);
	PlayerInputComponent->BindAction("SpaceBar", IE_Released, this, &APlayerCharacter::SpaceBarReleased);
	PlayerInputComponent->BindAction("Q", IE_Pressed, this, &APlayerCharacter::QPressed);
	PlayerInputComponent->BindAction("Q", IE_Released, this, &APlayerCharacter::QReleased);
	PlayerInputComponent->BindAction("E", IE_Pressed, this, &APlayerCharacter::EPressed);
	PlayerInputComponent->BindAction("E", IE_Released, this, &APlayerCharacter::EReleased);
	PlayerInputComponent->BindAction("R", IE_Pressed, this, &APlayerCharacter::RPressed);
	PlayerInputComponent->BindAction("R", IE_Released, this, &APlayerCharacter::RReleased);
	PlayerInputComponent->BindAction("C", IE_Pressed, this, &APlayerCharacter::CPressed);
	PlayerInputComponent->BindAction("C", IE_Released, this, &APlayerCharacter::CReleased);
	PlayerInputComponent->BindAction("Leap", IE_Pressed, this, &APlayerCharacter::ShiftPressed);
	PlayerInputComponent->BindAction("Leap", IE_Released, this, &APlayerCharacter::ShiftReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("Zoom", this, &APlayerCharacter::Zoom);
}

void APlayerCharacter::HitReact_Implementation()
{
	if (GetMesh())
	{
		if (GetMesh()->GetAnimInstance())
			GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false);
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	//if (CanMoveWhenAttack() == false) return;
	if (IsActionPlaying()) return;

	
	if (CameraComponent)
	{
		const FRotator YawRotation(0.f, CameraComponent->GetForwardVector().Rotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		if (GetIsMounted() == false)
			AddMovementInput(Direction, Value);
		else if (IsValid(Riding))
			Riding->AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	//if (CanMoveWhenAttack() == false) return;
	if (IsActionPlaying()) return;

	if (CameraComponent)
	{
		const FRotator YawRotation(0.f, CameraComponent->GetForwardVector().Rotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		if (GetIsMounted() == false)
			AddMovementInput(Direction, Value);
		else if (IsValid(Riding))
			Riding->AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	if (GetPlayerController()->IsCameraRotate() == false) return;

	if (SpringArmComponent)
	{
		FRotator Rotation = SpringArmComponent->GetRelativeRotation();
		Rotation.Yaw -= Value;
		SpringArmComponent->SetRelativeRotation(Rotation);
	}

	if (Aim_SpringArmComponent)
	{
		FRotator Rotation = Aim_SpringArmComponent->GetRelativeRotation();
		Rotation.Yaw -= Value;
		Aim_SpringArmComponent->SetRelativeRotation(Rotation);
	}
}

void APlayerCharacter::LookUp(float Value)
{
	if (GetPlayerController()->IsCameraRotate() == false) return;

	if (SpringArmComponent)
	{
		FRotator Rotation = SpringArmComponent->GetRelativeRotation();

		if (Rotation.Pitch + Value > MaxAngle)
			Rotation.Pitch = MaxAngle;
		else if (Rotation.Pitch + Value < MinAngle)
			Rotation.Pitch = MinAngle;
		else
			Rotation.Pitch += Value;
		
		SpringArmComponent->SetRelativeRotation(Rotation);
	}

	if (Aim_SpringArmComponent)
	{
		FRotator Rotation = Aim_SpringArmComponent->GetRelativeRotation();


		if (Rotation.Pitch + Value > MaxAngle || Rotation.Pitch + Value < MinAngle)
			return;

		Rotation.Pitch += Value;
		Aim_SpringArmComponent->SetRelativeRotation(Rotation);
	}
}

void APlayerCharacter::Zoom(float Value)
{
	if (IsEnabledGameplayMouse() == false)
		return;

	if (SpringArmComponent)
	{
		float NewArmLength = TargetZoomLength - Value * ZoomSpeed;

		if (NewArmLength > MaxZoom)
			NewArmLength = MaxZoom;
		else if (NewArmLength < MinZoom)
			NewArmLength = MinZoom;

		TargetZoomLength = NewArmLength;
	}
}

void APlayerCharacter::AttackPressed()
{
	if (IsActionPlaying()) return;
	if (CombatComponent == nullptr ||
		IsEnabledGameplayMouse() == false) return;
	
	CombatComponent->AttackPressed();

	if (MouseComponent)
	{
		MouseComponent->Pressed();
	}
}

void APlayerCharacter::AttackReleased()
{
	if (CombatComponent == nullptr) return;

	CombatComponent->AttackReleased();

	if (MouseComponent)
	{
		MouseComponent->Released();
	}
}

void APlayerCharacter::CameraRotatePressed()
{
	if (CombatComponent == nullptr ||
		IsEnabledGameplayMouse() == false) return;

	if (MouseComponent)
		MouseComponent->AimPreviewTraceMouse(false);

	GetPlayerController()->CameraRotatePressed();
}

void APlayerCharacter::CameraRotateReleased()
{
	if (CombatComponent == nullptr) return;

	if (MouseComponent)
		MouseComponent->AimPreviewTraceMouse(true);

	GetPlayerController()->CameraRotateReleased();
}

void APlayerCharacter::CancelPressed()
{
	if (GetPlayerController()->IsShowAnyDragableWindow())
	{
		GetPlayerController()->CancelPressed();
		if (InteractComponent)
		{
			InteractComponent->FinishFacilityInteract();
		}
	}
	else
	{
		OpenAndCloseEscapeWidget();
	}
	
	
}

void APlayerCharacter::InteractPressed()
{
	if (IsValid(InteractComponent) == false) return;

	if (bIsMounted && InteractComponent->IsAnyOverlapInteractables() == false)
	{
		IInteractable::Execute_Interact(Riding);
	}
	else
	{
		InteractComponent->Interact();
	}
}

void APlayerCharacter::InteractReleased()
{
}

void APlayerCharacter::InventoryOpenPressed()
{
	GetPlayerController()->InventoryOpenPressed();
}

void APlayerCharacter::Press1()
{
	if (Inventory)
		Inventory->SelectItem(1);
}

void APlayerCharacter::Press2()
{
	if (Inventory)
		Inventory->SelectItem(2);
}

void APlayerCharacter::Press3()
{
	if (Inventory)
		Inventory->SelectItem(3);
}

void APlayerCharacter::Press4()
{
	if (Inventory)
		Inventory->SelectItem(4);
}

void APlayerCharacter::Press5()
{
	if (Inventory)
		Inventory->SelectItem(5);
}

void APlayerCharacter::Press6()
{
	if (Inventory)
		Inventory->SelectItem(6);
}

void APlayerCharacter::Press7()
{
	if (Inventory)
		Inventory->SelectItem(7);
}

void APlayerCharacter::Press8()
{
	if (Inventory)
		Inventory->SelectItem(8);
}

void APlayerCharacter::Press9()
{
	if (Inventory)
		Inventory->SelectItem(9);
}

void APlayerCharacter::SpaceBarPressed()
{
	if (CombatComponent)
		CombatComponent->DashPressed();
}

void APlayerCharacter::SpaceBarReleased()
{
	if (CombatComponent)
		CombatComponent->DashReleased();
}

void APlayerCharacter::QPressed()
{
	if (CombatComponent)
		CombatComponent->QPressed();

	//SetActorLocation(GetActorLocation() + FVector{1000.f,0.f,0.f});
}

void APlayerCharacter::QReleased()
{
	if (CombatComponent)
		CombatComponent->QReleased();

	//etActorLocation(GetActorLocation() + FVector{ -10.f,0.f,0.f });
}

void APlayerCharacter::EPressed()
{
	if (CombatComponent)
		CombatComponent->EPressed();
}

void APlayerCharacter::EReleased()
{
	if (CombatComponent)
		CombatComponent->EReleased();
}

void APlayerCharacter::RPressed()
{
	if (CombatComponent)
		CombatComponent->RPressed();
}

void APlayerCharacter::RReleased()
{
	if (CombatComponent)
		CombatComponent->RReleased();
}

void APlayerCharacter::CPressed()
{
	if (CombatComponent)
		CombatComponent->CPressed();
}

void APlayerCharacter::CReleased()
{
	if (CombatComponent)
		CombatComponent->CReleased();
}

void APlayerCharacter::ShiftPressed()
{
	if (IsEnabledGameplayMouse())
	{
		if (CombatComponent)
			CombatComponent->LeapPressed();
	}
	else
	{

	}
	
}

void APlayerCharacter::ShiftReleased()
{
	
}

void APlayerCharacter::FlushBindingInputs()
{
	
}

AMyPlayerController* APlayerCharacter::GetPlayerController()
{
	return PlayerController = (IsValid(PlayerController) == false || PlayerController == nullptr) ? Cast<AMyPlayerController>(GetController()) : PlayerController;
}

AActor* APlayerCharacter::GetCameraActor(ECameraActor CameraActorType) const
{
	if (CameraActorType == ECameraActor::ECA_Main)
		return (AActor*)(this);
	else
		return CameraActor[(int8)CameraActorType];
}

void APlayerCharacter::SetCameraActor(ECameraActor CameraActorType, AActor* NewCameraActor)
{
	CameraActor[(int8)CameraActorType] = NewCameraActor;
}

bool APlayerCharacter::IsAiming() const
{
	return CombatComponent ? CombatComponent->IsAiming() : false;
}

bool APlayerCharacter::IsAttacking() const
{
	return CombatComponent ? CombatComponent->IsAttacking() : false;
}

bool APlayerCharacter::IsCameraRotate()
{
	if (GetPlayerController() == nullptr) return false;

	return GetPlayerController()->IsCameraRotate();
}

float APlayerCharacter::GetMouseDirectionAngle()
{
	if (GetPlayerController() == nullptr) return 0.f;

	return GetPlayerController()->GetMouseDirectionAngle();
}

bool APlayerCharacter::IsEnabledGameplayMouse()
{
	if (GetPlayerController() == nullptr) return false;

	return GetPlayerController()->IsEnabledGameplayMouse();
}

EWeaponType APlayerCharacter::GetCurrentSelectedWeaponType() const
{
	if (Inventory == nullptr) return EWeaponType::EWT_MAX;

	return Inventory->GetCurrentSelectedWeaponType();
}

AWeapon* APlayerCharacter::GetCurrentSelectedWeapon() const
{
	if (Inventory)
		return Inventory->GetCurrentSelectedWeapon();
	else
		return nullptr;
}

void APlayerCharacter::ChangeWeapon(EWeaponType NewWeaponType, bool bPlayEquipMontage, bool bSaveToSpare)
{
	if (CombatComponent)
	{
		CombatComponent->ChangeWeapon(NewWeaponType, bPlayEquipMontage, bSaveToSpare);
	}
}

bool APlayerCharacter::CanMoveWhenAttack()
{
	if (CombatComponent)
	{
		return CombatComponent->CanMoveWhenAttack();
	}

	return false;
}

float APlayerCharacter::GetAttackWalkSpeed()
{
	if (CombatComponent)
	{
		return CombatComponent->GetAttackWalkSpeed();
	}

	return 0.f;
}

void APlayerCharacter::SetMaxWalkSpeed(float Speed)
{
	if (bRestrictChangeWalkSpeed)
	{
		return;
	}

	auto Comp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (Comp)
		Comp->MaxWalkSpeed = Speed;
}

void APlayerCharacter::WalkSpeedToOrigin()
{
	if (bRestrictChangeWalkSpeed)
	{
		return;
	}

	SetMaxWalkSpeed(GetOriginWalkSpeed());
}

void APlayerCharacter::SetMaxAcceleration(float Acc)
{
	auto Comp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (Comp)
		Comp->MaxAcceleration = Acc;
}

void APlayerCharacter::AccelerationToOrigin()
{
	SetMaxAcceleration(OriginAcceleration);
}

void APlayerCharacter::SetIsMounted(bool bState, ACreatureCharacter* NewRiding)
{
	// 탑승해있는데 또 탑승하려는 경우나, 탑승하지않았는데 뛰어내리려는 경우를 방지
	if (bIsMounted == bState || (bState == true && IsValid(NewRiding) == false) || (bState == false && IsValid(Riding) == false))
		return;
	FVector RidingPos = (bState) ? NewRiding->GetMesh()->GetSocketLocation("MountAttachment") : Riding->GetMesh()->GetSocketLocation("MountAttachment");
	float ActionTime;

	if (bState)
	{
		if (IsValid(NewRiding) && IsValid(InteractComponent))
			InteractComponent->RemoveOverlapActor(NewRiding);
		ActionTime = MountAction(RidingPos);

		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, "MountVarSettings", bState, NewRiding);
		GetWorld()->GetTimerManager().SetTimer(MountTimerHandle, TimerDel, ActionTime, false);
	}
	else
	{
		ActionTime = UnMountAction(RidingPos);
		MountVarSettings(bState, NewRiding);
	}
}

const FRotator APlayerCharacter::GetSpringArmRelativeRotation() const
{
	if (IsValid(SpringArmComponent))
	{
		return SpringArmComponent->GetRelativeRotation();
	}
	return FRotator();
}

void APlayerCharacter::SetSpringArmRelativeRotation(FRotator Rotation)
{
	if (IsValid(SpringArmComponent))
	{
		SpringArmComponent->SetRelativeRotation(Rotation);
	}
}

void APlayerCharacter::InterpCameraZoom(float DeltaTime)
{
	if (SpringArmComponent)
	{
		InterpZoomLength = FMath::FInterpTo(InterpZoomLength, TargetZoomLength, DeltaTime, 5.f);

		SpringArmComponent->TargetArmLength = InterpZoomLength;
	}
}

void APlayerCharacter::ClearSpringArmSetting()
{
	if (IsValid(SpringArmComponent))
	{
		SpringArmComponent->TargetArmLength = MinZoom;

		InterpZoomLength = SpringArmComponent->TargetArmLength;
		TargetZoomLength = SpringArmComponent->TargetArmLength;

		SpringArmComponent->SetRelativeRotation(FRotator(MaxAngle, 0.f, 0.f));
		SpringArmComponent->SetRelativeLocation({ 0.f,0.f,0.f });
	}
	
}

void APlayerCharacter::FadeOutBetweenCamera()
{
	if (CameraComponent == nullptr) return;
	
	
//UKismetSystemLibrary::BoxTrace
	TEnumAsByte<EObjectTypeQuery> FadeMesh = UEngineTypes::ConvertToObjectType(ECC_FadeMesh);
	TEnumAsByte<EObjectTypeQuery> Creature = UEngineTypes::ConvertToObjectType(ECC_Creature);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeArr{ FadeMesh,Creature };
	TArray<FHitResult> HitResultArr;
	
	FVector Direction = (CameraComponent->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
	bool bSuccess = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), CameraComponent->GetComponentLocation(), GetActorLocation() + Direction * 50.f, FadeOutBoxExtent, CameraComponent->GetComponentRotation(), ObjectTypeArr, false, TArray<AActor*>(), EDrawDebugTrace::Type::None, HitResultArr, true);

	CurFadeOutArr.Empty();
	for (const FHitResult& HitResult : HitResultArr)
	{
		TWeakObjectPtr<UMeshComponent> Component = Cast<UMeshComponent>(HitResult.Component);
		TWeakObjectPtr<AFadeOutActor> FadeOutActor = Cast<AFadeOutActor>(HitResult.Actor);
		if (Component == nullptr || Component.IsValid() == false) continue;
		if (FadeOutActor == nullptr || FadeOutActor.IsValid() == false) continue;

		
		// 이전 배열에서 찾았는데 없을때
		if (PreviousFadeOutArr.Find(Component) == INDEX_NONE)
		{
			FadeOutActor->FadeOut(Component.Get());
		}
		

		CurFadeOutArr.Add(Component);
	}

	for (int i = 0; i < PreviousFadeOutArr.Num(); i++)
	{
		if (PreviousFadeOutArr[i].IsValid() == false)
		{
			continue;
		}

		// 현재 배열에서 이전 배열의 값을 찾았는데 없으면
		if (CurFadeOutArr.Find(PreviousFadeOutArr[i]) == INDEX_NONE)
		{
			
			TWeakObjectPtr<AFadeOutActor> FadeOutActor = Cast<AFadeOutActor>(PreviousFadeOutArr[i]->GetOwner());
			if (FadeOutActor.IsValid())
			{
				FadeOutActor->FadeIn(PreviousFadeOutArr[i].Get());
			}
		}
	}
	
	PreviousFadeOutArr = CurFadeOutArr;
}

void APlayerCharacter::ChangeWeaponMesh()
{
	if (IsValid(CombatComponent))
	{
		if (IsValid(VFXLeftWeaponComponent) == false || IsValid(VFXRightWeaponComponent) == false) return;

		USkeletalMesh* NewMesh = CombatComponent->GetCurrentWeaponSkeletalMesh();

		OriginLeftWeaponComponent->EmptyOverrideMaterials();
		OriginRightWeaponComponent->EmptyOverrideMaterials();

		OriginLeftWeaponComponent->SetSkeletalMesh(nullptr);
		OriginRightWeaponComponent->SetSkeletalMesh(nullptr);
		VFXLeftWeaponComponent->SetSkeletalMesh(nullptr);
		VFXRightWeaponComponent->SetSkeletalMesh(nullptr);

		enum EWeaponHand
		{
			Left, Right, Both, None
		};

		EWeaponHand WeaponHand = None;
		switch (CombatComponent->GetCurrentWeaponType())
		{
		case EWeaponType::EWT_Bow:
			WeaponHand = Right;
			break;
		case EWeaponType::EWT_Mace:
			WeaponHand = Left;
			break;
		case EWeaponType::EWT_Axe:
			WeaponHand = Both;
			break;
		case EWeaponType::EWT_Slasher:
			WeaponHand = Both;
			break;
		case EWeaponType::EWT_Sword:
			WeaponHand = Right;
			break;
		default:
			break;
		}

		
		if (WeaponHand == Both)
		{
			OriginLeftWeaponComponent->SetSkeletalMesh(NewMesh);
			OriginRightWeaponComponent->SetSkeletalMesh(NewMesh);

			VFXLeftWeaponComponent->SetSkeletalMesh(NewMesh);
			VFXRightWeaponComponent->SetSkeletalMesh(NewMesh);
		}
		else if (WeaponHand == Left)
		{
			OriginLeftWeaponComponent->SetSkeletalMesh(NewMesh);
			VFXLeftWeaponComponent->SetSkeletalMesh(NewMesh);
		}
		else if (WeaponHand == Right)
		{
			OriginRightWeaponComponent->SetSkeletalMesh(NewMesh);
			VFXRightWeaponComponent->SetSkeletalMesh(NewMesh);
		}
	}
}

void APlayerCharacter::MountVarSettings(bool bState, ACreatureCharacter* NewRiding)
{
	bIsMounted = bState;
	Riding = NewRiding;
}

float APlayerCharacter::MountAction(const FVector& RidingPos)
{
	if (IsValid(MountMontage) == false) return 0.f;

	FVector StartPos = GetActorLocation();
	FVector EndPos = RidingPos;
	FVector MiddlePos = FMath::Lerp(StartPos, EndPos, 0.5f);
	MiddlePos.Z = EndPos.Z + 300.f;

	FActionInfo Action;
	Action.AnimMontage = MountMontage;
	Action.bAttachToGround = false;
	Action.SectionName = "Mount";
	Action.bSweep = false;
	Action.MontagePlayRate = 1.f;

	Action.AddMovePoint(StartPos);
	Action.AddMovePoint(MiddlePos);
	Action.AddMovePoint(EndPos);

	float ActionTime = AddAction(Action);
	PlayAction();

	return ActionTime;
}

float APlayerCharacter::UnMountAction(const FVector& RidingPos)
{
	if (IsValid(MountMontage) == false) return 0.f;

	const float TraceOffset = 300.f;
	const FVector ForwardVector = FVector(FVector2D(GetActorForwardVector()), 0.f);

	FVector TraceStart = GetActorLocation() - ( ForwardVector * TraceOffset);
	FVector TraceEnd = FVector(FVector2D(TraceStart), TraceStart.Z - 300.f);

	FHitResult HitResult;

	while (true)
	{
		bool bTraceSucceed = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);


		if (bTraceSucceed) break;

		TraceStart += ForwardVector * 10.f;
		TraceEnd += ForwardVector * 10.f;
	}
	

	FVector StartPos = GetActorLocation();
	FVector EndPos = FVector(FVector2D(HitResult.ImpactPoint), HitResult.ImpactPoint.Z + GetDefaultHalfHeight());
	FVector MiddlePos = FMath::Lerp(StartPos, EndPos, 0.5f);
	MiddlePos.Z = StartPos.Z + 300.f;

	FActionInfo Action;
	Action.AnimMontage = MountMontage;
	Action.bAttachToGround = false;
	Action.SectionName = "Mount";
	Action.bSweep = false;
	Action.MontagePlayRate = -1.f;

	Action.AddMovePoint(StartPos);
	Action.AddMovePoint(MiddlePos);
	Action.AddMovePoint(EndPos);

	float ActionTime = AddAction(Action);

	FActionInfo LandingAction;
	LandingAction.AnimMontage = MountMontage;
	LandingAction.bAttachToGround = false;
	LandingAction.SectionName = "Landing";
	LandingAction.bSweep = false;
	LandingAction.PreActionMontagePlayTime = 0.1f;

	LandingAction.AddMovePoint(EndPos);
	ActionTime += AddAction(LandingAction);
	PlayAction();

	return ActionTime;
}

bool APlayerCharacter::CanInterpRotation()
{
	return Super::CanInterpRotation() && bIsMounted == false;
}

void APlayerCharacter::OpenAndCloseEscapeWidget()
{
	if (IsValid(EscapeWidget) == false)
	{
		EscapeWidget = CreateWidget<UEscapeWidget>(GetWorld(), EscapeWidgetClass);
	}

	if (EscapeWidget->IsInViewport())
	{
		EscapeWidget->FadeOut();
	}
	else
	{
		EscapeWidget->FadeIn();
	}
}

void APlayerCharacter::CalcIKOffsets(float DeltaSeconds)
{
	FIKFootTraceResult LeftResult = IKFootTrace("Left_Ankle_JNT", 200.f, true);
	FIKFootTraceResult RightResult = IKFootTrace("Right_Ankle_JNT", 200.f, true);

	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, LeftResult.FootZOffset, DeltaSeconds, 20.f);
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, RightResult.FootZOffset, DeltaSeconds, 20.f);

	FVector DiffLocation = LeftResult.HitLocation - RightResult.HitLocation;
	float IKHipOffsetTarget = FMath::Abs(DiffLocation.Z) * -1.f;

	if (FMath::Abs(DiffLocation.Z) >= 50.f)
	{
		IKHipOffsetTarget = 0.f;
	}

	IKHipOffset = FMath::FInterpTo(IKHipOffset, IKHipOffsetTarget, DeltaSeconds, 20.f);

	//UE_LOG(LogTemp, Warning, TEXT("LeftOffset: %.2f, RightOffset: %.2f HipOffset: %.2f"), IKLeftFootOffset, IKRightFootOffset, IKHipOffset);
}

const bool APlayerCharacter::GetCameraForwardVector(FVector& CameraVector)
{
	if (CameraComponent == nullptr) return false;

	CameraVector = CameraComponent->GetForwardVector();
	return true;
}

const bool APlayerCharacter::IsTravelMode() const
{
	if (CombatComponent == nullptr) return false;
	return CombatComponent->IsTravelMode();
}

FVector APlayerCharacter::GetCameraLocation()
{
	if (CameraComponent == nullptr) return FVector::ZeroVector;

	return CameraComponent->GetComponentLocation();
}

FTransform APlayerCharacter::GetCameraTransform()
{
	if (CameraComponent == nullptr) return FTransform();

	return CameraComponent->GetComponentTransform();
}

