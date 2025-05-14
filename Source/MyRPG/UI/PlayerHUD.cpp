
#include "PlayerHUD.h"
#include "CharacterOverlay.h"
#include "DragableWindow.h"
#include "MyRPG/Controller/MyPlayerController.h"
#include <MyRPG/Enemy/Enemy.h>

APlayerHUD::APlayerHUD()
{
	MouseCursorClass.Init(nullptr, (int32)ECursorType::ECT_MAX);
}

void APlayerHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/*if (GetPlayerController() == nullptr) return;

	for (int i = 0; i < MouseCursorClass.Num(); i++)
	{
		if (MouseCursorClass[i])
		{
			UUserWidget* NewMouse = CreateWidget<UUserWidget>(GetPlayerController(), MouseCursorClass[i]);
			MouseCursor.Add(NewMouse);
		}
	}
	ChangeCursorType(ECursorType::ECT_Crosshair);

	GetMouseCursor(ECursorType::ECT_Default)->SetDesiredSizeInViewport({ 50.f,50.f });
	GetMouseCursor(ECursorType::ECT_Crosshair)->SetDesiredSizeInViewport({ 50.f,50.f });

	CursorPivot.Add(TTuple<ECursorType, FVector2D>(ECursorType::ECT_Default, FVector2D(0.f, 0.f)));
	CursorPivot.Add(TTuple<ECursorType, FVector2D>(ECursorType::ECT_Crosshair, FVector2D(0.5f, 0.5f)));

	for (auto& Item : DragableWindowClasses)
	{
		UDragableWindow* NewWindow = CreateWidget<UDragableWindow>(GetPlayerController(), Item.Value);
		DragableWindows.Add(TTuple<EDragableWindow, UDragableWindow*>(Item.Key, NewWindow));

		if (IsValid(NewWindow))
			NewWindow->DragableWindow_Initialize();
	}*/
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetPlayerController() == nullptr) return;

	for (int i = 0; i < MouseCursorClass.Num(); i++)
	{
		if (MouseCursorClass[i])
		{
			UUserWidget* NewMouse= CreateWidget<UUserWidget>(GetPlayerController(), MouseCursorClass[i]);
			MouseCursor.Add(NewMouse);
		}
	}
	ChangeCursorType(ECursorType::ECT_Crosshair);

	GetMouseCursor(ECursorType::ECT_Default)->SetDesiredSizeInViewport({ 50.f,50.f });
	GetMouseCursor(ECursorType::ECT_Crosshair)->SetDesiredSizeInViewport({ 50.f,50.f });
	
	CursorPivot.Add(TTuple<ECursorType, FVector2D>(ECursorType::ECT_Default, FVector2D(0.f,0.f)));
	CursorPivot.Add(TTuple<ECursorType, FVector2D>(ECursorType::ECT_Crosshair, FVector2D(0.5f, 0.5f)));

	ShowCharacterOverlay();

	for (auto& Item : DragableWindowClasses)
	{
		UDragableWindow* NewWindow = CreateWidget<UDragableWindow>(GetPlayerController(), Item.Value);
		DragableWindows.Add(TTuple<EDragableWindow, UDragableWindow*>(Item.Key, NewWindow));

		AddDragableWindowInCharacterOverlay(NewWindow);

		if (IsValid(NewWindow))
			NewWindow->DragableWindow_Initialize();
	}

	
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	//SortDragableWindowZOrder();
	
}

void APlayerHUD::ShowCharacterOverlay()
{
	if (GetPlayerController() && CharacterOverlayClass)
	{
		if (CharacterOverlay == nullptr)
		{
			CharacterOverlay = CreateWidget<UCharacterOverlay>(GetPlayerController(), CharacterOverlayClass);
		}
		
		if (CharacterOverlay->IsInViewport() == false)
			CharacterOverlay->AddToViewport();

		CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void APlayerHUD::HideCharacterOverlay()
{
	if (CharacterOverlay)
	{
		UE_LOG(LogTemp, Error, TEXT("HideCharacterOverlay Succeed"));
		CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HideCharacterOverlay Failed"));
	}
}

void APlayerHUD::ChangeCursorType(ECursorType Type)
{
	if (CursorType == Type) return;

	CursorType = Type;

	for (int i = 0; i < MouseCursorClass.Num(); i++)
	{
		if (CursorType == (ECursorType)i && GetMouseCursor((ECursorType)i))
		{
			ShowCursor(CursorType);
		}
		else if (GetMouseCursor((ECursorType)i))
		{
			HideCursor((ECursorType)i);
		}
	}

	if (CursorType == ECursorType::ECT_Default)
		GetPlayerController()->DisableGameplayMouse();
	else
		GetPlayerController()->EnableGameplayMouse();
}

bool APlayerHUD::CursorUpdate(const FVector2D& MousePosition)
{
	CurMousePosition = MousePosition;
	//UE_LOG(LogTemp, Warning, TEXT("Mouse: %s"), *CurMousePosition.ToString());

	UUserWidget* Widget = GetMouseCursor(CursorType);
	if (Widget == nullptr) return false;

	FVector2D* MyPivot = CursorPivot.Find(CursorType);
	if (MyPivot == nullptr) return false;

	FVector2D WidgetSize = Widget->GetDesiredSize();

	FVector2D Padding = { WidgetSize.X * (MyPivot->X), WidgetSize.Y * (MyPivot->Y) };
	Widget->SetPositionInViewport(MousePosition - Padding);

	return true;
}

void APlayerHUD::HideCursor(ECursorType Type, bool bNextTick)
{
	ECursorType TargetType;
	if (Type == ECursorType::ECT_MAX)
		TargetType = CursorType;
	else
		TargetType = Type;
	
	if (bNextTick == false)
	{
		GetMouseCursor(TargetType)->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick
		(
			[this, TargetType]() 
			{
				GetMouseCursor(TargetType)->SetVisibility(ESlateVisibility::Hidden);
			}
		);
	}
}

void APlayerHUD::ShowCursor(ECursorType Type, bool bNextTick)
{
	ECursorType TargetType;
	if (Type == ECursorType::ECT_MAX)
		TargetType = CursorType;
	else
		TargetType = Type;

	if (GetMouseCursor(TargetType)->IsInViewport() == false)
		GetMouseCursor(TargetType)->AddToViewport(1000);

	if (bNextTick == false)
	{
		GetMouseCursor(TargetType)->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		GetWorldTimerManager().SetTimer(ShowCursorTimer,
			[this, TargetType]()
			{
				UE_LOG(LogTemp, Warning, TEXT("SSSSSSSSSShow Cursor"));
				GetMouseCursor(TargetType)->SetVisibility(ESlateVisibility::HitTestInvisible);
			}, 0.05f, false);
		/*GetWorldTimerManager().SetTimerForNextTick
		(
			[this, TargetType]()
			{
				UE_LOG(LogTemp, Warning, TEXT("SSSSSSSSSShow Cursor"));
				GetMouseCursor(TargetType)->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		);*/
	}	
}

UDragableWindow* APlayerHUD::GetDragableWindow(EDragableWindow Type)
{
	bool bFound = DragableWindows.Contains(Type);
	if (bFound == false)
	{
		return nullptr;
	}

	UDragableWindow* FindWindow = nullptr;
	if (IsValid(DragableWindows[Type]) == false || DragableWindows[Type] == nullptr)
	{
		FindWindow = DragableWindowClasses.Contains(Type) ? CreateWidget<UDragableWindow>(GetPlayerController(), DragableWindowClasses[Type]) : nullptr;
		DragableWindows[Type] = FindWindow;

		UE_LOG(LogTemp, Warning, TEXT("FindWindow : %x"), FindWindow);
	}
	else
	{
		FindWindow = DragableWindows[Type];
	}
	
	return FindWindow;	
}

void APlayerHUD::ShowDragableWindow(EDragableWindow Type, bool bHideAllWindows)
{
	UDragableWindow* Window = GetDragableWindow(Type);
	if (IsValid(Window) == false || IsValid(GetPlayerController()) == false) return;

	if (bHideAllWindows)
	{
		HideAllDragableWindow(false, Type);
	}

	Window->ShowWindow();

	if (IsShowDragableWindow(Type))
		ChangeCursorType(ECursorType::ECT_Default);

	/* 아래는 지우기 */
	
}

void APlayerHUD::HideDragableWindow(EDragableWindow Type)
{
	UDragableWindow* Window = GetDragableWindow(Type);

	if (Window)
	{
		Window->HideWindow();
	}

	if (IsShowAnyDragableWindow() == false)
	{
		ChangeCursorType(ECursorType::ECT_Crosshair);
	}

}

void APlayerHUD::HideAllDragableWindow(bool bEnableGameplayMouse, EDragableWindow ExceptType)
{
	if (bEnableGameplayMouse)
	{
		ChangeCursorType(ECursorType::ECT_Crosshair);
	}

	for (auto& Item : DragableWindows)
	{
		if (Item.Value == nullptr || 
			IsValid(Item.Value) == false)
		{
			Item.Value = GetDragableWindow(Item.Key);
		}

		if (ExceptType != Item.Key)
			Item.Value->HideWindow();
	}
}

void APlayerHUD::ShowEnemyNameLevelHP(bool bState, AEnemy* Enemy)
{
	if (IsValid(CharacterOverlay))
	{
		CharacterOverlay->ShowEnemyNameLevelHP(bState, Enemy);
	}
	
}

void APlayerHUD::RegisterFacilityWindow(UDragableWindow* FacilityWindow)
{
	if (IsValid(FacilityWindow) == false)
	{
		return;
	}

	TSubclassOf<UDragableWindow>* FindClass = DragableWindowClasses.Find(EDragableWindow::EDW_Facility);

	if (FindClass == nullptr)
	{
		DragableWindowClasses.Add(TTuple<EDragableWindow, TSubclassOf<UDragableWindow>>(EDragableWindow::EDW_Facility, FacilityWindow->GetClass()));
	}
	else
	{
		DragableWindowClasses[EDragableWindow::EDW_Facility] = FacilityWindow->GetClass();
	}

	UDragableWindow** FindWindow = DragableWindows.Find(EDragableWindow::EDW_Facility);

	if (FindWindow == nullptr)
	{
		DragableWindows.Add(TTuple<EDragableWindow, UDragableWindow*> (EDragableWindow::EDW_Facility, FacilityWindow));
	}
	else
	{
		DragableWindows[EDragableWindow::EDW_Facility] = FacilityWindow;
	}
}

void APlayerHUD::AddDragableWindowInCharacterOverlay(UDragableWindow* InDragableWindow)
{
	if (IsValid(CharacterOverlay))
	{
		CharacterOverlay->AddDragableWindow(InDragableWindow);
	}
}

AMyPlayerController* APlayerHUD::GetPlayerController()
{
	return PlayerController = (IsValid(PlayerController) == false || PlayerController == nullptr)? Cast<AMyPlayerController>(GetOwningPlayerController()) : PlayerController;
}

UUserWidget* APlayerHUD::GetMouseCursor(ECursorType Type)
{
	if (MouseCursor[(uint32)Type] == nullptr || 
		IsValid(MouseCursor[(uint32)Type]) == false)
	{
		MouseCursor[(uint32)Type] = CreateWidget<UUserWidget>(GetPlayerController(), MouseCursorClass[(uint32)Type]); 
	}

	return MouseCursor[(uint32)Type];
	
}

void APlayerHUD::SortDragableWindowZOrder()
{
	EDragableWindow SelectedKey = EDragableWindow::EDW_None;
	float MinDistance = 999999.f;

	for (auto& Item : DragableWindows)
	{
		if (IsValid(Item.Value) == false || Item.Value->IsShow() == false)
		{
			continue;
		}

		float Distance = FVector2D::Distance(CurMousePosition, Item.Value->GetWindowPosition());
		if (MinDistance >= Distance)
		{
			MinDistance = Distance;
			SelectedKey = Item.Key;
		}
	}

	if (SelectedKey != EDragableWindow::EDW_None)
	{
		for (auto& Item : DragableWindows)
		{
			if (IsValid(Item.Value) == false || Item.Value->IsShow() == false)
			{
				continue;
			}

			if (SelectedKey == Item.Key)
			{
				/*AMyPlayerController* Controller = Cast<AMyPlayerController>(GetOwningPlayerController());
				if (IsValid(Controller))
				{
					Controller->ChangeFocusWidget(Item.Value);
				}*/
				Item.Value->SetZOrder(10);
				UE_LOG(LogTemp, Warning, TEXT("CurDragable => %s"), *Item.Value->GetFName().ToString());
			}
			else
			{
				Item.Value->SetZOrder(0);
			}
		}
	}
}

bool APlayerHUD::IsShowDragableWindow(EDragableWindow Type)
{
	UDragableWindow* Window = GetDragableWindow(Type);

	if (Window == nullptr) return false;

	return Window->IsShow();
}

bool APlayerHUD::IsShowAnyDragableWindow()
{
	bool result = false;
	for (auto Window : DragableWindows)
	{
		if (Window.Value)
		{
			result = Window.Value->IsShow();
			if (result) break;
		}
	}

	return result;
}
