#include "Commands/FunKAutomationLatentTestRunCommand.h"

bool FFunKAutomationLatentTestRunCommand::Update()
{
	if(TestRun.IsValid())
	{
		return TestRun->Test(TestInstructions);
	}

	return true;
}
