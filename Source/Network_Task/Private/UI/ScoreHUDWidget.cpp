// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ScoreHUDWidget.h"
#include "Components/TextBlock.h"
#include "Framework/TaskPlayerState.h"
#include "Framework/TaskGameState.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogScoreHUDWidget);

void UScoreHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogScoreHUDWidget, Log, TEXT("UScoreHUDWidget::NativeConstruct on %s Owner=%s"),
		*GetNameSafe(this), GetOwningPlayer() ? *GetOwningPlayer()->GetName() : TEXT("None"));

	// 처음 바인딩 시도
	TryBindPlayerStates();

	// 초기 시간 표시 (GameState가 있으면 바로 반영)
	if (UWorld* World = GetWorld())
	{
		if (ATaskGameState* GS = World->GetGameState<ATaskGameState>())
		{
			UpdateTimeLeft(static_cast<float>(GS->RemainingTime));
		}
	}
}

void UScoreHUDWidget::NativeDestruct()
{
	// 바인딩 해제
	if (CachedMyPlayerState.IsValid())
	{
		CachedMyPlayerState->OnScoreChanged.RemoveDynamic(this, &UScoreHUDWidget::HandleMyScoreChanged);
		CachedMyPlayerState = nullptr;
	}
	if (CachedEnemyPlayerState.IsValid())
	{
		CachedEnemyPlayerState->OnScoreChanged.RemoveDynamic(this, &UScoreHUDWidget::HandleEnemyScoreChanged);
		CachedEnemyPlayerState = nullptr;
	}
	bBoundToPlayerStates = false;

	Super::NativeDestruct();
}

void UScoreHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 아직 바인딩 안 됐으면 재시도
	if (!bBoundToPlayerStates)
	{
		TryBindPlayerStates();
	}

	// GameState의 RemainingTime을 읽어 UI 갱신 (초 단위로만 갱신)
	if (UWorld* World = GetWorld())
	{
		if (ATaskGameState* GS = World->GetGameState<ATaskGameState>())
		{
			int32 TimeSec = GS->RemainingTime;
			if (TimeSec != LastDisplayedTime)
			{
				LastDisplayedTime = TimeSec;
				UpdateTimeLeft(static_cast<float>(TimeSec));
			}
		}
	}
}

void UScoreHUDWidget::TryBindPlayerStates()
{
	if (bBoundToPlayerStates)
		return;

	UE_LOG(LogScoreHUDWidget, Log, TEXT("TryBindPlayerStates called"));

	// 소유 플레이어 컨트롤러에서 PlayerState 가져와 바인딩
	if (APlayerController* PC = GetOwningPlayer())
	{
		UE_LOG(LogScoreHUDWidget, Log, TEXT("TryBind: OwningPlayer=%s"), *GetNameSafe(PC));
		ATaskPlayerState* MyPS = PC->GetPlayerState<ATaskPlayerState>();
		if (MyPS && !CachedMyPlayerState.IsValid())
		{
			CachedMyPlayerState = MyPS;
			MyPS->OnScoreChanged.AddDynamic(this, &UScoreHUDWidget::HandleMyScoreChanged);
			UpdateMyScore(MyPS->GetMyScore());
			UE_LOG(LogScoreHUDWidget, Log, TEXT("Bound to MyPlayerState %s (score=%d)"), *MyPS->GetName(), MyPS->GetMyScore());
		}
	}

	// 적 플레이어(1:1 가정) 찾기 및 바인딩
	if (UWorld* World = GetWorld())
	{
		if (ATaskGameState* GS = World->GetGameState<ATaskGameState>())
		{
			for (APlayerState* PSBase : GS->PlayerArray)
			{
				if (!PSBase) continue;
				ATaskPlayerState* PS = Cast<ATaskPlayerState>(PSBase);
				if (!PS) continue;

				// 나와 다른 플레이어를 적으로 간주
				if (CachedMyPlayerState.IsValid() && PS != CachedMyPlayerState.Get())
				{
					if (!CachedEnemyPlayerState.IsValid())
					{
						CachedEnemyPlayerState = PS;
						PS->OnScoreChanged.AddDynamic(this, &UScoreHUDWidget::HandleEnemyScoreChanged);
						UpdateEnemyScore(PS->GetMyScore());
						UE_LOG(LogScoreHUDWidget, Log, TEXT("Bound to EnemyPlayerState %s (score=%d)"), *PS->GetName(), PS->GetMyScore());
					}
					break;
				}
			}
		}
	}

	// 변경: 둘 다 바인딩되었을 때만 true로 설정합니다.
	bool bHaveMy = CachedMyPlayerState.IsValid();
	bool bHaveEnemy = CachedEnemyPlayerState.IsValid();

	bBoundToPlayerStates = (bHaveMy && bHaveEnemy);

	UE_LOG(LogScoreHUDWidget, Log, TEXT("TryBindPlayerStates finished: bHaveMy=%d bHaveEnemy=%d bBoundToPlayerStates=%d"),
		(int)bHaveMy, (int)bHaveEnemy, (int)bBoundToPlayerStates);
}
 
void UScoreHUDWidget::HandleMyScoreChanged(int32 NewScore)
{
	UE_LOG(LogScoreHUDWidget, Log, TEXT("HandleMyScoreChanged received NewScore=%d"), NewScore);
	UpdateMyScore(NewScore);
}

void UScoreHUDWidget::HandleEnemyScoreChanged(int32 NewScore)
{
	UE_LOG(LogScoreHUDWidget, Log, TEXT("HandleEnemyScoreChanged received NewScore=%d"), NewScore);
	UpdateEnemyScore(NewScore);
}

void UScoreHUDWidget::UpdateMyScore(int32 NewScore)
{
	if (MyScore)
	{
		MyScore->SetText(FText::AsNumber(NewScore));
	}
	else
	{
		UE_LOG(LogScoreHUDWidget, Warning, TEXT("MyScore TextBlock is null"));
	}
}

void UScoreHUDWidget::UpdateEnemyScore(int32 NewScore)
{
	if (EnemyScore)
	{
		EnemyScore->SetText(FText::AsNumber(NewScore));
	}
	else
	{
		UE_LOG(LogScoreHUDWidget, Warning, TEXT("EnemyScore TextBlock is null"));
	}
}

void UScoreHUDWidget::UpdateTimeLeft(float InSeconds)
{
	if (!TimeLeft)
	{
		UE_LOG(LogScoreHUDWidget, Warning, TEXT("TimeLeft TextBlock is null"));
		return;
	}

	// 초 -> mm:ss 포맷으로 표시
	int32 TotalSec = FMath::Max(0, FMath::FloorToInt(InSeconds));
	int32 Minutes = TotalSec / 60;
	int32 Seconds = TotalSec % 60;

	FText Formatted = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
	TimeLeft->SetText(Formatted);
}
