// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayAbilitiesDeveloperSettings.h"

#if WITH_EDITOR
bool UGameplayAbilitiesDeveloperSettings::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEdit = Super::CanEditChange(InProperty);
	if (InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, GlobalGameplayCueManagerClass))
	{
		bCanEdit = GlobalGameplayCueManagerName.IsNull();
	}
	if (InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, DefaultGameplayModEvaluationChannel))
	{
		bCanEdit = bAllowGameplayModEvaluationChannels;
	}
	if (InProperty->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, GameplayModEvaluationChannelAliases))
	{
		bCanEdit = bAllowGameplayModEvaluationChannels;
	}
	return bCanEdit;
}
#endif