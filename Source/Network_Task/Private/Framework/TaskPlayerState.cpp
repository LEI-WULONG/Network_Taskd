// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/TaskPlayerState.h"
#include "Net/UnrealNetwork.h"

void ATaskPlayerState::AddMyScore(int32 Point)
{
	UE_LOG(LogTemp, Log, TEXT("AddMyScore called on %s (Role=%d). Point=%d. PrevScore=%d"),
		*GetName(), (int)GetLocalRole(), Point, MyScore);

	if (HasAuthority())
	{
		MyScore += Point;

		// 서버(호스트)에서 즉시 알림도 발생시키기 위해 RepNotify를 직접 호출
		OnRep_MyScore();

		UE_LOG(LogTemp, Log, TEXT("AddMyScore: NewScore=%d (server) for %s"), MyScore, *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AddMyScore called on non-authority instance for %s"), *GetName());
	}
}

void ATaskPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaskPlayerState, MyScore);

}

void ATaskPlayerState::OnRep_MyScore()
{
	UE_LOG(LogTemp, Log, TEXT("OnRep_MyScore called on %s (Role=%d). MyScore=%d"),
		*GetName(), (int)GetLocalRole(), MyScore);

	// 블루프린트 바인딩이 있다면 UI 등에서 처리할 수 있도록 델리게이트 방송
	OnScoreChanged.Broadcast(MyScore);
}
