
#pragma once

#include <vector>

class ObjectID {
	int nextID;
	std::vector<int> freeIDs;

public:
	ObjectID() : nextID(0) {
	}

	int makeID() {
		if(freeIDs.empty())
			return ++nextID;
		int id = freeIDs.back();
		freeIDs.pop_back();
		return id;
	}

	int freeID(int id) {
		freeIDs.push_back(id);
	}
};