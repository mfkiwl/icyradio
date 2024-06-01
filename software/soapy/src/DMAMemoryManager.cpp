#include "DMAMemoryManager.hpp"

DMAMemoryManager::DMAMemoryManager(int fd, AXIPCIe *axi_pcie)
{
    this->fd = fd;
    this->axi_pcie = axi_pcie;
}
DMAMemoryManager::~DMAMemoryManager()
{
    this->deinit();
}

void DMAMemoryManager::init()
{

}
void DMAMemoryManager::deinit()
{

}