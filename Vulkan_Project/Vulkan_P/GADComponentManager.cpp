#include "GADComponentManager.h"

using namespace GAD;

template<class ComponentType>
void GADComponentManager<ComponentType>::awake() {

}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::start()
{
}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::update()
{
}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::destroy()
{
}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::addComponent(std::shared_ptr<GADComponent<ComponentType>> component)
{
}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::removeComponent(std::shared_ptr<GADComponent<ComponentType>> component)
{
}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::removeComponent(std::string & name)
{
}

template<class ComponentType>
void GAD::GADComponentManager<ComponentType>::forEash(std::function<void()> f)
{
}

template<class ComponentType>
GAD::GADComponentManager<ComponentType>::GADComponentManager(const std::string & name) : BaseComponentManager(name)
{
}

template<class ComponentType>
GAD::GADComponentManager<ComponentType>::~GADComponentManager()
{
}
