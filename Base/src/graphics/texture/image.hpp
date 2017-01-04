
#pragma once

#include <string>

struct Image {
	Image(): data(nullptr) {
		sizeX = 0;
		sizeY = 0;
	}

	~Image() {
		if(data)
			unload();
	}
	
	void loadImage(const std::string& filename);
	void unload();
	
	unsigned long sizeX;
	unsigned long sizeY;
	char *data = nullptr;
	bool hasAlpha;
};

