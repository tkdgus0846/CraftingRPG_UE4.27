
#include "MainScreen.h"
#include "MyRPG/GameModes/MyRPGGameModeBase.h"
#include "MyRPG/UI/MainScreenButton.h"
#include "MyRPG/UI/SaveSlotWidget.h"

#include <Kismet/KismetSystemLibrary.h>
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"

void UMainScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(Play))
	{
		Play->GetButtonClickedEvent().AddDynamic(this, &UMainScreen::PlayGame);
		Play->SetText("PLAY", 40);
	}

	if (IsValid(Load))
	{
		Load->GetButtonClickedEvent().AddDynamic(this, &UMainScreen::LoadGame);
		Load->SetText("LOAD GAME", 35);
	}

	if (IsValid(Exit))
	{
		Exit->GetButtonClickedEvent().AddDynamic(this, &UMainScreen::ExitGame);
		Exit->SetText("EXIT", 40);
	}

	if (IsValid(OK))
	{
		OK->GetButtonClickedEvent().AddDynamic(this, &UMainScreen::OnOKButtonClicked);
		OK->SetText("OK", 30);
	}

	if (IsValid(Cancel))
	{
		Cancel->GetButtonClickedEvent().AddDynamic(this, &UMainScreen::OnCancelButtonClicked);
		Cancel->SetText("Cancel", 30);
	}

	FadeInFinishedEvent.BindDynamic(this, &UMainScreen::FadeInAnimationFinished);
	FadeOutFinishedEvent.BindDynamic(this, &UMainScreen::FadeOutAnimationFinished);


	if (IsValid(SelectSaveSlotFadeIn))
	{
		BindToAnimationFinished(SelectSaveSlotFadeIn, FadeInFinishedEvent);
	}

	if (IsValid(SelectSaveSlotFadeOut))
	{
		BindToAnimationFinished(SelectSaveSlotFadeOut, FadeOutFinishedEvent);
	}

	SaveSlotWidgetArray.Add(SaveSlotWidget1);
	SaveSlotWidgetArray.Add(SaveSlotWidget2);
	SaveSlotWidgetArray.Add(SaveSlotWidget3);

	for (TWeakObjectPtr<USaveSlotWidget> SaveSlotWidget : SaveSlotWidgetArray)
	{
		if (SaveSlotWidget.IsValid() == false)
		{
			continue;
		}

		SaveSlotWidget.Get()->OnSaveSlotClicked.AddDynamic(this, &UMainScreen::ClickedSaveSlot);
	}
}

void UMainScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(SelectSaveSlot))
	{
		SelectSaveSlot->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainScreen::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMainScreen::PlayGame()
{
	if (IsValid(NewOrLoadText))
	{
		NewOrLoadText->SetText(FText::FromString("New Game"));
	}
	SetVisibilityVisibleToSlots();
	FadeIn();
}

void UMainScreen::LoadGame()
{
	if (IsValid(NewOrLoadText))
	{
		NewOrLoadText->SetText(FText::FromString("Load Game"));
	}
	SetVisibilityNotHitableToEmptySlots();
	FadeIn();
}

void UMainScreen::ExitGame()
{
	if (GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(
			this,  
			GetWorld()->GetFirstPlayerController(),
			EQuitPreference::Quit,
			true 
		);
	}
}

void UMainScreen::OnOKButtonClicked()
{
	AMyRPGGameModeBase* MyGamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(MyGamemode) && IsValid(NewOrLoadText))
	{
		if (NewOrLoadText->Text.EqualTo(FText::FromString("New Game")))
		{
			MyGamemode->NewGame();
		}
		else
		{
			MyGamemode->LoadGame();
		}
	}
}

void UMainScreen::OnCancelButtonClicked()
{
	FadeOut();
}

void UMainScreen::FadeIn()
{
	if (IsValid(SelectSaveSlot) == false)
	{
		return;
	}
	ClearSelectSlot();
	SelectSaveSlot->SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayAnimationForward(SelectSaveSlotFadeIn);
}

void UMainScreen::FadeOut()
{
	if (IsValid(SelectSaveSlot) == false)
	{
		return;
	}
	SelectSaveSlot->SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayAnimationReverse(SelectSaveSlotFadeOut);
}

void UMainScreen::FadeInAnimationFinished()
{
	if (IsValid(SelectSaveSlot) == false)
	{
		return;
	}
	SelectSaveSlot->SetVisibility(ESlateVisibility::Visible);
}

void UMainScreen::SetVisibilityNotHitableToEmptySlots()
{
	for (TWeakObjectPtr<USaveSlotWidget> SaveSlotWidget : SaveSlotWidgetArray)
	{
		if (SaveSlotWidget.IsValid() == false)
		{
			continue;
		}

		if (SaveSlotWidget.Get()->IsSlotDataEmpty())
		{
			SaveSlotWidget.Get()->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UMainScreen::SetVisibilityVisibleToSlots()
{
	for (TWeakObjectPtr<USaveSlotWidget> SaveSlotWidget : SaveSlotWidgetArray)
	{
		if (SaveSlotWidget.IsValid() == false)
		{
			continue;
		}

		SaveSlotWidget.Get()->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainScreen::FadeOutAnimationFinished()
{
	if (IsValid(SelectSaveSlot) == false)
	{
		return;
	}
	SelectSaveSlot->SetVisibility(ESlateVisibility::Hidden);
}

void UMainScreen::ClickedSaveSlot(const FString& ClickedSlotName)
{
	for (TWeakObjectPtr<USaveSlotWidget> SaveSlotWidget : SaveSlotWidgetArray)
	{
		if (SaveSlotWidget.IsValid() == false)
		{
			continue;
		}

		if (SaveSlotWidget.Get()->SlotName.Equals(ClickedSlotName))
		{
			SaveSlotWidget.Get()->SelectAppearanceSlot(true);
		}
		else
		{
			SaveSlotWidget.Get()->SelectAppearanceSlot(false);
		}
		
	}
}

void UMainScreen::ClearSelectSlot()
{
	for (TWeakObjectPtr<USaveSlotWidget> SaveSlotWidget : SaveSlotWidgetArray)
	{
		if (SaveSlotWidget.IsValid() == false)
		{
			continue;
		}

		SaveSlotWidget.Get()->SelectAppearanceSlot(false);
	}

	AMyRPGGameModeBase* MyGamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(MyGamemode))
	{
		MyGamemode->SetCurSaveSlotName("");
	}
}