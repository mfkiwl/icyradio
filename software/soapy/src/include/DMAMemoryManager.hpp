#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include "ioctl.hpp"
#include "AXIPCIe.hpp"
#include "MappedRegion.hpp"

class DMAMemoryManager
{
public:
    DMAMemoryManager(int fd, AXIPCIe *axi_pcie);
    ~DMAMemoryManager();

    void init();
    void deinit();

private:
    int fd;
    AXIPCIe *axi_pcie;

    std::mutex mutex;
};