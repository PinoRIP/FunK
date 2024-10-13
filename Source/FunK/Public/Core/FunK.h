#pragma once

#include "FunKServiceProviderSubsystem.h"

namespace FunK
{
	template <typename TService>
	TService* Service()
	{
		return GEngine->GetEngineSubsystem<UFunKServiceProviderSubsystem>()->Service<TService>();
	}
}
