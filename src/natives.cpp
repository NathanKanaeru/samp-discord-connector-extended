#include "natives.hpp"
#include "Network.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "User.hpp"
#include "Role.hpp"
#include "Guild.hpp"
#include "Bot.hpp"
#include "misc.hpp"
#include "types.hpp"
#include "Logger.hpp"
#include "Callback.hpp"
#include "Embed.hpp"
#include "Emoji.hpp"
#include "Command.hpp"
#include "CommandInteraction.hpp"
#include "Component.hpp"
#include "Modal.hpp"
#include <fmt/printf.h>
#include <unordered_map>
#include <functional>

#ifdef ERROR
#undef ERROR
#endif

namespace
{
	std::unordered_map<cell, CommandOption_t> g_PendingCommandOptions;
	std::unordered_map<cell, CommandOption*> g_PendingCommandOptionIndex;
	cell g_NextCommandOptionId = 1;

	void ClearPendingCommandOptions()
	{
		g_PendingCommandOptions.clear();
		g_PendingCommandOptionIndex.clear();
		g_NextCommandOptionId = 1;
	}
}
/*
// native native_name(...);
AMX_DECLARE_NATIVE(Native::native_name)
{
	ScopedDebugInfo dbg_info(amx, "native_name", params, "sd");



	cell ret_val = ;
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}
*/

// native DCC_Channel:DCC_FindChannelByName(const channel_name[]);
AMX_DECLARE_NATIVE(Native::DCC_FindChannelByName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindChannelByName", params, "s");

	std::string const channel_name = amx_GetCppString(amx, params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannelByName(channel_name);

	cell ret_val = channel ? channel->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Channel:DCC_FindChannelById(const channel_id[]);
AMX_DECLARE_NATIVE(Native::DCC_FindChannelById)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindChannelById", params, "s");

	Snowflake_t const channel_id = amx_GetCppString(amx, params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannelById(channel_id);

	cell ret_val = channel ? channel->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetChannelId(DCC_Channel:channel, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelId", params, "drd");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], channel->GetId(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetChannelType(DCC_Channel:channel, &DCC_ChannelType:type);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelType)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelType", params, "dr");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(channel->GetType());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetChannelGuild(DCC_Channel:channel, &DCC_Guild:guild);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelGuild)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelGuild", params, "dr");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(channel->GetGuildId());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetChannelName(DCC_Channel:channel, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelName", params, "drd");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], channel->GetName(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetChannelTopic(DCC_Channel:channel, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelTopic)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelTopic", params, "drd");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], channel->GetTopic(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetChannelPosition(DCC_Channel:channel, &position);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelPosition)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelPosition", params, "dr");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(channel->GetPosition());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_IsChannelNsfw(DCC_Channel:channel, &bool:is_nsfw);
AMX_DECLARE_NATIVE(Native::DCC_IsChannelNsfw)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsChannelNsfw", params, "dr");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	if (channel->GetType() != Channel::Type::GUILD_TEXT)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid channel type; must be guild text channel");
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(channel->IsNsfw() ? 1 : 0);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetChannelParentCategory(DCC_Channel:channel, &DCC_Channel:category);
AMX_DECLARE_NATIVE(Native::DCC_GetChannelParentCategory)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetChannelParentCategory", params, "dr");

	ChannelId_t channelid = params[1];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = channel->GetParentId();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SendChannelMessage(DCC_Channel:channel, const message[],
//     const callback[] = "", const format[] = "", {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_SendChannelMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SendChannelMessage", params, "ds");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	auto message = amx_GetCppString(amx, params[2]);
	if (message.length() > 2000)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"message must be shorter than 2000 characters");
		return 0;
	}

	auto
		cb_name = amx_GetCppString(amx, params[3]),
		cb_format = amx_GetCppString(amx, params[4]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(
		amx, cb_name.c_str(), cb_format.c_str(), params, 5, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}


	channel->SendMessage(std::move(message), std::move(cb));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetChannelName(DCC_Channel:channel, const name[]);
AMX_DECLARE_NATIVE(Native::DCC_SetChannelName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetChannelName", params, "ds");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	auto name = amx_GetCppString(amx, params[2]);
	if (name.length() < 2 || name.length() > 100)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"name must be between 2 and 100 characters in length");
		return 0;
	}

	channel->SetChannelName(name);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetChannelTopic(DCC_Channel:channel, const topic[]);
AMX_DECLARE_NATIVE(Native::DCC_SetChannelTopic)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetChannelTopic", params, "ds");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	if (channel->GetType() != Channel::Type::GUILD_TEXT)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid channel type; must be guild text channel");
		return 0;
	}

	auto topic = amx_GetCppString(amx, params[2]);
	if (topic.length() > 1024)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"topic must be between 0 and 1024 characters in length");
		return 0;
	}

	channel->SetChannelTopic(topic);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetChannelPosition(DCC_Channel:channel, position);
AMX_DECLARE_NATIVE(Native::DCC_SetChannelPosition)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetChannelPosition", params, "dd");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	channel->SetChannelPosition(static_cast<int>(params[2]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetChannelNsfw(DCC_Channel:channel, bool:is_nsfw);
AMX_DECLARE_NATIVE(Native::DCC_SetChannelNsfw)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetChannelNsfw", params, "dd");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	channel->SetChannelNsfw(params[2] != 0);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetChannelParentCategory(DCC_Channel:channel, DCC_Channel:parent_category);
AMX_DECLARE_NATIVE(Native::DCC_SetChannelParentCategory)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetChannelParentCategory", params, "dd");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	ChannelId_t parent_channelid = static_cast<ChannelId_t>(params[2]);
	Channel_t const &parent_channel = ChannelManager::Get()->FindChannel(parent_channelid);
	if (!parent_channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid parent channel id '{}'",
			parent_channelid);
		return 0;
	}

	if (parent_channel->GetType() != Channel::Type::GUILD_CATEGORY)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid channel type; must be guild category channel");
		return 0;
	}

	channel->SetChannelParentCategory(parent_channel);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_DeleteChannel(DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_DeleteChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteChannel", params, "d");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	channel->DeleteChannel();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageId(DCC_Message:message, dest[DCC_ID_SIZE], max_size = DCC_ID_SIZE);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageId", params, "drd");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], msg->GetId(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetMessageChannel(DCC_Message:message, &DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageChannel", params, "dr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(msg->GetChannel());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageAuthor(DCC_Message:message, &DCC_User:author);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageAuthor)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageAuthor", params, "dr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(msg->GetAuthor());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageContent(DCC_Message:message, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageContent)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageContent", params, "drd");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell ret_val = amx_SetCppString(
		amx, params[2], msg->GetContent(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_IsMessageTts(DCC_Message:message, &bool:is_tts);
AMX_DECLARE_NATIVE(Native::DCC_IsMessageTts)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsMessageTts", params, "dr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(msg->IsTts() ? 1 : 0);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_IsMessageMentioningEveryone(DCC_Message:message, &bool:mentions_everyone);
AMX_DECLARE_NATIVE(Native::DCC_IsMessageMentioningEveryone)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsMessageMentioningEveryone", params, "dr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(msg->MentionsEveryone() ? 1 : 0);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageUserMentionCount(DCC_Message:message, &mentioned_user_count);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageUserMentionCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageUserMentionCount", params, "dr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(msg->GetUserMentions().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageUserMention(DCC_Message:message, offset, &DCC_User:mentioned_user);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageUserMention)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageUserMention", params, "ddr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	auto const offset = static_cast<unsigned int>(params[2]);
	auto const &mentions = msg->GetUserMentions();
	if (offset >= mentions.size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, mentions.size());
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(mentions.at(offset));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageRoleMentionCount(DCC_Message:message, &mentioned_role_count);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageRoleMentionCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageRoleMentionCount", params, "dr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(msg->GetRoleMentions().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetMessageRoleMention(DCC_Message:message, offset, &DCC_Role:mentioned_role);
AMX_DECLARE_NATIVE(Native::DCC_GetMessageRoleMention)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageRoleMention", params, "ddr");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	auto const offset = static_cast<unsigned int>(params[2]);
	auto const &mentions = msg->GetRoleMentions();
	if (offset >= mentions.size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, mentions.size());
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(mentions.at(offset));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_DeleteMessage(DCC_Message:message);
AMX_DECLARE_NATIVE(Native::DCC_DeleteMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteMessage", params, "d");

	MessageId_t id = params[1];
	Message_t const &msg = MessageManager::Get()->Find(id);
	if (!msg)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", id);
		return 0;
	}

	msg->DeleteMessage();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Message:DCC_GetCreatedMessage();
AMX_DECLARE_NATIVE(Native::DCC_GetCreatedMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetCreatedMessage", params);

	auto ret_val = static_cast<cell>(MessageManager::Get()->GetCreatedMessageId());
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{:d}'", ret_val);
	return ret_val;
}

// native DCC_User:DCC_FindUserByName(const user_name[], const user_discriminator[]);
AMX_DECLARE_NATIVE(Native::DCC_FindUserByName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindUserByName", params, "ss");

	std::string const
		user_name = amx_GetCppString(amx, params[1]),
		discriminator = amx_GetCppString(amx, params[2]);
	User_t const &user = UserManager::Get()->FindUserByName(user_name, discriminator);

	cell ret_val = user ? user->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_User:DCC_FindUserById(const user_id[]);
AMX_DECLARE_NATIVE(Native::DCC_FindUserById)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindUserById", params, "s");

	Snowflake_t const user_id = amx_GetCppString(amx, params[1]);
	User_t const &user = UserManager::Get()->FindUserById(user_id);

	cell ret_val = user ? user->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetUserId(DCC_User:user, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetUserId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetUserId", params, "drd");

	UserId_t userid = params[1];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], user->GetId(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetUserName(DCC_User:user, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetUserName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetUserName", params, "drd");

	UserId_t userid = params[1];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], user->GetUsername(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetUserDiscriminator(DCC_User:user, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetUserDiscriminator)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetUserDiscriminator", params, "drd");

	UserId_t userid = params[1];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], user->GetDiscriminator(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_IsUserBot(DCC_User:user, &bool:is_bot);
AMX_DECLARE_NATIVE(Native::DCC_IsUserBot)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsUserBot", params, "dr");

	UserId_t userid = params[1];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = user->IsBot() ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_IsUserVerified(DCC_User:user, &bool:is_verified);
AMX_DECLARE_NATIVE(Native::DCC_IsUserVerified)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsUserVerified", params, "dr");

	UserId_t userid = params[1];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = user->IsVerified() ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Role:DCC_FindRoleByName(DCC_Guild:guild, const role_name[]);
AMX_DECLARE_NATIVE(Native::DCC_FindRoleByName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindRoleByName", params, "ds");

	GuildId_t guildid = params[1];
	std::string const role_name = amx_GetCppString(amx, params[2]);

	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return INVALID_ROLE_ID;
	}

	cell ret_val = INVALID_ROLE_ID;
	for (auto const &r : guild->GetRoles())
	{
		Role_t const &role = RoleManager::Get()->FindRole(r);
		if (!role)
			continue;

		if (role->GetName() == role_name)
		{
			ret_val = role->GetPawnId();
			break;
		}
	}

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Role:DCC_FindRoleById(const role_id[]);
AMX_DECLARE_NATIVE(Native::DCC_FindRoleById)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindRoleById", params, "s");

	Snowflake_t const role_id = amx_GetCppString(amx, params[1]);
	Role_t const &role = RoleManager::Get()->FindRoleById(role_id);

	cell ret_val = role ? role->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetRoleId(DCC_Role:role, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetRoleId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetRoleId", params, "drd");

	RoleId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], role->GetId(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetRoleName(DCC_Role:role, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetRoleName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetRoleName", params, "drd");

	ChannelId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], role->GetName(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetRoleColor(DCC_Role:role, &color);
AMX_DECLARE_NATIVE(Native::DCC_GetRoleColor)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetRoleColor", params, "dr");

	RoleId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(role->GetColor());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetRolePermissions(DCC_Role:role, &perm_high, &perm_low);
AMX_DECLARE_NATIVE(Native::DCC_GetRolePermissions)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetRolePermissions", params, "drr");

	RoleId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference for 'perm_high'");
		return 0;
	}

	*dest = static_cast<cell>((role->GetPermissions() >> 32) & 0xFFFFFFFF);

	dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference for 'perm_low'");
		return 0;
	}

	*dest = static_cast<cell>(role->GetPermissions() & 0xFFFFFFFF);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_IsRoleHoist(DCC_Role:role, &bool:is_hoist);
AMX_DECLARE_NATIVE(Native::DCC_IsRoleHoist)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsRoleHoist", params, "dr");

	RoleId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = role->IsHoist() ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetRolePosition(DCC_Role:role, &position);
AMX_DECLARE_NATIVE(Native::DCC_GetRolePosition)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetRolePosition", params, "dr");

	RoleId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(role->GetPosition());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_IsRoleMentionable(DCC_Role:role, &bool:is_mentionable);
AMX_DECLARE_NATIVE(Native::DCC_IsRoleMentionable)
{
	ScopedDebugInfo dbg_info(amx, "DCC_IsRoleMentionable", params, "dr");

	RoleId_t roleid = params[1];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = role->IsMentionable() ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Guild:DCC_FindGuildByName(const guild_name[]);
AMX_DECLARE_NATIVE(Native::DCC_FindGuildByName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindGuildByName", params, "s");

	std::string const guild_name = amx_GetCppString(amx, params[1]);
	Guild_t const &guild = GuildManager::Get()->FindGuildByName(guild_name);

	cell ret_val = guild ? guild->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Guild:DCC_FindGuildById(const guild_id[]);
AMX_DECLARE_NATIVE(Native::DCC_FindGuildById)
{
	ScopedDebugInfo dbg_info(amx, "DCC_FindGuildById", params, "s");

	Snowflake_t const guild_id = amx_GetCppString(amx, params[1]);
	Guild_t const &guild = GuildManager::Get()->FindGuildById(guild_id);

	cell ret_val = guild ? guild->GetPawnId() : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetGuildId(DCC_Guild:guild, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildId", params, "drd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], guild->GetId(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetGuildName(DCC_Guild:guild, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildName", params, "drd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], guild->GetName(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetGuildOwnerId(DCC_Guild:guild, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildOwnerId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildOwnerId", params, "drd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[2], guild->GetOwnerId(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetGuildRole(DCC_Guild:guild, offset, &DCC_Role:role);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildRole", params, "ddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto offset = static_cast<unsigned int>(params[2]);
	auto const &roles = guild->GetRoles();
	if (offset >= roles.size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, roles.size());
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(roles.at(offset));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildRoleCount(DCC_Guild:guild, &count);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildRoleCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildRoleCount", params, "dr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(guild->GetRoles().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildMember(DCC_Guild:guild, offset, &DCC_User:user);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMember)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMember", params, "ddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto offset = static_cast<unsigned int>(params[2]);
	auto const &members = guild->GetMembers();
	if (offset >= members.size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, members.size());
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(members.at(offset).UserId);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildMemberCount(DCC_Guild:guild, &count);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMemberCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMemberCount", params, "dr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(guild->GetMembers().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildMemberVoiceChannel(DCC_Guild:guild, DCC_User:user, &DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMemberVoiceChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMemberVoiceChannel", params, "ddr");

	GuildId_t guild_id = params[1];
	auto const &guild = GuildManager::Get()->FindGuild(guild_id);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guild_id);
		return 0;
	}

	UserId_t user_id = params[2];

	for (auto &m : guild->GetMembers())
	{
		if (m.UserId != user_id)
			continue;

		cell *dest = nullptr;
		if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
		{
			Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
			return 0;
		}

		*dest = static_cast<cell>(m.GetVoiceChannel());
		Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
		return 1;
	}

	Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", user_id);
	return 0;
}

// native DCC_GetGuildMemberNickname(DCC_Guild:guild, DCC_User:user, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMemberNickname)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMemberNickname", params, "ddrd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	std::string nick;
	bool member_found = false;
	for (auto &m : guild->GetMembers())
	{
		if (m.UserId != userid)
			continue;

		nick = m.Nickname;
		member_found = true;
		break;
	}

	if (!member_found)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user specified");
		return 0;
	}

	cell ret_val = amx_SetCppString(amx, params[3], nick, params[4]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetGuildMemberRole(DCC_Guild:guild, DCC_User:user, offset, &DCC_Role:role);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMemberRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMemberRole", params, "dddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	std::vector<RoleId_t> const *roles = nullptr;
	for (auto &m : guild->GetMembers())
	{
		if (m.UserId != userid)
			continue;

		roles = &m.Roles;
		break;
	}

	if (roles == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user specified");
		return 0;
	}

	auto offset = static_cast<unsigned int>(params[3]);
	if (offset >= roles->size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, roles->size());
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[4], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(roles->at(offset));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildMemberRoleCount(DCC_Guild:guild, DCC_User:user, &count);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMemberRoleCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMemberRoleCount", params, "ddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	std::vector<RoleId_t> const *roles = nullptr;
	for (auto &m : guild->GetMembers())
	{
		if (m.UserId != userid)
			continue;

		roles = &m.Roles;
		break;
	}

	if (roles == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user specified");
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(roles->size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_HasGuildMemberRole(DCC_Guild:guild, DCC_User:user, DCC_Role:role, &bool:has_role);
AMX_DECLARE_NATIVE(Native::DCC_HasGuildMemberRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_HasGuildMemberRole", params, "dddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	std::vector<RoleId_t> const *roles = nullptr;
	for (auto &m : guild->GetMembers())
	{
		if (m.UserId != userid)
			continue;

		roles = &m.Roles;
		break;
	}

	if (roles == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user specified");
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[4], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	RoleId_t roleid = params[3];
	bool res = false;
	for (auto &r : *roles)
	{
		if (r == roleid)
		{
			res = true;
			break;
		}
	}

	*dest = res ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildMemberStatus(DCC_Guild:guild, DCC_User:user, &DCC_UserPresenceStatus:status);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildMemberStatus)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildMemberStatus", params, "ddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	auto status = Guild::Member::PresenceStatus::INVALID;
	for (auto &m : guild->GetMembers())
	{
		if (m.UserId != userid)
			continue;

		status = m.Status;
		break;
	}

	if (status == Guild::Member::PresenceStatus::INVALID)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user specified");
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(status);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildChannel(DCC_Guild:guild, offset, &DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildChannel", params, "ddr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto offset = static_cast<unsigned int>(params[2]);
	auto const &channels = guild->GetChannels();
	if (offset >= channels.size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, channels.size());
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(channels.at(offset));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetGuildChannelCount(DCC_Guild:guild, &count);
AMX_DECLARE_NATIVE(Native::DCC_GetGuildChannelCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetGuildChannelCount", params, "dr");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(guild->GetChannels().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetAllGuilds(DCC_Guild:dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetAllGuilds)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetAllGuilds", params, "rd");

	cell *dest = nullptr;
	if (amx_GetAddr(amx, params[1], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	auto guild_ids = GuildManager::Get()->GetAllGuildIds();

	cell const
		max_dest_size = params[2],
		guilds_count = static_cast<cell>(guild_ids.size());

	if (guilds_count > max_dest_size)
	{
		Logger::Get()->LogNative(samplog_LogLevel::WARNING,
			"destination array is too small (should be at least '{}' cells)",
			guilds_count);
	}

	cell const count = std::min(max_dest_size, guilds_count);
	for (cell i = 0; i != count; ++i)
		dest[i] = guild_ids.at(i);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", count);
	return count;
}

// native DCC_SetGuildName(DCC_Guild:guild, const name[]);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildName", params, "ds");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto name = amx_GetCppString(amx, params[2]);
	if (name.length() < 2 || name.length() > 100)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"name must be between 2 and 100 characters in length");
		return 0;
	}

	guild->SetGuildName(name);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_CreateGuildChannel(DCC_Guild:guild, const name[], DCC_ChannelType:type,
//     const callback[], const format[], {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_CreateGuildChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateGuildChannel", params, "dsdss");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto name = amx_GetCppString(amx, params[2]);
	if (name.length() < 2 || name.length() > 100)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"name must be between 2 and 100 characters in length");
		return 0;
	}

	auto type = static_cast<Channel::Type>(params[3]);
	if (type != Channel::Type::GUILD_CATEGORY
		&& type != Channel::Type::GUILD_TEXT
		&& type != Channel::Type::GUILD_VOICE)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel type");
		return 0;
	}

	auto
		cb_name = amx_GetCppString(amx, params[4]),
		cb_format = amx_GetCppString(amx, params[5]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(
		amx, cb_name.c_str(), cb_format.c_str(), params, 6, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}

	ChannelManager::Get()->CreateGuildChannel(guild, name, type, std::move(cb));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Channel:DCC_GetCreatedGuildChannel();
AMX_DECLARE_NATIVE(Native::DCC_GetCreatedGuildChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetCreatedGuildChannel", params);
	return ChannelManager::Get()->GetCreatedGuildChannelId();
}

// native DCC_SetGuildMemberNickname(DCC_Guild:guild, DCC_User:user, const nickname[]);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildMemberNickname)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildMemberNickname", params, "dds");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	guild->SetMemberNickname(user, amx_GetCppString(amx, params[3]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildMemberVoiceChannel(DCC_Guild:guild, DCC_User:user, DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildMemberVoiceChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildMemberVoiceChannel", params, "ddd");

	GuildId_t guild_id = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guild_id);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guild_id);
		return 0;
	}

	UserId_t user_id = params[2];
	User_t const &user = UserManager::Get()->FindUser(user_id);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", user_id);
		return 0;
	}

	ChannelId_t channel_id = params[3];
	Channel_t const &channel = ChannelManager::Get()->FindChannel(channel_id);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channel_id);
		return 0;
	}
	guild->SetMemberVoiceChannel(user, channel->GetId());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_AddGuildMemberRole(DCC_Guild:guild, DCC_User:user, DCC_Role:role);
AMX_DECLARE_NATIVE(Native::DCC_AddGuildMemberRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddGuildMemberRole", params, "ddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	RoleId_t roleid = params[3];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->AddMemberRole(user, role);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_RemoveGuildMemberRole(DCC_Guild:guild, DCC_User:user, DCC_Role:role);
AMX_DECLARE_NATIVE(Native::DCC_RemoveGuildMemberRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_RemoveGuildMemberRole", params, "ddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	RoleId_t roleid = params[3];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->RemoveMemberRole(user, role);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_RemoveGuildMember(DCC_Guild:guild, DCC_User:user);
AMX_DECLARE_NATIVE(Native::DCC_RemoveGuildMember)
{
	ScopedDebugInfo dbg_info(amx, "DCC_RemoveGuildMember", params, "dd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	guild->KickMember(user);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_CreateGuildMemberBan(DCC_Guild:guild, DCC_User:user, const reason[] = "");
AMX_DECLARE_NATIVE(Native::DCC_CreateGuildMemberBan)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateGuildMemberBan", params, "dds");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	guild->CreateMemberBan(user, amx_GetCppString(amx, params[3]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_RemoveGuildMemberBan(DCC_Guild:guild, DCC_User:user);
AMX_DECLARE_NATIVE(Native::DCC_RemoveGuildMemberBan)
{
	ScopedDebugInfo dbg_info(amx, "DCC_RemoveGuildMemberBan", params, "dd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	UserId_t userid = params[2];
	User_t const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	guild->RemoveMemberBan(user);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildRolePosition(DCC_Guild:guild, DCC_Role:role, position);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildRolePosition)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildRolePosition", params, "ddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->SetRolePosition(role, static_cast<int>(params[3]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildRoleName(DCC_Guild:guild, DCC_Role:role, const name[]);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildRoleName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildRoleName", params, "dds");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->SetRoleName(role, amx_GetCppString(amx, params[3]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildRolePermissions(DCC_Guild:guild, DCC_Role:role, perm_high, perm_low);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildRolePermissions)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildRolePermissions", params, "dddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	unsigned long long
		perm_high = static_cast<unsigned long long>(params[3]) << 32,
		perm_low = static_cast<unsigned long long>(params[4]),
		permissions = perm_high & perm_low;

	guild->SetRolePermissions(role, permissions);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildRoleColor(DCC_Guild:guild, DCC_Role:role, color);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildRoleColor)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildRoleColor", params, "ddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->SetRoleColor(role, static_cast<unsigned int>(params[3]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildRoleHoist(DCC_Guild:guild, DCC_Role:role, bool:hoist);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildRoleHoist)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildRoleColor", params, "ddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->SetRoleHoist(role, params[3] != 0);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetGuildRoleMentionable(DCC_Guild:guild, DCC_Role:role, bool:mentionable);
AMX_DECLARE_NATIVE(Native::DCC_SetGuildRoleMentionable)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetGuildRoleColor", params, "ddd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->SetRoleMentionable(role, params[3] != 0);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_CreateGuildRole(DCC_Guild:guild, const name[],
//     const callback[], const format[], {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_CreateGuildRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateGuildRole", params, "dsss");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto name = amx_GetCppString(amx, params[2]);
	if (name.length() < 2 || name.length() > 100)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"name must be between 2 and 100 characters in length");
		return 0;
	}

	auto
		cb_name = amx_GetCppString(amx, params[3]),
		cb_format = amx_GetCppString(amx, params[4]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(
		amx, cb_name.c_str(), cb_format.c_str(), params, 5, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}

	GuildManager::Get()->CreateGuildRole(guild, name, std::move(cb));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Role:DCC_GetCreatedGuildRole();
AMX_DECLARE_NATIVE(Native::DCC_GetCreatedGuildRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetCreatedGuildRole", params);
	return GuildManager::Get()->GetCreatedRoleChannelId();
}

// native DCC_DeleteGuildRole(DCC_Guild:guild, DCC_Role:role);
AMX_DECLARE_NATIVE(Native::DCC_DeleteGuildRole)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteGuildRole", params, "dd");

	GuildId_t guildid = params[1];
	Guild_t const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	RoleId_t roleid = params[2];
	Role_t const &role = RoleManager::Get()->FindRole(roleid);
	if (!role)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid role id '{}'", roleid);
		return 0;
	}

	guild->DeleteRole(role);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_BotPresenceStatus:DCC_GetBotPresenceStatus();
AMX_DECLARE_NATIVE(Native::DCC_GetBotPresenceStatus)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetBotPresenceStatus", params);
	return static_cast<cell>(ThisBot::Get()->GetPresenceStatus());
}

// native DCC_TriggerBotTypingIndicator(DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_TriggerBotTypingIndicator)
{
	ScopedDebugInfo dbg_info(amx, "DCC_TriggerBotTypingIndicator", params, "d");

	auto channelid = static_cast<ChannelId_t>(params[1]);
	auto const &channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	ThisBot::Get()->TriggerTypingIndicator(channel);
	return 1;
}

// native DCC_SetBotNickname(DCC_Guild:guild, const nickname[]);
AMX_DECLARE_NATIVE(Native::DCC_SetBotNickname)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetBotNickname", params, "ds");

	auto guildid = static_cast<GuildId_t>(params[1]);
	auto const &guild = GuildManager::Get()->FindGuild(guildid);
	if (!guild)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid guild id '{}'", guildid);
		return 0;
	}

	auto nickname = amx_GetCppString(amx, params[2]);
	if (!nickname.empty()) // if nickname is empty it gets resetted/removed
	{
		// see https://discordapp.com/developers/docs/resources/user#usernames-and-nicknames
		if (nickname.length() < 2 || nickname.length() > 32
			|| nickname == "discordtag" || nickname == "everyone" || nickname == "here"
			|| nickname.front() == '@' || nickname.front() == '#' || nickname.front() == ':'
			|| nickname.find("```") == 0)
		{
			Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid nickname '{:s}'", nickname);
			return 0;
		}
	}

	ThisBot::Get()->SetNickname(guild, nickname);
	return 1;
}

// native DCC_CreatePrivateChannel(DCC_User:user, const callback[] = "",
//		const format[] = "", {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_CreatePrivateChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreatePrivateChannel", params, "dss");

	auto userid = static_cast<UserId_t>(params[1]);
	auto const &user = UserManager::Get()->FindUser(userid);
	if (!user)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid user id '{}'", userid);
		return 0;
	}

	auto
		cb_name = amx_GetCppString(amx, params[2]),
		cb_format = amx_GetCppString(amx, params[3]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(
		amx, cb_name.c_str(), cb_format.c_str(), params, 4, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}

	ThisBot::Get()->CreatePrivateChannel(user, std::move(cb));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Channel:DCC_GetCreatedPrivateChannel();
AMX_DECLARE_NATIVE(Native::DCC_GetCreatedPrivateChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetCreatedPrivateChannel", params);
	return ThisBot::Get()->GetCreatedPrivateChannelId();
}

// native DCC_SetBotPresenceStatus(DCC_BotPresenceStatus:status);
AMX_DECLARE_NATIVE(Native::DCC_SetBotPresenceStatus)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetBotPresenceStatus", params, "d");

	bool ret_val = ThisBot::Get()->SetPresenceStatus(
		static_cast<ThisBot::PresenceStatus>(params[1]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_SetBotActivity(const name[]);
AMX_DECLARE_NATIVE(Native::DCC_SetBotActivity)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetBotActivity", params, "s");

	ThisBot::Get()->SetActivity(amx_GetCppString(amx, params[1]));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_EscapeMarkdown(const src[], dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_EscapeMarkdown)
{
	ScopedDebugInfo dbg_info(amx, "DCC_EscapeMarkdown", params, "drs");

	auto const src = amx_GetCppString(amx, params[1]);
	std::string dest;
	dest.reserve(src.length());

	static const std::unordered_set<char> markdown_chars{
		'_', '*', '~', '`', '|'
	};

	bool skip = false;
	for (auto it = src.begin(); it != src.end(); ++it)
	{
		auto ch = *it;

		if (ch == '\\')
		{
			skip = true; // skip next escape
		}
		else if (markdown_chars.find(ch) != markdown_chars.end())
		{
			if (skip)
				skip = false;
			else
				dest.push_back('\\');
		}

		dest.push_back(ch);
	}

	if (amx_SetCppString(amx, params[2], dest, params[3]) != AMX_ERR_NONE)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "couldn't set destination string");
		return -1;
	}

	auto ret_val = static_cast<cell>(dest.length());
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_CreateEmbedMessage(const title[] = "", const description[] = "", const url[] = "", const timestamp[] = "", int color = 0, const footer_text[] = "", const footer_icon_url[] = "",
//		const thumbnail_url[] = "", const image_url[] = "");
AMX_DECLARE_NATIVE(Native::DCC_CreateEmbed)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateEmbed", params, "ssssdssss");
	auto const title = amx_GetCppString(amx, params[1]);
	auto const description = amx_GetCppString(amx, params[2]);
	auto const url = amx_GetCppString(amx, params[3]);
	auto const timestamp = amx_GetCppString(amx, params[4]);
	auto const color = static_cast<int>(params[5]);
	auto const footer_text = amx_GetCppString(amx, params[6]);
	auto const footer_icon_url = amx_GetCppString(amx, params[7]);
	auto const thumbnail_url = amx_GetCppString(amx, params[8]);
	auto const image_url = amx_GetCppString(amx, params[9]);

	EmbedId_t id = EmbedManager::Get()->AddEmbed(title, description, url, timestamp, color, footer_text, footer_icon_url, thumbnail_url, image_url);
	if (!id)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "failed to create embed");
		return 0;
	}
	return id;
}

// native DCC_DeleteEmbed(DCC_Embed:embed);
AMX_DECLARE_NATIVE(Native::DCC_DeleteEmbed)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteEmbed", params, "d");
	EmbedId_t embedid = static_cast<EmbedId_t>(params[1]);
	Embed_t const& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	EmbedManager::Get()->DeleteEmbed(embedid);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SendChannelEmbedMessage(DCC_Channel:channel, DCC_Embed:embed, const message[] = "",
//     const callback[] = "", const format[] = "", {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_SendChannelEmbedMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SendChannelEmbedMessage", params, "ddsss");

	ChannelId_t channelid = static_cast<ChannelId_t>(params[1]);
	Channel_t const& channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	EmbedId_t embedid = static_cast<EmbedId_t>(params[2]);
	Embed_t const& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto message = amx_GetCppString(amx, params[3]);
	if (message.length() > 2000)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"message must be shorter than 2000 characters");
		return 0;
	}

	auto
		cb_name = amx_GetCppString(amx, params[4]),
		cb_format = amx_GetCppString(amx, params[5]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(
		amx, cb_name.c_str(), cb_format.c_str(), params, 6, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}

	channel->SendEmbeddedMessage(embed, std::move(message), std::move(cb));
	EmbedManager::Get()->DeleteEmbed(embedid);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_AddEmbedField(DCC_Embed:embed, const name[], const value[], bool:embed = false);
AMX_DECLARE_NATIVE(Native::DCC_AddEmbedField)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddEmbeddedField", params, "dssd");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const name = amx_GetCppString(amx, params[2]);
	auto const value = amx_GetCppString(amx, params[3]);
	auto const inline_ = static_cast<bool>(params[4]);
	embed->AddField(name, value, inline_);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedTitle(DCC_Embed:embed, const title[]);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedTitle)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedTitle", params, "ds");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const title = amx_GetCppString(amx, params[2]);
	embed->SetTitle(title);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedDescription(DCC_Embed:embed, const description[]);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedDescription)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedDescription", params, "ds");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const description = amx_GetCppString(amx, params[2]);
	embed->SetDescription(description);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedUrl(DCC_Embed:embed, const url[]);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedUrl)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedUrl", params, "ds");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const url = amx_GetCppString(amx, params[2]);
	embed->SetUrl(url);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedTimestamp(DCC_Embed:embed, const timestamp[]);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedTimestamp)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedTimestamp", params, "ds");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const timestamp = amx_GetCppString(amx, params[2]);
	embed->SetTimestamp(timestamp);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedColor(DCC_Embed:embed, color);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedColor)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedColor", params, "dd");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const color = static_cast<int>(params[2]);
	embed->SetColor(color);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedFooter(DCC_Embed:embed, const footer_text[], const footer_icon_url[] = "");
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedFooter)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedFooter", params, "dss");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const text = amx_GetCppString(amx, params[2]);
	auto const icon_url = amx_GetCppString(amx, params[3]);
	embed->SetFooterText(text);
	embed->SetFooterIconUrl(icon_url);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedThumbnail(DCC_Embed:embed, const thumbnail_url[]);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedThumbnail)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedThumbnail", params, "ds");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const url = amx_GetCppString(amx, params[2]);
	embed->SetThumbnailUrl(url);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SetEmbedImage(DCC_Embed:embed, const image_url[]);
AMX_DECLARE_NATIVE(Native::DCC_SetEmbedImage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetEmbedImage", params, "ds");
	auto embedid = static_cast<EmbedId_t>(params[1]);
	auto& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto const url = amx_GetCppString(amx, params[2]);
	embed->SetImageUrl(url);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_DeleteInternalMessage(DCC_Message:message);
AMX_DECLARE_NATIVE(Native::DCC_DeleteInternalMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteInternalMessage", params, "d");
	auto messageid = static_cast<MessageId_t>(params[1]);
	auto& message = MessageManager::Get()->Find(messageid);
	if (!message)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", messageid);
		return 0;
	}
	MessageManager::Get()->Delete(messageid);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_Emoji:DCC_CreateEmoji(const name[DCC_EMOJI_NAME_SIZE], const snowflake[64] = "");
AMX_DECLARE_NATIVE(Native::DCC_CreateEmoji)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateEmoji", params, "ss");
	const auto& name = amx_GetCppString(amx, params[1]);
	const auto& snowflake = amx_GetCppString(amx, params[2]);

	EmojiId_t id = EmojiManager::Get()->AddEmoji(snowflake, name);
	if (!id)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "failed to create emoji");
		return 0;
	}
	return id;
}

// native DCC_DeleteEmoji(DCC_Emoji:emoji);
AMX_DECLARE_NATIVE(Native::DCC_DeleteEmoji)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteEmoji", params, "d");
	const auto& emojid = static_cast<EmojiId_t>(params[1]);
	const auto& emoji = EmojiManager::Get()->FindEmoji(emojid);
	if (!emoji)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid emoji id '{}'", emojid);
		return 0;
	}

	EmojiManager::Get()->DeleteEmoji(emojid);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetEmojiName(DCC_Emoji:emoji, dest[DCC_EMOJI_NAME_SIZE], maxlen = DCC_EMOJI_NAME_SIZE);
AMX_DECLARE_NATIVE(Native::DCC_GetEmojiName)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetEmojiName", params, "dsd");
	std::string dest;

	auto emojid = static_cast<EmojiId_t>(params[1]);
	auto& emoji = EmojiManager::Get()->FindEmoji(emojid);
	if (!emoji)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid emoji id '{}'", emojid);
		return 0;
	}

	dest = emoji->GetName();
	if (amx_SetCppString(amx, params[2], dest, params[3]) != AMX_ERR_NONE)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "couldn't set destination string");
		return -1;
	}

	return dest.length();
}

// native DCC_CreateReaction(DCC_Message:message, DCC_Emoji:reaction_emoji);
AMX_DECLARE_NATIVE(Native::DCC_CreateReaction)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateReaction", params, "dd");
	const auto& messageid = static_cast<MessageId_t>(params[1]);
	const auto& message = MessageManager::Get()->Find(messageid);
	if (!message)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", messageid);
		return 0;
	}

	const auto& emojid = static_cast<EmojiId_t>(params[2]);
	const auto& emoji = EmojiManager::Get()->FindEmoji(emojid);
	if (!emoji)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid emoji id '{}'", emojid);
		return 0;
	}

	message->AddReaction(emoji);
	EmojiManager::Get()->DeleteEmoji(emojid);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_DeleteMessageReaction(DCC_Message:message, DCC_Emoji:emoji = 0);
AMX_DECLARE_NATIVE(Native::DCC_DeleteMessageReaction)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteMessageReactions", params, "dd");
	const auto& messageid = static_cast<MessageId_t>(params[1]);
	const auto& message = MessageManager::Get()->Find(messageid);
	if (!message)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", messageid);
		return 0;
	}

	const auto& emojid = static_cast<EmojiId_t>(params[2]);
	cell retval = static_cast<cell>(message->DeleteReaction(emojid));
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", retval);
	return retval;
}

// native DCC_EditMessage(DCC_Message:message, const message[], DCC_Embed:embed = 0);
AMX_DECLARE_NATIVE(Native::DCC_EditMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_EditMessage", params, "dsd");
	const auto& messageid = static_cast<MessageId_t>(params[1]);
	const auto& message = MessageManager::Get()->Find(messageid);
	if (!message)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", messageid);
		return 0;
	}

	const auto content = amx_GetCppString(amx, params[2]);
	if (content.length() > 2000)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"message must be shorter than 2000 characters");
		return 0;
	}

	const auto& embedid = static_cast<EmbedId_t>(params[3]);
	cell retval = static_cast<cell>(message->EditMessage(content, embedid));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", retval);
	return retval;
}

// native DCC_SetMessage(DCC_Message:message, bool persistent);
AMX_DECLARE_NATIVE(Native::DCC_SetMessagePersistent)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SetMessagePersistent", params, "dd");
	const auto& messageid = static_cast<MessageId_t>(params[1]);
	const auto& message = MessageManager::Get()->Find(messageid);
	bool persistent = static_cast<bool>(params[2]);
	if (!message)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid message id '{}'", messageid);
		return 0;
	}

	message->SetPresistent(persistent);
	Logger::Get()->LogNative(samplog_LogLevel::INFO, "return value: '1'");
	return 1;
}

// native DCC_CacheChannelMessage(const channel_id[DCC_ID_SIZE], const message_id[DCC_ID_SIZE], const callback[] = "", const format[] = "", {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_CacheChannelMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CacheChannelMessage", params, "ssss");
	const auto& channel_snowflake = amx_GetCppString(amx, params[1]);
	const auto& message_snowflake = amx_GetCppString(amx, params[2]);

	if (!channel_snowflake.length() || !message_snowflake.length())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel/message length '{} {}'", channel_snowflake, message_snowflake);
		return 0;
	}

	if (!ChannelManager::Get()->FindChannelById(channel_snowflake))
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel snowflake '{}'", channel_snowflake);
		return 0;
	}
	else if(MessageManager::Get()->FindById(message_snowflake))
	{
		Logger::Get()->LogNative(samplog_LogLevel::INFO, "message is already in cache! '{}'", message_snowflake);
		return 0;
	}

	auto
		cb_name = amx_GetCppString(amx, params[3]),
		cb_format = amx_GetCppString(amx, params[4]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(
		amx, cb_name.c_str(), cb_format.c_str(), params, 5, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}

	MessageManager::Get()->CreateFromSnowflake(channel_snowflake, message_snowflake, std::move(cb));
	Logger::Get()->LogNative(samplog_LogLevel::INFO, "return value: '1'");
	return 1;
}

// native DCC_Command:DCC_CreateCommand(const command_name[DCC_COMMAND_SIZE], const description[DCC_COMMAND_DESCRIPTION_SIZE], const callback[], bool:allow_everyone = true, DCC_Guild:guild = DCC_INVALID_GUILD);
AMX_DECLARE_NATIVE(Native::DCC_CreateCommand)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateCommand", params, "sssii");
	const auto& name = amx_GetCppString(amx, params[1]);
	const auto& description = amx_GetCppString(amx, params[2]);
	const auto& callback = amx_GetCppString(amx, params[3]);
	const auto& allow_everyone = static_cast<bool>(params[4]);
	const auto& guild = static_cast<GuildId_t>(params[5]);
	//const auto& permissions_size = static_cast<size_t>(params[6]);

	if (!GuildManager::Get()->FindGuild(guild) && guild != INVALID_GUILD_ID) {
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "passed invalid guild id '{}'", guild);
		return 0;
	}

	CommandId_t id = CommandManager::Get()->FindCommandIdByName(name, guild);
	if (id == INVALID_COMMAND_ID)
	{
		// New command!
		id = CommandManager::Get()->AddCommand(name, description);
		auto & command = CommandManager::Get()->FindCommand(id);
		command->SetCallback(callback);

		/*if (options_size)
		{
			cell* array = nullptr;
			if (amx_GetAddr(amx, params[4], &array) != AMX_ERR_NONE)
			{
				Logger::Get()->LogNative(samplog_LogLevel::INFO, "unable to find the array for options");
				return 0;
			}

			for (size_t i = 0; i < options_size; i++)
			{
				// TODO: find option and assign it
			}
		}
		else
		{
			Logger::Get()->LogNative(samplog_LogLevel::INFO, "command '{:s}' created with NO options!", name);
		}
		*/

		/*if (permissions_size)
		{
			cell* array = nullptr;

			if (amx_GetAddr(amx, params[4], &array) != AMX_ERR_NONE)
			{
				Logger::Get()->LogNative(samplog_LogLevel::INFO, "unable to find the array for permissions");
				return 0;
			}

			int location = 0;
			size_t i = permissions_size - 2; ignore default ""
			for (; i != 0;)
			{
				int len;
				amx_StrLen(&array[location], &len);
				std::string string(len, ' ');
				amx_GetString(&string[0], &array[location], 0, len + 1);
				i -= len;
				location += len + 1;

				if (string.length())
				{
					Logger::Get()->Log(samplog_LogLevel::ERROR, "role {} {}", string, i);
					auto const& role = RoleManager::Get()->FindRoleById(string);
					if (role) {
						command->SetPermission(role->GetPawnId(), true);
					}
				}
			}
		}
		else
		{
			Logger::Get()->LogNative(samplog_LogLevel::INFO, "command '{:s}' created with NO permissions!", name);
		}*/
		command->AllowEveryone(allow_everyone);
		if (g_PendingCommandOptions.empty())
		{
			CommandOption_t option = std::unique_ptr<CommandOption>(new CommandOption());
			option->SetType(COMMAND_OPTION_TYPE::OPTION_STRING);
			option->SetName("arguments");
			option->SetRequired(false);
			option->SetDescription("just type");
			command->AddOption("arguments", option);
		}
		else
		{
			for (auto & pending_option : g_PendingCommandOptions)
			{
				command->AddOption(pending_option.second->GetName(), pending_option.second);
			}
			ClearPendingCommandOptions();
		}
		command->SetGuild(guild);
		command->HandleNewCreation();
	}
	else
	{
		if (!g_PendingCommandOptions.empty())
		{
			Logger::Get()->LogNative(
				samplog_LogLevel::WARNING,
				"command '{}' already exists; pending options were ignored",
				name);
			ClearPendingCommandOptions();
		}
		auto & command = CommandManager::Get()->FindCommand(id);
		command->SetCallback(callback);
	}
	return id;
}

// native DCC_GetInteractionOptionCount(DCC_Interaction:interaction, &option_count);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionOptionCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionOptionCount", params, "dr");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(interaction->GetOptions().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetInteractionOption(DCC_Interaction:interaction, offset, path[], value[], &DCC_CommandOptionType:type, path_size = sizeof path, value_size = sizeof value);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionOption)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionOption", params, "ddssrdd");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	auto const offset = static_cast<unsigned int>(params[2]);
	auto const* option = interaction->GetOption(offset);
	if (option == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, interaction->GetOptions().size());
		return 0;
	}

	cell* type_dest = nullptr;
	if (amx_GetAddr(amx, params[5], &type_dest) != AMX_ERR_NONE || type_dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	if (amx_SetCppString(amx, params[3], option->m_FullName, params[6]) != AMX_ERR_NONE)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid path output buffer");
		return 0;
	}

	if (amx_SetCppString(amx, params[4], option->m_Value, params[7]) != AMX_ERR_NONE)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid value output buffer");
		return 0;
	}

	*type_dest = static_cast<cell>(option->m_Type);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetInteractionMentionCount(DCC_Interaction:interaction, &mentioned_user_count);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionMentionCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionMentionCount", params, "dr");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(interaction->GetMentions().size());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetInteractionMention(DCC_Interaction:interaction, offset, &DCC_User:mentioned_user);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionMention)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionMention", params, "ddr");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	auto const offset = static_cast<unsigned int>(params[2]);
	auto const& mentions = interaction->GetMentions();
	if (offset >= mentions.size())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid offset '{}', max size is '{}'",
			offset, mentions.size());
		return 0;
	}

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(mentions.at(offset));

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetInteractionContent(DCC_Interaction:interaction, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionContent)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetMessageContent", params, "drd");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	cell ret_val = amx_SetCppString(
		amx, params[2], interaction->GetContent(), params[3]) == AMX_ERR_NONE;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_GetInteractionChannel(DCC_Interaction:interaction, &DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionChannel)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionChannel", params, "dr");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}


	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(interaction->GetChannelID());

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetInteractionGuild(DCC_Interaction:interaction, &DCC_Channel:channel);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionGuild)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionGuild", params, "dr");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid reference");
		return 0;
	}

	*dest = static_cast<cell>(interaction->GetGuildID());
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SendInteractionEmbed(DCC_Interaction:interaction, DCC_Embed:embed, const message[] = "");
AMX_DECLARE_NATIVE(Native::DCC_SendInteractionEmbed)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SendInteractionEmbed", params, "ddsss");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	EmbedId_t embedid = static_cast<EmbedId_t>(params[2]);
	Embed_t const& embed = EmbedManager::Get()->FindEmbed(embedid);
	if (!embed)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid embed id '{}'", embedid);
		return 0;
	}

	auto message = amx_GetCppString(amx, params[3]);
	if (message.length() > 2000)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"message must be shorter than 2000 characters");
		return 0;
	}

	interaction->SendEmbed(embedid, message);
	EmbedManager::Get()->DeleteEmbed(embedid);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_SendInteractionMessage(DCC_Interaction:interaction, const message[] = "");
AMX_DECLARE_NATIVE(Native::DCC_SendInteractionMessage)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SendInteractionMessage", params, "ds");

	CommandInteractionId_t id = params[1];
	CommandInteraction_t const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(id);
	if (!interaction)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid interaction id '{}'", id);
		return 0;
	}

	auto message = amx_GetCppString(amx, params[2]);
	if (message.length() > 2000)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"message must be shorter than 2000 characters");
		return 0;
	}

	interaction->SendInteractionMessage(message);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_DeleteCommand(DCC_Command:command);
AMX_DECLARE_NATIVE(Native::DCC_DeleteCommand)
{
	ScopedDebugInfo dbg_info(amx, "DCC_DeleteCommand", params, "d");

	CommandId_t id = params[1];
	Command_t const& command = CommandManager::Get()->FindCommand(id);
	if (!command)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid command id '{}'", id);
		return 0;
	}

	command->DeleteFromDiscord();
	CommandManager::Get()->DeleteCommand(id);
	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

AMX_DECLARE_NATIVE(Native::DCC_AddCommandOption)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddCommandOption", params, "ssiii");
	const auto& name = amx_GetCppString(amx, params[1]);
	const auto& description = amx_GetCppString(amx, params[2]);
	const int type = static_cast<int>(params[3]);
	bool required = static_cast<bool>(params[4]);
	const cell parent_option_id = params[5];

	if (name.empty() || description.empty())
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "name/description must not be empty");
		return 0;
	}

	if (type < static_cast<int>(COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND) ||
		type > static_cast<int>(COMMAND_OPTION_TYPE::OPTION_ROLE))
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid option type '{}'", type);
		return 0;
	}

	if (type == static_cast<int>(COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND) ||
		type == static_cast<int>(COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND_GROUP))
	{
		required = false;
	}

	CommandOption_t option = std::unique_ptr<CommandOption>(new CommandOption());
	option->SetType(static_cast<COMMAND_OPTION_TYPE>(type));
	option->SetName(name);
	option->SetDescription(description);
	option->SetRequired(required);

	cell id = g_NextCommandOptionId++;
	if (id == 0)
	{
		id = g_NextCommandOptionId++;
	}

	g_PendingCommandOptionIndex.emplace(id, option.get());

	if (parent_option_id != 0)
	{
		auto parent_it = g_PendingCommandOptionIndex.find(parent_option_id);
		if (parent_it == g_PendingCommandOptionIndex.end() || parent_it->second == nullptr)
		{
			Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid parent option '{}'", parent_option_id);
			g_PendingCommandOptionIndex.erase(id);
			return 0;
		}

		COMMAND_OPTION_TYPE parent_type = parent_it->second->GetType();
		if (parent_type == COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND_GROUP &&
			type != static_cast<int>(COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND))
		{
			Logger::Get()->LogNative(
				samplog_LogLevel::ERROR,
				"sub-command groups may only contain sub-commands");
			g_PendingCommandOptionIndex.erase(id);
			return 0;
		}

		if (parent_type == COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND &&
			(type == static_cast<int>(COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND) ||
				type == static_cast<int>(COMMAND_OPTION_TYPE::OPTION_SUB_COMMAND_GROUP)))
		{
			Logger::Get()->LogNative(
				samplog_LogLevel::ERROR,
				"sub-commands may not contain sub-commands or sub-command groups");
			g_PendingCommandOptionIndex.erase(id);
			return 0;
		}

		parent_it->second->AddOption(option->GetName(), option);
	}
	else
	{
		g_PendingCommandOptions.emplace(id, std::move(option));
	}

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", id);
	return id;
}

// native DCC_SetCommandPermission(DCC_Command:command, DCC_Role:role, bool:enable);
/*AMX_DECLARE_NATIVE(Native::DCC_SetCommandPermission)
{
	return 1;
}*/
// native DCC_Component:DCC_CreateButton(const text[], DCC_ButtonStyle:style, const custom_id[], const url[], bool:disabled, DCC_Emoji:emoji);
AMX_DECLARE_NATIVE(Native::DCC_CreateButton)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateButton", params, "sdssdd");

	auto text = amx_GetCppString(amx, params[1]);
	auto style = static_cast<ButtonStyle>(params[2]);
	auto custom_id = amx_GetCppString(amx, params[3]);
	auto url = amx_GetCppString(amx, params[4]);
	auto disabled = params[5] != 0;
	auto emoji = static_cast<EmojiId_t>(params[6]);

	cell ret_val = ComponentManager::Get()->CreateButton(text, style, custom_id, url, disabled, emoji);

	// Validate eagerly so misconfiguration shows up here instead of as a
	// silent 400 once the message is sent. Validation only logs - the
	// component is still created so callers can patch and retry.
	auto const& comp = ComponentManager::Get()->FindComponent(ret_val);
	if (comp)
		(void)comp->Validate();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Component:DCC_CreateSelectMenu(DCC_ComponentType:type, const custom_id[], const placeholder[], min_values, max_values, bool:disabled);
AMX_DECLARE_NATIVE(Native::DCC_CreateSelectMenu)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateSelectMenu", params, "dssddd");

	auto type = static_cast<ComponentType>(params[1]);
	auto custom_id = amx_GetCppString(amx, params[2]);
	auto placeholder = amx_GetCppString(amx, params[3]);
	auto min_values = static_cast<int>(params[4]);
	auto max_values = static_cast<int>(params[5]);
	auto disabled = params[6] != 0;

	cell ret_val = ComponentManager::Get()->CreateSelectMenu(type, custom_id, placeholder, min_values, max_values, disabled);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native bool:DCC_AddSelectMenuOption(DCC_Component:menu, const label[], const value[], const description[], DCC_Emoji:emoji, bool:is_default);
AMX_DECLARE_NATIVE(Native::DCC_AddSelectMenuOption)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddSelectMenuOption", params, "dsssdd");

	auto menu = static_cast<ComponentId_t>(params[1]);
	auto label = amx_GetCppString(amx, params[2]);
	auto value = amx_GetCppString(amx, params[3]);
	auto description = amx_GetCppString(amx, params[4]);
	auto emoji = static_cast<EmojiId_t>(params[5]);
	auto is_default = params[6] != 0;

	cell ret_val = ComponentManager::Get()->AddSelectMenuOption(menu, label, value, description, emoji, is_default) ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Component:DCC_CreateTextInput(const custom_id[], DCC_TextInputStyle:style, const label[], min_length = -1, max_length = -1, bool:required = true, const value[] = "", const placeholder[] = "");
AMX_DECLARE_NATIVE(Native::DCC_CreateTextInput)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateTextInput", params, "sdsdddss");

	auto custom_id  = amx_GetCppString(amx, params[1]);
	auto style      = static_cast<TextInputStyle>(params[2]);
	auto label      = amx_GetCppString(amx, params[3]);
	auto min_length = static_cast<int>(params[4]);
	auto max_length = static_cast<int>(params[5]);
	auto required   = params[6] != 0;
	auto value      = amx_GetCppString(amx, params[7]);
	auto placeholder= amx_GetCppString(amx, params[8]);

	cell ret_val = ComponentManager::Get()->CreateTextInput(
		custom_id, style, label, min_length, max_length, required, value, placeholder);

	auto const& comp = ComponentManager::Get()->FindComponent(ret_val);
	if (comp)
		(void)comp->Validate();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Component:DCC_CreateFileUpload(const custom_id[], min_values = 1, max_values = 1, bool:required = true);
AMX_DECLARE_NATIVE(Native::DCC_CreateFileUpload)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateFileUpload", params, "sddd");

	auto custom_id = amx_GetCppString(amx, params[1]);
	auto min_v     = static_cast<int>(params[2]);
	auto max_v     = static_cast<int>(params[3]);
	auto required  = params[4] != 0;

	cell ret_val = ComponentManager::Get()->CreateFileUpload(custom_id, min_v, max_v, required);

	auto const& comp = ComponentManager::Get()->FindComponent(ret_val);
	if (comp)
		(void)comp->Validate();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_ActionRow:DCC_CreateActionRow();
AMX_DECLARE_NATIVE(Native::DCC_CreateActionRow)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateActionRow", params);

	cell ret_val = ComponentManager::Get()->CreateActionRow();

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native bool:DCC_AddComponentToActionRow(DCC_ActionRow:row, DCC_Component:component);
AMX_DECLARE_NATIVE(Native::DCC_AddComponentToActionRow)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddComponentToActionRow", params, "dd");

	auto row = static_cast<ActionRowId_t>(params[1]);
	auto comp = static_cast<ComponentId_t>(params[2]);

	cell ret_val = ComponentManager::Get()->AddComponentToActionRow(row, comp) ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_Modal:DCC_CreateModal(const title[], const custom_id[]);
AMX_DECLARE_NATIVE(Native::DCC_CreateModal)
{
	ScopedDebugInfo dbg_info(amx, "DCC_CreateModal", params, "ss");

	auto title = amx_GetCppString(amx, params[1]);
	auto custom_id = amx_GetCppString(amx, params[2]);

	cell ret_val = ModalManager::Get()->CreateModal(title, custom_id);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native bool:DCC_AddModalInput(DCC_Modal:modal, const custom_id[], DCC_TextInputStyle:style, const label[], min_length, max_length, bool:required, const value[], const placeholder[]);
AMX_DECLARE_NATIVE(Native::DCC_AddModalInput)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddModalInput", params, "dsdsdddss");

	auto modal = static_cast<ModalId_t>(params[1]);
	auto custom_id = amx_GetCppString(amx, params[2]);
	auto style = static_cast<TextInputStyle>(params[3]);
	auto label = amx_GetCppString(amx, params[4]);
	auto min_length = static_cast<int>(params[5]);
	auto max_length = static_cast<int>(params[6]);
	auto required = params[7] != 0;
	auto value = amx_GetCppString(amx, params[8]);
	auto placeholder = amx_GetCppString(amx, params[9]);

	cell ret_val = ModalManager::Get()->AddModalInput(modal, custom_id, style, label, min_length, max_length, required, value, placeholder) ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native bool:DCC_AddModalComponent(DCC_Modal:modal, DCC_Component:component, const label[] = "", const description[] = "");
//
// Adds any input-style component (text input, select menu, file upload) to a
// modal. The `label` / `description` arguments only apply to non-text-input
// components - they are surfaced as the visible Label wrapper text. Pass empty
// strings to keep the historic Action Row layout (text inputs only).
AMX_DECLARE_NATIVE(Native::DCC_AddModalComponent)
{
	ScopedDebugInfo dbg_info(amx, "DCC_AddModalComponent", params, "ddss");

	auto modal     = static_cast<ModalId_t>(params[1]);
	auto component = static_cast<ComponentId_t>(params[2]);
	auto label     = amx_GetCppString(amx, params[3]);
	auto description = amx_GetCppString(amx, params[4]);

	cell ret_val = ModalManager::Get()->AddModalComponent(modal, component,
		std::move(label), std::move(description)) ? 1 : 0;

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DCC_SendInteractionModal(DCC_Interaction:interaction, DCC_Modal:modal);
AMX_DECLARE_NATIVE(Native::DCC_SendInteractionModal)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SendInteractionModal", params, "dd");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto modalid = static_cast<ModalId_t>(params[2]);

	auto const& modal = ModalManager::Get()->FindModal(modalid);
	if (!modal)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"invalid modal id '{}'", modalid);
		return 0;
	}
	if (!modal->Validate())
		return 0;

	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (interaction)
	{
		interaction->SendModal(modalid);
		return 1;
	}

	Logger::Get()->LogNative(samplog_LogLevel::ERROR,
		"invalid interaction id '{}'", interactionid);
	return 0;
}

// native DCC_SendChannelMessageEx(DCC_Channel:channel, const message[], DCC_Embed:embed, DCC_ActionRow:rows[], rows_size, const callback[] = "", const format[] = "", {Float, _}:...);
AMX_DECLARE_NATIVE(Native::DCC_SendChannelMessageEx)
{
	ScopedDebugInfo dbg_info(amx, "DCC_SendChannelMessageEx", params, "dsdadss");

	auto channelid = static_cast<ChannelId_t>(params[1]);
	auto const& channel = ChannelManager::Get()->FindChannel(channelid);
	if (!channel)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "invalid channel id '{}'", channelid);
		return 0;
	}

	auto message = amx_GetCppString(amx, params[2]);
	if (message.length() > 2000)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR,
			"message must be shorter than 2000 characters");
		return 0;
	}

	auto embedid = static_cast<EmbedId_t>(params[3]);

	cell* rows_ptr = nullptr;
	amx_GetAddr(amx, params[4], &rows_ptr);
	int rows_size = static_cast<int>(params[5]);

	std::vector<ActionRowId_t> rows;
	if (rows_ptr && rows_size > 0)
	{
		if (rows_size > 5)
		{
			Logger::Get()->LogNative(samplog_LogLevel::ERROR,
				"a message can only carry up to 5 action rows, got {}", rows_size);
			return 0;
		}

		for (int i = 0; i < rows_size; ++i)
		{
			ActionRowId_t row_id = static_cast<ActionRowId_t>(rows_ptr[i]);
			auto const& row = ComponentManager::Get()->FindActionRow(row_id);
			if (!row)
			{
				Logger::Get()->LogNative(samplog_LogLevel::ERROR,
					"invalid action row id '{}' at index {}", row_id, i);
				return 0;
			}
			if (!row->Validate())
			{
				// Validate() already logs the precise reason. Refuse to send so
				// the caller can fix the payload instead of guessing why their
				// button never appeared.
				return 0;
			}
			rows.push_back(row_id);
		}
	}

	auto cb_name   = amx_GetCppString(amx, params[6]);
	auto cb_format = amx_GetCppString(amx, params[7]);

	pawn_cb::Error cb_error;
	auto cb = pawn_cb::Callback::Prepare(amx, cb_name.c_str(), cb_format.c_str(),
		params, 8, cb_error);
	if (cb_error && cb_error.get() != pawn_cb::Error::Type::EMPTY_NAME)
	{
		Logger::Get()->LogNative(samplog_LogLevel::ERROR, "could not prepare callback");
		return 0;
	}

	channel->SendMessage(std::move(message), std::move(cb), embedid, rows);

	Logger::Get()->LogNative(samplog_LogLevel::DEBUG, "return value: '1'");
	return 1;
}

// native DCC_GetInteractionMenuValue(DCC_Interaction:interaction, offset, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionMenuValue)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionMenuValue", params, "ddrd");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	auto const& data = interaction->GetInteractionData();
	if (data.find("data") == data.end() || data.at("data").find("values") == data.at("data").end())
		return 0;

	auto const& values = data.at("data").at("values");
	int offset = static_cast<int>(params[2]);
	if (offset < 0 || offset >= static_cast<int>(values.size()))
		return 0;

	return amx_SetCppString(amx, params[3], values.at(offset).get<std::string>(), params[4]) == AMX_ERR_NONE;
}

// native DCC_GetInteractionMenuCount(DCC_Interaction:interaction, &count);
AMX_DECLARE_NATIVE(Native::DCC_GetInteractionMenuCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetInteractionMenuCount", params, "dr");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	auto const& data = interaction->GetInteractionData();
	if (data.find("data") == data.end() || data.at("data").find("values") == data.at("data").end())
		return 0;

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[2], &dest) != AMX_ERR_NONE || dest == nullptr)
		return 0;

	*dest = static_cast<cell>(data.at("data").at("values").size());
	return 1;
}

// native DCC_GetModalInputValue(DCC_Interaction:interaction, const custom_id[], dest[], max_size = sizeof dest);
//
// Walks the modal submission payload looking for a component matching
// `custom_id`. Supports both the V1 layout (Action Row > Text Input) and the
// V2 layout (Label > component) so it keeps working when the modal also hosts
// selects or file uploads.
namespace
{
	// Returns a pointer to the first component within the modal submission
	// whose custom_id matches `target`, or nullptr if no match was found.
	json const* FindModalSubmissionComponent(json const& data, std::string const& target)
	{
		auto data_it = data.find("data");
		if (data_it == data.end())
			return nullptr;

		auto comps_it = data_it->find("components");
		if (comps_it == data_it->end() || !comps_it->is_array())
			return nullptr;

		// Recursively walk component trees: action rows expose `components[]`
		// while label wrappers expose either `component` (singular) or
		// `components[]` depending on the API revision in flight.
		std::function<json const*(json const&)> walk = [&](json const& node) -> json const* {
			auto cid_it = node.find("custom_id");
			if (cid_it != node.end() && cid_it->is_string()
				&& cid_it->get<std::string>() == target)
			{
				return &node;
			}

			auto inner_arr = node.find("components");
			if (inner_arr != node.end() && inner_arr->is_array())
			{
				for (auto const& child : *inner_arr)
				{
					if (auto* hit = walk(child))
						return hit;
				}
			}

			auto inner_one = node.find("component");
			if (inner_one != node.end() && inner_one->is_object())
			{
				if (auto* hit = walk(*inner_one))
					return hit;
			}

			return nullptr;
		};

		for (auto const& row : *comps_it)
		{
			if (auto* hit = walk(row))
				return hit;
		}
		return nullptr;
	}
}

AMX_DECLARE_NATIVE(Native::DCC_GetModalInputValue)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetModalInputValue", params, "dsrd");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	std::string custom_id = amx_GetCppString(amx, params[2]);

	json const* hit = FindModalSubmissionComponent(interaction->GetInteractionData(), custom_id);
	if (!hit)
		return 0;

	auto val_it = hit->find("value");
	if (val_it == hit->end() || !val_it->is_string())
		return 0;

	return amx_SetCppString(amx, params[3], val_it->get<std::string>(), params[4]) == AMX_ERR_NONE;
}

// native DCC_GetModalSelectCount(DCC_Interaction:interaction, const custom_id[], &count);
AMX_DECLARE_NATIVE(Native::DCC_GetModalSelectCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetModalSelectCount", params, "dsr");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	std::string custom_id = amx_GetCppString(amx, params[2]);
	json const* hit = FindModalSubmissionComponent(interaction->GetInteractionData(), custom_id);
	if (!hit)
		return 0;

	auto values = hit->find("values");
	if (values == hit->end() || !values->is_array())
		return 0;

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
		return 0;

	*dest = static_cast<cell>(values->size());
	return 1;
}

// native DCC_GetModalSelectValue(DCC_Interaction:interaction, const custom_id[], offset, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetModalSelectValue)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetModalSelectValue", params, "dsdrd");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	std::string custom_id = amx_GetCppString(amx, params[2]);
	int offset = static_cast<int>(params[3]);

	json const* hit = FindModalSubmissionComponent(interaction->GetInteractionData(), custom_id);
	if (!hit)
		return 0;

	auto values = hit->find("values");
	if (values == hit->end() || !values->is_array())
		return 0;
	if (offset < 0 || offset >= static_cast<int>(values->size()))
		return 0;

	return amx_SetCppString(amx, params[4],
		values->at(offset).get<std::string>(), params[5]) == AMX_ERR_NONE;
}

// native DCC_GetModalAttachmentCount(DCC_Interaction:interaction, const custom_id[], &count);
//
// Returns the number of attachments that were uploaded for the file-upload
// component identified by `custom_id`.
AMX_DECLARE_NATIVE(Native::DCC_GetModalAttachmentCount)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetModalAttachmentCount", params, "dsr");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	std::string custom_id = amx_GetCppString(amx, params[2]);
	json const* hit = FindModalSubmissionComponent(interaction->GetInteractionData(), custom_id);
	if (!hit)
		return 0;

	// File upload submissions carry a `values` array of attachment ids and
	// the resolved attachments live under `data.resolved.attachments`.
	auto values = hit->find("values");
	if (values == hit->end() || !values->is_array())
		return 0;

	cell* dest = nullptr;
	if (amx_GetAddr(amx, params[3], &dest) != AMX_ERR_NONE || dest == nullptr)
		return 0;

	*dest = static_cast<cell>(values->size());
	return 1;
}

namespace
{
	// Looks up the attachment object referenced by the `offset`-th value of
	// the file-upload component identified by `custom_id`. Returns nullptr if
	// any step of the lookup fails.
	json const* FindModalAttachment(json const& data, std::string const& custom_id, int offset)
	{
		json const* hit = FindModalSubmissionComponent(data, custom_id);
		if (!hit)
			return nullptr;

		auto values = hit->find("values");
		if (values == hit->end() || !values->is_array())
			return nullptr;
		if (offset < 0 || offset >= static_cast<int>(values->size()))
			return nullptr;

		std::string id = values->at(offset).get<std::string>();

		auto data_it = data.find("data");
		if (data_it == data.end())
			return nullptr;
		auto resolved = data_it->find("resolved");
		if (resolved == data_it->end())
			return nullptr;
		auto attachments = resolved->find("attachments");
		if (attachments == resolved->end() || !attachments->is_object())
			return nullptr;

		auto att_it = attachments->find(id);
		if (att_it == attachments->end() || !att_it->is_object())
			return nullptr;

		return &(*att_it);
	}
}

// native DCC_GetModalAttachmentId(DCC_Interaction:interaction, const custom_id[], offset, dest[DCC_ID_SIZE], max_size = DCC_ID_SIZE);
AMX_DECLARE_NATIVE(Native::DCC_GetModalAttachmentId)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetModalAttachmentId", params, "dsdrd");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	std::string custom_id = amx_GetCppString(amx, params[2]);
	int offset = static_cast<int>(params[3]);

	json const* att = FindModalAttachment(interaction->GetInteractionData(), custom_id, offset);
	if (!att)
		return 0;

	auto id_it = att->find("id");
	if (id_it == att->end() || !id_it->is_string())
		return 0;

	return amx_SetCppString(amx, params[4],
		id_it->get<std::string>(), params[5]) == AMX_ERR_NONE;
}

// native DCC_GetModalAttachmentUrl(DCC_Interaction:interaction, const custom_id[], offset, dest[], max_size = sizeof dest);
AMX_DECLARE_NATIVE(Native::DCC_GetModalAttachmentUrl)
{
	ScopedDebugInfo dbg_info(amx, "DCC_GetModalAttachmentUrl", params, "dsdrd");

	auto interactionid = static_cast<CommandInteractionId_t>(params[1]);
	auto const& interaction = CommandInteractionManager::Get()->FindCommandInteraction(interactionid);
	if (!interaction)
		return 0;

	std::string custom_id = amx_GetCppString(amx, params[2]);
	int offset = static_cast<int>(params[3]);

	json const* att = FindModalAttachment(interaction->GetInteractionData(), custom_id, offset);
	if (!att)
		return 0;

	auto url_it = att->find("url");
	if (url_it == att->end() || !url_it->is_string())
		return 0;

	return amx_SetCppString(amx, params[4],
		url_it->get<std::string>(), params[5]) == AMX_ERR_NONE;
}
