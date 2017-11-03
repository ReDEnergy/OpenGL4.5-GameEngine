#pragma once

template<class T>
class Instance: public T
{
	public:
		Instance() {};
};

#define INSTANCE(Class) new Instance<Class>()

