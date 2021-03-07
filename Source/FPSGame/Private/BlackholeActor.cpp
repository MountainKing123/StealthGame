// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackholeActor.h"

#include "AABB.h"
#include "AABB.h"
#include "Components/SphereComponent.h"

#define ECC_Blackhole ECollisionChannel::ECC_GameTraceChannel2

// Sets default values
ABlackholeActor::ABlackholeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = MeshComp;
	
	AttractionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AttractionSphere"));
	AttractionSphereComp->SetCollisionObjectType(ECC_Blackhole);
	AttractionSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttractionSphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttractionSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AttractionSphereComp->SetCollisionResponseToChannel(ECC_Blackhole, ECR_Ignore);
	AttractionSphereComp->SetupAttachment(MeshComp);
	
	EventHorizonSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("EventHorizon"));
	EventHorizonSphereComp->SetCollisionObjectType(ECC_Blackhole);
	EventHorizonSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EventHorizonSphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	EventHorizonSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	EventHorizonSphereComp->SetCollisionResponseToChannel(ECC_Blackhole, ECR_Ignore);
	EventHorizonSphereComp->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void ABlackholeActor::BeginPlay()
{
	Super::BeginPlay();

	this->EventHorizonSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackholeActor::DestroyAttracktedObject);
}

// Called every frame
void ABlackholeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttractionSphereComp->GetOverlappingComponents(this->AttracktedObjects);

	// for (auto element : AttracktedObjects)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, element->GetName());
	// }

	this->ApplyForces(this->AttracktedObjects);
	this->AttractionForce += 300 * DeltaTime;
}

void ABlackholeActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ABlackholeActor::ApplyForces(TArray<UPrimitiveComponent*> AttracktedObjects)
{
	for (auto object : AttracktedObjects)
	{
		if(object->Mobility == EComponentMobility::Movable) //&& object->IsAnySimulatingPhysics())
		{
			FVector startPosition = object->GetComponentTransform().GetTranslation();
			FVector targetPosition = this->GetTransform().GetTranslation();
			FVector finalForce = (targetPosition - startPosition) * this->AttractionForce;  
			object->AddForce(finalForce);
		}			
	}
}

void ABlackholeActor::DestroyAttracktedObject(UPrimitiveComponent* OverlappedComponent, 
			                                  AActor* OtherActor, 
			                                  UPrimitiveComponent* OtherComp, 
			                                  int32 OtherBodyIndex, 
			                                  bool bFromSweep, 
			                                  const FHitResult &SweepResult )
{
	OtherActor->Destroy();
}