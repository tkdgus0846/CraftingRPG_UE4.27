
#include "SplineGeometryActor.h"
#include "Components/SplineComponent.h"
#include "MyRPG/Types/ChannelType.h"


ASplineGeometryActor::ASplineGeometryActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	if (IsValid(RootScene))
	{
		RootComponent = RootScene;
	}

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));

	if (IsValid(Spline))
	{
		Spline->SetupAttachment(RootComponent);
		Spline->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	}
}

void ASplineGeometryActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASplineGeometryActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IsValid(Spline) == false)
	{
		return;
	}

	MakeSplineShape();
	DrawSpline();
}

void ASplineGeometryActor::MakeSplineShape()
{
	switch (Shape)
	{
	case ESplineGeometryShape::ESGT_Custom:
		break;
	case ESplineGeometryShape::ESGT_Circle:
		SplineCircle();
		break;
	default:
		break;
	}


}

void ASplineGeometryActor::SplineCircle()
{
	Spline->ClearSplinePoints();

	float AngleBetweenPoint = 360.f / Segments;
	for (int i = EmptySegments; i < Segments; ++i)
	{
		float Radian = FMath::DegreesToRadians(AngleBetweenPoint * i);

		float X = FMath::Cos(Radian) * Radius;
		float Y = FMath::Sin(Radian) * Radius;

		FVector PointLocation(X, Y, 0.f);
		Spline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::Local);
	}

	if (EmptySegments > 0)
	{
		Spline->SetClosedLoop(false);
	}
	else
	{
		Spline->SetClosedLoop(true);
	}

	Spline->UpdateSpline();
}

void ASplineGeometryActor::DrawSpline()
{
	for (int i = 0; i < SplineMeshComponentArray.Num(); ++i)
	{
		if (IsValid(SplineMeshComponentArray[i]))
		{
			SplineMeshComponentArray[i]->DestroyComponent();
		}
	}
	SplineMeshComponentArray.Empty();

	if (MeshNums == 0)
	{
		for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; ++i)
		{
			RegisterSplineMesh(i, i + 1);
		}

		if (Spline->IsClosedLoop())
		{
			RegisterSplineMesh(Spline->GetNumberOfSplinePoints() - 1, 0);
		}
	}
	else
	{
		RegisterSpacingSplineMesh();
	}
	
}

void ASplineGeometryActor::RegisterSplineMesh(int StartIndex, int EndIndex)
{
	FVector StartLocation, EndLocation, StartTangent, EndTangent;

	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
	SplineMesh->SetStaticMesh(Mesh);
	SplineMesh->SetForwardAxis(ForwardAxis);

	SplineMeshComponentArray.Add(SplineMesh);

	Spline->GetLocationAndTangentAtSplinePoint(StartIndex, StartLocation, StartTangent, ESplineCoordinateSpace::Type::World);
	Spline->GetLocationAndTangentAtSplinePoint(EndIndex, EndLocation, EndTangent, ESplineCoordinateSpace::Type::World);

	SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);

	SplineMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	SplineMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SplineMesh->SetCollisionResponseToChannel(ECC_Creature, ECollisionResponse::ECR_Block);
	SplineMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SplineMesh->RegisterComponent();
}

void ASplineGeometryActor::RegisterSpacingSplineMesh()
{
	FVector StartLocation, EndLocation, StartTangent, EndTangent;

	float SplineLength = Spline->GetSplineLength();
	float Spacing = SplineLength - (Length * MeshNums);
	if (Spacing < 0.f)
	{
		return;
	}

	Spacing = Spacing / MeshNums;

	float DistanceAlongSpline = 0.f;
	for (uint32 i = 0; i < MeshNums; ++i)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetStaticMesh(Mesh);
		SplineMesh->SetForwardAxis(ForwardAxis);

		SplineMeshComponentArray.Add(SplineMesh);

		StartLocation = Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Type::World);
		StartTangent = Spline->GetTangentAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Type::World);
	
		float NextDistance = DistanceAlongSpline + Length;

		EndLocation = Spline->GetLocationAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::Type::World);
		EndTangent = Spline->GetTangentAtDistanceAlongSpline(NextDistance, ESplineCoordinateSpace::Type::World);

		SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);

		SplineMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		SplineMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SplineMesh->SetCollisionResponseToChannel(ECC_Creature, ECollisionResponse::ECR_Block);
		SplineMesh->SetCollisionObjectType(ECC_WorldDynamic);
		SplineMesh->RegisterComponent();

		DistanceAlongSpline = NextDistance + Spacing;
	}
}

