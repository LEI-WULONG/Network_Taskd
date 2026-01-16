// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TaskPlayerController.generated.h"

class APickupItem;

/**
 * 
 */
UCLASS()
class NETWORK_TASK_API ATaskPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 클라이언트 -> 서버: 픽업 수거 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Pickup")
	void ServerCollectPickup(APickupItem* Pickup);

	// 서버 -> 클라이언트: 매치 결과 표시 요청
	UFUNCTION(Client, Reliable)
	void ClientShowMatchResult(int32 WinnerPlayerId);
};
