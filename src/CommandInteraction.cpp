#include "Command.hpp"
#include "Network.hpp"
#include "PawnDispatcher.hpp"
#include "Callback.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include "Bot.hpp"
#include "Guild.hpp"
#include "CommandInteraction.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Embed.hpp"
#include "Http.hpp"
#include "Component.hpp"
#include "Modal.hpp"
#include <json.hpp>
#include <regex>

namespace
{
	std::string GetInteractionOptionValue(const json& value)
	{
		if (value.is_string())
		{
			return value.get<std::string>();
		}

		return value.dump();
	}
}

CommandInteraction::CommandInteraction(CommandInteractionId_t id, UserId_t user, nlohmann::json const& interaction_json) :
	m_InteractionUser(user),
	m_InteractionJson(interaction_json)
{
	m_ID = id;
	m_IDSnowflake = interaction_json.at("id").get<std::string>();
	m_Token = interaction_json.at("token").get<std::string>();
	
	std::string dump;
	utils::TryDumpJson(interaction_json, dump);

	if (interaction_json.at("type").get<int>() == 2 /*application command*/)
	{
		std::string name = interaction_json.at("data").at("name").get<std::string>();
		
		std::string guild_str = "";
		bool has_guild = utils::TryGetJsonValue(interaction_json.at("data"), guild_str, "guild_id");
		if (has_guild )
		{
			
			auto & guild = GuildManager::Get()->FindGuildById(guild_str);
			auto & command = CommandManager::Get()->FindCommand(CommandManager::Get()->FindCommandIdByName(name, guild->GetPawnId()));
			if (!guild || command->GetGuild() != guild->GetPawnId())
			{
				Logger::Get()->Log(samplog_LogLevel::WARNING, "recieved a command interaction for command {} (guild {}) but the callee guild doesn't match", name, guild_str);
				return;
			}

			m_Guild = guild->GetPawnId();
		}
		else
		{
			auto& command = CommandManager::Get()->FindCommand(CommandManager::Get()->FindCommandIdByName(name));
			if (!command)
			{
				Logger::Get()->Log(samplog_LogLevel::WARNING, "recieved a command interaction for command {} but no command exists", name);
				return;
			}
		}
		ParseOptions(interaction_json, m_Guild != INVALID_GUILD_ID ? guild_str : "");
	}
	else
	{
		std::string guild_str = "";
		if (utils::TryGetJsonValue(interaction_json, guild_str, "guild_id"))
		{
			auto& guild = GuildManager::Get()->FindGuildById(guild_str);
			if (guild)
				m_Guild = guild->GetPawnId();
		}
	}

	std::string channel_str = interaction_json.at("channel_id").get<std::string>();
	Channel_t const & channel = ChannelManager::Get()->FindChannelById(channel_str);
	if (!channel && m_Guild == INVALID_GUILD_ID)
	{
		ChannelId_t cid = ChannelManager::Get()->AddDMChannel(interaction_json);
		m_Channel = ChannelManager::Get()->FindChannel(cid)->GetPawnId();
	}
	else
	{
		m_Channel = channel ? channel->GetPawnId() : INVALID_CHANNEL_ID;
	}
	m_InteractionUser = user;
}

void CommandInteraction::ParseOptions(nlohmann::json const& interaction_json, std::string const & guildid)
{
	auto data_it = interaction_json.find("data");
	if (data_it == interaction_json.end())
	{
		return;
	}

	auto options_it = data_it->find("options");
	if (options_it == data_it->end() || !options_it->is_array())
	{
		return;
	}

	for (const auto& option_json : *options_it)
	{
		ParseOption(option_json, guildid);
	}
}

void CommandInteraction::ParseOption(nlohmann::json const& option_json, std::string const& guildid, std::string const& parent_path)
{
	CommandInteractionOption tmpoption;
	tmpoption.m_Type = static_cast<COMMAND_OPTION_TYPE>(option_json.at("type").get<int>());
	tmpoption.m_Name = option_json.at("name").get<std::string>();
	tmpoption.m_FullName = parent_path.empty() ? tmpoption.m_Name : fmt::format("{}.{:s}", parent_path, tmpoption.m_Name);

	auto value_it = option_json.find("value");
	if (value_it != option_json.end())
	{
		tmpoption.m_Value = GetInteractionOptionValue(*value_it);
	}

	if (tmpoption.m_Type == COMMAND_OPTION_TYPE::OPTION_STRING && !tmpoption.m_Value.empty() && !guildid.empty())
	{
		std::regex mentions("<@!\\s*(\\d+)\\s*>");
		auto mentions_begin = std::sregex_iterator(tmpoption.m_Value.begin(), tmpoption.m_Value.end(), mentions);
		auto mentions_end = std::sregex_iterator();

		for (std::sregex_iterator i = mentions_begin; i != mentions_end; ++i)
		{
			std::string match = (*i)[1].str();
			const User_t & user = UserManager::Get()->FindUserById(match);
			if (user && user->IsValid())
			{
				m_Mentions.push_back(user->GetPawnId());
			}
		}
	}

	CommandInteractionOption_t option = std::unique_ptr<CommandInteractionOption>(new CommandInteractionOption(std::move(tmpoption)));
	AddInteractionOption(option);

	auto child_options_it = option_json.find("options");
	if (child_options_it != option_json.end() && child_options_it->is_array())
	{
		for (const auto& child_option : *child_options_it)
		{
			ParseOption(child_option, guildid, m_InteractionOptions.back()->m_FullName);
		}
	}
}

void CommandInteraction::AddInteractionOption(CommandInteractionOption_t & option)
{
	m_InteractionOptions.push_back(std::move(option));
}

CommandInteractionOption const * CommandInteraction::GetOption(std::size_t offset) const
{
	if (offset >= m_InteractionOptions.size())
	{
		return nullptr;
	}

	return m_InteractionOptions.at(offset).get();
}

std::string CommandInteraction::GetContent() const
{
	for (const auto& option : m_InteractionOptions)
	{
		if (option && !option->m_Value.empty())
		{
			return option->m_Value;
		}
	}

	for (const auto& option : m_InteractionOptions)
	{
		if (option)
		{
			return option->m_FullName;
		}
	}

	return "";
}

void CommandInteraction::SendEmbed(EmbedId_t embedid, const std::string message, std::vector<ActionRowId_t> const &rows)
{
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	json data = {
		{ "content", message },
		{ "embeds", {
			json::object()
		}}
	};

	// Add fields (if any).

	data["embeds"][0] = json::object({
			{ "title", embed->GetTitle() },
			{ "description", embed->GetDescription() },
			{ "url", embed->GetUrl() },
			{ "timestamp", embed->GetTimestamp() },
			{ "color", embed->GetColor() },
			{ "footer", {
				{"text", embed->GetFooterText()},
				{"icon_url", embed->GetFooterIconUrl()},
			} },
			{ "thumbnail", json::object() },
			{ "image", json::object() }
		});
	
	if (!embed->GetThumbnailUrl().empty())
	{
		data["embeds"][0]["thumbnail"]["url"] = embed->GetThumbnailUrl();
	}

	if (!embed->GetImageUrl().empty())
	{
		data["embeds"][0]["image"]["url"] = embed->GetImageUrl();
	}

	if (embed->GetFields().size())
	{
		json field_array = json::array();
		for (const auto& i : embed->GetFields())
		{
			field_array.push_back({
				{"name", i._name},
				{"value", i._value},
				{"inline", i._inline_}
			});
		}
		data["embeds"][0]["fields"] = field_array;
	}

	if (!rows.empty())
	{
		json components = json::array();
		for (auto row_id : rows)
		{
			auto const& row = ComponentManager::Get()->FindActionRow(row_id);
			if (row)
				components.push_back(row->ToJson());
		}
		data["components"] = components;
	}

	std::string json_str;
	if (!utils::TryDumpJson(data, json_str))
		Logger::Get()->Log(samplog_LogLevel::ERROR, "can't serialize JSON: {}", json_str);

	/*/webhooks/{application.id}/{interaction.token}/messages/@original*/
	Network::Get()->Http().Patch(fmt::format("/webhooks/{:s}/{:s}/messages/@original", ThisBot::Get()->GetApplicationID(), m_Token), json_str);
}

void CommandInteraction::SendInteractionMessage(const std::string message, std::vector<ActionRowId_t> const &rows)
{
	json data = {
		{ "content", message }
	};

	if (!rows.empty())
	{
		json components = json::array();
		for (auto row_id : rows)
		{
			auto const& row = ComponentManager::Get()->FindActionRow(row_id);
			if (row)
				components.push_back(row->ToJson());
		}
		data["components"] = components;
	}

	std::string json_str;
	if (!utils::TryDumpJson(data, json_str))
		Logger::Get()->Log(samplog_LogLevel::ERROR, "can't serialize JSON: {}", json_str);

	/*/webhooks/{application.id}/{interaction.token}/messages/@original*/
	Network::Get()->Http().Patch(fmt::format("/webhooks/{:s}/{:s}/messages/@original", ThisBot::Get()->GetApplicationID(), m_Token), json_str);
}

void CommandInteraction::SendModal(ModalId_t modalid)
{
	auto const& modal = ModalManager::Get()->FindModal(modalid);
	if (!modal)
		return;

	json data = {
		{ "type", 9 }, // MODAL_SUBMIT (Wait, type 9 is response type for modals)
		{ "data", modal->ToJson() }
	};

	std::string json_str;
	if (!utils::TryDumpJson(data, json_str))
		Logger::Get()->Log(samplog_LogLevel::ERROR, "can't serialize JSON: {}", json_str);

	// Modals must be a response to the interaction, not a patch to the original message.
	// But CommandInteraction current implementation seems to only use PATCH to @original.
	// Interaction callback for modals is POST /interactions/{id}/{token}/callback
	Network::Get()->Http().Post(fmt::format("/interactions/{:s}/{:s}/callback", m_IDSnowflake, m_Token), json_str);
}

CommandInteraction_t const & CommandInteractionManager::FindCommandInteraction(CommandInteractionId_t interaction)
{
	static CommandInteraction_t invalid_command_interaction;
	auto it = m_Interactions.find(interaction);
	if (it == m_Interactions.end())
		return invalid_command_interaction;
	return it->second;
}

CommandInteractionId_t CommandInteractionManager::AddCommandInteraction(UserId_t user, nlohmann::json const& interaction_json)
{
	CommandInteractionId_t id = 1;
	while (m_Interactions.find(id) != m_Interactions.end())
		++id;

	if (!m_Interactions.emplace(id, CommandInteraction_t(new CommandInteraction(id, user, interaction_json))).first->second)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"can't create command interaction: duplicate key '{}'", id);
		return INVALID_USER_ID;
	}

	Logger::Get()->Log(samplog_LogLevel::INFO, "successfully created command interaction with id '{}'", id);
	return id;
}

bool CommandInteractionManager::DeleteCommandInteraction(CommandInteractionId_t interaction)
{
	if (m_Interactions.find(interaction) == m_Interactions.end())
	{
		Logger::Get()->Log(samplog_LogLevel::WARNING, "attempted to delete command interaction with id '{}' but it does not exist", interaction);
		return false;
	}

	m_Interactions.erase(interaction);
	Logger::Get()->Log(samplog_LogLevel::INFO, "successfully deleted command interaction with id '{}'", interaction);
	return true;
}