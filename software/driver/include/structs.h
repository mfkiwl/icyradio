#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>

typedef struct icyradio_dma_buffer_t icyradio_dma_buffer_t;
typedef struct icyradio_dev_t icyradio_dev_t;
struct icyradio_dma_buffer_t
{
    void *pVirt;
    dma_addr_t ulPhys;
    uint32_t ulSize;
    icyradio_dma_buffer_t *pNext;
};

struct icyradio_dev_t
{
    struct mutex sMutex;
    uint32_t ulDevID;
    uint64_t ullSerialNumber;
    struct pci_dev *pPCIDev;
    struct file *pFile;
    struct cdev sCharDev;
    icyradio_dma_buffer_t *pDMABuffers;
    int iNumIRQs;
    wait_queue_head_t sIRQWaitQueue;
    spinlock_t sIRQLock;
    uint64_t ullIRQCount;
    uint8_t ubIRQFlush;
};

#endif  // __STRUCTS_H__
