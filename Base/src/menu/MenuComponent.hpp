

#pragma once

#include "util/vec3.hpp"
#include "input/userio.hpp"
#include "graphics/window/window.hpp"
#include "graphics/graphics.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace sa {
	class MeshRenderer;

	class MenuComponent {

	protected:
		MenuComponent* m_pParent;
		std::shared_ptr<UserIO> m_pUserIO;
		std::shared_ptr<Window> m_pWindow;

		std::string m_name;
		bool m_focus;
		bool instantPositionUpdate = false;
		bool instantScaleUpdate = false;

		std::vector<std::shared_ptr<MenuComponent>> m_children;

	protected:
		sa::vec3<float> m_currentPosition;
		sa::vec3<float> m_worldPosition;
		std::function<sa::vec3<float>()> m_targetPosition;

		sa::vec3<float> m_currentScale;
		sa::vec3<float> m_worldScale;
		sa::vec3<float> m_targetScale;

		std::function<sa::vec3<float>()> m_defaultPosition;
		sa::vec3<float> m_defaultScale;

	private:
		void updatePosition() {
			if (m_pParent == nullptr) {
				m_worldPosition = m_currentPosition;
			}
			else {
				m_worldPosition = m_currentPosition;
			}
		}

		void updateScale() {
			if (m_pParent == nullptr) {
				m_worldScale = m_currentScale;
			}
			else {
				m_worldScale = m_currentScale;
			}
		}

		virtual void childComponentCall(const std::string& who, const std::string& what, int value = 0) = 0;
		virtual void draw(std::shared_ptr<sa::Graphics> graphics) const = 0;
		virtual void update(float dt) = 0;

	public:
		enum PositionAlign
		{
			NONE = 0,
			LEFT = 1,
			RIGHT = 2,
			TOP = 4,
			BOTTOM = 8
		};

		// defaults to middle x middle
		int positionAlign = PositionAlign::NONE;

		MenuComponent(
			std::shared_ptr<Window> window,
			std::shared_ptr<UserIO> input,
			const std::string& name,
			const sa::vec3<float>& position,
			const sa::vec3<float>& scale
		) {
			this->m_pParent = nullptr;
			this->m_pUserIO = input;
			this->m_pWindow = window;
			this->m_name = name;

			m_currentScale = scale;
			m_targetScale = scale;
			m_defaultScale = scale;

			m_currentPosition = position;
			m_targetPosition = [position]() { return position; };
			m_defaultPosition = [position]() { return position; };

			m_focus = true;
		}

		MenuComponent(
			MenuComponent* parent,
			const std::string& name,
			std::function<sa::vec3<float>()> position,
			const sa::vec3<float>& scale
		) {
			this->m_pParent = parent;
			this->m_pUserIO = parent->m_pUserIO;
			this->m_pWindow = parent->m_pWindow;
			this->m_name = name;

			m_currentScale = scale;
			m_targetScale = scale;
			m_defaultScale = scale;

			m_currentPosition = position();
			m_targetPosition = position;
			m_defaultPosition = position;

			m_focus = true;
		}

		MenuComponent(
			MenuComponent* parent,
			const std::string& name,
			const sa::vec3<float>& position,
			const sa::vec3<float>& scale
		) {
			this->m_pParent = parent;
			this->m_pUserIO = parent->m_pUserIO;
			this->m_pWindow = parent->m_pWindow;
			this->m_name = name;

			m_currentScale = scale;
			m_targetScale = scale;
			m_defaultScale = scale;

			m_currentPosition = position;
			m_targetPosition = [position]() { return position; };
			m_defaultPosition = [position]() { return position; };

			m_focus = true;
		}

		virtual ~MenuComponent() {
		}

		void setPositionUpdateType(bool instant)
		{
			instantPositionUpdate = instant;
		}

		void setScaleUpdateType(bool instant)
		{
			instantScaleUpdate = instant;
		}

		void addChild(std::shared_ptr<MenuComponent> child) {
			m_children.push_back(child);
		}

		void tick(float dt) {
			float inverseAR = 1.0f / m_pWindow->getAspectRatio();
			vec3<float> aspectFix = vec3<float>(1, inverseAR, 1);

			if (!instantPositionUpdate)
			{
				m_currentPosition += (m_targetPosition() * aspectFix - m_currentPosition) * dt * 5;
			}
			else
			{
				m_currentPosition = m_targetPosition() * aspectFix;
			}

			if (!instantScaleUpdate)
			{
				m_currentScale += (m_targetScale - m_currentScale) * dt * 8;
			}
			else
			{
				m_currentScale = m_targetScale;
			}

			updatePosition();
			updateScale();

			if (positionAlign & LEFT)
				m_worldPosition.x += m_worldScale.x * 0.5f;
			if (positionAlign & RIGHT)
				m_worldPosition.x -= m_worldScale.x * 0.5f;
			if (positionAlign & TOP)
				m_worldPosition.y -= m_worldScale.y * 0.5f;
			if (positionAlign & BOTTOM)
				m_worldPosition.y += m_worldScale.y * 0.5f;

			update(dt);
			for (auto child : m_children) {
				child->tick(dt);
			}
		}

		vec3<float> recursivePosition() const {
			if (m_pParent) {
				return m_pParent->recursivePosition() + m_worldPosition;
			}
			return m_worldPosition;
		}

		void visualise(std::shared_ptr<Graphics> graphics) const {
			if (inScreen()) {
				draw(graphics);
			}
			for (auto child : m_children) {
				child->visualise(graphics);
			}
		}

		void callParent(const std::string& eventName, int value = 0) const {
			getParent()->childComponentCall(m_name, eventName, value);
		}

		bool hasFocus() const {
			return m_focus;
		}

		// only true for axis aligned rectangles!!
		// If you need more accurate checks, implement them for your component.
		bool inComponent(float x, float y) const {
			float halfWidth = m_worldScale.x * 0.5f;
			float halfHeight = m_worldScale.y * 0.5f;
			bool x_inComponent = !(x < m_worldPosition.x - halfWidth || x > m_worldPosition.x + halfWidth);
			bool y_inComponent = !(y < m_worldPosition.y - halfHeight || y > m_worldPosition.y + halfHeight);
			return x_inComponent && y_inComponent;
		}

		bool isMouseOver() const {
			sa::vec3<float> mousePos;
			m_pUserIO->getCursorPosition(mousePos);
			mousePos.y /= m_pWindow->getAspectRatio();
			return inComponent(mousePos.x, mousePos.y);
		}

		bool inScreen() const {
			bool out = false;
			float inverseAR = 1.0f / m_pWindow->getAspectRatio();
			out |= m_worldPosition.x + m_worldScale.x * 0.5f < -1.0f;
			out |= m_worldPosition.x - m_worldScale.x * 0.5f > +1.0f;
			out |= (m_worldPosition.y + m_worldScale.y * 0.5f) < -1.0f * inverseAR;
			out |= (m_worldPosition.y - m_worldScale.y * 0.5f) > +1.0f * inverseAR;
			return !out;
		}

		MenuComponent* getParent() const {
			return m_pParent;
		}

		const sa::vec3<float>& getRelativePosition() const {
			return m_currentPosition;
		}

		const sa::vec3<float>& getPosition() const {
			return m_worldPosition;
		}

		const sa::vec3<float>& getScale() const {
			return m_worldScale;
		}

		void setPosition(const sa::vec3<float>& pos) {
			m_worldPosition = pos;
		}

		void setScale(const sa::vec3<float>& scale) {
			m_worldScale = scale;
		}

		void setFocus(bool v) {
			m_focus = v;
		}

		virtual void show() {
			m_focus = true;
			m_targetPosition = m_defaultPosition;
			m_targetScale = m_defaultScale;
		}

		virtual void hide() {
			m_focus = false;
			m_targetPosition = [this]() { auto value = m_defaultPosition(); value.x = -2.5f; return value; };
			// m_targetScale = sa::vec3<float>(0, 0, 0);
		}

		sa::vec3<float> getExteriorPosition(PositionAlign positionAlign) {
			sa::vec3<float> result = m_worldPosition;
			float aspectRatio = m_pWindow->getAspectRatio();
			if (positionAlign & LEFT)
				result.x -= m_worldScale.x * 0.5f;
			if (positionAlign & RIGHT)
				result.x += m_worldScale.x * 0.5f;
			if (positionAlign & TOP)
				result.y += m_worldScale.y * 0.5f;
			if (positionAlign & BOTTOM)
				result.y -= m_worldScale.y * 0.5f;
			
			// since aspect ratio is fixed automatically in destination,
			// we have to break this final value with aspect ratio.
			result.y *= aspectRatio;
			return result;
		}

		sa::vec3<float> getExteriorPositionForChild(PositionAlign positionAlign) {
			sa::vec3<float> result = sa::vec3<float>();
			float aspectRatio = m_pWindow->getAspectRatio();
			if (positionAlign & LEFT)
				result.x -= m_worldScale.x * 0.5f;
			if (positionAlign & RIGHT)
				result.x += m_worldScale.x * 0.5f;
			if (positionAlign & TOP)
				result.y += m_worldScale.y * 0.5f;
			if (positionAlign & BOTTOM)
				result.y -= m_worldScale.y * 0.5f;

			// since aspect ratio is fixed automatically in destination,
			// we have to break this final value with aspect ratio.
			result.y *= aspectRatio;
			return result;
		}

		sa::vec3<float> getLocalExteriorPosition(PositionAlign positionAlign) {
			sa::vec3<float> result = m_worldPosition;
			float aspectRatio = m_pWindow->getAspectRatio();
			if (positionAlign & LEFT)
				result.x -= m_currentScale.x * 0.5f;
			if (positionAlign & RIGHT)
				result.x += m_currentScale.x * 0.5f;
			if (positionAlign & TOP)
				result.y += m_currentScale.y * 0.5f;
			if (positionAlign & BOTTOM)
				result.y -= m_currentScale.y * 0.5f;

			// since aspect ratio is fixed automatically in destination,
			// we have to break this final value with aspect ratio.
			result.y *= aspectRatio;
			return result;
		}

		sa::vec3<float>& getTargetScale() {
			return m_targetScale;
		}

		sa::vec3<float> getTargetPosition() {
			return m_targetPosition();
		}

		void setTargetPosition(std::function<sa::vec3<float>()> posFun) {
			m_targetPosition = std::move(posFun);
			m_defaultPosition = m_targetPosition;
		}

		void setTargetScale(const sa::vec3<float>& scale) {
			m_targetScale = scale;
		}

		void setDefaultScale(const sa::vec3<float>& scale) {
			m_defaultScale = scale;
		}

	};

}

inline sa::MenuComponent::PositionAlign operator | (sa::MenuComponent::PositionAlign a, sa::MenuComponent::PositionAlign b) {
	return static_cast<sa::MenuComponent::PositionAlign>(static_cast<int>(a) | static_cast<int>(b));
}