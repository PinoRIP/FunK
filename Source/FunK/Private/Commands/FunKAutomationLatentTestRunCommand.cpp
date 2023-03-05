#include "Commands/FunKAutomationLatentTestRunCommand.h"

bool FFunKAutomationLatentTestRunCommand::Update()
{
	if(TestRun.IsValid())
	{
		return TestRun->Next();
	}

	return true;
}
