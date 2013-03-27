#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <string>
#include <csignal>
#include <enet/enet.h>
#include <msgpack.hpp>
#include "defs.h"

bool running = true;

static void sigint_handler(int signum)
{
    std::cout << "Stopping server..." << std::endl;
    running = false;
}

static void handle_data(ENetEvent *ev)
{
    std::string str((char *) ev->packet->data, ev->packet->dataLength);

    msgpack::zone mempool;
    msgpack::object packet;
    msgpack::unpack(str.data(), str.size(), NULL, &mempool, &packet);

    msgpack::type::tuple<pid_t, int> dst;
    packet.convert(&dst);

    pid_t pid = dst.get<0>();
    int signum = dst.get<1>();

    std::cout << "Sending signal: " << signum << " to PID " << pid << std::endl;
    kill(pid, signum);
}

static void run_server(ENetHost *server)
{
    ENetEvent ev;

    while (running)
    {
        if (enet_host_service(server, &ev, 1000) > 0)
        {
            switch (ev.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    handle_data(&ev);
                    break;

                default:
                    break;
            }
        }
    }
}
    // msgpack::type::tuple<int, bool, std::string> dst;
    // deserialized.convert(&dst);
int main(int argc, char **argv)
{
    ENetAddress address;
    ENetHost *server;

    if (enet_initialize() != 0)
    {
        std::cerr << "Error during initializing ENet." << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    address.host = ENET_HOST_ANY;
    address.port = NETKILL_PORT;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == NULL)
    {
        std::cerr << "Error during creating ENet host." << std::endl;
        return EXIT_FAILURE;
    }

    signal(SIGINT, &sigint_handler);

    std::cout << "Server started at port " << address.port << std::endl;
    run_server(server);

    enet_host_destroy(server);
    return EXIT_SUCCESS;
}
