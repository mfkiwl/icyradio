#ifndef __IOCTL_H__
#define __IOCTL_H__

#include <linux/ioctl.h>

typedef struct
{
    union
    {
        uint64_t ullPhysAddr; // For querying by physical address or to return the physical address
        uint64_t ullIndex;    // For querying by index
    };
    uint32_t ulSize;          // Size of the buffer
} icyradio_ioctl_dma_buffer_t;

#define ICYRADIO_IOCTL_DMA_ALLOC            _IOWR('B', 64, icyradio_ioctl_dma_buffer_t *)       // Allocate a DMA buffer
#define ICYRADIO_IOCTL_DMA_GET_COUNT        _IOR('B', 65, uint64_t *)                           // Get the number of allocated DMA buffers
#define ICYRADIO_IOCTL_DMA_QUERY_BY_ADDR    _IOR('B', 66, icyradio_ioctl_dma_buffer_t *)        // Query a DMA buffer by physical address
#define ICYRADIO_IOCTL_DMA_QUERY_BY_INDEX   _IOR('B', 67, icyradio_ioctl_dma_buffer_t *)        // Query a DMA buffer by index
#define ICYRADIO_IOCTL_DMA_FREE             _IOW('B', 68, icyradio_ioctl_dma_buffer_t *)        // Free a DMA buffer
#define ICYRADIO_IOCTL_DMA_FREE_ALL         _IO('B', 69)                                        // Free all DMA buffers

#define ICYRADIO_IOCTL_IRQ_QUERY            _IOR('B', 80, uint8_t *)                            // Query the number of allocated IRQ vectors
#define ICYRADIO_IOCTL_IRQ_FLUSH            _IO('B', 81)                                        // Flush the IRQ count (resets to zero and does not rise)
#define ICYRADIO_IOCTL_IRQ_NOFLUSH          _IO('B', 82)                                        // Do not flush the IRQ count (returns to normal operation)

#define ICYRADIO_IOCTL_SERIAL_QUERY         _IOR('B', 96, uint64_t *)                           // Query the serial number of the device

#endif // __IOCTL_H__