// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Iris/Serialization/NetSerializer.h"

namespace UE::Net
{

UE_NET_DECLARE_SERIALIZER(FGameplayAbilityTargetDataHandleNetSerializer, GAMEPLAYABILITIES_API);

}

void InitGameplayAbilityTargetDataHandleNetSerializerTypeCache();

