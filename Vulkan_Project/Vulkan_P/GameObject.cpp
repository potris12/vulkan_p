#include "stdafx.h"
#include "GameObject.h"



void GameObject::update(float dt)
{
	rotate(90.f * dt, 0.f, 0.f);
}

void GameObject::setBufferData(std::vector<std::shared_ptr<InstancingBuffer>>& instancing_buffers)
{

	//위험해 보인다 ?? 
	instancing_buffers[0]->addBufferData(&world_);
}

void GameObject::move(glm::vec3 dir, float distance)
{
	dir = glm::normalize(dir);
	dir *= distance;

	world_[3].x += dir.x;
	world_[3].y += dir.y;
	world_[3].z += dir.z;
}

void GameObject::rotate(glm::mat4 mtx) {
	world_ = world_ * mtx;
}

void GameObject::rotate(float x, float y, float z) {
	glm::mat4 rotate_mtx;
	if (x != 0.0f)
	{
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(x), getRight());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetRight(), x));
	}
	if (y != 0.0f)
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(y), getUp());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetUp(), y));
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
		rotate_mtx = glm::rotate(glm::mat4(1.f), glm::radians(z), getLook());
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetLook(), z));
	}

	rotate(rotate_mtx);\
}

//--------------------------------getter--------------------------------
glm::vec3 GameObject::getRight() {
	return world_[0];
}
glm::vec3 GameObject::getUp() {
	return world_[1];
}
glm::vec3 GameObject::getLook() {
	return world_[2];
}
glm::vec3 GameObject::getPosition() {
	return world_[3];
}
glm::mat4 GameObject::getWorldMtx() {
	return world_;
}

GameObject::GameObject(int index)
{
	static constexpr float offset = 2.f;
	static constexpr float x = 10.f;

	world_ = glm::mat4(1.f);

	auto mtx = glm::rotate(glm::mat4(1.f), glm::radians(30.0f) * index, glm::vec3(0.f, 0.f, 1.f));
	rotate(mtx);
	move(glm::vec3(1.f, 0.f, 0.f), index * offset - x);
}


GameObject::~GameObject()
{
}
