// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ResultWidget.h"
#include "Components/TextBlock.h"

void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 텍스트 클리어 (선택)
	if (ResultText) ResultText->SetText(FText::GetEmpty());
	if (MyScoreText) MyScoreText->SetText(FText::GetEmpty());
	if (EnemyScoreText) EnemyScoreText->SetText(FText::GetEmpty());
}

void UResultWidget::ShowResult(int32 WinnerPlayerId, int32 MyPlayerId, int32 MyScore, int32 EnemyScore)
{
	// ResultText 업데이트
	if (ResultText)
	{
		if (WinnerPlayerId < 0)
		{
			ResultText->SetText(FText::FromString(TEXT("무승부")));
		}
		else if (WinnerPlayerId == MyPlayerId)
		{
			ResultText->SetText(FText::FromString(TEXT("승리")));
		}
		else
		{
			ResultText->SetText(FText::FromString(TEXT("패배")));
		}
	}

	// 점수 텍스트 업데이트
	if (MyScoreText)
	{
		MyScoreText->SetText(FText::FromString(FString::Printf(TEXT("내 점수: %d"), MyScore)));
	}
	if (EnemyScoreText)
	{
		EnemyScoreText->SetText(FText::FromString(FString::Printf(TEXT("상대 점수: %d"), EnemyScore)));
	}
}

