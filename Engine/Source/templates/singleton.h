#pragma once

template<class T>
class Singleton: public T {
	public: 
		static T* Instance();
		static void Destroy();

	private: 
		static Singleton<T> *pInstance;

	protected:
		Singleton() {};
		~Singleton() {};
		Singleton(Singleton const&) {};
		Singleton& operator=(Singleton const&) {};
};

template<class T>
T* Singleton<T>::Instance() {
	if ( pInstance == nullptr ) {
        pInstance = new Singleton<T>();
		std::atexit(Destroy);
    }
    return static_cast<T*>(pInstance);
}

template<class T>
void Singleton<T>::Destroy() {
	if (pInstance != nullptr ) {
		delete pInstance;
        pInstance = nullptr;
    }
}

template<class T>
Singleton<T>* Singleton<T>::pInstance = nullptr;