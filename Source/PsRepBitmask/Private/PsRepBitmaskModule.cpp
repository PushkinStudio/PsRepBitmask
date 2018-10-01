// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsRepBitmaskPrivatePCH.h"
#include "PsRepBitmaskModule.h"

#define LOCTEXT_NAMESPACE "PsRepBitmaskModule"

void PsRepBitmaskModule::StartupModule()
{
}

void PsRepBitmaskModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(PsRepBitmaskModule, PsRepBitmask);

DEFINE_LOG_CATEGORY(LogPsRepBitmask);

#undef LOCTEXT_NAMESPACE
