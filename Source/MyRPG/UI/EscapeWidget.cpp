
#include "EscapeWidget.h"
#include "MyRPG/UI/MainScreenButton.h"
#include <Kismet/GameplayStatics.h>
#include <MyRPG/GameModes/MyRPGGameModeBase.h>
#include "MyRPG/UI/PlayerHUD.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Controller/MyPlayerController.h"

void UEscapeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FadeOutFinishedEvent.BindDynamic(this, &UEscapeWidget::FadeOutAnimationFinished);

	if (IsValid(FadeOutAnimation))
	{
		BindToAnimationFinished(FadeOutAnimation, FadeOutFinishedEvent);
	}

	if (Resume)
	{
		Resume->SetText("RESUME", 30);
		Resume->GetButtonClickedEvent().AddDynamic(this, &UEscapeWidget::FadeOut);
	}
	if (Save)
	{
		Save->SetText("SAVE", 30);
		Save->GetButtonClickedEvent().AddDynamic(this, &UEscapeWidget::OnSaveButtonClicked);
	}
	if (Load)
	{
		Load->SetText("LOAD", 30);
		Load->GetButtonClickedEvent().AddDynamic(this, &UEscapeWidget::OnLoadButtonClicked);
	}
	if (Quit)
	{
		Quit->SetText("QUIT", 30);
		Quit->GetButtonClickedEvent().AddDynamic(this, &UEscapeWidget::OnQuitButtonClicked);
	}
	if (SaveAndQuit)
	{
		SaveAndQuit->SetText("SAVE & QUIT", 30);
		SaveAndQuit->GetButtonClickedEvent().AddDynamic(this, &UEscapeWidget::OnSaveAndQuitButtonClicked);
	}

	Gamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
}

void UEscapeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UEscapeWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UEscapeWidget::FadeIn()
{
	AddToViewport(100);
	PlayAnimationForward(FadeInAnimation);

	if (IsValid(Gamemode))
	{
		Gamemode->PlayerInputSetting(false);
		Gamemode->PlayerCursorSetting(true, (uint8)ECursorType::ECT_Default);
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDele;
	TimerDele.BindLambda(
		[this]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.f);
		}
	);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDele, 0.06f, false);
}

void UEscapeWidget::FadeOut()
{
	PlayAnimationReverse(FadeOutAnimation);
}

void UEscapeWidget::FadeOutAnimationFinished()
{
	RemoveFromParent();
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

	if (IsValid(Gamemode) && Gamemode->IsLoading() == false)
	{
		Gamemode->PlayerInputSetting(true);
		Gamemode->PlayerCursorSetting(true, (uint8)ECursorType::ECT_Crosshair);
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(Player) && IsValid(Player->GetPlayerController()))
	{
		Player->GetPlayerController()->CursorPosUpdate();
	}
}

void UEscapeWidget::OnSaveButtonClicked()
{
	if (IsValid(Gamemode))
	{

		Gamemode->SaveGame();
	}
}

void UEscapeWidget::OnLoadButtonClicked()
{
	if (IsValid(Gamemode))
	{
		Gamemode->LoadGame();
	}
}

void UEscapeWidget::OnQuitButtonClicked()
{
	if (IsValid(Gamemode))
	{
		FadeOut();
		Gamemode->LoadLevel(ELevelType::ELT_MainLevel);
	}
}

void UEscapeWidget::OnSaveAndQuitButtonClicked()
{
	if (IsValid(Gamemode))
	{
		FadeOut();
		Gamemode->SaveGame();
		Gamemode->LoadLevel(ELevelType::ELT_MainLevel);
	}
}
