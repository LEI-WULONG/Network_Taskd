// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/TaskGameMode.h"
#include "Framework/TaskGameState.h"
#include "Framework/TaskPlayerState.h"
#include "Framework/TaskPlayerController.h"
#include "Framework/MainHUD.h"
#include "Actor/PickupItem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

ATaskGameMode::ATaskGameMode()
{
	// GameState / PlayerState 기본 클래스 지정(에디터에서 오버라이드 가능)
	GameStateClass = ATaskGameState::StaticClass();
	PlayerStateClass = ATaskPlayerState::StaticClass();
}

void ATaskGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ATaskGameState* GS = GetTaskGameState();
		if (GS)
		{
			GS->RemainingTime = MatchDurationSeconds;
			GS->bMatchEnded = false;
			GS->WinnerPlayerId = -1;
		}

		// 매 초마다 남은 시간 업데이트
		GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ATaskGameMode::UpdateMatchTimer, 1.0f, true);

		// 픽업 스폰 타이머 시작
		if (SpawnIntervalSeconds > 0.0f && PickupClass)
		{
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ATaskGameMode::SpawnPickup, SpawnIntervalSeconds, true);
		}
	}
}

void ATaskGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(MatchTimerHandle);
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void ATaskGameMode::UpdateMatchTimer()
{
	if (!HasAuthority())
		return;

	ATaskGameState* GS = GetTaskGameState();
	if (!GS)
		return;

	// 1초 감소
	GS->RemainingTime = FMath::Max(0, GS->RemainingTime - 1);

	// 매치 종료 체크
	if (GS->RemainingTime <= 0)
	{
		// 타이머 정지
		GetWorldTimerManager().ClearTimer(MatchTimerHandle);
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

		EndMatch();
	}
}

void ATaskGameMode::SpawnPickup()
{
	if (!HasAuthority() || !PickupClass)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	FVector SpawnLocation = GetRandomSpawnLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APickupItem* NewPickup = World->SpawnActor<APickupItem>(PickupClass, SpawnLocation, SpawnRotation, Params);
	if (NewPickup)
	{
		NewPickup->SetReplicates(true);
		NewPickup->SetReplicateMovement(true);
		UE_LOG(LogTemp, Log, TEXT("SpawnPickup: Spawned %s at %s"), *NewPickup->GetName(), *SpawnLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnPickup: Failed to spawn PickupClass"));
	}
}

void ATaskGameMode::EndMatch()
{
	if (!HasAuthority())
		return;

	ATaskGameState* GS = GetTaskGameState();
	if (!GS)
		return;

	// 플레이어 점수 비교 (1:1 기준)
	APlayerState* Winner = nullptr;
	int32 BestScore = INT_MIN;

	for (APlayerState* PS : GS->PlayerArray)
	{
		if (!PS) continue;

		ATaskPlayerState* TPS = Cast<ATaskPlayerState>(PS);
		if (!TPS) continue;

		const int32 Score = TPS->GetMyScore();
		if (Score > BestScore)
		{
			BestScore = Score;
			Winner = TPS;
		}
		else if (Score == BestScore)
		{
			// 동점 처리: Winner를 null로 두어 무승부로 표시
			Winner = nullptr;
		}
	}

	int32 WinnerId = -1;
	if (Winner)
	{
		WinnerId = Winner->GetPlayerId();
	}

	// GameState에 결과 저장 (복제)
	GS->bMatchEnded = true;
	GS->WinnerPlayerId = WinnerId;
	GS->RemainingTime = 0;

	// 서버 로그
	UE_LOG(LogTemp, Log, TEXT("Match ended. WinnerId=%d"), WinnerId);

	// 타이머 정리(안전성)
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	// 모든 플레이어 컨트롤러에 결과 표시 RPC 호출
	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			// 우선 커스텀 컨트롤러로 캐스트해 Client RPC 호출
			if (ATaskPlayerController* TaskPC = Cast<ATaskPlayerController>(PC))
			{
				TaskPC->ClientShowMatchResult(WinnerId);
			}
			else
			{
				// 만약 커스텀 컨트롤러가 아니면 HUD 직접 호출(로컬)
				if (AMainHUD* HUD = PC->GetHUD<AMainHUD>())
				{
					HUD->ShowMatchResult(WinnerId);
				}
			}
		}
	}
}

FVector ATaskGameMode::GetRandomSpawnLocation() const
{
	// 박스 영역 내 랜덤
	const float X = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
	const float Y = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
	const float Z = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);

	return FVector(X, Y, Z);
}

ATaskGameState* ATaskGameMode::GetTaskGameState() const
{
	return GetWorld() ? GetWorld()->GetGameState<ATaskGameState>() : nullptr;
}

