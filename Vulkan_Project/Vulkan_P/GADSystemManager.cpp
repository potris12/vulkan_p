#include "GADSystemManager.h"

using namespace GAD;

void GAD::GADSystemManager::awake()
{
	for (auto system : systems_)
	{
		system->awake();
	}
}

void GAD::GADSystemManager::start()
{
	for (auto system : systems_)
	{
		system->start();
	}
}

void GAD::GADSystemManager::update()
{
	for (auto system : systems_)
	{
		system->update();
	}
}

void GAD::GADSystemManager::destroy()
{
	for (auto system : systems_)
	{
		system->destroy();
	}
}

void GAD::GADSystemManager::addSystem(std::shared_ptr<GADSystem> system)
{
	systems_.push_back(system);
}

void GAD::GADSystemManager::removeSystem(std::shared_ptr<GADSystem> system)
{
	systems_.erase(std::remove_if(systems_.begin(),
		systems_.end(),
		[weak_system = std::weak_ptr<GADSystem>(system)](auto data) {
		if (auto real_system = weak_system.lock()) {
			return data == real_system;
		}
		return false;
	}),
		systems_.end());

}

void GAD::GADSystemManager::removeSystem(std::string & name)
{
	systems_.erase(std::remove_if(systems_.begin(),
		systems_.end(),
		[name](auto data) {
		return data->getName().compare(name) == 0;
	}),
		systems_.end());

}

std::shared_ptr<GADSystem> GAD::GADSystemManager::getSystem(std::string & name)
{
	auto iter = std::find_if(systems_.begin(), systems_.end(), [name](auto data) {
		return data->getName().compare(name) == 0;
	});

	if (iter != systems_.end()) {
		return *iter;
	}
	return nullptr;
}

GADSystemManager::GADSystemManager(const std::string& name) : GADBase(name)
{

}


GADSystemManager::~GADSystemManager()
{
}
