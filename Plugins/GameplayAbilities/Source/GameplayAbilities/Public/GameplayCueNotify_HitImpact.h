// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayCueNotify_HitImpact.generated.h"

#define UE_API GAMEPLAYABILITIES_API

class UParticleSystem;
class USoundBase;

/**
 *	Non instanced GameplayCueNotify for spawning particle and sound FX.
 *	Still WIP - needs to be fleshed out more.
 */

UCLASS(Blueprintable, meta = (DisplayName = "GCN Hit Impact (Deprecated)", Category = "GameplayCue", ShortTooltip = "This class is deprecated. Use UFortGameplayCueNotify_Burst (GCN Burst) instead."), MinimalAPI)
class UGameplayCueNotify_HitImpact : public UGameplayCueNotify_Static
{
	GENERATED_UCLASS_BODY()

	/** Does this GameplayCueNotify handle this type of GameplayCueEvent? */
	UE_API virtual bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;
	
	UE_API virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<USoundBase> Sound;

	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UParticleSystem> ParticleSystem;
};

#undef UE_API
