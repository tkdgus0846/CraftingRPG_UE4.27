
#include "ModifyLandscapeVolume.h"
#include "Components/BoxComponent.h"
#include "MyRPG/Types/ChannelType.h"
#include "MyRPG/Player/PlayerCharacter.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "LandscapeEdit.h"
#include "LandscapeComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

AModifyLandscapeVolume::AModifyLandscapeVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	ModifyBound = CreateDefaultSubobject<UBoxComponent>("ModifyBound");
	if (IsValid(ModifyBound))
	{
		SetRootComponent(ModifyBound);
		
		ModifyBound->SetCollisionObjectType(ECC_OverlapVolume);
		ModifyBound->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		ModifyBound->OnComponentBeginOverlap.AddDynamic(this, &AModifyLandscapeVolume::OnOverlapBegin);
		ModifyBound->OnComponentEndOverlap.AddDynamic(this, &AModifyLandscapeVolume::OnOverlapEnd);
	}

}

void AModifyLandscapeVolume::BeginPlay()
{
	Super::BeginPlay();

	//if (IsValid(TargetLandscape) && IsValid(ModifyBound))
	//{
	//	FVector Origin = ModifyBound->Bounds.Origin;
	//	FVector Extent = ModifyBound->Bounds.BoxExtent;

	//	FVector MinPos = Origin - Extent;
	//	FVector MaxPos = Origin + Extent;

	//	// X Y 좌표 정보만 가져와서 인덱스를 가져와본다.


	//	LandscapeInfo = TargetLandscape->GetLandscapeInfo();
	//	;
	//	LandscapeInfo->ComponentNumSubsections;
	//	LandscapeInfo->SubsectionSizeQuads;


	//	for (const auto& Item : LandscapeInfo->XYtoComponentMap)
	//	{
	//		FIntPoint Point = Item.Value->GetSectionBase();

	//		/*FLinearColor BlackColor = FLinearColor::Black;
	//		FLinearColor RedColor = FLinearColor::Red;
	//		FLinearColor BlueColor = FLinearColor::Blue;
	//		
	//		FLinearColor XColor = UKismetMathLibrary::LinearColorLerp(BlackColor, RedColor, (float)Point.X / 32.f);
	//		FLinearColor YColor = UKismetMathLibrary::LinearColorLerp(BlackColor, BlueColor, (float)Point.Y / 32.f);
	//		FLinearColor MixedColor = UKismetMathLibrary::LinearColorLerp(XColor, YColor, 0.5f);
	//		DrawDebugPoint(GetWorld(), Item.Value->GetComponentLocation(), 10.f, FColor(XColor.R * 255.f, 0.f, YColor.B * 255.f), true, -1.f, 10U);*/

	//		if (Point.X == Point.Y)
	//		{
	//			DrawDebugString(GetWorld(), Item.Value->GetComponentLocation(), FString::Printf(TEXT("(%d %d)"), Point.X, Point.Y), nullptr, FColor::Black, 1000.f);
	//			LandscapeInfo->ComponentSizeQuads = Item.Value->ComponentSizeQuads;
	//			LandscapeInfo->ComponentNumSubsections = Item.Value->NumSubsections;
	//			LandscapeInfo->SubsectionSizeQuads = Item.Value->SubsectionSizeQuads;

	//			
	//		}
	//			
	//		
	//		/*FHeightmapAccessor<false> HeightmapAccessor();
	//		HeightmapAccessor.SetData(ModifyX1, ModifyY1, ModifyX2, ModifyY2, HeightData.GetData());*/
	//	}

	//	FLandscapeEditDataInterface LandscapeEdit(LandscapeInfo);
	//	int32 ModifyX1 = 1;
	//	int32 ModifyY1 = 1;
	//	int32 ModifyX2 = 31;
	//	int32 ModifyY2 = 31;

	//	const int32 NumVertices = (ModifyX2 - ModifyX1 + 1) * (ModifyY2 - ModifyY1 + 1);

	//	TArray<uint16> HeightData;
	//	HeightData.Init(10000, NumVertices);

	//	int32 InStride = (ModifyX2 - ModifyX1 + 1);
	//	LandscapeEdit.SetHeightData(ModifyX1, ModifyY1, ModifyX2, ModifyY2, HeightData.GetData(), 0, true);
	//	

	//	/*int32 ModifyX1 = 1;
	//	int32 ModifyY1 = 1;
	//	int32 ModifyX2 = 5;
	//	int32 ModifyY2 = 5;

	//	const int32 NumVertices = (ModifyX2 - ModifyX1 + 1) * (ModifyY2 - ModifyY1 + 1);

	//	TArray<uint16> HeightData;
	//	HeightData.Init(0, NumVertices);

	//	for (int i = 0; i < HeightData.Num(); i++)
	//	{
	//		HeightData[i] = 10000;
	//	}

	//	
	//	FHeightmapAccessor<false> HeightmapAccessor(LandscapeInfo);
	//	HeightmapAccessor.SetData(ModifyX1, ModifyY1, ModifyX2, ModifyY2, HeightData.GetData());*/

	//	
	//}
	
}

void AModifyLandscapeVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AModifyLandscapeVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
	}

}

void AModifyLandscapeVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
	}
}

