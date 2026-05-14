#include "Modal.hpp"
#include "Logger.hpp"

using json = nlohmann::json;

void Modal::AddInput(std::string custom_id, TextInputStyle style, std::string label, int min_length, int max_length, bool required, std::string value, std::string placeholder)
{
	m_Inputs.push_back({ custom_id, style, label, min_length, max_length, required, value, placeholder });
}

json Modal::ToJson() const
{
	json components = json::array();
	for (auto const& input : m_Inputs)
	{
		json text_input = {
			{ "type", 4 }, // Text Input
			{ "custom_id", input.custom_id },
			{ "style", static_cast<int>(input.style) },
			{ "label", input.label },
			{ "required", input.required }
		};

		if (input.min_length >= 0)
			text_input["min_length"] = input.min_length;

		if (input.max_length >= 0)
			text_input["max_length"] = input.max_length;

		if (!input.value.empty())
			text_input["value"] = input.value;

		if (!input.placeholder.empty())
			text_input["placeholder"] = input.placeholder;

		// Each text input must be in its own Action Row in a modal
		components.push_back({
			{ "type", 1 }, // Action Row
			{ "components", { text_input } }
		});
	}

	return {
		{ "title", m_Title },
		{ "custom_id", m_CustomID },
		{ "components", components }
	};
}

ModalId_t ModalManager::CreateModal(std::string title, std::string custom_id)
{
	ModalId_t id = m_NextModalID++;
	m_Modals.emplace(id, Modal_t(new Modal(id, title, custom_id)));
	return id;
}

bool ModalManager::AddModalInput(ModalId_t modal, std::string custom_id, TextInputStyle style, std::string label, int min_length, int max_length, bool required, std::string value, std::string placeholder)
{
	auto const& m = FindModal(modal);
	if (!m)
		return false;

	m->AddInput(custom_id, style, label, min_length, max_length, required, value, placeholder);
	return true;
}

Modal_t const& ModalManager::FindModal(ModalId_t id)
{
	static Modal_t invalid;
	auto it = m_Modals.find(id);
	if (it == m_Modals.end())
		return invalid;
	return it->second;
}
