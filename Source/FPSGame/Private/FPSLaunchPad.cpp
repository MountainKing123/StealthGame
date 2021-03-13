// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"


#include "FPSCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	this->OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	this->OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	this->OverlapComp->SetCollisionResponseToChannels(ECR_Ignore);
	this->OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->OverlapComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	this->OverlapComp->SetMobility(EComponentMobility::Static);
	this->OverlapComp->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	RootComponent = this->OverlapComp;

	this->OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::HandleOverlap);

	this->MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	this->MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->MeshComp->SetMobility(EComponentMobility::Static);
	this->MeshComp->SetupAttachment(RootComponent);

	this->OverlayMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OverlayMeshComp"));
	this->OverlayMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->OverlayMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->OverlayMeshComp->SetMobility(EComponentMobility::Static);
	this->OverlayMeshComp->SetupAttachment(RootComponent);

	this->ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchDirectionComp"));
	this->ArrowComp->SetRelativeRotation(FRotator(45.0f, 0.0f, 0.0f));
	this->ArrowComp->SetMobility(EComponentMobility::Static);
	this->ArrowComp->SetupAttachment(RootComponent);
	this->ArrowComp->SetHiddenInGame(true);

}

void AFPSLaunchPad::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FVector launchDirection = ArrowComp->GetComponentTransform().GetRotation().Vector();
	launchDirection.Normalize();
	
	if(OtherActor->IsA(AFPSCharacter::StaticClass()))
	{
		AFPSCharacter* launchableCharacter = Cast<AFPSCharacter>(OtherActor);
		if(launchableCharacter != nullptr)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Overlapping Component: %s"), *OtherActor->GetName()));
			
			launchableCharacter->LaunchCharacter(launchDirection * this->LaunchForce, true, true);
			this->PlayEffects();
		}	
	}else if(OtherComp !=  nullptr && OtherComp->IsSimulatingPhysics())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Overlapping Component: %s"), *OtherActor->GetName()));
		
		OtherComp->AddForce(launchDirection * this->LaunchForce * this->MeshForceMultiplier * OtherComp->GetMass());
		this->PlayEffects();
	}
}

void AFPSLaunchPad::PlayEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, this->LaunchPadFX, GetActorLocation());
}

// // Called when the game starts or when spawned
// void AFPSLaunchPad::BeginPlay()
// {
// 	Super::BeginPlay();
// 	
// }
//
// // Called every frame
// void AFPSLaunchPad::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }

