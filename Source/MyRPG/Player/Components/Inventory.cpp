
#include "Inventory.h"
#include "MyRPG/Item/Item.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include <MyRPG/Item/NonEquipment/Consumable.h>
#include <MyRPG/Item/Equipment/Weapon.h>

#include "MyRPG/SaveGame/PlayerSave.h"


UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

}

// BeginPlay 보다 먼저 호출되는것..
void UInventory::InitializeComponent()
{
	Super::InitializeComponent();

	ItemArray.Init(nullptr, InventorySize);
}


void UInventory::BeginPlay()
{
	Super::BeginPlay();

	
}


void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

TArray<AItem*>& UInventory::GetItemArr()
{
	return ItemArray;
}

bool UInventory::IsItemValid(int FindIndex)
{
	return ItemArray[FindIndex] == nullptr ? false : true;
}

void UInventory::DiscardItem(int Index, bool bDestroy, FVector PopPosOffset, float PopRadius)
{
	if (IsItemValid(Index) == false) return;

	RemoveIndexFromMap(ItemArray[Index]->GetItemID(), Index);
	
	if (bDestroy == true)
	{
		ItemArray[Index]->Destroy();
	}
	else
	{
		FVector PopPos = GetOwner()->GetActorLocation();
		ItemArray[Index]->PopFromInventory(PopPos + PopPosOffset, PopRadius);
	}
		
	ItemArray[Index] = nullptr;
	OnUpdateInventory.Broadcast();
	

	// 앞으로 돌면서 찾아야함. 어떤 아이템이 null이 아닌가를.
	if (Index == CurrentSelectedItemIndex)
	{
		ChangeCurrentItemAndIndex();
	}
}

void UInventory::ExchangeItem(int AIndex, int BIndex)
{
	if (AIndex == BIndex) return;

	if (IsItemValid(AIndex) == false &&
		IsItemValid(BIndex) == false) return;

	if (IsItemValid(AIndex) == true)
	{
		RemoveIndexFromMap(ItemArray[AIndex]->GetItemID(), AIndex);
		AddIndexToMap(ItemArray[AIndex]->GetItemID(), BIndex);
	}
	if (IsItemValid(BIndex) == true)
	{
		RemoveIndexFromMap(ItemArray[BIndex]->GetItemID(), BIndex);
		AddIndexToMap(ItemArray[BIndex]->GetItemID(), AIndex);
	}

	AItem* TempItem = ItemArray[AIndex];
	ItemArray[AIndex] = ItemArray[BIndex];
	ItemArray[BIndex] = TempItem;

	OnUpdateInventory.Broadcast();

	// 아이템 위치를 바꿨을때에 대한처리.
	if (AIndex == CurrentSelectedItemIndex || BIndex == CurrentSelectedItemIndex)
	{
		ChangeCurrentItemAndIndex();
	}
}

void UInventory::ExchangeItemOtherInventory(int ThisIndex, UInventory* OtherInventory, int OtherIndex)
{
	if (IsValid(OtherInventory) == false || 
		(IsItemValid(ThisIndex) == false && OtherInventory->IsItemValid(OtherIndex) == false))
	{
		return;
	}

	if (IsItemValid(ThisIndex) == true)
	{
		RemoveIndexFromMap(ItemArray[ThisIndex]->GetItemID(), ThisIndex);
		OtherInventory->AddIndexToMap(ItemArray[ThisIndex]->GetItemID(), OtherIndex);
	}

	if (OtherInventory->IsItemValid(OtherIndex) == true)
	{
		OtherInventory->RemoveIndexFromMap(OtherInventory->ItemArray[OtherIndex]->GetItemID(), OtherIndex);
		AddIndexToMap(OtherInventory->ItemArray[OtherIndex]->GetItemID(), ThisIndex);
	}

	AItem* TempItem = ItemArray[ThisIndex];
	ItemArray[ThisIndex] = OtherInventory->ItemArray[OtherIndex];
	OtherInventory->ItemArray[OtherIndex] = TempItem;

	OnUpdateInventory.Broadcast();
	OtherInventory->OnUpdateInventory.Broadcast();

	// 아이템 위치를 바꿨을때에 대한처리.
	if (ThisIndex == CurrentSelectedItemIndex)
	{
		ChangeCurrentItemAndIndex();
	}
	if (OtherIndex == OtherInventory->CurrentSelectedItemIndex)
	{
		OtherInventory->ChangeCurrentItemAndIndex();
	}
}

//void UInventory::LootItem()
//{
//	if (CurrentOverlapItems.Num() <= 0 || CurrentNearestItemIndex == -1 || CurrentNearestItemIndex >= CurrentOverlapItems.Num()) return;
//
//	AItem* NewItem = CurrentOverlapItems[CurrentNearestItemIndex];
//	CurrentOverlapItems.RemoveAt(CurrentNearestItemIndex);
//
//	uint32 Index = FindItemIndex(NewItem);
//	PushItem(NewItem, Index, true);
//
//	OnUpdateInventory.Broadcast();
//}

void UInventory::LootItem(AItem* NewItem, bool bWasDropped)
{
	if (IsValid(NewItem) == false)
	{
		return;
	}

	uint32 Index = FindItemIndex(NewItem);
	if (Index == InventorySize)
	{
		return;
	}

	PushItem(NewItem, Index, bWasDropped);

	OnUpdateInventory.Broadcast();
}

AItem* UInventory::GainItemByInventoryIndex(AItem* NewItem, int Index)
{
	if (Index >= ItemArray.Num()) return nullptr;

	if (IsValid(ItemArray[Index]))
	{
		RemoveIndexFromMap(ItemArray[Index]->GetItemID(), Index);
	}
	
	if (IsValid(NewItem))
	{
		AddIndexToMap(NewItem->GetItemID(), Index);
	}

	AItem* OriginItem = ItemArray[Index];

	ItemArray[Index] = NewItem;
	OnUpdateInventory.Broadcast();

	return OriginItem;
}

void UInventory::SelectItem(int Num)
{
	if (CanSelectItem() == false) return;

	Num = Num - 1;

	if (!IsItemValid(Num) || CurrentSelectedItemIndex == Num) 
		return;

	if (ItemArray[Num]->IsConsumable())
	{
		AConsumable* Item = Cast<AConsumable>(ItemArray[Num]);
		if (Item)
		{
			Item->Consume();

			if (Item->GetItemCount() == 0)
			{
				Item->Destroy();
				ItemArray[Num] = nullptr;
			}
			
			OnUpdateInventory.Broadcast();
		}
	}
	else
	{
		AWeapon* Item = Cast<AWeapon>(ItemArray[Num]);

		if (Item)
		{
			if (CurrentSelectedItemIndex != -1)
				OnDeselectItem.Broadcast(CurrentSelectedItemIndex);

			ChangeCurrentSelectedWeapon(Item);
			CurrentSelectedItemIndex = Num;
			OnSelectItem.Broadcast(Num);
		}
	}
}

void UInventory::SetNewItemArray(const TArray<FItemSaveData>& NewItemArray)
{
	int index = 0;
	for (const FItemSaveData& ItemSaveData : NewItemArray)
	{
		if (ItemSaveData.ItemClass)
		{
			AItem* NewItem = GetWorld()->SpawnActor<AItem>(ItemSaveData.ItemClass);
			if (IsValid(NewItem))
			{
				NewItem->SetItemCount(ItemSaveData.ItemNum);
				PushItem(NewItem, index);
			}
		}
		index++;
	}
	OnUpdateInventory.Broadcast();
}

void UInventory::ClearItemArray()
{
	ItemArray.Empty();
	ItemArray.Init(nullptr, InventorySize);

	IndexMap.Empty();
	OnUpdateInventory.Broadcast();
}

TArray<FCheckRecipeInfo> UInventory::CheckInventoryItemWithRecipe(TArray<FRecipeInfo> RecipeInfoArray)
{
	TArray<FCheckRecipeInfo> RetArray;

	for (const FRecipeInfo& RecipeInfo : RecipeInfoArray)
	{
		FCheckRecipeInfo CheckRecipeInfo;

		for (AItem* Item : ItemArray)
		{
			if (IsValid(Item) == false)
			{
				continue;
			}

			if (Item->GetClass() == RecipeInfo.ItemClass)
			{
				CheckRecipeInfo.Count += Item->GetItemCount();
			}
		}

		AItem* Item = nullptr;
		AItem** FindRecipeInfo = ItemForCheckRecipeMap.Find(RecipeInfo.ItemClass);

		if (FindRecipeInfo == nullptr)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();

			Item = ItemForCheckRecipeMap.Add(RecipeInfo.ItemClass, GetWorld()->SpawnActor<AItem>(RecipeInfo.ItemClass, FTransform(FVector(0.f, 0.f, -999999.f)), SpawnParams));
		}
		else
		{
			Item = *FindRecipeInfo;
		}

		if (IsValid(Item))
		{
			CheckRecipeInfo.ItemID = Item->GetItemID();
			CheckRecipeInfo.ItemPreviewTexture = Item->GetPreviewTexture();
			CheckRecipeInfo.RequireNum = RecipeInfo.RequireNum;
		}

		if (CheckRecipeInfo.Count >= RecipeInfo.RequireNum)
		{
			CheckRecipeInfo.bCanCraftCondition = true;
		}

		RetArray.Add(CheckRecipeInfo);
	}

	return RetArray;
}

void UInventory::SpendItem(TSubclassOf<AItem> ItemClass, int Count)
{
	int TotalCount = 0;
	for (int i = ItemArray.Num() - 1; i >= 0; --i)
	{
		AItem* Item = ItemArray[i];
		if (IsValid(Item) == false)
		{
			continue;
		}

		if (Item->GetClass() == ItemClass)
		{
			TotalCount += Item->GetItemCount();

			if (TotalCount <= Count)
			{
				DiscardItem(i, true);
			}
			else
			{
				Item->SetItemCount(TotalCount - Count);
				OnUpdateInventory.Broadcast();
			}
		}

		if (TotalCount >= Count)
		{
			break;
		}
	}
}

void UInventory::PushItem(AItem* Item, uint32 Index, bool bWasDropped)
{
	if (Item == nullptr || (int)Index >= InventorySize) return;

	Item->PushInInventory(bWasDropped, GetOwner());

	if (IsItemValid(Index) == false)
	{
		ItemArray[Index] = Item;
		AddIndexToMap(Item->GetItemID(), Index);
	}
	else
	{
		bool bCombineSuccess = ItemArray[Index]->CombineItem(Item);
		if (bCombineSuccess && Item->GetLifeSpan() == 0.f)
		{
			uint32 NewIndex = FindItemIndex(Item);
			PushItem(Item, NewIndex);
		}
	}
}

// Stack 가능한지 등등 체크
uint32 UInventory::FindItemIndex(AItem* Item)
{
	if (Item == nullptr) return InventorySize;

	// 우선적으로 IndexMap에 있는지 체크해서 집어넣는다. IsStackable이 true 일때만.
	if (Item->IsStackable())
	{
		TArray<uint32>* IndexArr = IndexMap.Find(Item->GetItemID());

		if (IndexArr != nullptr && IndexArr->Num() > 0)
		{
			for (int IndexFromEnd = 0; IndexFromEnd < IndexArr->Num(); IndexFromEnd++)
			{
				int Index = IndexArr->Last(IndexFromEnd);
				if (IsValid(ItemArray[Index]) && ItemArray[Index]->IsItemMaxStackCount() == false)
				{
					return Index;
				}
			}
			
		}
	}
	
	for (int i = 0; i < InventorySize; i++)
	{
		if (IsItemValid(i) == false)
			return i;
	}

	return InventorySize;
}

bool UInventory::RemoveIndexFromMap(const FName& ItemName, uint32 Index)
{
	TArray<uint32>* FindArray = IndexMap.Find(ItemArray[Index]->GetItemID());

	if (FindArray == nullptr)
		return false;

	FindArray->Remove(Index);
	return true;
}

void UInventory::AddIndexToMap(const FName& ItemName, uint32 Index)
{
	TArray<uint32>* IndexArr = IndexMap.Find(ItemName);

	if (IndexArr == nullptr)
	{
		IndexMap.Add(TTuple<FName, TArray<uint32>>(ItemName, TArray<uint32>({ Index })));
	}
	else
	{
		IndexArr->Add(Index);
		IndexArr->Sort();
	}
}

void UInventory::ChangeCurrentSelectedWeapon(AWeapon* NewWeapon)
{
	CurrentSelectedWeapon = NewWeapon;
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (Player)
	{
		if (CurrentSelectedWeapon == nullptr)
			Player->ChangeWeapon(EWeaponType::EWT_Unarmed, true, Player->GetIsMounted());
		else
			Player->ChangeWeapon(CurrentSelectedWeapon->GetWeaponType(), true, Player->GetIsMounted());
	}
}

void UInventory::ChangeCurrentItemAndIndex()
{
	if (CurrentSelectedItemIndex == -1) return;

	OnDeselectItem.Broadcast(CurrentSelectedItemIndex);
	// CurrentSelectedWeapon = nullptr;

	AWeapon* NewItem = nullptr;
	int NextItemIndex = CurrentSelectedItemIndex;
	for (; NextItemIndex >= 0; NextItemIndex--)
	{
		if (IsItemValid(NextItemIndex))
		{
			NewItem = Cast<AWeapon>(ItemArray[NextItemIndex]);

			if (NewItem) break;
		}
	}

	if (NextItemIndex >= 0)
	{
		CurrentSelectedItemIndex = NextItemIndex;	
		OnSelectItem.Broadcast(CurrentSelectedItemIndex);
	}
	else
	{
		CurrentSelectedItemIndex = -1;
	}

	ChangeCurrentSelectedWeapon(NewItem);
}

bool UInventory::CanSelectItem()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
	if (Player)
	{
		return !Player->IsActionPlaying();
	}

	return true;
}

EWeaponType UInventory::GetCurrentSelectedWeaponType() const
{
	if (CurrentSelectedWeapon == nullptr) return EWeaponType::EWT_Unarmed;
	else return CurrentSelectedWeapon->GetWeaponType();
}

AWeapon* UInventory::GetCurrentSelectedWeapon() const
{
	return CurrentSelectedWeapon;
}
