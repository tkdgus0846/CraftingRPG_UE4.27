
#include "SaveSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include <MyRPG/GameModes/MyRPGGameModeBase.h>
#include <MyRPG/SaveGame/PlayerSave.h>
#include <Kismet/GameplayStatics.h>
#include "MyRPG/FunctionLibrary/RenderTargetFunctionLibrary.h"

void USaveSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(SlotButton))
	{
		SlotButton->OnClicked.AddDynamic(this, &USaveSlotWidget::OnSlotButtonClicked);
	}
	URenderTargetFunctionLibrary::ExportHDRToRenderTarget(GetWorld(), SlotName, "/Game/Assets/Texture2D/RenderTarget", SlotName);
}

void USaveSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UPlayerSave* PlayerSave = Cast<UPlayerSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (IsValid(PlayerSave))
	{
		SetPlayTimeText(PlayerSave->LastPlayGameTime);

		if (IsValid(Switcher))
		{
			Switcher->SetActiveWidgetIndex(1);
		}
	}
	else
	{
		if (IsValid(Switcher))
		{
			Switcher->SetActiveWidgetIndex(0);
		}
	}
	
}

void USaveSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

}

void USaveSlotWidget::SetPlayTimeText(float GameplayTime)
{
	if (IsValid(GamePlayTimeText))
	{
		GamePlayTimeText->SetText(FText::FromString(CalcTime(GameplayTime)));
	}
}

const bool USaveSlotWidget::IsSlotDataEmpty()
{
	if (IsValid(Switcher))
	{
		int CurrentIndex = Switcher->GetActiveWidgetIndex();
		return (CurrentIndex == 1) ? false : true;
	}
	return true;
}

void USaveSlotWidget::SelectAppearanceSlot(bool bSelect)
{
	if (IsValid(SlotButton) == false)
	{
		return;
	}

	if (bSelect)
	{
		FLinearColor Color = SlotButton->ColorAndOpacity;
		Color = FLinearColor(1.f, 1.f, 1.f);
		SlotButton->SetColorAndOpacity(Color);

		FSlateBrush SlateBrush;
		SlateBrush.TintColor = FSlateColor(FLinearColor(1.f, 0.f, 0.f));
		SlotButton->WidgetStyle.SetNormal(SlateBrush);
		SlotButton->WidgetStyle.SetHovered(SlateBrush);
		SlotButton->WidgetStyle.SetPressed(SlateBrush);
	}
	else
	{
		FLinearColor Color = SlotButton->ColorAndOpacity;
		Color = FLinearColor(0.4f, 0.4f, 0.4f);
		SlotButton->SetColorAndOpacity(Color);

		FSlateBrush SlateBrush;
		SlateBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f));
		SlotButton->WidgetStyle.SetNormal(SlateBrush);
		SlotButton->WidgetStyle.SetHovered(SlateBrush);
		SlotButton->WidgetStyle.SetPressed(SlateBrush);
	}
	
}

FString USaveSlotWidget::CalcTime(const float GameplayTime)
{
	//00:00:00
	int Sep = 3600;
	int GameTime = (int)GameplayTime;
	FString ResultString = "";

	for (int i = 0; i < 3; i++)
	{
		int TimeInfo = (GameTime / Sep);
		GameTime = GameTime - (Sep * TimeInfo);
		Sep /= 60;

		FString TimeInfoString = FString::FromInt(TimeInfo);
		if (TimeInfoString.Len() == 1)
		{
			TimeInfoString = "0" + TimeInfoString;
		}
		ResultString = ResultString + TimeInfoString + ":";
	}
	ResultString.RemoveAt(ResultString.Len() - 1);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *ResultString);

	return ResultString;
}

void USaveSlotWidget::OnSlotButtonClicked()
{
	OnSaveSlotClicked.Broadcast(SlotName);

	AMyRPGGameModeBase* MyGamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (MyGamemode)
	{
		MyGamemode->SetCurSaveSlotName(SlotName);
	}
}
