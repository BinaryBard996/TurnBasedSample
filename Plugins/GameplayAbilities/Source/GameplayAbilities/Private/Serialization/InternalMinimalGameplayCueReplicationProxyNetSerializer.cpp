// Copyright Epic Games, Inc. All Rights Reserved.

#include "InternalMinimalGameplayCueReplicationProxyNetSerializer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InternalMinimalGameplayCueReplicationProxyNetSerializer)


#include "GameplayCueInterface.h"

void FMinimalGameplayCueReplicationProxyForNetSerializer::CopyReplicatedFieldsFrom(const FMinimalGameplayCueReplicationProxy& ReplicationProxy)
{
	Tags = ReplicationProxy.ReplicatedTags;
	Locations = ReplicationProxy.ReplicatedLocations;
}

void FMinimalGameplayCueReplicationProxyForNetSerializer::AssignReplicatedFieldsTo(FMinimalGameplayCueReplicationProxy& ReplicationProxy) const
{
	ReplicationProxy.ReplicatedTags = Tags;
	ReplicationProxy.ReplicatedLocations = Locations;
}

