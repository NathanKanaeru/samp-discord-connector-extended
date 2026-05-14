#include "Component.hpp"
#include "Emoji.hpp"
#include "Logger.hpp"

using json = nlohmann::json;

Button::Button(ComponentId_t id, std::string text, ButtonStyle style, std::string custom_id, std::string url, bool disabled, EmojiId_t emoji) :
	Component(id, ComponentType::BUTTON),
	m_Text(text),
	m_Style(style),
	m_CustomID(custom_id),
	m_URL(url),
	m_Disabled(disabled),
	m_Emoji(emoji)
{
}

json Button::ToJson() const
{
	json j = {
		{ "type", static_cast<int>(m_Type) },
		{ "style", static_cast<int>(m_Style) },
		{ "disabled", m_Disabled }
	};

	if (!m_Text.empty())
		j["label"] = m_Text;

	if (m_Style == ButtonStyle::LINK)
	{
		j["url"] = m_URL;
	}
	else
	{
		j["custom_id"] = m_CustomID;
	}

	if (m_Emoji != INVALID_EMOJI_ID)
	{
		auto const& emoji = EmojiManager::Get()->FindEmoji(m_Emoji);
		if (emoji)
		{
			if (emoji->GetSnowflake().empty())
				j["emoji"] = { { "name", emoji->GetName() } };
			else
				j["emoji"] = { { "id", emoji->GetSnowflake() } };
		}
	}

	return j;
}

SelectMenu::SelectMenu(ComponentId_t id, ComponentType type, std::string custom_id, std::string placeholder, int min_values, int max_values, bool disabled) :
	Component(id, type),
	m_CustomID(custom_id),
	m_Placeholder(placeholder),
	m_MinValues(min_values),
	m_MaxValues(max_values),
	m_Disabled(disabled)
{
}

void SelectMenu::AddOption(std::string label, std::string value, std::string description, EmojiId_t emoji, bool is_default)
{
	m_Options.push_back({ label, value, description, emoji, is_default });
}

json SelectMenu::ToJson() const
{
	json j = {
		{ "type", static_cast<int>(m_Type) },
		{ "custom_id", m_CustomID },
		{ "disabled", m_Disabled }
	};

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
				{ "label", opt.label },
				{ "value", opt.value },
				{ "default", opt.is_default }
			};

			if (!opt.description.empty())
				o["description"] = opt.description;

			if (opt.emoji != INVALID_EMOJI_ID)
			{
				auto const& emoji = EmojiManager::Get()->FindEmoji(opt.emoji);
				if (emoji)
				{
					if (emoji->GetSnowflake().empty())
						o["emoji"] = { { "name", emoji->GetName() } };
					else
						o["emoji"] = { { "id", emoji->GetSnowflake() } };
				}
			}
			options.push_back(o);
		}
		j["options"] = options;
	}

	return j;
}

void ActionRow::AddComponent(ComponentId_t component)
{
	m_Components.push_back(component);
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

ComponentId_t ComponentManager::CreateButton(std::string text, ButtonStyle style, std::string custom_id, std::string url, bool disabled, EmojiId_t emoji)
{
	ComponentId_t id = m_NextComponentID++;
	m_Components.emplace(id, Component_t(new Button(id, text, style, custom_id, url, disabled, emoji)));
	return id;
}

ComponentId_t ComponentManager::CreateSelectMenu(ComponentType type, std::string custom_id, std::string placeholder, int min_values, int max_values, bool disabled)
{
	ComponentId_t id = m_NextComponentID++;
	m_Components.emplace(id, Component_t(new SelectMenu(id, type, custom_id, placeholder, min_values, max_values, disabled)));
	return id;
}

bool ComponentManager::AddSelectMenuOption(ComponentId_t menu, std::string label, std::string value, std::string description, EmojiId_t emoji, bool is_default)
{
	auto const& comp = FindComponent(menu);
	if (!comp || comp->GetType() != ComponentType::STRING_SELECT)
		return false;

	static_cast<SelectMenu*>(comp.get())->AddOption(label, value, description, emoji, is_default);
	return true;
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
