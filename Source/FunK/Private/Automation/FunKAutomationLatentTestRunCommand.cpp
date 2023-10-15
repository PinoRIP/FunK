#include "FunKAutomationLatentTestRunCommand.h"

bool FFunKAutomationLatentTestRunCommand::Update()
{
	if(TestRun.IsValid())
	{
		return TestRun->Update();
	}

	return true;
}
