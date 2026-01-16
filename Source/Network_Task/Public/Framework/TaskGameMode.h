// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TaskGameMode.generated.h"

class ATaskGameState;
class APickupItem;

UCLASS()
class NETWORK_TASK_API ATaskGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATaskGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchDurationSeconds = 60;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float SpawnIntervalSeconds = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<APickupItem> PickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector SpawnAreaMin = FVector(-1000.f, -1000.f, 100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	FVector SpawnAreaMax = FVector(1000.f, 1000.f, 300.f);

private:
	FTimerHandle MatchTimerHandle;
	FTimerHandle SpawnTimerHandle;

	void UpdateMatchTimer();

	void SpawnPickup();

	void EndMatch();

	FVector GetRandomSpawnLocation() const;

	ATaskGameState* GetTaskGameState() const;
};
