#pragma once

#if DO_NETWORKING

#include <enet/enet.h>
#include <Source/Game/game.h>

enum PacketID {
    MESSAGE_PACKET = 1,
    INIT_PLAYER_PACKET = 2,
    REMOVE_PLAYER_PACKET = 3,
    POSITION_PACKET = 4
};

void sendPacket(ENetPeer* peer, PacketID packetID, const char* data);
void sendPacketVec2(ENetPeer* peer, PacketID packetID, float x, float y);
void handlePacket(ENetPeer* peer, ENetPacket* packet, Game* game);
void networkThread(ENetPeer* peer, Game* game);
void initializeENet();
ENetPeer* createAndConnectClient();

#endif