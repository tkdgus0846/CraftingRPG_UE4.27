
#include "DragableWindow.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


void UDragableWindow::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("DragableWindow Construct"));
}

void UDragableWindow::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Warning, TEXT("DragableWindow Destruct"));
}

void UDragableWindow::SetZOrder(int ZOrder)
{
	/*UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(RootCanvas->Slot);

	if (IsValid(CanvasPanelSlot))
	{
		CanvasPanelSlot->SetZOrder(ZOrder);
		RootCanvas->InvalidateLayoutAndVolatility();
	}

	UCanvasPanelSlot* WindowPanelSlot = Cast<UCanvasPanelSlot>(WindowPanel->Slot);

	if (IsValid(WindowPanelSlot))
	{
		WindowPanelSlot->SetZOrder(ZOrder);
		WindowPanel->InvalidateLayoutAndVolatility();
	}*/

	/*RemoveFromParent();
	AddToViewport(ZOrder);*/
}

FVector2D UDragableWindow::GetWindowPosition()
{
	if (IsValid(WindowPanelSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("Window: %s"), *WindowPanelSlot->GetPosition().ToString());
		return WindowPanelSlot->GetPosition();
	}
	return FVector2D();
}
