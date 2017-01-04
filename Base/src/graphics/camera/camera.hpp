#pragma once

#include "math/matrix/matrix4.hpp"
#include "util/vec3.hpp"

namespace sa {
	class Camera {
	public:
		Camera();
		~Camera();

		void setOrtho(float width, float height, float zNear, float zFar);
		void setProjection(float zNear, float zFar, float aspect);
		void setPosition(const sa::vec3<float>& cameraPosition);

		const sa::Matrix4& getView() const;
		const sa::Matrix4& getProjection() const;
	private:
		sa::Matrix4 view, projection;
		sa::vec3<float> cameraPosition; // Phoe TODO: Get rid of this
	};
}