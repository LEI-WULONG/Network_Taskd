// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TaskGameState.generated.h"

/**
 * GameState: 남은 시간 및 매치 결과를 복제하여 클라이언트 HUD에 전달
 */
UCLASS()
class NETWORK_TASK_API ATaskGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATaskGameState();

	// 남은 시간(초)
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "Match")
	int32 RemainingTime = 0;

	// 매치 종료 플래그
	UPROPERTY(ReplicatedUsing = OnRep_MatchEnded, BlueprintReadOnly, Category = "Match")
	bool bMatchEnded = false;

	// 승리자 PlayerId (-1 = 무승부/없음)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Match")
	int32 WinnerPlayerId = -1;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_RemainingTime();

	UFUNCTION()
	void OnRep_MatchEnded();
};
