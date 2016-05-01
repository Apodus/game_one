
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"

#include <sstream>

class GameContent {
public:
	const float posx = -0.60f;
	const float maxWidth = 2 * -posx;
	
	GameContent() : activated(false), optionContent(false) {}

	virtual bool isReady() const = 0;
	virtual void setTargetInternal() = 0;
	virtual void disableIfOption() {}

	bool isActive() const { return activated; }
	void activate() { activated = true; }

	void setTarget(float t) {
		targetY = t;
		setTargetInternal();
	}

	void adjustTarget(float diff) {
		targetY += diff;
		setTargetInternal();
	}

	float getHeight() const {
		return height;
	}

	void tick(float dt) {
		menuTickForward(dt);
	}

	bool isOption() const { return optionContent; }
	virtual bool gameContentInScreen() const = 0;
	virtual void menuTickForward(float dt) = 0;

	std::string faceTex;
	std::string faceSide;

	bool optionContent;
	bool activated;
	float targetY;
	float height;
};

class GameOption : public sa::MenuButton, public GameContent {
	std::string targetScript;
public:
	GameOption(sa::MenuComponent* parent, const std::string& text, const std::string& targetScript) : sa::MenuButton(parent, targetScript, sa::vec3<float>(0.0f, -1.0f, 0.0f), sa::vec3<float>(1, 0.1f, 1), "ButtonBase", text, Color::WHITE, Color::GOLDEN, sa::TextRenderer::Align::CENTER) {
		this->targetScript = targetScript;
		targetY = -1;
		height = 0.1f;
		optionContent = true;
		setTargetScale(sa::vec3<float>(maxWidth, height, 0.0f));
	}
	
	const std::string& getTargetScript() const {
		return targetScript;
	}

	virtual void disableIfOption() override {
		m_focus = false;
	}

	virtual bool isReady() const override {
		return true;
	}

	virtual void setTargetInternal() override {
		this->setTargetPosition(sa::vec3<float>(posx + getTargetScale().x / 2.0f, targetY, 0));
	}

	virtual bool gameContentInScreen() const {
		return inScreen();
	}

	virtual void menuTickForward(float dt) {
		MenuComponent::tick(dt);
	}

};

class GameLine : public sa::MenuComponent, public GameContent {
	const float speedScale = 150.0f;
	const float meldingCount = 5.0f;
	const float widthPadding = 0.05f;
	const float maxTextWidth = 2 * -posx - 0.1f;

	sa::MenuFrameBackground frame;
public:
	GameLine(sa::MenuComponent* parent, std::shared_ptr<sa::Graphics> graphics, std::string faceTex, std::string line, const std::string& direction, const sa::vec4<float>& lineColor)
		: sa::MenuComponent(parent, "GameLine", sa::vec3<float>(-1.0f, -1.0f, 0.0f), sa::vec3<float>(0.1f, 0.1f, 1)),
		frame(this, "BGFrame", "ButtonBase")
	{
		this->faceTex = faceTex;
		this->faceSide = direction;

        textVisible = 0;
		textLength = static_cast<float>(line.length());

        color = lineColor;
        align = sa::TextRenderer::LEFT;
		scale = 0.05f;

		auto splitter = [this, &graphics](std::string& text, float scale) -> std::string {
			std::stringstream ss(text);
			std::string line;
			std::string word;
			while (ss >> word) {
				if (graphics->m_fontLenka.getLength(line + " " + word, scale) >= maxTextWidth) {
					std::string remaining = word + " ";
					std::string remainingLine;
					while (ss.good() && !ss.eof()) {
						getline(ss, remainingLine);
						remaining += remainingLine + " ";
					}
					text = remaining;
					return line;
				}
				line += std::string(" ") + word;
			}
			return line;
		};

		float maxWidth = 0.0f;
		while (true) {
			float width = graphics->m_fontLenka.getLength(line, scale);

			if (width > maxTextWidth) {
				std::string extractedLine = splitter(line, scale);
				textLines.push_back(extractedLine);
				width = graphics->m_fontLenka.getLength(extractedLine, scale);
				if (width > maxWidth)
					maxWidth = width;
			}
			else {
				textLines.push_back(line);
				if (width > maxWidth)
					maxWidth = width;
				break;
			}
		}

		targetY = -1;
		height = textLines.size() * scale * 0.75f + 0.03f;
		setTargetScale(sa::vec3<float>(maxWidth + widthPadding, height, 0.0f));
		activated = false;
    }

    std::vector<std::string> textLines;
    float textVisible;
	float textLength;

    sa::vec4<float> color;
    sa::TextRenderer::Align align;
	float scale;

    virtual void childComponentCall(const std::string& who, const std::string& what, int value) override {}

    virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override {
		frame.visualise(graphics);

		int sumCharacters = 0;
		for (size_t i = 0; i < textLines.size(); ++i) {
			float numLines = static_cast<float>(textLines.size());
			float y_offset = 0.5f - (i + 1.0f) / (numLines + 1.0f);

			graphics->m_pTextRenderer->drawText(textLines[i], widthPadding / 2.0f + getPosition().x - getScale().x / 2, getPosition().y + y_offset * getScale().y, scale, color, align, graphics->m_fontLenka,
				[this, sumCharacters](const std::string&, int index, float& posx, float& posy, float& sx, float& sy, sa::vec4<float>& color) {
				/*
				sx *= 1.1f;
				float ydiff = sy * 0.1f;
				sy += ydiff;
				posy -= ydiff / 2;
				*/

				if (textVisible * speedScale > sumCharacters + index + meldingCount) color.a = 1;
				else if (textVisible * speedScale < sumCharacters + index) color.a = 0;
				else color.a = (textVisible * speedScale - (sumCharacters + index)) / meldingCount;
			});

			sumCharacters += textLines[i].length();
		}
    }

    virtual void update(float dt) override {
		frame.tick(dt);
		if (activated) {
			textVisible += dt;
		}
    }

    virtual bool isReady() const override {
		return textVisible * speedScale > textLength + meldingCount;
    }

	virtual void setTargetInternal() override {
		this->setTargetPosition(sa::vec3<float>(posx + getTargetScale().x / 2.0f, targetY, 0));
	}

	virtual bool gameContentInScreen() const {
		return inScreen();
	}

	virtual void menuTickForward(float dt) {
		MenuComponent::tick(dt);
	}
};
