// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class NETWORK_TASK_API APickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupItem();

protected:
	virtual void BeginPlay() override;

	// Called when something overlaps with this actor
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);

	// Called when the 'bAvailable' property is replicated
	UFUNCTION()
	void OnRep_Available();

public:	
	virtual void Tick(float DeltaTime) override;

	// Setup replication for properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	USphereComponent* CollisionComp = nullptr;

	// Mesh component for visuals
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* MeshComp = nullptr;

	// The score that this pickup item provides
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	int32 PointValue = 1;

	// Flag to determine if the item is available for pickup (replicated)
	UPROPERTY(ReplicatedUsing = OnRep_Available)
	bool bAvailable = true;

	// Maximum distance for client to request pickup (when client directly requests)
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float MaxClientCollectDistance = 300.f;
};
