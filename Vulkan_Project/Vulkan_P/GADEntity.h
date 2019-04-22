#pragma once

namespace GADBased
{
	class GADWorld;

	class GADEntity
	{
	public:
		void setID(int id);
		const int const getID() const;
	private:
		int id_;

	public:
		GADEntity(unsigned int id);
		~GADEntity();

		//To put it in maps
		friend bool operator<(const GADEntity &l, const GADEntity &r);
	};

}
