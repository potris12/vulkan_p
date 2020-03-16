#pragma once

#include "InstancingBuffer.h"

/*
게임 오브젝트임 
이녀석은 world mtx를 가짐 
이녀석이 어떤 메시를 가지는지? 
Renderer를 가지고 있어야 하긴 해야함 어떻게 그릴지 결정할 객체가 필요함 이건.. 
*/
class GameObject
{
public:
	void update(float dt);
	void setBufferData(std::vector<std::shared_ptr<InstancingBuffer>>& instancing_buffers);

	void move(glm::vec3 dir, float distance);
	void rotate(glm::mat4 mtx);
	void rotate(float x, float y, float z);


	glm::vec3 getRight();
	glm::vec3 getUp();
	glm::vec3 getLook();
	glm::vec3 getPosition();
	glm::mat4 getWorldMtx();
private:
	glm::mat4 world_;

public:
	GameObject(int index);
	~GameObject();
};

