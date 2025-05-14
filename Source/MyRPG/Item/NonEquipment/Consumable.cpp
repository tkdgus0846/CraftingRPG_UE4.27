
#include "Consumable.h"
#include "MyRPG/Data/ItemDataTable.h"
#include "Sound/Soundcue.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AConsumable::AConsumable()
{
	bConsumable = true;
}

void AConsumable::BringDataTableDatas()
{
	Super::BringDataTableDatas();

	FConsumableDataTable* DataRow = DataTable->FindRow<FConsumableDataTable>(RowName, FString("ConsumableDataTable"));


	if (DataRow == nullptr) return;

	ConsumeSound = DataRow->ConsumeSound;
}

void AConsumable::Consume_Implementation()
{
	if (ItemCount - 1 >= 0)
	{
		ItemCount -= 1;
		if (ConsumeSound)
		{
			ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
			
			if (Player)
			{
				FVector SoundPlayPos;
				SoundPlayPos = Player->GetActorLocation();
				UGameplayStatics::PlaySoundAtLocation(this, ConsumeSound, SoundPlayPos);
			}
			
		}
	}
}
