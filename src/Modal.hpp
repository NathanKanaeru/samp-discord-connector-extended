#pragma once

#include "types.hpp"
#include "Singleton.hpp"
#include "Component.hpp"
#include <string>
#include <vector>
#include <map>
#include <json.hpp>

// A modal is a top-level interaction response that contains rows of components.
// Each row may either be:
//   * an Action Row wrapping a Text Input  (Discord modal V1, the historic format)
//   * a Label component wrapping a Select Menu / File Upload  (modal V2, opt-in)
// Rows are stored as { component_id, label, description }. When `label` is empty
// the component is wrapped in a plain Action Row; otherwise it is wrapped in a
// Label component which carries the visible label and optional description text.
struct ModalRow
{
	ComponentId_t component;
	std::string   label;
	std::string   description;
};

class Modal
{
public:
	Modal(ModalId_t id, std::string title, std::string custom_id) :
		m_ID(id), m_Title(std::move(title)), m_CustomID(std::move(custom_id))
	{}

	// Adds an existing component as a new modal row. Returns false if the row
	// limit (5) is reached or if the component cannot live inside a modal.
	bool AddComponent(ComponentId_t component, std::string label, std::string description);

	// Validates the modal payload before it is sent.
	bool Validate() const;

	nlohmann::json ToJson() const;

	ModalId_t GetID() const { return m_ID; }
	std::size_t GetRowCount() const { return m_Rows.size(); }

private:
	ModalId_t   m_ID;
	std::string m_Title;
	std::string m_CustomID;
	std::vector<ModalRow> m_Rows;
};


class ModalManager : public Singleton<ModalManager>
{
	friend class Singleton<ModalManager>;
public:
	ModalId_t CreateModal(std::string title, std::string custom_id);

	// Convenience wrapper kept for backward compatibility with the
	// pre-refactor `DCC_AddModalInput` native: builds a TextInput component
	// and adds it to the modal as a plain text row.
	bool AddModalInput(ModalId_t modal, std::string custom_id, TextInputStyle style,
		std::string label, int min_length, int max_length, bool required,
		std::string value, std::string placeholder);

	// Adds any pre-built component (text input, select, file upload) to the modal.
	// `label` and `description` are only used for non-text-input components - they
	// are surfaced as the visible Label wrapper in V2 modals.
	bool AddModalComponent(ModalId_t modal, ComponentId_t component,
		std::string label, std::string description);

	Modal_t const& FindModal(ModalId_t id);

	void Initialize() {}

private:
	std::map<ModalId_t, Modal_t> m_Modals;
	ModalId_t m_NextModalID = 1;
};
