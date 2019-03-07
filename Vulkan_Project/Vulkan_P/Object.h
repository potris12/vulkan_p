#pragma once
#include "stdafx.h"

class Object
{
public:
	virtual void awake() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void destroy() = 0;
private:
	__int32 object_id_ = 0;
	std::string obj_name_;
public:
	Object(std::string name);
	virtual ~Object();
};

