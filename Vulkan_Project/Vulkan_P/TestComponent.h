#pragma once

#include "GADComponent.h"

class TestComponent : public GAD::GADComponent<TestComponent>
{
public:
	int a = 0;
	int b = 0;

public:
	TestComponent();
	virtual ~TestComponent();
};

