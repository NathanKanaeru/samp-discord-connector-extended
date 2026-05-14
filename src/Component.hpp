#pragma once

#include "types.hpp"
#include "Singleton.hpp"
#include <string>
#include <vector>
#include <map>
#include <json.hpp>

enum class ComponentType
{
	ACTION_ROW = 1,
	BUTTON = 2,
	STRING_SELECT = 3,
	TEXT_INPUT = 4,
	USER_SELECT = 5,
	ROLE_SELECT = 6,
	MENTIONABLE_SELECT = 7,
	CHANNEL_SELECT = 8
};

enum class ButtonStyle
{
	PRIMARY = 1,
	SECONDARY = 2,
	SUCCESS = 3,
	DANGER = 4,
	LINK = 5
};

class Component
{
public:
	Component(ComponentId_t id, ComponentType type) : m_ID(id), m_Type(type) {}
	virtual ~Component() = default;

	virtual nlohmann::json ToJson() const = 0;

	ComponentId_t GetID() const { return m_ID; }
	ComponentType GetType() const { return m_Type; }

protected:
	ComponentId_t m_ID;
	ComponentType m_Type;
};

class Button : public Component
{
public:
	Button(ComponentId_t id, std::string text, ButtonStyle style, std::string custom_id, std::string url, bool disabled, EmojiId_t emoji);
	nlohmann::json ToJson() const override;

private:
	std::string m_Text;
	ButtonStyle m_Style;
	std::string m_CustomID;
	std::string m_URL;
	bool m_Disabled;
	EmojiId_t m_Emoji;
};

struct SelectOption
{
	std::string label;
	std::string value;
	std::string description;
	EmojiId_t emoji;
	bool is_default;
};

class SelectMenu : public Component
{
public:
	SelectMenu(ComponentId_t id, ComponentType type, std::string custom_id, std::string placeholder, int min_values, int max_values, bool disabled);
	void AddOption(std::string label, std::string value, std::string description, EmojiId_t emoji, bool is_default);
	nlohmann::json ToJson() const override;

private:
	std::string m_CustomID;
	std::string m_Placeholder;
	int m_MinValues;
	int m_MaxValues;
	bool m_Disabled;
	std::vector<SelectOption> m_Options;
};

class ActionRow
{
public:
	ActionRow(ActionRowId_t id) : m_ID(id) {}
	void AddComponent(ComponentId_t component);
	nlohmann::json ToJson() const;

	ActionRowId_t GetID() const { return m_ID; }

private:
	ActionRowId_t m_ID;
	std::vector<ComponentId_t> m_Components;
};

class ComponentManager : public Singleton<ComponentManager>
{
	friend class Singleton<ComponentManager>;
public:
	ComponentId_t CreateButton(std::string text, ButtonStyle style, std::string custom_id, std::string url, bool disabled, EmojiId_t emoji);
	ComponentId_t CreateSelectMenu(ComponentType type, std::string custom_id, std::string placeholder, int min_values, int max_values, bool disabled);
	bool AddSelectMenuOption(ComponentId_t menu, std::string label, std::string value, std::string description, EmojiId_t emoji, bool is_default);

	ActionRowId_t CreateActionRow();
	bool AddComponentToActionRow(ActionRowId_t row, ComponentId_t component);

	Component_t const& FindComponent(ComponentId_t id);
	ActionRow_t const& FindActionRow(ActionRowId_t id);

	void Initialize() {}

private:
	std::map<ComponentId_t, Component_t> m_Components;
	std::map<ActionRowId_t, ActionRow_t> m_ActionRows;
	ComponentId_t m_NextComponentID = 1;
	ActionRowId_t m_NextActionRowID = 1;
};
