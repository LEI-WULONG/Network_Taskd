// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/TaskGameState.h"
#include "Net/UnrealNetwork.h"

ATaskGameState::ATaskGameState()
{
	// 초기값은 헤더에 있음
}

void ATaskGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaskGameState, RemainingTime);
	DOREPLIFETIME(ATaskGameState, bMatchEnded);
	DOREPLIFETIME(ATaskGameState, WinnerPlayerId);
}

void ATaskGameState::OnRep_RemainingTime()
{
	// 클라이언트 측에서 UI 업데이트 트리거 필요 시 여기에 구현 또는 블루프린트 이벤트 호출
}

void ATaskGameState::OnRep_MatchEnded()
{
	// 클라이언트 측에서 매치 종료 UI 처리 트리거 필요 시 여기에 구현 또는 블루프린트 이벤트 호출
}

