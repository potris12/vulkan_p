#pragma once

#include "GADComponentMask.h"
#include "GADEntity.h"

namespace GADBased {

	class GADWorld;

	class GADSystem
	{
	public:
		GADSystem() = default;
		virtual ~GADSystem() = default;
		GADSystem(const GADSystem&) = default;
		GADSystem &operator=(const GADSystem &) = default;
		GADSystem(GADSystem &&) = default;
		GADSystem &operator=(GADSystem &&) = default;

		//Called before the game starts but after the world initializes
		virtual void init() {};

		//Called every game update
		virtual void update(float dt) {};

		//Called every frame
		virtual void render() {};

		//When a system is added to the world, the world will register itself
		void registerWorld(GADWorld* world);

		//When a component is added such that his system should begin acting on it,
		//register will be called
		void registerEntity(const GADEntity &entity);

		//If a component is removed from an entity such that the system should stop
		//acting on it, unRegister will be called
		void unRegisterEntity(const GADEntity &entity);
		GADComponentMask getSignature();

	protected:
		std::vector<GADEntity> registeredEntities;
		GADWorld* parentWorld;
		GADComponentMask signature;

	public:
		//GADSystem();
		//~GADSystem();
	};


}
