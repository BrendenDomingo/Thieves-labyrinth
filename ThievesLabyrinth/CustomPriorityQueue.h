#pragma once
#include <vector>
#include <queue>
#include <algorithm>

template<class T,
	class _Container = std::vector<T>,
	class C = std::less<typename _Container::value_type> >
	class custom_priority_queue : public std::priority_queue<T, std::vector<T>, C>
{
public:
	bool remove(const T& value)
	{
		auto iter = std::find(this->c.begin(), this->c.end(), value);
		if (iter != this->c.end())
		{
			this->c.erase(iter);
			std::make_heap(this->c.begin(), this->c.end(), this->comp);
			return true;
		}

		return false;
	}

	void clear()
	{
		while (!this->empty())
		{
			this->pop();
		}
	}
};
