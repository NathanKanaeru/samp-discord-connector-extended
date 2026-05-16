#pragma once

#include "types.hpp"
#include "Singleton.hpp"
#include <string>
#include <vector>
#include <map>
#include <json.hpp>

// Discord component types as defined by the v10 API.
// We map every supported variant so the same enum can be used both
// inside message components (action rows) and inside modals.
enum class ComponentType
{
	ACTION_ROW         = 1,
	BUTTON             = 2,
	STRING_SELECT      = 3,
	TEXT_INPUT         = 4,
	USER_SELECT        = 5,
	ROLE_SELECT        = 6,
	MENTIONABLE_SELECT = 7,
	CHANNEL_SELECT     = 8,
	// Components V2 - used inside modals to wrap selects / file uploads
	LABEL              = 18,
	FILE_UPLOAD        = 19
};

enum class ButtonStyle
{
	PRIMARY   = 1,
	SECONDARY = 2,
	SUCCESS   = 3,
	DANGER    = 4,
	LINK      = 5
};

enum class TextInputStyle
{
	SHORT     = 1,
	PARAGRAPH = 2
};

class Component
{
public:
	Component(ComponentId_t id, ComponentType type) : m_ID(id), m_Type(type) {}
	virtual ~Component() = default;

	virtual nlohmann::json ToJson() const = 0;

	// Validate the component, returns true when payload is well-formed.
	// On failure the implementation should log an error explaining why
	// the component is rejected so the cause is visible to the caller.
	virtual bool Validate() const { return true; }

	ComponentId_t GetID()   const { return m_ID; }
	ComponentType GetType() const { return m_Type; }

protected:
	ComponentId_t m_ID;
	ComponentType m_Type;
};


// ---------------------- Button ----------------------
class Button : public Component
{
public:
	Button(ComponentId_t id, std::string text, ButtonStyle style,
		std::string custom_id, std::string url, bool disabled, EmojiId_t emoji);

	nlohmann::json ToJson() const override;
	bool Validate() const override;

private:
	std::string m_Text;
	ButtonStyle m_Style;
	std::string m_CustomID;
	std::string m_URL;
	bool        m_Disabled;
	EmojiId_t   m_Emoji;
};


// ---------------------- Select Menu ----------------------
struct SelectOption
{
	std::string label;
	std::string value;
	std::string description;
	EmojiId_t   emoji;
	bool        is_default;
};

class SelectMenu : public Component
{
public:
	SelectMenu(ComponentId_t id, ComponentType type,
		std::string custom_id, std::string placeholder,
		int min_values, int max_values, bool disabled);

	void AddOption(std::string label, std::string value,
		std::string description, EmojiId_t emoji, bool is_default);

	nlohmann::json ToJson() const override;
	bool Validate() const override;

private:
	std::string m_CustomID;
	std::string m_Placeholder;
	int  m_MinValues;
	int  m_MaxValues;
	bool m_Disabled;
	std::vector<SelectOption> m_Options;
};


// ---------------------- Text Input ----------------------
// Lives as a real Component now, so it can be added to a Modal or referenced
// from a Label wrapper. Persists in the same ComponentManager id space.
class TextInput : public Component
{
public:
	TextInput(ComponentId_t id, std::string custom_id, TextInputStyle style,
		std::string label, int min_length, int max_length, bool required,
		std::string value, std::string placeholder);

	nlohmann::json ToJson() const override;
	bool Validate() const override;

	std::string const& GetCustomID() const { return m_CustomID; }
	std::string const& GetLabel()    const { return m_Label; }

private:
	std::string    m_CustomID;
	TextInputStyle m_Style;
	std::string    m_Label;
	int            m_MinLength;
	int            m_MaxLength;
	bool           m_Required;
	std::string    m_Value;
	std::string    m_Placeholder;
};


// ---------------------- File Upload (modal only) ----------------------
class FileUpload : public Component
{
public:
	FileUpload(ComponentId_t id, std::string custom_id,
		int min_values, int max_values, bool required);

	nlohmann::json ToJson() const override;
	bool Validate() const override;

	std::string const& GetCustomID() const { return m_CustomID; }

private:
	std::string m_CustomID;
	int  m_MinValues;
	int  m_MaxValues;
	bool m_Required;
};


// ---------------------- Action Row ----------------------
class ActionRow
{
public:
	ActionRow(ActionRowId_t id) : m_ID(id) {}

	void AddComponent(ComponentId_t component);
	nlohmann::json ToJson() const;
	bool Validate() const;

	ActionRowId_t GetID() const { return m_ID; }
	std::vector<ComponentId_t> const& GetComponents() const { return m_Components; }

private:
	ActionRowId_t m_ID;
	std::vector<ComponentId_t> m_Components;
};


// ---------------------- Manager ----------------------
class ComponentManager : public Singleton<ComponentManager>
{
	friend class Singleton<ComponentManager>;
public:
	ComponentId_t CreateButton(std::string text, ButtonStyle style,
		std::string custom_id, std::string url, bool disabled, EmojiId_t emoji);

	ComponentId_t CreateSelectMenu(ComponentType type, std::string custom_id,
		std::string placeholder, int min_values, int max_values, bool disabled);

	bool AddSelectMenuOption(ComponentId_t menu, std::string label, std::string value,
		std::string description, EmojiId_t emoji, bool is_default);

	ComponentId_t CreateTextInput(std::string custom_id, TextInputStyle style,
		std::string label, int min_length, int max_length, bool required,
		std::string value, std::string placeholder);

	ComponentId_t CreateFileUpload(std::string custom_id,
		int min_values, int max_values, bool required);

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
