// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"


#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComp->SetBoxExtent(FVector(200.0f));
	RootComponent = OverlapComp;

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->DecalSize = FVector(200.0f);
	DecalComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
// void AFPSExtractionZone::BeginPlay()
// {
// 	Super::BeginPlay();
// 	
// }

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	AFPSCharacter* myPawn = Cast<AFPSCharacter>(OtherActor);
	if(myPawn == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Entered extraction zone!"));
	
	if(myPawn->bIsCarryingObjective)
	{
		AFPSGameMode* gm = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if(gm)
		{
			gm->CompleteMission(myPawn);
		}
	}else
	{
		UGameplayStatics::PlaySound2D(this, ObjectiveMissingSound);
	}
}

// Called every frame
// void AFPSExtractionZone::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }
