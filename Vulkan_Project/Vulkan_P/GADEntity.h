#pragma once

#include "stdafx.h"
#include "GADBase.h"
#include "GADComponent.h"

namespace GAD {
	class GADWorld;
	class GADEntityHandle;

	class GADEntity : public GADBase
	{
		friend class GADEntityHandle;
	public:
		const int64_t key_ = 0;

		void awake();
		void start();
		void update();
		void destroy();

	private:
		
		template<class ComponentType>
		ComponentType* addComponent(ComponentType* component) {
			
			components_.push_back(component);
			return static_cast<ComponentType*>(components_[components_.size() - 1]);
		}

		std::vector<GADComponentBase*> components_;
	public:
		GADEntity();
		GADEntity(const std::string& name, const int64_t key_);

		~GADEntity();
	};

}

