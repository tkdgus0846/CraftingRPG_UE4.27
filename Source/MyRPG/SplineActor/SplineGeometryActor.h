
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "SplineGeometryActor.generated.h"

UENUM(BlueprintType)
enum class ESplineGeometryShape : uint8
{
	ESGT_Custom UMETA(DisplayName = "Custom"),
	ESGT_Circle UMETA(DisplayName = "Circle")
};

UCLASS()
class MYRPG_API ASplineGeometryActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASplineGeometryActor();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere)
	class USplineComponent* Spline;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere)
	uint32 MeshNums = 0;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "MeshNums > 0", EditConditionHides))
	float Length = 10.f;

	UPROPERTY(EditAnywhere)
	ESplineGeometryShape Shape = ESplineGeometryShape::ESGT_Custom;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "Shape == ESplineGeometryShape::ESGT_Circle", EditConditionHides))
	float Radius = 200.f;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "Shape == ESplineGeometryShape::ESGT_Circle", EditConditionHides))
	int32 Segments = 10;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "Shape == ESplineGeometryShape::ESGT_Circle", EditConditionHides))
	uint32 EmptySegments = 0;

	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshComponentArray;

	void MakeSplineShape();
	void SplineCircle();

	void DrawSpline();
	void RegisterSplineMesh(int StartIndex, int EndIndex);
	void RegisterSpacingSplineMesh();
};
