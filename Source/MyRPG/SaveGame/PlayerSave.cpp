
#include "PlayerSave.h"
#include <Kismet/GameplayStatics.h>
#include "MyRPG/Player/PlayerCharacter.h"
#include "MyRPG/Player/Components/Inventory.h"
#include "MyRPG/GameModes/MyRPGGameModeBase.h"

void UPlayerSave::SaveGame(APlayerCharacter* Player, const FString SlotName, float GamePlayTime)
{
	if (IsValid(Player) == false)
	{
		return;
	}

	HP = Player->GetHP();
	Transform = Player->GetTransform();

	LastLevelType = (uint8)Player->GetWorld()->GetAuthGameMode<AMyRPGGameModeBase>()->GetCurLevelType();

	TArray<AItem*>& ItemArray = Player->GetInventory()->GetItemArr();
	for (AItem* Item : ItemArray)
	{
		if (IsValid(Item))
		{
			ItemData.Add(FItemSaveData(Item->GetClass(), Item->GetItemCount()));
		}
		else
		{
			ItemData.Add(FItemSaveData(nullptr, 0));
		}
	}

	LastPlayGameTime = GamePlayTime;
	
	LastInterpZoomLength = Player->GetInterpZoomLength();
	LastSpringArmRelativeRotation = Player->GetSpringArmRelativeRotation();

	UGameplayStatics::SaveGameToSlot(this, SlotName, 0);
	UE_LOG(LogTemp, Warning, TEXT("Save Finished!!"));
}
