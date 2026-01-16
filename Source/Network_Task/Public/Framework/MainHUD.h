// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UUserWidget;
class UScoreHUDWidget;

/**
 * Main HUD: 메인 HUD 위젯 생성 및 경기 결과 위젯 생성 지원
 */
UCLASS()
class NETWORK_TASK_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	// 기본적으로 생성된 메인 위젯(원시 타입)
	inline TWeakObjectPtr<class UUserWidget> GetMainHudWidget() const { return MainHudWidget; }

	// 메인 위젯을 ScoreHUDWidget으로 캐스트해서 가져옴 (없으면 nullptr)
	UScoreHUDWidget* GetScoreHudWidget() const;

	// 경기 결과를 보여주는 별도 위젯을 생성하여 화면에 표시합니다.
	// WinnerPlayerId: -1 = 무승부 / >=0 = 해당 PlayerId가 승자
	void ShowMatchResult(int32 WinnerPlayerId);

protected:
	// 메인 HUD 위젯 블루프린트 클래스 (에디터에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainHudWidgetClass = nullptr;

	// 경기 결과 표시용 위젯 클래스 (에디터에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> ResultWidgetClass = nullptr;

private:
	// 생성된 메인 위젯 인스턴스(약한 참조)
	UPROPERTY()
	TWeakObjectPtr<UUserWidget> MainHudWidget = nullptr;

	// 생성된 결과 위젯 인스턴스(약한 참조)
	UPROPERTY()
	TWeakObjectPtr<UUserWidget> ResultWidget = nullptr;
};
