#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <enet/enet.h>
#include <msgpack.hpp>
#include "defs.h"

static void send_signal(ENetHost *client, ENetPeer *peer, const char *host, pid_t pid, int signum)
{
    msgpack::type::tuple<pid_t, int> src(pid, signum);

    std::stringstream buffer;
    msgpack::pack(buffer, src);

    std::string data(buffer.str());

    ENetPacket *packet = enet_packet_create(data.c_str(), data.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
}

int main(int argc, char **argv)
{
    ENetAddress address;
    ENetEvent ev;
    ENetHost *client;
    ENetPeer *peer;
    const char *host;
    pid_t pid;
    int signum;

    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <host> <pid> <signum>" << std::endl;
        return EXIT_FAILURE;
    }

    host = argv[1];
    pid = atoi(argv[2]);
    signum = atoi(argv[3]);

    client = enet_host_create(NULL, 1, 2, 57600, 14400);
    if (client == NULL)
    {
        std::cerr << "Error during initializing client." << std::endl;
        return EXIT_FAILURE;
    }

    enet_address_set_host(&address, host);
    address.port = NETKILL_PORT;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL)
    {
        std::cerr << "No available peers for initiating an ENet connection." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Connecting to remote host..." << std::endl;
    if (enet_host_service(client, &ev, 5000) > 0 && ev.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connected to remote host, now send signal." << std::endl;
        send_signal(client, peer, host, pid, signum);
    }
    else
    {
        std::cerr << "Connection to remote host failed: timeout." << std::endl;
    }

    enet_peer_reset(peer);
    enet_host_destroy(client);
    return EXIT_SUCCESS;
}
