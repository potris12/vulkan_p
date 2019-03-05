#pragma once
class Object
{
public:
	virtual void active() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void destroy() = 0;
private:
	__int32 object_id_ = 0;
public:
	Object();
	virtual ~Object();
};

