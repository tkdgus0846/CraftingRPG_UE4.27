
#include "InteractComponent.h"
#include "Blueprint/UserWidget.h"
#include "MyRPG/Interfaces/Interactable.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "Inventory.h"
#include "MyRPG/Facility/Facility.h"


UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}

void UInteractComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		AddOverlapActor(OtherActor);
	}
}

void UInteractComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		RemoveOverlapActor(OtherActor);
	}
}

void UInteractComponent::Interact()
{
	if (CurrentOverlapInteractables.Num() <= 0 || CurrentNearestInteractableIndex == -1 || CurrentNearestInteractableIndex >= CurrentOverlapInteractables.Num()) return;

	
	AActor* Interactable = CurrentOverlapInteractables[CurrentNearestInteractableIndex];
	AItem* Item = Cast<AItem>(Interactable);

	if (IsValid(Item) && IsValid(PlayerCharacter) && PlayerCharacter->GetInventory())
	{
		PlayerCharacter->GetInventory()->LootItem(Item, true);
	}
	else
	{
		AFacility* Facility = Cast<AFacility>(Interactable);
		if (IsValid(Facility) && IsValid(PlayerCharacter))
		{
			if (Facility == CurrentFacility.Get())
			{
				return;
			}

			PlayerCharacter->SetWeaponVisiblity(false);
			PlayerCharacter->PlayAnimMontage(InteractMontage);
			if (CurrentFacility.IsValid())
			{
				if (CurrentFacility->IsPlayerUsing())
				{
					CurrentFacility->HideFacilityWindow();
				}
				CurrentFacility->FinishFacilityInteract(!CurrentFacility->IsPlayerUsing());
			}
			CurrentFacility = Facility;
		}

		
		IInteractable::Execute_Interact(Interactable);
	}
}

void UInteractComponent::RemoveOverlapActor(AActor* OverlapActor)
{
	if (CurrentOverlapInteractables.Find(OverlapActor) == INDEX_NONE) return;

	IInteractable::Execute_SetInteractWidgetVisibility(OverlapActor, false);

	CurrentOverlapInteractables.Remove(OverlapActor);
	// 제거 했더니 오버랩된 아이템이 0개이면 타이머 종료하고 CurrentNearestItemIndex를 -1로 만들어라
	if (CurrentOverlapInteractables.Num() <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckNearestInteractableTimer);
		CurrentNearestInteractableIndex = -1;
	}
}

void UInteractComponent::AddOverlapActor(AActor* OverlapActor)
{
	if (IsValid(OverlapActor) == false) return;

	CurrentOverlapInteractables.Add(OverlapActor);
	// 가장 가까운 아이템을 찾는 타이머 시작해라.
	GetWorld()->GetTimerManager().SetTimer(CheckNearestInteractableTimer, this, &UInteractComponent::CheckNearestInteractable, 0.1f, true);
}

bool UInteractComponent::IsAnyOverlapInteractables()
{
	return CurrentOverlapInteractables.Num() > 0;
}

void UInteractComponent::FinishFacilityInteract()
{
	if (CurrentFacility.IsValid())
	{
		CurrentFacility->FinishFacilityInteract(true);
	}
}

void UInteractComponent::CheckNearestInteractable()
{
	FVector MyLocation = GetOwner()->GetActorLocation();
	FVector ItemLocation;
	int ResultIndex = -1;
	int MinDistance = 99999.f;

	for (int i = 0; i < CurrentOverlapInteractables.Num(); ++i)
	{
		ItemLocation = CurrentOverlapInteractables[i]->GetActorLocation();
		float Distance = FVector::Distance(MyLocation, ItemLocation);

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ResultIndex = i;
		}
		IInteractable::Execute_SetInteractWidgetVisibility(CurrentOverlapInteractables[i], false);
	}

	if (ResultIndex < CurrentOverlapInteractables.Num())
	{
		CurrentNearestInteractableIndex = ResultIndex;
		IInteractable::Execute_SetInteractWidgetVisibility(CurrentOverlapInteractables[ResultIndex], true);
	}
	else
	{
		CurrentNearestInteractableIndex = -1;
	}
}


