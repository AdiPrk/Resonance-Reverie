/* ./server/main.c */

#include <stdio.h>
#include <enet/enet.h>
#include <vector>
#include <unordered_map>

float playerIDCounter = 0;

// Peer linked with an ID
std::unordered_map<ENetPeer*, int> Players;
int idCounter = 0;

void PushPlayer(ENetPeer* peer)
{
	Players[peer] = idCounter++;
}

void PopPlayer(ENetPeer* peer)
{
	auto it = Players.find(peer);
	if (it != Players.end()) {
		Players.erase(it);
	}
}

// Define packet IDs
enum PacketID {
	MESSAGE_PACKET = 1,
	INIT_PLAYER_PACKET = 2,
	REMOVE_PLAYER_PACKET = 3,
	POSITION_PACKET = 4
};

void sendPacket(ENetPeer* peer, PacketID packetID, const char* data) {
	char packetData[256];
	snprintf(packetData, sizeof(packetData), "%d %s", packetID, data);
	ENetPacket* packet = enet_packet_create(packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void sendPacketInt(ENetPeer* peer, PacketID packetID, int data) {
	char packetData[256];
	snprintf(packetData, sizeof(packetData), "%d %i", packetID, data);
	ENetPacket* packet = enet_packet_create(packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void sendPacketVec2(ENetPeer* peer, PacketID packetID, float x, float y) {
	char packetData[256];
	snprintf(packetData, sizeof(packetData), "%d %f %f", packetID, x, y);
	ENetPacket* packet = enet_packet_create(packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void sendPacketVec3(ENetPeer* peer, PacketID packetID, float x, float y, float z) {
	char packetData[256];
	snprintf(packetData, sizeof(packetData), "%d %f %f %f", packetID, x, y, z);
	ENetPacket* packet = enet_packet_create(packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void broadcastPacket(ENetHost* host, ENetPeer* sender, PacketID packetID, const char* data) {
	ENetPeer* currentPeer;
	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer == sender) {
			continue;
		}
		sendPacket(currentPeer, packetID, data);
	}
}

void broadcastPacketInt(ENetHost* host, ENetPeer* sender, PacketID packetID, int x) {
	ENetPeer* currentPeer;
	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer == sender) {
			continue;
		}
		sendPacketInt(currentPeer, packetID, x);
	}
}

void broadcastPacketVec2(ENetHost* host, ENetPeer* sender, PacketID packetID, float x, float y) {
	ENetPeer* currentPeer;
	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer == sender) {
			continue;
		}
		sendPacketVec2(currentPeer, packetID, x, y);
	}
}

void broadcastPacketVec3(ENetHost* host, ENetPeer* sender, PacketID packetID, float x, float y, float z) {
	ENetPeer* currentPeer;
	for (currentPeer = host->peers; currentPeer < &host->peers[host->peerCount]; ++currentPeer) {
		if (currentPeer->state != ENET_PEER_STATE_CONNECTED || currentPeer == sender) {
			continue;
		}
		sendPacketVec3(currentPeer, packetID, x, y, z);
	}
}

// Handle specific packets recieved from client
void handlePacket(ENetPeer* peer, ENetPacket* packet) {
	int packetID;
	char data[256];

	// Reading the packetID as an integer and the rest of the packet as string data
	int numArgs = sscanf_s((char*)packet->data, "%d %[^\t\n]", &packetID, data, (unsigned)_countof(data));

	if (numArgs < 2) { // Check if both arguments were successfully read
		fprintf(stderr, "Error: Could not parse the packet correctly.\n");
		return;
	}

	switch (packetID) {
	case MESSAGE_PACKET: {
		printf("Received message: %s\n", data);

		break;
	}
	case INIT_PLAYER_PACKET: {
		printf("Init Player %i\n", Players[peer]);

		broadcastPacketInt(peer->host, peer, INIT_PLAYER_PACKET, Players[peer]);

		break;
	}
	case POSITION_PACKET: {
		float x, y;

		sscanf_s((char*)packet->data, "%*d %f %f", &x, &y);
		//printf("Received position: (%f, %f)\n", x, y);
		broadcastPacketVec3(peer->host, peer, POSITION_PACKET, (float)Players[peer], x, y);

		break;
	}
	}
}

int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	ENetEvent event;
	ENetAddress address;
	ENetHost* server;

	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY; // This allows
	/* Bind the server to port 7777. */
	address.port = 7777;

	server = enet_host_create(&address	/* the address to bind the server host to */,
		32	/* allow up to 32 clients and/or outgoing connections */,
		1	/* allow up to 1 channel to be used, 0. */,
		0	/* assume any amount of incoming bandwidth */,
		0	/* assume any amount of outgoing bandwidth */);

	if (server == NULL)
	{
		printf("An error occurred while trying to create an ENet server host.");
		return 1;
	}

	printf("Server Started at port 7777!\n");

	// Packets recieved from client
	while (1) {
		while (enet_host_service(server, &event, 10) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT: {
				printf("Client %i connected.\n", idCounter);
				for (auto it : Players) {
					sendPacketInt(event.peer, INIT_PLAYER_PACKET, it.second);
				}

				PushPlayer(event.peer);
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE: {
				//printf("Received a packet from %i.\n", Players[event.peer]);
				handlePacket(event.peer, event.packet);
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT: {
				printf("Client %i disconnected.\n", Players[event.peer]);
				broadcastPacketInt(event.peer->host, event.peer, REMOVE_PLAYER_PACKET, Players[event.peer]);
				PopPlayer(event.peer);
				break;
			}
			}
		}
	}

	enet_host_destroy(server);
	enet_deinitialize();

	return 0;
}