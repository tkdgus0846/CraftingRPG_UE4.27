

#include "MouseComponent.h"
#include "../PlayerCharacter.h"
#include "MyRPG/Controller/MyPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Engine/DecalActor.h"
#include "MyRPG/Types/ChannelType.h"
#include "Components/DecalComponent.h"
#include <MyRPG/UI/PlayerHUD.h>
#include <MyRPG/Enemy/Enemy.h>
UMouseComponent::UMouseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UMouseComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	PlayerHUD = PlayerCharacter->GetPlayerController()->GetHUD<APlayerHUD>();
	
	for (int i = 1; i < (int)EAimPreview::EAP_MAX; i++)
	{
		TSubclassOf<ADecalActor>* FindClass = AimPreviewClasses.Find((EAimPreview)i);

		if (FindClass == nullptr) continue;


		ADecalActor* Decal = Cast<ADecalActor>(GetWorld()->SpawnActor(*FindClass));

		if (Decal == nullptr) continue;

		/*FTimerHandle Timer;
		FTimerDelegate TimerDele;
		TimerDele.BindUFunction(Decal, "SetActorHiddenInGame", true);
		GetWorld()->GetTimerManager().SetTimer(Timer, TimerDele, 0.1f, false);*/
		Decal->SetActorHiddenInGame(true);
		AimPreviews.Add((EAimPreview)i, Decal);
	}

	GetWorld()->GetTimerManager().SetTimer(TraceEnemyTimer, this, &UMouseComponent::TraceEnemy, 0.5f, true);
}


void UMouseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AimPreview();
}

void UMouseComponent::Pressed()
{
	
	
	/*DrawDebugLine(GetWorld(), WorldLocation, TargetLocation, FColor::Red, false, 10.f, 0, 10.f);*/

}

void UMouseComponent::Released()
{
}

void UMouseComponent::TurnOnAimPreview(EAimPreview AimType, FVector Size, float OffsetFromCenter)
{
	// x y 교체
	float Temp = Size.Y;
	Size.Y = Size.X;
	Size.X = Temp;

	// x z 교체
	Temp = Size.Z;
	Size.Z = Size.X;
	Size.X = Temp;


	ADecalActor** Decal = AimPreviews.Find(AimType);
	if (Decal == nullptr) return;

	(*Decal)->SetActorHiddenInGame(false);
	CurAimType = AimType;
	CurAimActor = *Decal;
	CurAimActor->GetDecal()->DecalSize = Size;

	bUseAimPreview = true;
	DecalOffsetFromCenter = OffsetFromCenter;
}

void UMouseComponent::TurnOffAimPreview()
{
	if (CurAimActor == nullptr) return;

	CurAimActor->SetActorHiddenInGame(true);
	CurAimType = EAimPreview::EAP_None;
	CurAimActor = nullptr;

	bUseAimPreview = false;
}

void UMouseComponent::AimPreviewTraceMouse(bool bState)
{
	bAimPreviewTraceMouse = bState;
}

void UMouseComponent::RotateByAimPreview()
{
	if (PlayerCharacter)
	{
		FRotator NewRotation = AimPreviewRotation;
		NewRotation.Pitch = 0.f;
		NewRotation.Yaw -= 90.f;
		PlayerCharacter->SetActorRotation(NewRotation);
		PlayerCharacter->SetRotationCurrentToInterp();
	}
}

void UMouseComponent::RotatePlayerByMouseDirection()
{
	FVector WorldLocation, WorldDirection;
	PlayerCharacter->GetPlayerController()->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector TargetLocation = WorldLocation + (WorldDirection * 4000.f);
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TargetLocation, ECC_AimTrace))
	{
		FVector ImpactPoint = HitResult.ImpactPoint;

		if (PlayerCharacter)
		{
			FVector Direction = ImpactPoint - PlayerCharacter->GetActorLocation();
			Direction.Z = 0.f;
			Direction = Direction.GetSafeNormal2D();

			PlayerCharacter->SetActorRotation(Direction.Rotation());
		}
	}
}

void UMouseComponent::AimPreview()
{
	if (bUseAimPreview == false || PlayerCharacter == nullptr || PlayerCharacter->GetPlayerController() == nullptr || bAimPreviewTraceMouse == false) return;

	switch (CurAimType)
	{
	case EAimPreview::EAP_Circle:
		AimCircle();
		break;
	case EAimPreview::EAP_Line:
		AimLine();
		break;
	case EAimPreview::EAP_Sector:
		AimSector();
		break;
	default:
		break;
	}

	AimPreviewRotation.Pitch = 90.f;

	CurAimActor->SetActorLocation(AimPreviewPos);
	CurAimActor->SetActorRotation(AimPreviewRotation);
}

void UMouseComponent::AimCircle()
{
	FVector WorldLocation, WorldDirection;
	PlayerCharacter->GetPlayerController()->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector TargetLocation = WorldLocation + (WorldDirection * 4000.f);
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TargetLocation, ECC_AimTrace))
	{
		AimPreviewPos = HitResult.ImpactPoint;
		AimPreviewRotation = FRotator::ZeroRotator;
	}
}

void UMouseComponent::AimLine()
{
	FVector WorldLocation, WorldDirection;
	PlayerCharacter->GetPlayerController()->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector TargetLocation = WorldLocation + (WorldDirection * 4000.f);

	if (CurAimActor)
	{
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TargetLocation, ECC_AimTrace))
		{
			FVector ImpactPoint = HitResult.ImpactPoint;

			if (PlayerCharacter)
			{
				FVector Direction = ImpactPoint - PlayerCharacter->GetActorLocation();
				Direction = Direction.GetSafeNormal2D();

				AimPreviewRotation = Direction.Rotation();
				AimPreviewRotation.Yaw += 90.f;

				FVector PlayerLocation = PlayerCharacter->GetActorLocation();
				PlayerLocation.Z -= PlayerCharacter->GetDefaultHalfHeight();
				
				AimPreviewPos = PlayerLocation + Direction * DecalOffsetFromCenter;
			}
			
		}
	}
}

void UMouseComponent::AimSector()
{
	FVector WorldLocation, WorldDirection;
	PlayerCharacter->GetPlayerController()->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector TargetLocation = WorldLocation + (WorldDirection * 4000.f);

	if (CurAimActor)
	{
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TargetLocation, ECC_AimTrace))
		{
			FVector ImpactPoint = HitResult.ImpactPoint;

			if (PlayerCharacter)
			{
				FVector Direction = ImpactPoint - PlayerCharacter->GetActorLocation();
				Direction = Direction.GetSafeNormal2D();

				AimPreviewRotation = Direction.Rotation();
				AimPreviewRotation.Yaw += 90.f;

				FVector PlayerLocation = PlayerCharacter->GetActorLocation();
				PlayerLocation.Z -= PlayerCharacter->GetDefaultHalfHeight();

				AimPreviewPos = PlayerLocation + Direction * DecalOffsetFromCenter;
			}

			UE_LOG(LogTemp, Warning, TEXT("Aim: %s"), *HitResult.Actor->GetFName().ToString());
		}
	}
}

void UMouseComponent::TraceEnemy()
{
	if (IsValid(PlayerCharacter) == false || IsValid(PlayerCharacter->GetPlayerController()) == false) return;


	FHitResult HitResult;
	FVector WorldLocation, WorldDirection;
	PlayerCharacter->GetPlayerController()->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FVector TargetLocation = WorldLocation + (WorldDirection * 4000.f);

	AEnemy* TraceEnemy = nullptr;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TargetLocation, ECC_EnemyTrace))
	{
		TraceEnemy = Cast<AEnemy>(HitResult.Actor);
		TraceEnemy = (IsValid(TraceEnemy) && TraceEnemy->GetDead()) ? nullptr : TraceEnemy;
	}

	ShowEnemyNameLevelHP(IsValid(TraceEnemy), TraceEnemy);
}

void UMouseComponent::ShowEnemyNameLevelHP(bool bState, AEnemy* Enemy)
{
	if (IsValid(PlayerHUD))
	{
		PlayerHUD->ShowEnemyNameLevelHP(bState, Enemy);
	}
		
}

FVector UMouseComponent::GetAimPreviewPos()
{
	return AimPreviewPos;
}

