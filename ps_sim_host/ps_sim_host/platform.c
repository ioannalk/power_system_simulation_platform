#include "xparameters.h"
#include "xil_cache.h"

#include "platform_config.h"

void
enable_caches()
{
//  Xil_ICacheEnable();
//  Xil_DCacheEnable();
}

void
disable_caches()
{
//  Xil_DCacheDisable();
//  Xil_ICacheDisable();
}

void
init_platform()
{
    enable_caches();
//  init_uart();
}

void
cleanup_platform()
{
    disable_caches();
}
