// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/TaskPlayerController.h"
#include "Actor/PickupItem.h"
#include "Framework/TaskPlayerState.h"
#include "Framework/MainHUD.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ATaskPlayerController::ServerCollectPickup_Implementation(APickupItem* Pickup)
{
	UE_LOG(LogTemp, Log, TEXT("ServerCollectPickup_Implementation called on controller %s (Role=%d). Pickup=%s"),
		*GetName(), (int)GetLocalRole(), Pickup ? *Pickup->GetName() : TEXT("null"));

	// 서버에서만 처리
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerCollectPickup: Not authority"));
		return;
	}

	// IsValid 은 null 및 pending-kill 상태를 검사함 (안전한 검사)
	if (!IsValid(Pickup))
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerCollectPickup: Pickup invalid or pending kill"));
		return;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerCollectPickup: Pawn not found"));
		return;
	}

	// 거리 검증: 플레이어가 픽업에 근접해 있어야 수거 허용 (튜닝: 300.f)
	const float MaxCollectDist = 300.f;
	const float DistSq = FVector::DistSquared(MyPawn->GetActorLocation(), Pickup->GetActorLocation());
	UE_LOG(LogTemp, Verbose, TEXT("ServerCollectPickup: DistSq=%.1f Max^2=%.1f"), DistSq, MaxCollectDist*MaxCollectDist);

	if (DistSq > MaxCollectDist * MaxCollectDist)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerCollectPickup: Too far from pickup"));
		return;
	}

	// 점수 추가 (서버 권한)
	if (ATaskPlayerState* TPS = GetPlayerState<ATaskPlayerState>())
	{
		UE_LOG(LogTemp, Log, TEXT("ServerCollectPickup: Adding score to PlayerState (PlayerId=%d Name=%s)"),
			TPS->GetPlayerId(), *TPS->GetPlayerName());

		const int32 PointValue = 1; // 필요시 APickupItem에서 포인트를 가져오도록 확장
		TPS->AddMyScore(PointValue);

		UE_LOG(LogTemp, Log, TEXT("ServerCollectPickup: After AddMyScore server value = %d"), TPS->GetMyScore());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerCollectPickup: PlayerState cast failed"));
	}

	// 픽업 제거 (서버에서 제거하면 클라이언트로 복제됨)
	Pickup->Destroy();
}

bool ATaskPlayerController::ServerCollectPickup_Validate(APickupItem* Pickup)
{
	// 간단 검증: 포인터가 유효하면 우선 허용. 추가 검증은 _Implementation에서 수행.
	return true;
}

void ATaskPlayerController::ClientShowMatchResult_Implementation(int32 WinnerPlayerId)
{
	// 클라이언트 HUD에 결과 표시 요청 전달
	if (AMainHUD* HUD = GetHUD<AMainHUD>())
	{
		HUD->ShowMatchResult(WinnerPlayerId);
		return;
	}

	// HUD가 없을 경우 로그
	UE_LOG(LogTemp, Warning, TEXT("ClientShowMatchResult: HUD not available"));
}