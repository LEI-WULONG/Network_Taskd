// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/TaskCharacter.h"
#include "Framework/TaskPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"

ATaskCharacter::ATaskCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 입력/카메라/메시 등은 블루프린트에서 처리하므로 C++에서는 최소한만 설정
	bReplicates = true;
}

void ATaskCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input: 로컬 플레이어인 경우에만 MappingContext 추가
	// (서버나 원격 클라이언트 Pawn에서는 LocalPlayerSubsystem이 없음)
	if (IsLocallyControlled())
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					if (DefaultMappingContext)
					{
						Subsys->AddMappingContext(DefaultMappingContext, MappingPriority);
					}
				}
			}
		}
	}
}

void ATaskCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATaskCharacter::AddScore(int32 Point)
{
	// 클라이언트에서 호출되면 서버 RPC로 전달
	if (!HasAuthority())
	{
		Server_AddScore(Point);
		return;
	}

	// 서버 권한이면 PlayerState에 점수 적용
	if (ATaskPlayerState* PS = GetPlayerState<ATaskPlayerState>())
	{
		PS->AddMyScore(Point);
	}
}

void ATaskCharacter::Server_AddScore_Implementation(int32 Point)
{
	// 서버에서 안전하게 처리
	if (ATaskPlayerState* PS = GetPlayerState<ATaskPlayerState>())
	{
		PS->AddMyScore(Point);
	}
}

