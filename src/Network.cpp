#include "Network.hpp"
#include "Logger.hpp"


void Network::Initialize(std::string const &token, int intents)
{
	Logger::Get()->Log(samplog_LogLevel::DEBUG, "Network::Initialize");

	m_Http = std::unique_ptr<::Http>(new ::Http(token));

	// Connect WebSocket directly to gateway.discord.gg
	m_WebSocket->Initialize(token, "gateway.discord.gg", intents);
}

Network::~Network()
{
	Logger::Get()->Log(samplog_LogLevel::DEBUG, "Network::~Network");
}

::Http &Network::Http()
{
	return *m_Http;
}

::WebSocket &Network::WebSocket()
{
	return *m_WebSocket;
}
