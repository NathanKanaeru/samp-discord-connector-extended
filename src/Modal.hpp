#pragma once

#include "types.hpp"
#include "Singleton.hpp"
#include <string>
#include <vector>
#include <map>
#include <json.hpp>

enum class TextInputStyle
{
	SHORT = 1,
	PARAGRAPH = 2
};

struct TextInput
{
	std::string custom_id;
	TextInputStyle style;
	std::string label;
	int min_length;
	int max_length;
	bool required;
	std::string value;
	std::string placeholder;
};

class Modal
{
public:
	Modal(ModalId_t id, std::string title, std::string custom_id) : m_ID(id), m_Title(title), m_CustomID(custom_id) {}
	void AddInput(std::string custom_id, TextInputStyle style, std::string label, int min_length, int max_length, bool required, std::string value, std::string placeholder);
	nlohmann::json ToJson() const;

	ModalId_t GetID() const { return m_ID; }

private:
	ModalId_t m_ID;
	std::string m_Title;
	std::string m_CustomID;
	std::vector<TextInput> m_Inputs;
};

class ModalManager : public Singleton<ModalManager>
{
	friend class Singleton<ModalManager>;
public:
	ModalId_t CreateModal(std::string title, std::string custom_id);
	bool AddModalInput(ModalId_t modal, std::string custom_id, TextInputStyle style, std::string label, int min_length, int max_length, bool required, std::string value, std::string placeholder);

	Modal_t const& FindModal(ModalId_t id);

	void Initialize() {}

private:
	std::map<ModalId_t, Modal_t> m_Modals;
	ModalId_t m_NextModalID = 1;
};
