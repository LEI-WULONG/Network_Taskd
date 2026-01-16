// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/MainHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/ScoreHUDWidget.h"
#include "UI/ResultWidget.h"
#include "GameFramework/PlayerController.h"
#include "Framework/TaskPlayerState.h"
#include "Framework/TaskGameState.h"
#include "Kismet/GameplayStatics.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHudWidgetClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), MainHudWidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
			MainHudWidget = Widget;
		}
	}
}

UScoreHUDWidget* AMainHUD::GetScoreHudWidget() const
{
	if (!MainHudWidget.IsValid())
	{
		return nullptr;
	}

	return Cast<UScoreHUDWidget>(MainHudWidget.Get());
}

void AMainHUD::ShowMatchResult(int32 WinnerPlayerId)
{
	// 이미 결과 위젯이 떠있다면 제거 후 재생성
	if (ResultWidget.IsValid())
	{
		if (UUserWidget* Existing = ResultWidget.Get())
		{
			Existing->RemoveFromParent();
		}
		ResultWidget = nullptr;
	}

	if (!ResultWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResultWidgetClass is not set on MainHUD"));
		return;
	}

	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), ResultWidgetClass);
	if (!NewWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create ResultWidget instance"));
		return;
	}

	// 화면에 추가
	NewWidget->AddToViewport();
	ResultWidget = NewWidget;

	// 결과 위젯에 승패 및 점수 전달
	UResultWidget* RW = Cast<UResultWidget>(NewWidget);
	if (RW)
	{
		// 로컬 플레이어 정보 및 상대 점수 조회
		int32 MyId = -1;
		int32 MyScore = 0;
		int32 EnemyScore = 0;

		if (APlayerController* PC = GetOwningPlayerController())
		{
			if (PC->PlayerState)
			{
				MyId = PC->PlayerState->GetPlayerId();
				if (ATaskPlayerState* MyPS = Cast<ATaskPlayerState>(PC->PlayerState))
				{
					MyScore = MyPS->GetMyScore();
				}
			}

			// GameState에서 상대 점수 찾기 (1:1 가정)
			if (UWorld* World = GetWorld())
			{
				if (ATaskGameState* GS = World->GetGameState<ATaskGameState>())
				{
					for (APlayerState* PSBase : GS->PlayerArray)
					{
						if (!PSBase) continue;
						if (PSBase->GetPlayerId() == MyId) continue;

						if (ATaskPlayerState* OtherPS = Cast<ATaskPlayerState>(PSBase))
						{
							EnemyScore = OtherPS->GetMyScore();
							break;
						}
					}
				}
			}
		}

		// 호출: WinnerId, MyId, MyScore, EnemyScore
		RW->ShowResult(WinnerPlayerId, MyId, MyScore, EnemyScore);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowMatchResult: ResultWidget is not UResultWidget"));
	}
}
