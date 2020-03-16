#pragma once

#include "InstancingBuffer.h"

/*
���� ������Ʈ�� 
�̳༮�� world mtx�� ���� 
�̳༮�� � �޽ø� ��������? 
Renderer�� ������ �־�� �ϱ� �ؾ��� ��� �׸��� ������ ��ü�� �ʿ��� �̰�.. 
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

