
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UENUM(BlueprintType)
enum class ECursorType : uint8
{
	ECT_Default		UMETA(DisplayName = "Default"),
	ECT_Crosshair	UMETA(DisplayName = "Crosshair"),

	ECT_MAX			UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class EDragableWindow : uint8
{
	EDW_None UMETA(DisplayName = "None"),
	EDW_Inventory UMETA(DisplayName = "Inventory"),
	EDW_Option UMETA(DisplayName = "Option"),
	EDW_Build UMETA(DisplayName = "Build"),
	EDW_Progress UMETA(DisplayName = "Progress"),
	EDW_Map UMETA(DisplayName = "Map"),
	EDW_Journal UMETA(DisplayName = "Journal"),
	EDW_Facility UMETA(DisplayName = "Facility"),

	EDW_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class MYRPG_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	APlayerHUD();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable)
	void ShowCharacterOverlay();
	UFUNCTION(BlueprintCallable)
	void HideCharacterOverlay();

	UFUNCTION(BlueprintCallable)
	void ChangeCursorType(ECursorType Type);
	UFUNCTION(BlueprintCallable)
	bool CursorUpdate(const FVector2D& MousePosition);

	UFUNCTION(BlueprintCallable)
	void HideCursor(ECursorType Type = ECursorType::ECT_MAX, bool bNextTick = false);
	UFUNCTION(BlueprintCallable)
	void ShowCursor(ECursorType Type = ECursorType::ECT_MAX, bool bNextTick = false);

	UFUNCTION(BlueprintCallable)
	class UDragableWindow* GetDragableWindow(EDragableWindow Type);

	UFUNCTION(BlueprintCallable)
	void ShowDragableWindow(EDragableWindow Type, bool bHideAllWindows = true);

	UFUNCTION(BlueprintCallable)
	void HideDragableWindow(EDragableWindow Type);

	UFUNCTION(BlueprintCallable)
	void HideAllDragableWindow(bool bEnableGameplayMouse = true, EDragableWindow ExceptType = EDragableWindow::EDW_MAX);

	void ShowEnemyNameLevelHP(bool bState, class AEnemy* Enemy);

	UFUNCTION(BlueprintCallable)
	void RegisterFacilityWindow(class UDragableWindow* FacilityWindow);

	UFUNCTION()
	void AddDragableWindowInCharacterOverlay(class UDragableWindow* InDragableWindow);

protected:

private:
	UPROPERTY()
	class AMyPlayerController* PlayerController = nullptr;
	class AMyPlayerController* GetPlayerController();

	UPROPERTY()
	FTimerHandle ShowCursorTimer;

	UPROPERTY(EditAnywhere, Category = "MouseCursor")
	TArray<TSubclassOf<class UUserWidget>> MouseCursorClass;

	UPROPERTY()
	TArray<class UUserWidget*> MouseCursor;
	class UUserWidget* GetMouseCursor(ECursorType Type);

	ECursorType CursorType;
	TMap<ECursorType, FVector2D> CursorPivot;
	FVector2D CurMousePosition;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<EDragableWindow, TSubclassOf<class UDragableWindow>> DragableWindowClasses;

	UPROPERTY()
	TMap<EDragableWindow, class UDragableWindow*> DragableWindows;

	UFUNCTION()
	void SortDragableWindowZOrder();

public:
	bool IsShowDragableWindow(EDragableWindow Type);
	bool IsShowAnyDragableWindow();
	
	
	
};
