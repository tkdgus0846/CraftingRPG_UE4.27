
#include "ProjectileActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AProjectileActor::AProjectileActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(RootScene);

		if (CollisionBox)
		{
			CollisionBox->SetupAttachment(StaticMeshComponent);
		}
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Movement");
}

void AProjectileActor::BeginPlay()
{
	bAttachToOwner = false;
	bRotateInherit = false;
	bLocationInherit = false;

	Super::BeginPlay();
}

void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*DrawDebugBox(GetWorld(), GetActorLocation(), CollisionBox->GetScaledBoxExtent(), FColor::Red);*/
	
}

void AProjectileActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}
