

#pragma once

#include "util/vec3.hpp"
#include "input/userio.hpp"
#include "graphics/window/window.hpp"
#include "graphics/graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sa {
  class MeshRenderer;

  class MenuComponent {

  protected:
    MenuComponent* m_pParent;
    std::shared_ptr<UserIO> m_pUserIO;
    std::shared_ptr<Window> m_pWindow;

    std::string m_name;
    bool m_focus;

    std::vector<std::shared_ptr<MenuComponent>> m_children;

  protected:
    sa::vec3<float> m_currentPosition;
    sa::vec3<float> m_worldPosition;
    sa::vec3<float> m_targetPosition;

    sa::vec3<float> m_currentScale;
    sa::vec3<float> m_worldScale;
    sa::vec3<float> m_targetScale;

    sa::vec3<float> m_defaultPosition;
    sa::vec3<float> m_defaultScale;

  private:
    void updatePosition() {
      if(m_pParent == nullptr) {
        m_worldPosition = m_currentPosition;
      }
      else {
				m_worldPosition = m_pParent->getPosition() + m_currentPosition * m_pParent->getScale();
      }
    }

    void updateScale() {
      if(m_pParent == nullptr) {
        m_worldScale = m_currentScale;
      }
      else {
        m_worldScale = m_pParent->getScale() * m_currentScale;
      }
    }

    virtual void childComponentCall(const std::string& who, const std::string& what, int value = 0) = 0;
    virtual void draw(std::shared_ptr<sa::Graphics> graphics) const = 0;
    virtual void update(float dt) = 0;

  public:
		enum PositionAlign
		{
			LEFT = 1,
			RIGHT = 2,
			TOP = 4,
			BOTTOM = 8
		};

		// defaults to middle x middle
		int positionAlign = 0;

    MenuComponent(std::shared_ptr<Window> window, std::shared_ptr<UserIO> input, const std::string& name, const sa::vec3<float>& position, const sa::vec3<float>& scale) {
      this->m_pParent = nullptr;
      this->m_pUserIO = input;
      this->m_pWindow = window;
      this->m_name = name;

      m_currentScale = scale;
      m_targetScale = scale;
      m_defaultScale = scale;

      m_currentPosition = position;
      m_targetPosition = position;
      m_defaultPosition = position;

      m_focus = true;
    }

    MenuComponent(MenuComponent* parent, const std::string& name, const sa::vec3<float>& position, const sa::vec3<float>& scale) {
      this->m_pParent = parent;
      this->m_pUserIO = parent->m_pUserIO;
      this->m_pWindow = parent->m_pWindow;
      this->m_name = name;

      m_currentScale = scale;
      m_targetScale = scale;
      m_defaultScale = scale;

      m_currentPosition = position;
      m_targetPosition = position;
      m_defaultPosition = position;

      m_focus = true;
    }

    virtual ~MenuComponent() {
    }

    void addChild(std::shared_ptr<MenuComponent> child) {
      m_children.push_back(child);
    }

    void tick(float dt) {
			float inverseAR = 1.0f / m_pWindow->getAspectRatio();
			vec3<float> aspectFix = vec3<float>(1, inverseAR, 1);
      m_currentPosition += (m_targetPosition * aspectFix - m_currentPosition) * dt * 2;
      m_currentScale += (m_targetScale - m_currentScale) * dt * 2;

      updatePosition();
      updateScale();

			if (positionAlign & LEFT)
				m_worldPosition.x += m_worldScale.x * 0.5f;
			if (positionAlign & RIGHT)
				m_worldPosition.x -= m_worldScale.x * 0.5f;
			if (positionAlign & TOP)
				m_worldPosition.y -= m_worldScale.y * 0.5f * inverseAR;
			if (positionAlign & BOTTOM)
				m_worldPosition.y += m_worldScale.y * 0.5f * inverseAR;

      update(dt);

      if(inScreen()) {
        for(auto child : m_children) {
          child->tick(dt);
        }
      }
    }

    void visualise(std::shared_ptr<Graphics> graphics) const {
      if(inScreen()) {
        draw(graphics);
        for(auto child : m_children) {
          child->visualise(graphics);
        }
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
      // mousePos.y /= m_pWindow->getAspectRatio();
      return inComponent(mousePos.x, mousePos.y);
    }

    bool inScreen() const {
      bool out = false;
      float aspectRatio = m_pWindow->getAspectRatio();
      out |= m_worldPosition.x + m_worldScale.x * 0.5f < -1.0f;
      out |= m_worldPosition.x - m_worldScale.x * 0.5f > +1.0f;
      out |= (m_worldPosition.y + m_worldScale.y * 0.5f) < -1.0f;
      out |= (m_worldPosition.y - m_worldScale.y * 0.5f) > +1.0f;
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

    void show() {
      m_focus = true;
      m_targetPosition = m_defaultPosition;
      m_targetScale = m_defaultScale;
    }

    void hide() {
      m_focus = false;
      m_targetPosition.x = -3.0f;
      m_targetScale = sa::vec3<float>(0, 0, 0);
    }

	sa::vec3<float>& getTargetScale() {
		return m_targetScale;
	}

	sa::vec3<float>& getTargetPosition() {
		return m_targetPosition;
	}

    void setTargetPosition(const sa::vec3<float>& pos) {
      m_targetPosition = pos;
    }

    void setTargetScale(const sa::vec3<float>& scale) {
      m_targetScale = scale;
    }

  };

}