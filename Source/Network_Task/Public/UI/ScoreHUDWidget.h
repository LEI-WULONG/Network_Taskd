// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreHUDWidget.generated.h"

// 헤더에서는 extern 선언을 사용
DECLARE_LOG_CATEGORY_EXTERN(LogScoreHUDWidget, Log, All);

UCLASS()
class NETWORK_TASK_API UScoreHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void UpdateMyScore(int32 NewScore);
	void UpdateEnemyScore(int32 NewScore);
	void UpdateTimeLeft(float InSeconds);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MyScore = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> EnemyScore = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TimeLeft = nullptr;

private:
	// 바인딩 핸들러
	UFUNCTION()
	void HandleMyScoreChanged(int32 NewScore);

	UFUNCTION()
	void HandleEnemyScoreChanged(int32 NewScore);

	// 바인딩 시도/관리
	void TryBindPlayerStates();

	// 캐시된 플레이어 상태 포인터 (약한 참조)
	TWeakObjectPtr<class ATaskPlayerState> CachedMyPlayerState;
	TWeakObjectPtr<class ATaskPlayerState> CachedEnemyPlayerState;

	// 바인딩 상태
	bool bBoundToPlayerStates = false;

	// 마지막 표시된 시간(초) 캐시
	int32 LastDisplayedTime = -1;
};
