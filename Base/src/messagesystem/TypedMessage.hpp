
#pragma once

#include <vector>
#include <mutex>

template<class T>
class TypedMessage {
	
	static std::vector<TypedMessage<T>*> listeners;
	static std::mutex listener_mutex;
	
public:
	virtual void handle(const T&) = 0;
	
	TypedMessage() {
		std::unique_lock<std::mutex> lock(listener_mutex);
		listeners.push_back(this);
	}
	
	virtual ~TypedMessage() {
		std::unique_lock<std::mutex> lock(listener_mutex);
		for (auto it = listeners.begin(); it != listeners.end(); ++it) {
			if (*it == this) {
				listeners.erase(it);
				return;
			}
		}
	}

	static void sendMessage(const T& t) {
		std::unique_lock<std::mutex> lock(listener_mutex);
		for(TypedMessage<T>* listener : listeners) {
			listener->handle(t);
		}
	}
};

template<class T> std::vector<TypedMessage<T>*> TypedMessage<T>::listeners;
template<class T> std::mutex TypedMessage<T>::listener_mutex;

template<class T> void sendMessage(const T& t) {
	TypedMessage<T>::sendMessage(t);
}
