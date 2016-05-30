#include "NetworkClient.hpp"



noob::network_client::~network_client() noexcept(true)
{
	if (local_host != NULL)
	{
		enet_host_destroy(local_host);
	}
}


bool noob::network_client::init(size_t num_channels, uint32_t incoming_bandwidth = 0, uint32_t outgoing_bandwidth = 0) noexcept(true)
{
	if (!enet_initialize())
	{
		connected = false;
		logger::log("[NetworkClient] Failed to init eNet");
		return false;
	}

	local_host = enet_host_create(NULL, 1, num_channels, incoming_bandwidth, outgoing_bandwidth);

	if (local_host == NULL)
	{
		connected = false;
		logger::log("[NetworkClient] Failed to create eNet localhost");
		return false;
	}

	
	return true;
}


bool noob::network_client::connect(const std::string& address, uint16_t port, uint32_t timeout_in_millis) noexcept(true)
{
	if (local_host == NULL)
	{
		connected = false;
		return false;
	}

	ENetAddress address_struct;
	enet_address_set_host(&address_struct, address.c_str());
	address_struct.port = port;

	peer = enet_host_connect(local_host, &address_struct, local_host->channelLimit, 0);

	if (peer == NULL)
	{
		connected = false;
		logger::log("[NetworkClient] No available peers tto initiate an eNet connection.");
		return false;
	}

	ENetEvent event;

	if (enet_host_service(local_host, &event, timeout_in_millis) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		connected = true;
		fmt::MemoryWriter ww;
		ww << "[NetworkClient] Connection to " << address << " success!";
		logger::log(ww.str());
		
		return true;
	}
	else
	{
		connected = false;
		enet_peer_reset(peer);
		fmt::MemoryWriter ww;
		ww << "[NetworkClient] Connection attempt to " << address << " timed out.";
		logger::log(ww.str());
		
		return false;
	}

}


void noob::network_client::send(const std::string& message, size_t channel, bool reliable) noexcept(true)
{
	if (connected)
	{
		ENetPacket* packet;
		if (reliable)
		{
			packet = enet_packet_create(message.c_str(), message.size() + 1, ENET_PACKET_FLAG_RELIABLE);
		}
		else
		{
			packet = enet_packet_create(message.c_str(), message.size() + 1, 0);
		}

		enet_peer_send(peer, std::min(channel, local_host->channelLimit), packet);
	}
}


bool noob::network_client::has_message() const noexcept(true)
{
	if (packets.empty())
	{
		return false;
	}
	return true;
}


std::string noob::network_client::get_message() noexcept(true)
{
	if (packets.empty())
	{
		return "";
	}

	std::string results = packets.front();
	packets.pop_front();
	return results;
}


void noob::network_client::tick() noexcept(true)
{
	if (connected)
	{
		int service_result = 1;
		ENetEvent event;

		while (service_result > 0)
		{
			service_result = enet_host_service(local_host, &event, 0);

			if (service_result > 0)
			{
				switch(event.type)
				{
					case ENET_EVENT_TYPE_CONNECT:
						{
							logger::log("[NetworkClient] Connection to server succeeded!");
							break;
						}
					case ENET_EVENT_TYPE_RECEIVE:
						{
							packets.push_back(std::string(event.packet->data));
							enet_packet_destroy(event.packet);
							break;
						}
					case ENET_EVENT_TYPE_DISCONNECT:
						{
							logger::log("[NetworkClient] Disconnected from server.");
						}
				}
			}
		}
	}
}
