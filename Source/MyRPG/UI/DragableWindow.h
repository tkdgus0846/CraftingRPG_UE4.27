
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragableWindow.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UDragableWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowWindow();

	UFUNCTION()
	void ShowWindow_Implementation() { }

	UFUNCTION(BlueprintNativeEvent)
	void HideWindow();

	UFUNCTION()
	void HideWindow_Implementation() { }

	UFUNCTION(BlueprintNativeEvent)
	void DragableWindow_Initialize();

	UFUNCTION()
	void DragableWindow_Initialize_Implementation() { }

	void SetZOrder(int ZOrder);
	FVector2D GetWindowPosition();

protected:
	UPROPERTY(BlueprintReadWrite)
	bool bIsWindowShow = false;

private:
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	class UCanvasPanelSlot* WindowPanelSlot;

	UPROPERTY(EditDefaultsOnly)
	FVector2D InitPos;

	UPROPERTY(EditDefaultsOnly)
	FVector2D InitSize;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<class AFacility> OwnerFacility = nullptr;
	
public:
	FORCEINLINE const bool IsShow() const { return bIsWindowShow; }
	FORCEINLINE const FVector2D GetInitPos() const { return InitPos; }
	FORCEINLINE const FVector2D GetInitSize() const { return InitSize; }
	FORCEINLINE void SetWindowPanelSlot(class UCanvasPanelSlot* InSlot) { WindowPanelSlot = InSlot; }
	FORCEINLINE void SetOwnerFacility(TWeakObjectPtr<class AFacility> NewFacility) { OwnerFacility = NewFacility; }
};
