
#include "LoadingScreen.h"
#include "MyRPG/GameModes/MyRPGGameModeBase.h"
#include <MyRPG/UI/PlayerHUD.h>
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>

void ULoadingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FadeInFinishedEvent.BindDynamic(this, &ULoadingScreen::FadeInAnimationFinished);
	FadeOutFinishedEvent.BindDynamic(this, &ULoadingScreen::FadeOutAnimationFinished);

	if (IsValid(FadeInAnimation))
	{
		BindToAnimationFinished(FadeInAnimation, FadeInFinishedEvent);
	}
	if (IsValid(FadeOutAnimation))
	{
		BindToAnimationFinished(FadeOutAnimation, FadeOutFinishedEvent);
	}
}

void ULoadingScreen::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULoadingScreen::FadeIn()
{
	if (IsValid(FadeInAnimation) == false)
	{
		return;
	}
	
	AddToViewport(10000);

	bFadeInAnimationFinished = false;
	PlayAnimationForward(FadeInAnimation);

	AMyRPGGameModeBase* Gamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(Gamemode))
	{
		Gamemode->PlayerInputSetting(false);
		Gamemode->SetIsLoading(true);
	}
}

void ULoadingScreen::FadeOut()
{
	if (IsValid(FadeInAnimation) == false || bFadeInAnimationFinished == false || bLoadingFinished == false)
	{
		return;
	}
	
	AttachPlayerToGround();

	PlayAnimationForward(FadeOutAnimation);
}

void ULoadingScreen::Finishloading()
{
	bLoadingFinished = true;
}

void ULoadingScreen::FadeOutAnimationFinished()
{
	RemoveFromParent();
	bLoadingFinished = false;

	AMyRPGGameModeBase* Gamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(Gamemode))
	{
		ELevelType LevelType = Gamemode->GetCurLevelType();
		if (LevelType == ELevelType::ELT_MainLevel)
		{
			Gamemode->PlayerInputSetting(false);
		}
		else
		{
			Gamemode->PlayerInputSetting(true);
		}

		Gamemode->SetIsLoading(false);

	}
}

void ULoadingScreen::FadeInAnimationFinished()
{
	bFadeInAnimationFinished = true;
	FadeOut();
	OnLoadingFadeInFinished.Broadcast();

	AMyRPGGameModeBase* Gamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(Gamemode))
	{
		ELevelType LevelType = Gamemode->GetCurLevelType();
		if (LevelType == ELevelType::ELT_MainLevel)
		{
			Gamemode->PlayerCursorSetting(true, (uint8)ECursorType::ECT_Default);
		}
		else
		{
			Gamemode->PlayerCursorSetting(true, (uint8)ECursorType::ECT_Crosshair);
		}
	}
}

void ULoadingScreen::AttachPlayerToGround()
{
	AMyRPGGameModeBase* Gamemode = Cast<AMyRPGGameModeBase>(GetWorld()->GetAuthGameMode());
	if (IsValid(Gamemode))
	{
		ELevelType LevelType = Gamemode->GetCurLevelType();
		if (LevelType == ELevelType::ELT_MainLevel)
		{
			return;
		}
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (IsValid(PlayerCharacter) && IsValid(PlayerCharacter->GetCharacterMovement()))
	{
		FHitResult HitResult;
		FVector StartLocation = PlayerCharacter->GetActorLocation();
		FVector EndLocation = StartLocation - FVector(0.f, 0.f, 200.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PlayerCharacter);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, PlayerCharacter->GetActorLocation(), EndLocation, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			PlayerCharacter->SetActorLocation(HitResult.ImpactPoint + FVector(0.f, 0.f, PlayerCharacter->GetDefaultHalfHeight()), true);
		}

		FTimerDelegate TimerDele;
		TimerDele.BindLambda(
			[PlayerCharacter]() 
			{
				PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); 
			}
		);
		GetWorld()->GetTimerManager().SetTimer(AttachPlayerToGroundTimer, TimerDele, 0.2f, false);
	}
}
