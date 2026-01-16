// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/TaskPlayerState.h"
#include "Framework/TaskPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogPickupItem, Log, All);

// Sets default values
APickupItem::APickupItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	// Collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(64.f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	RootComponent = CollisionComp;

	// Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Ensure components are flagged for replication where appropriate
	// This helps initial component state (visibility/mesh) be valid on clients
	CollisionComp->SetIsReplicated(true);
	MeshComp->SetIsReplicated(true);

	// 바인딩
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnOverlapBegin);

	// 기본값
	bAvailable = true;
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();

	// 디버그: 역할/네트모드 출력
	ENetRole LocalRole = GetLocalRole();
	ENetMode NetMode = GetNetMode();
	UE_LOG(LogPickupItem, Log, TEXT("BeginPlay: %s Role=%d NetMode=%d Actor=%s"),
		*GetName(), (int)LocalRole, (int)NetMode, *GetActorLocation().ToString());

	// 클라이언트에서 메시에 대한 초기 가시성 보정 (Blueprint에서 Mesh를 세팅했다면 필요 없음)
	if (MeshComp)
	{
		MeshComp->SetVisibility(bAvailable, true);
	}
}

// Called every frame
void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                 bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	APawn* OverlapPawn = Cast<APawn>(OtherActor);
	if (!OverlapPawn) return;
	if (!bAvailable) return;

	if (HasAuthority())
	{
		APlayerController* PC = Cast<APlayerController>(OverlapPawn->GetController());
		if (PC && PC->PlayerState)
		{
			ATaskPlayerState* TPS = Cast<ATaskPlayerState>(PC->PlayerState);
			if (TPS)
			{
				bAvailable = false;
				UE_LOG(LogPickupItem, Log, TEXT("Server: Collected by %s, awarding %d"), *PC->GetName(), PointValue);
				TPS->AddMyScore(PointValue);
				Destroy();
			}
		}
	}
	else
	{
		// 로컬 클라이언트일 경우만 서버에 요청
		if (OverlapPawn->IsLocallyControlled())
		{
			APlayerController* PC = Cast<APlayerController>(OverlapPawn->GetController());
			if (PC)
			{
				ATaskPlayerController* TaskPC = Cast<ATaskPlayerController>(PC);
				if (TaskPC)
				{
					const float DistSq = FVector::DistSquared(OverlapPawn->GetActorLocation(), GetActorLocation());
					if (DistSq <= MaxClientCollectDistance * MaxClientCollectDistance)
					{
						UE_LOG(LogPickupItem, Verbose, TEXT("Client: Request collect (local)."));
						TaskPC->ServerCollectPickup(this);
					}
				}
			}
		}
	}
}

void APickupItem::OnRep_Available()
{
	UE_LOG(LogPickupItem, Log, TEXT("OnRep_Available Called: bAvailable=%d Actor=%s"), bAvailable, *GetName());
	if (!bAvailable && MeshComp)
	{
		MeshComp->SetVisibility(false, true);
	}
}

void APickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickupItem, bAvailable);
}

