#include "../include/server.h"

static Config *cnfg;

int main(void) {
    cnfg = init_config();
    init_server(cnfg);
    return 0;
}