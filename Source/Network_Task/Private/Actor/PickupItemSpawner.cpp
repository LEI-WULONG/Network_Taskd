// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/PickupItemSpawner.h"
#include "Actor/PickupItem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

APickupItemSpawner::APickupItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스포너는 레벨에만 존재하면 되므로 기본적으로 복제할 필요 없음
	bReplicates = false;
}

void APickupItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 스폰 타이머를 동작시킴
	if (HasAuthority() && bAutoActivate)
	{
		StartSpawning();
	}
}

void APickupItemSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawning();
	Super::EndPlay(EndPlayReason);
}

void APickupItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupItemSpawner::StartSpawning()
{
	if (!HasAuthority())
		return;

	if (!PickupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PickupItemSpawner: PickupClass not set"));
		return;
	}

	if (SpawnIntervalSeconds <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("PickupItemSpawner: SpawnIntervalSeconds <= 0, not starting"));
		return;
	}

	// 이미 타이머가 있다면 재설정
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &APickupItemSpawner::SpawnPickup, SpawnIntervalSeconds, true, 0.0f);
}

void APickupItemSpawner::StopSpawning()
{
	if (!HasAuthority())
		return;

	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void APickupItemSpawner::SpawnPickup()
{
	if (!HasAuthority() || !PickupClass)
		return;

	// 활성 리스트 정리
	for (int32 i = ActivePickups.Num() - 1; i >= 0; --i)
	{
		if (!ActivePickups[i].IsValid())
		{
			ActivePickups.RemoveAtSwap(i);
		}
	}

	// 최대 개수 제한 처리
	if (MaxActivePickups > 0 && ActivePickups.Num() >= MaxActivePickups)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
		return;

	const FVector SpawnLocation = GetRandomSpawnLocation();
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickupItem* NewPickup = World->SpawnActor<APickupItem>(PickupClass, SpawnLocation, SpawnRotation, Params);
	if (NewPickup)
	{
		// 활성 리스트에 추가 및 파괴 바인딩
		ActivePickups.Add(NewPickup);
		NewPickup->OnDestroyed.AddDynamic(this, &APickupItemSpawner::OnSpawnedDestroyed);
	}
}

FVector APickupItemSpawner::GetRandomSpawnLocation() const
{
	// 안전하게 min/max 적용
	const float X = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
	const float Y = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
	const float Z = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);
	return FVector(X, Y, Z);
}

void APickupItemSpawner::OnSpawnedDestroyed(AActor* DestroyedActor)
{
	if (!DestroyedActor)
		return;

	for (int32 i = ActivePickups.Num() - 1; i >= 0; --i)
	{
		if (!ActivePickups[i].IsValid())
		{
			ActivePickups.RemoveAtSwap(i);
			continue;
		}

		if (ActivePickups[i].Get() == DestroyedActor)
		{
			ActivePickups.RemoveAtSwap(i);
			break;
		}
	}
}

