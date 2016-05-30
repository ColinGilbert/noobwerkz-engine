// This class provides basic network client functionality: It allows to connect to a single server, and then send/receive messages from it.
#pragma once

#include <deque>
#include <enet/enet.h>

#include "Logger.hpp"

namespace noob
{
	class network_client
	{
		public:
			network_client() noexcept(true) : local_host(NULL), peer(NULL) {}

			~network_client() noexcept(true);

			bool init(size_t num_channels, uint32_t incoming_bandwidth = 0, uint32_t outgoing_bandwidth = 0) noexcept(true);

			// Blocking function... I think.
			bool connect(const std::string& address, uint16_t port, uint32_t timeout_in_millis = 5000) noexcept(true);

			void send(const std::string& message, size_t channel, bool reliable) noexcept(true);
			
			bool has_message() const noexcept(true);
			
			std::string get_message() noexcept(true);
			
			void tick() noexcept(true);

		protected:
			ENetHost* local_host;
			ENetPeer* peer;
			std::deque<std::string> packets;
	};
}
