// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackholeActor.generated.h"

class USphereComponent;

UCLASS()
class FPSGAME_API ABlackholeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackholeActor();

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* AttractionSphereComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* EventHorizonSphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Blackhole Physics")
	TArray<UPrimitiveComponent*> AttracktedObjects;

	UPROPERTY(EditAnywhere, Category="Blackhole Physics")
	float AttractionForce = 10.f;

	UPROPERTY(EditAnywhere, Category="Blackhole Physics")
	float AttractionIncrement = 10.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void ApplyForces(TArray<UPrimitiveComponent*> AttracktedObjects);
	
	UFUNCTION()
	void DestroyAttracktedObject(UPrimitiveComponent* OverlappedComponent,
	                             AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex,
	                             bool bFromSweep,
	                             const FHitResult& SweepResult);
};
