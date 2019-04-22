#pragma once

#include "GADComponentHandle.h"
#include "GADEntity.h"
#include "GADWorld.h"

namespace GADBased {
/*
ShieldComponent shield;
if(shiled.health <= 0)[
mgr->destroyComponent(entity);
...

->

ShieldHandle shiled;
if(shield.component->health <= 0){
shield.destroy();
...

*/

	class GADEntityHandle {
	public:
		void destroy() {
			world_->destroyEntity(entity_);
		}

		template<class ComponentType>
		void addComponent(ComponentType c) {
			world->addComponent(entity, c);
		}

		template<class ComponentType>
		void removeComponent() {
			world->removeComponent<ComponentType>(entity);
		}

		template<class ComponentType>
		ComponentHandle<ComponentType> getComponent() {
			ComponentHandle<ComponentType> handle;
			world->unPack(endtity, handle);
			return handle;
		}

	private:
		GADEntity entity_;
		GADWorld* world_;

	public:
		GADEntityHandle(GADEntity entity, GADWorld* world) : entity_(entity), world_(world) {}

	};

}
