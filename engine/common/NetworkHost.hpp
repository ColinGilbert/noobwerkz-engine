#pragma once


#include <algorithm>
#include <vector>

#include <enet/enet.h>

#include <rdestl/hash_map.h>
#include <rdestl/sorted_vector.h>

#include "Logger.hpp"

namespace noob
{
	// OnReceive must be a class with overloaded operator()(const std::string &)
	template <typename OnReceive> class network_host
	{
		public:
			network_host() noexcept(true) : local_host(NULL) {}

			~network_host() noexcept(true)
			{
				if (local_host != NULL)
				{
					enet_host_destroy(local_host);
				}
			}


			bool init(uint32_t host_address, uint16_t port, size_t num_clients, size_t num_channels, uint32_t incoming_bandwidth = 0, uint32_t outgoing_bandwidth = 0) noexcept(true)
			{
				if (!enet_initialize())
				{
					logger::log("[NetworkHost] Failed to init eNet");
					return false;
				}

				ENetAddress address_struct;
				address_struct.host = host_address;
				address_struct.port = port;

				local_host = enet_host_create(&address_struct, num_clients, num_channels, incoming_bandwidth, outgoing_bandwidth);

				if (local_host == NULL)
				{
					logger::log("[NetworkHost] Failed to create eNet localhost");
					return false;
				}

				return true;
			}


			void send(const std::vector<uint32_t>& hosts, const std::string& message, size_t channel, bool reliable) noexcept(true)
			{
				ENetPacket* packet;
				packet = enet_packet_create(message.c_str(), message.size() + 1, (reliable ? ENET_PACKET_FLAG_RELIABLE : 0));

				if (hosts.size() == 0)
				{
					enet_host_broadcast(local_host, channel, packet);
				}

				for (auto h : hosts)
				{
					auto search = clients.find(h);
					if (search != clients.end())
					{
						ENetPeer* p;
						p = search->second;
						if (!p->state == ENET_PEER_STATE_ZOMBIE)
						{
							enet_peer_send(p, std::min(channel, local_host->channelLimit), packet);
						}
					}
				}
			}


			void tick() noexcept(true)
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
									fmt::MemoryWriter ww;
									ww << "[NetworkHost] " << event.peer->address.host << " just connected!";
									logger::log(ww.str());
									break;
								}
							case ENET_EVENT_TYPE_RECEIVE:
								{
									recv_fun(std::string(event.packet->data));
									enet_packet_destroy(event.packet);
									break;
								}
							case ENET_EVENT_TYPE_DISCONNECT:
								{
									fmt::MemoryWriter ww;
									uint32_t disconnected_host = event.peer->address.host;
									
									auto search = clients.find(disconnected_host);
									if (search != clients.end())
									{
										clients.erase(search);
									}

									ww << "[NetworkHost] Client " << disconnected_host << " disconnected.";
									logger::log(ww.str());

									break;
								}
						}
					}
				}
			}


			void disconnect(const std::vector<uint32_t>& peers_to_zap) noexcept(true)
			{
				for (auto z : peers_to_zap)
				{
					auto search = clients.find(z);
					if (search != clients.end())
					{
						enet_peer_reset(search->second);
						clients.erase(search);
					}
				}
			}


			void cull_zombies() noexcept(true)
			{
				std::vector<uint32_t> zap_list;
				for (auto c : clients)
				{
					ENetPeer* p = c;
					if (p->state == ENET_PEER_STATE_ZOMBIE)
					{
						zap_list.push_back(p->host->address.host);
					}
				}

				disconnect(zap_list);
			}


		protected:
			ENetHost* local_host;
			OnReceive recv_fun;
			rde::hash_map<uint32_t, ENetPeer*> clients;
	};
}
