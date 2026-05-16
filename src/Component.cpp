#include "Component.hpp"
#include "Emoji.hpp"
#include "Logger.hpp"

using json = nlohmann::json;

namespace
{
	// Adds an "emoji" field to the given JSON object referenced by `j`.
	// Discord expects either { "name": "🙂" } for unicode or
	// { "id": "1234567" } for custom guild emojis.
	void AppendEmoji(json& j, EmojiId_t emoji_id)
	{
		if (emoji_id == INVALID_EMOJI_ID)
			return;

		auto const& emoji = EmojiManager::Get()->FindEmoji(emoji_id);
		if (!emoji)
			return;

		if (emoji->GetSnowflake().empty())
			j["emoji"] = { { "name", emoji->GetName() } };
		else
			j["emoji"] = { { "id", emoji->GetSnowflake() } };
	}
}


// ---------------------- Button ----------------------

Button::Button(ComponentId_t id, std::string text, ButtonStyle style,
	std::string custom_id, std::string url, bool disabled, EmojiId_t emoji) :
	Component(id, ComponentType::BUTTON),
	m_Text(std::move(text)),
	m_Style(style),
	m_CustomID(std::move(custom_id)),
	m_URL(std::move(url)),
	m_Disabled(disabled),
	m_Emoji(emoji)
{
}

bool Button::Validate() const
{
	// A button must have either a label or an emoji - Discord rejects
	// payloads that have neither, which is one of the most common reasons
	// for messages to silently fail to deliver.
	if (m_Text.empty() && m_Emoji == INVALID_EMOJI_ID)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"button {}: must have either a label or an emoji", m_ID);
		return false;
	}

	if (m_Style == ButtonStyle::LINK)
	{
		if (m_URL.empty())
		{
			Logger::Get()->Log(samplog_LogLevel::ERROR,
				"button {}: link buttons require a non-empty URL", m_ID);
			return false;
		}
		if (!m_CustomID.empty())
		{
			Logger::Get()->Log(samplog_LogLevel::WARNING,
				"button {}: link buttons must not set custom_id; ignoring it", m_ID);
		}
	}
	else
	{
		if (m_CustomID.empty())
		{
			Logger::Get()->Log(samplog_LogLevel::ERROR,
				"button {}: non-link buttons require a non-empty custom_id", m_ID);
			return false;
		}
		if (m_CustomID.length() > 100)
		{
			Logger::Get()->Log(samplog_LogLevel::ERROR,
				"button {}: custom_id is longer than the 100 char limit", m_ID);
			return false;
		}
		if (!m_URL.empty())
		{
			Logger::Get()->Log(samplog_LogLevel::WARNING,
				"button {}: non-link buttons must not set url; ignoring it", m_ID);
		}
	}

	if (m_Text.length() > 80)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"button {}: label is longer than the 80 char limit", m_ID);
		return false;
	}

	return true;
}

json Button::ToJson() const
{
	json j = {
		{ "type",  static_cast<int>(m_Type) },
		{ "style", static_cast<int>(m_Style) }
	};

	if (m_Disabled)
		j["disabled"] = true;

	if (!m_Text.empty())
		j["label"] = m_Text;

	if (m_Style == ButtonStyle::LINK)
		j["url"] = m_URL;
	else
		j["custom_id"] = m_CustomID;

	AppendEmoji(j, m_Emoji);
	return j;
}


// ---------------------- Select Menu ----------------------

SelectMenu::SelectMenu(ComponentId_t id, ComponentType type, std::string custom_id,
	std::string placeholder, int min_values, int max_values, bool disabled) :
	Component(id, type),
	m_CustomID(std::move(custom_id)),
	m_Placeholder(std::move(placeholder)),
	m_MinValues(min_values),
	m_MaxValues(max_values),
	m_Disabled(disabled)
{
}

void SelectMenu::AddOption(std::string label, std::string value,
	std::string description, EmojiId_t emoji, bool is_default)
{
	m_Options.push_back({ std::move(label), std::move(value),
		std::move(description), emoji, is_default });
}

bool SelectMenu::Validate() const
{
	if (m_CustomID.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"select menu {}: custom_id must not be empty", m_ID);
		return false;
	}
	if (m_CustomID.length() > 100)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"select menu {}: custom_id is longer than the 100 char limit", m_ID);
		return false;
	}
	if (m_Type == ComponentType::STRING_SELECT && m_Options.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"select menu {}: string select must have at least one option", m_ID);
		return false;
	}
	return true;
}

json SelectMenu::ToJson() const
{
	json j = {
		{ "type",      static_cast<int>(m_Type) },
		{ "custom_id", m_CustomID }
	};

	if (m_Disabled)
		j["disabled"] = true;

	if (!m_Placeholder.empty())
		j["placeholder"] = m_Placeholder;

	if (m_MinValues >= 0)
		j["min_values"] = m_MinValues;

	if (m_MaxValues >= 0)
		j["max_values"] = m_MaxValues;

	if (m_Type == ComponentType::STRING_SELECT)
	{
		json options = json::array();
		for (auto const& opt : m_Options)
		{
			json o = {
				{ "label",   opt.label },
				{ "value",   opt.value },
				{ "default", opt.is_default }
			};
			if (!opt.description.empty())
				o["description"] = opt.description;
			AppendEmoji(o, opt.emoji);
			options.push_back(o);
		}
		j["options"] = options;
	}

	return j;
}


// ---------------------- Text Input ----------------------

TextInput::TextInput(ComponentId_t id, std::string custom_id, TextInputStyle style,
	std::string label, int min_length, int max_length, bool required,
	std::string value, std::string placeholder) :
	Component(id, ComponentType::TEXT_INPUT),
	m_CustomID(std::move(custom_id)),
	m_Style(style),
	m_Label(std::move(label)),
	m_MinLength(min_length),
	m_MaxLength(max_length),
	m_Required(required),
	m_Value(std::move(value)),
	m_Placeholder(std::move(placeholder))
{
}

bool TextInput::Validate() const
{
	if (m_CustomID.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"text input {}: custom_id must not be empty", m_ID);
		return false;
	}
	if (m_Label.empty())
	{
		// In V1 modals (action row wrapping) Discord requires the field-level label.
		// In V2 modals (Label wrapper) the label belongs to the wrapper instead, so
		// missing label is OK there. We still warn so misuse is easier to spot.
		Logger::Get()->Log(samplog_LogLevel::DEBUG,
			"text input {}: label is empty (only valid when wrapped in a Label component)",
			m_ID);
	}
	return true;
}

json TextInput::ToJson() const
{
	json j = {
		{ "type",      static_cast<int>(m_Type) },
		{ "custom_id", m_CustomID },
		{ "style",     static_cast<int>(m_Style) },
		{ "required",  m_Required }
	};

	// In V2 modals the label is held by the surrounding Label component,
	// so only emit it when set.
	if (!m_Label.empty())
		j["label"] = m_Label;

	if (m_MinLength >= 0)
		j["min_length"] = m_MinLength;

	if (m_MaxLength >= 0)
		j["max_length"] = m_MaxLength;

	if (!m_Value.empty())
		j["value"] = m_Value;

	if (!m_Placeholder.empty())
		j["placeholder"] = m_Placeholder;

	return j;
}


// ---------------------- File Upload ----------------------

FileUpload::FileUpload(ComponentId_t id, std::string custom_id,
	int min_values, int max_values, bool required) :
	Component(id, ComponentType::FILE_UPLOAD),
	m_CustomID(std::move(custom_id)),
	m_MinValues(min_values),
	m_MaxValues(max_values),
	m_Required(required)
{
}

bool FileUpload::Validate() const
{
	if (m_CustomID.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"file upload {}: custom_id must not be empty", m_ID);
		return false;
	}
	return true;
}

json FileUpload::ToJson() const
{
	json j = {
		{ "type",      static_cast<int>(m_Type) },
		{ "custom_id", m_CustomID },
		{ "required",  m_Required }
	};

	if (m_MinValues >= 0)
		j["min_values"] = m_MinValues;
	if (m_MaxValues >= 0)
		j["max_values"] = m_MaxValues;

	return j;
}


// ---------------------- Action Row ----------------------

void ActionRow::AddComponent(ComponentId_t component)
{
	m_Components.push_back(component);
}

bool ActionRow::Validate() const
{
	if (m_Components.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"action row {}: must contain at least one component", m_ID);
		return false;
	}

	bool has_button = false;
	bool has_select = false;
	int  button_count = 0;

	for (auto cid : m_Components)
	{
		auto const& comp = ComponentManager::Get()->FindComponent(cid);
		if (!comp)
		{
			Logger::Get()->Log(samplog_LogLevel::ERROR,
				"action row {}: contains unknown component id {}", m_ID, cid);
			return false;
		}

		if (!comp->Validate())
			return false;

		switch (comp->GetType())
		{
		case ComponentType::BUTTON:
			has_button = true;
			++button_count;
			break;
		case ComponentType::STRING_SELECT:
		case ComponentType::USER_SELECT:
		case ComponentType::ROLE_SELECT:
		case ComponentType::MENTIONABLE_SELECT:
		case ComponentType::CHANNEL_SELECT:
			has_select = true;
			break;
		default:
			Logger::Get()->Log(samplog_LogLevel::ERROR,
				"action row {}: component {} of type {} cannot live inside an action row",
				m_ID, cid, static_cast<int>(comp->GetType()));
			return false;
		}
	}

	if (has_button && has_select)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"action row {}: buttons and select menus cannot coexist", m_ID);
		return false;
	}
	if (button_count > 5)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"action row {}: at most 5 buttons per row, got {}", m_ID, button_count);
		return false;
	}
	if (has_select && m_Components.size() > 1)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"action row {}: only one select menu per row is allowed", m_ID);
		return false;
	}

	return true;
}

json ActionRow::ToJson() const
{
	json components = json::array();
	for (auto id : m_Components)
	{
		auto const& comp = ComponentManager::Get()->FindComponent(id);
		if (comp)
			components.push_back(comp->ToJson());
	}

	return {
		{ "type", 1 }, // Action Row
		{ "components", components }
	};
}


// ---------------------- Manager ----------------------

ComponentId_t ComponentManager::CreateButton(std::string text, ButtonStyle style,
	std::string custom_id, std::string url, bool disabled, EmojiId_t emoji)
{
	ComponentId_t id = m_NextComponentID++;
	m_Components.emplace(id, Component_t(new Button(id,
		std::move(text), style, std::move(custom_id), std::move(url),
		disabled, emoji)));
	return id;
}

ComponentId_t ComponentManager::CreateSelectMenu(ComponentType type, std::string custom_id,
	std::string placeholder, int min_values, int max_values, bool disabled)
{
	ComponentId_t id = m_NextComponentID++;
	m_Components.emplace(id, Component_t(new SelectMenu(id, type,
		std::move(custom_id), std::move(placeholder), min_values, max_values, disabled)));
	return id;
}

bool ComponentManager::AddSelectMenuOption(ComponentId_t menu, std::string label, std::string value,
	std::string description, EmojiId_t emoji, bool is_default)
{
	auto const& comp = FindComponent(menu);
	if (!comp || comp->GetType() != ComponentType::STRING_SELECT)
		return false;

	static_cast<SelectMenu*>(comp.get())->AddOption(
		std::move(label), std::move(value), std::move(description), emoji, is_default);
	return true;
}

ComponentId_t ComponentManager::CreateTextInput(std::string custom_id, TextInputStyle style,
	std::string label, int min_length, int max_length, bool required,
	std::string value, std::string placeholder)
{
	ComponentId_t id = m_NextComponentID++;
	m_Components.emplace(id, Component_t(new TextInput(id,
		std::move(custom_id), style, std::move(label),
		min_length, max_length, required,
		std::move(value), std::move(placeholder))));
	return id;
}

ComponentId_t ComponentManager::CreateFileUpload(std::string custom_id,
	int min_values, int max_values, bool required)
{
	ComponentId_t id = m_NextComponentID++;
	m_Components.emplace(id, Component_t(new FileUpload(id,
		std::move(custom_id), min_values, max_values, required)));
	return id;
}

ActionRowId_t ComponentManager::CreateActionRow()
{
	ActionRowId_t id = m_NextActionRowID++;
	m_ActionRows.emplace(id, ActionRow_t(new ActionRow(id)));
	return id;
}

bool ComponentManager::AddComponentToActionRow(ActionRowId_t row, ComponentId_t component)
{
	auto const& ar = FindActionRow(row);
	if (!ar)
		return false;

	auto const& comp = FindComponent(component);
	if (!comp)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"action row {}: tried to add unknown component id {}", row, component);
		return false;
	}

	// Action rows in messages can only host buttons or select menus.
	switch (comp->GetType())
	{
	case ComponentType::BUTTON:
	case ComponentType::STRING_SELECT:
	case ComponentType::USER_SELECT:
	case ComponentType::ROLE_SELECT:
	case ComponentType::MENTIONABLE_SELECT:
	case ComponentType::CHANNEL_SELECT:
		break;
	default:
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"action row {}: component {} (type {}) cannot be added to a message action row "
			"(text input / file upload belong inside a modal)",
			row, component, static_cast<int>(comp->GetType()));
		return false;
	}

	ar->AddComponent(component);
	return true;
}

Component_t const& ComponentManager::FindComponent(ComponentId_t id)
{
	static Component_t invalid;
	auto it = m_Components.find(id);
	if (it == m_Components.end())
		return invalid;
	return it->second;
}

ActionRow_t const& ComponentManager::FindActionRow(ActionRowId_t id)
{
	static ActionRow_t invalid;
	auto it = m_ActionRows.find(id);
	if (it == m_ActionRows.end())
		return invalid;
	return it->second;
}
