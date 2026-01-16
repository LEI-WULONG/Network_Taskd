// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItemSpawner.generated.h"

class APickupItem;

UCLASS()
class NETWORK_TASK_API APickupItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupItemSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 스폰 시작 / 중지 (서버에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void StopSpawning();

protected:
	// 실제 스폰 함수 (타이머에서 호출)
	void SpawnPickup();

	// 스폰 위치 계산
	FVector GetRandomSpawnLocation() const;

	// 스폰된 액터가 Destroy될 때 호출되어 리스트에서 제거
	UFUNCTION()
	void OnSpawnedDestroyed(AActor* DestroyedActor);

protected:
	// 스폰할 픽업 클래스 (에디터에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<APickupItem> PickupClass;

	// 스폰 간격 (초)
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnIntervalSeconds = 3.0f;

	// 한 번에 유지할 최대 활성 픽업 수 (0 = 제한 없음)
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 MaxActivePickups = 0;

	// 자동 시작 여부 (BeginPlay에서 자동 시작)
	UPROPERTY(EditAnywhere, Category = "Spawning")
	bool bAutoActivate = true;

	// 스폰 영역(박스) - 월드 좌표
	UPROPERTY(EditAnywhere, Category = "Spawning")
	FVector SpawnAreaMin = FVector(-1000.f, -1000.f, 100.f);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	FVector SpawnAreaMax = FVector(1000.f, 1000.f, 300.f);

private:
	FTimerHandle SpawnTimerHandle;

	// 활성 스폰 추적(중복 제거를 위함)
	UPROPERTY()
	TArray<TWeakObjectPtr<APickupItem>> ActivePickups;
};
