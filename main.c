#include "data_structures/mhash_table.h"
#include "listener/listener.h"


int main() {
    int server_fd = init_server();
    start_server(server_fd);
    return 0;
}