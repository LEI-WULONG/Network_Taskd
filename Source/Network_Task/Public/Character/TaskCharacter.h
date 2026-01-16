// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TaskCharacter.generated.h"

class UInputMappingContext;

UCLASS()
class NETWORK_TASK_API ATaskCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATaskCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 클라이언트나 블루프린트에서 호출 가능. 클라이언트 호출 시 서버로 전달되어 서버가 점수를 적용함.
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Point = 1);

protected:
	// 서버 RPC: 실제 점수 반영은 서버에서 수행
	UFUNCTION(Server, Reliable)
	void Server_AddScore(int32 Point);
	void Server_AddScore_Implementation(int32 Point);

	// (옵션) Enhanced Input 매핑 컨텍스트를 에디터에서 할당
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	// 매핑 우선순위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 MappingPriority = 0;
};
