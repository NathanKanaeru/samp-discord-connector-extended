#include "Modal.hpp"
#include "Logger.hpp"

using json = nlohmann::json;

namespace
{
	constexpr std::size_t kMaxModalRows = 5;
}

bool Modal::AddComponent(ComponentId_t component, std::string label, std::string description)
{
	if (m_Rows.size() >= kMaxModalRows)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: cannot add more than {} components", m_ID, kMaxModalRows);
		return false;
	}

	auto const& comp = ComponentManager::Get()->FindComponent(component);
	if (!comp)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: component id {} does not exist", m_ID, component);
		return false;
	}

	switch (comp->GetType())
	{
	case ComponentType::TEXT_INPUT:
	case ComponentType::STRING_SELECT:
	case ComponentType::USER_SELECT:
	case ComponentType::ROLE_SELECT:
	case ComponentType::MENTIONABLE_SELECT:
	case ComponentType::CHANNEL_SELECT:
	case ComponentType::FILE_UPLOAD:
		break;
	default:
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: component {} (type {}) cannot be placed inside a modal",
			m_ID, component, static_cast<int>(comp->GetType()));
		return false;
	}

	m_Rows.push_back({ component, std::move(label), std::move(description) });
	return true;
}

bool Modal::Validate() const
{
	if (m_Title.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: title must not be empty", m_ID);
		return false;
	}
	if (m_Title.length() > 45)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: title is longer than the 45 char limit", m_ID);
		return false;
	}
	if (m_CustomID.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: custom_id must not be empty", m_ID);
		return false;
	}
	if (m_Rows.empty())
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: must contain at least one component", m_ID);
		return false;
	}
	if (m_Rows.size() > kMaxModalRows)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: at most {} components allowed, got {}",
			m_ID, kMaxModalRows, m_Rows.size());
		return false;
	}

	for (auto const& row : m_Rows)
	{
		auto const& comp = ComponentManager::Get()->FindComponent(row.component);
		if (!comp || !comp->Validate())
			return false;
	}

	return true;
}

json Modal::ToJson() const
{
	json components = json::array();
	for (auto const& row : m_Rows)
	{
		auto const& comp = ComponentManager::Get()->FindComponent(row.component);
		if (!comp)
			continue;

		ComponentType type = comp->GetType();

		// Pure text-input rows go through the historic Action Row path, which
		// is what every Discord client supports today.
		if (type == ComponentType::TEXT_INPUT && row.label.empty())
		{
			components.push_back({
				{ "type", 1 }, // Action Row
				{ "components", { comp->ToJson() } }
			});
			continue;
		}

		// Anything else (or a TextInput with an explicit wrapper label) is
		// wrapped in a Label component (Components V2). Discord routes the
		// resulting submission through `data.components[].component` instead
		// of `data.components[].components[]`; both cases are handled when
		// reading values back.
		json label_component = {
			{ "type", static_cast<int>(ComponentType::LABEL) },
			{ "label", row.label.empty() ? std::string("Field") : row.label },
			{ "component", comp->ToJson() }
		};

		if (!row.description.empty())
			label_component["description"] = row.description;

		components.push_back(label_component);
	}

	return {
		{ "title",      m_Title },
		{ "custom_id",  m_CustomID },
		{ "components", components }
	};
}


// ---------------------- Manager ----------------------

ModalId_t ModalManager::CreateModal(std::string title, std::string custom_id)
{
	ModalId_t id = m_NextModalID++;
	m_Modals.emplace(id, Modal_t(new Modal(id, std::move(title), std::move(custom_id))));
	return id;
}

bool ModalManager::AddModalInput(ModalId_t modal, std::string custom_id, TextInputStyle style,
	std::string label, int min_length, int max_length, bool required,
	std::string value, std::string placeholder)
{
	auto const& m = FindModal(modal);
	if (!m)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: invalid modal id", modal);
		return false;
	}

	ComponentId_t comp_id = ComponentManager::Get()->CreateTextInput(
		std::move(custom_id), style, std::move(label),
		min_length, max_length, required,
		std::move(value), std::move(placeholder));

	// Empty label / description -> goes through the V1 action-row path,
	// preserving exact backward-compatible behaviour for old Pawn scripts.
	return m->AddComponent(comp_id, "", "");
}

bool ModalManager::AddModalComponent(ModalId_t modal, ComponentId_t component,
	std::string label, std::string description)
{
	auto const& m = FindModal(modal);
	if (!m)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"modal {}: invalid modal id", modal);
		return false;
	}
	return m->AddComponent(component, std::move(label), std::move(description));
}

Modal_t const& ModalManager::FindModal(ModalId_t id)
{
	static Modal_t invalid;
	auto it = m_Modals.find(id);
	if (it == m_Modals.end())
		return invalid;
	return it->second;
}
