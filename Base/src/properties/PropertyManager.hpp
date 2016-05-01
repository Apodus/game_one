
#pragma once

#include "util/logging.hpp"
#include <unordered_map>

#define PROPERTYCONTAINER_TYPE std::unordered_map<int, T>

template<typename T>
class PropertyManager {
	static PROPERTYCONTAINER_TYPE storage;

public:
	static T& getProperty(int objectID) {
		ASSERT(storage.find(objectID) != storage.end(), "invalid objectID for getProperty request! id=%d", objectID);
		return storage[objectID];
	}

	static T& createProperty(int objectID) {
		ASSERT(storage.find(objectID) == storage.end(), "property for objectID already exists! id=%d", objectID);
		return storage[objectID];
	}

	static T& insertProperty(int objectID, const T& t) {
		ASSERT(storage.find(objectID) == storage.end(), "property for objectID already exists! id=%d", objectID);
		T& v = storage[objectID];
		v = t;
		return v;
	}

	static void eraseProperty(int objectID) {
		ASSERT(storage.find(objectID) != storage.end(), "trying to erase property for objectID that does not exist! id=%d", objectID);
		storage.erase(objectID);
	}

	static PROPERTYCONTAINER_TYPE& accessDirect() {
		return storage;
	}
};

template<typename T> PROPERTYCONTAINER_TYPE PropertyManager<T>::storage;

template <typename T>
PROPERTYCONTAINER_TYPE& getPropertyContainer() {
	return PropertyManager<T>::accessDirect();
}

template <typename T>
T& getProperty(int objectID) {
	return PropertyManager<T>::getProperty(objectID);
}

template <typename T>
void removeProperty(int objectID) {
	PropertyManager<T>::removeProperty(objectID);
}

#undef PROPERTYCONTAINER_TYPE
