#ifndef SINGLETON_INSTANCE_H
#define SINGLETON_INSTANCE_H

template<typename T>
class Singleton
{
public:
	Singleton() {};
	virtual ~Singleton() {};

	static T* Instance()
	{
		static T instance;
		return &instance;
	}
};

#define sInstance(T) Singleton<T>::Instance()

#endif