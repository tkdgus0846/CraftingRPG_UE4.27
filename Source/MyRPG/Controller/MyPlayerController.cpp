// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyRPG/UI/PlayerHUD.h"
#include "DrawDebugHelpers.h"
#include <MyRPG/Player/PlayerCharacter.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Widgets/SViewport.h"
#include "Blueprint/UserWidget.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = Cast<APlayerHUD>(GetHUD());

	// PlayerHUD->ShowCharacterOverlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);

	SetShowMouseCursor(false);
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MouseFixWhenRotate();
	CalcMouseDirectionAngle();
	CursorPosUpdate();	
}

void AMyPlayerController::CameraRotatePressed()
{
	bCameraRotate = true;
	GetMousePosition(MouseOriginX, MouseOriginY);
	if (PlayerHUD)
		PlayerHUD->HideCursor();
}

void AMyPlayerController::CameraRotateReleased()
{
	bCameraRotate = false;

	if (IsEnabledGameplayMouse())
		SetMouseLocation(MouseOriginX, MouseOriginY);

	if (PlayerHUD)
		PlayerHUD->ShowCursor();
}

void AMyPlayerController::CancelPressed()
{
	if (PlayerHUD)
	{
		PlayerHUD->HideAllDragableWindow(true);
	}
}

bool AMyPlayerController::IsShowAnyDragableWindow()
{
	if (PlayerHUD)
	{
		return PlayerHUD->IsShowAnyDragableWindow();
	}
	return false;
}

void AMyPlayerController::InventoryOpenPressed()
{
	if (PlayerHUD)
	{
		if (PlayerHUD->IsShowDragableWindow(EDragableWindow::EDW_Inventory))
		{
			PlayerHUD->HideDragableWindow(EDragableWindow::EDW_Inventory);
		}
		else
		{
			PlayerHUD->ShowDragableWindow(EDragableWindow::EDW_Inventory, false);
		}
		
	}
}

void AMyPlayerController::MouseFixWhenRotate()
{
	if (bCameraRotate)
	{
		int32 SizeX, SizeY;
		GetViewportSize(SizeX, SizeY);
		SetMouseLocation(SizeX / 2, SizeY / 2);
	}
}

void AMyPlayerController::CalcMouseDirectionAngle()
{
	if (bCalcMouseTransform == false ||
		bCameraRotate == true) return;

	FVector2D MousePosition;
	if (GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		int32 CenterX, CenterY;
		GetViewportSize(CenterX, CenterY);
		CenterX /= 2;
		CenterY /= 2;

		FVector2D MouseDirection = MousePosition - FVector2D(CenterX, CenterY);

		float AngleInRadians = FMath::Atan2(MouseDirection.Y, MouseDirection.X);
		float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

		MouseDirectionAngle = (AngleInDegrees + 450.f);

		if (MouseDirectionAngle > 360.f)
		{
			MouseDirectionAngle -= 360.f;
		}

	}
}

void AMyPlayerController::ChangeFocusWidget(UUserWidget* InWidget)
{
	if (IsValid(InWidget) == false)
	{
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetWidgetToFocus(InWidget->TakeWidget());
	SetInputMode(InputMode);
}

void AMyPlayerController::DisableGameplayMouse()
{
	bEnableGameplayMouse = false;
}

void AMyPlayerController::EnableGameplayMouse()
{
	bEnableGameplayMouse = true;
}

void AMyPlayerController::CursorPosUpdate()
{
	
	if (PlayerHUD)
	{
		float PosX, PosY;
		GetMousePosition(PosX, PosY);
		PlayerHUD->CursorUpdate(FVector2D(PosX, PosY));
	}
}

void AMyPlayerController::PlayerInputSetting(bool bEnableInput)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	
	if (IsValid(PlayerCharacter))
	{
		UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement();
		if (IsValid(MoveComp) == false)
		{
			return;
		}

		if (bEnableInput)
		{
			MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
			PlayerCharacter->EnableInput(this);
		}
		else
		{
			FlushPressedKeys();
			MoveComp->SetMovementMode(EMovementMode::MOVE_None);

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDele;
			TimerDele.BindLambda(
				[PlayerCharacter, this]()
				{
					PlayerCharacter->DisableInput(this);
				}
			);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDele, 0.05f, false);
		}
	}
}

void AMyPlayerController::SetUIRendererState(bool bState)
{
	
}
