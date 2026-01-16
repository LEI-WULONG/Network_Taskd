// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"

class UTextBlock;

/**
 * 경기 결과 위젯: 승/패/무승부 텍스트 및 점수 표시
 */
UCLASS()
class NETWORK_TASK_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 서버/게임모드가 호출하여 결과를 표시하도록 함
	UFUNCTION(BlueprintCallable, Category = "Result")
	void ShowResult(int32 WinnerPlayerId, int32 MyPlayerId, int32 MyScore, int32 EnemyScore);

protected:
	virtual void NativeConstruct() override;

	// 바인딩된 위젯들 (블루프린트에서 BindWidget)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MyScoreText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemyScoreText = nullptr;
};
