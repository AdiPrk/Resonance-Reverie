#include <PCH/pch.h>

#if DO_NETWORKING
#include "networking.h"

void sendPacket(ENetPeer* peer, PacketID packetID, const char* data) {
    char packetData[256];
    snprintf(packetData, sizeof(packetData), "%d %s", packetID, data);
    ENetPacket* packet = enet_packet_create(packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void sendPacketVec2(ENetPeer* peer, PacketID packetID, float x, float y) {
    char packetData[256];
    snprintf(packetData, sizeof(packetData), "%d %f %f", packetID, x, y);
    ENetPacket* packet = enet_packet_create(packetData, strlen(packetData) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void handlePacket(ENetPeer* peer, ENetPacket* packet, Game* game) {
    int packetID;
    char data[256];

    // Reading the packetID as an integer and the rest of the packet as string data
    int numArgs = sscanf_s((char*)packet->data, "%d %[^\t\n]", &packetID, data, (unsigned)_countof(data));

    if (numArgs < 2) { // Check if both arguments were successfully read
        fprintf(stderr, "Error: Could not parse the packet correctly.\n");
        return;
    }

    //printf("Packet id: %d\n", packetID);

    switch (packetID) {
    case MESSAGE_PACKET: {
        printf("Received message: %s\n", data);
        sendPacket(peer, MESSAGE_PACKET, "Hello, Server!");

        break;
    }
    case INIT_PLAYER_PACKET: {
        int id;

        sscanf_s((char*)packet->data, "%*d %i", &id);
        //printf("Init player %i at position: (%f, %f)\n", id, x, y);

        game->PushPlayer(id);

        break;
    }
    case REMOVE_PLAYER_PACKET: {
        float id;

        sscanf_s((char*)packet->data, "%*d %f", &id);
        //printf("Remove Player %i\n", (int)id);

        game->PopPlayer((int)id);

        break;
    }
    case POSITION_PACKET: {
        float id;
        float x, y;

        sscanf_s((char*)packet->data, "%*d %f %f %f", &id, &x, &y);
        //printf("Update player %i at position: (%f, %f)\n", (int)id, x, y);

        game->UpdatePlayer((int)id, glm::vec2(x, y));

        break;
    }
    }
}

void networkThread(ENetPeer* peer, Game* game) {
    ENetEvent event;
    while (1) {
        int eventStatus = enet_host_service(peer->host, &event, 10); // Wait up to 16 milliseconds for an event

        if (eventStatus > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                // Handle received packet
                handlePacket(peer, event.packet, game);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                // Handle disconnection
                printf("Disconnected from server.\n");
                enet_host_destroy(peer->host);
                enet_deinitialize();
                return;
            }
        }
        else if (eventStatus == 0) {
            // No event, sleep for a short duration to reduce CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

// Function definitions
void initializeENet() {
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet!\n");
        return;
    }
    atexit(enet_deinitialize);
}

ENetPeer* createAndConnectClient() {
    initializeENet();

    ENetHost* client;
    client = enet_host_create(NULL, 1, 1, 0, 0);

    if (client == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host!\n");
        return nullptr;
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, "localhost");
    address.port = 7777;
    // enet_address_set_host(&address, "adiprk-44119.portmap.io");
    // address.port = 44119;

    peer = enet_host_connect(client, &address, 1, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection!\n");
        return nullptr;
    }

    // Wait up to 5 seconds for the connection attempt to succeed
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to server succeeded.\n");
    }
    else {
        enet_peer_reset(peer);
        fprintf(stderr, "Connection to server failed.\n");
        return nullptr;
    }

    return peer;
}

#endif