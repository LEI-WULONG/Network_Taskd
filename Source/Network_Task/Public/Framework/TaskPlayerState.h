// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TaskPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, int32, NewScore);

/**
 * 
 */
UCLASS()
class NETWORK_TASK_API ATaskPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddMyScore(int32 Point);

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetMyScore() const { return MyScore; }

	// 블루프린트에서 바인딩해서 UI 등을 업데이트할 수 있게 함
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnScoreChangedSignature OnScoreChanged;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// RepNotify
	UFUNCTION()
	void OnRep_MyScore();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MyScore, BlueprintReadOnly, Category = "Data")
	int32 MyScore = 0;
};
