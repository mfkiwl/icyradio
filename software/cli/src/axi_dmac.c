#include "axi_dmac.h"

/*******************************************************************************
 * @brief ISR for dev to mem DMA transfer. It computes the next transfer params,
 *			if any, and sets the transfer structure fields accordingly.
 *
 * @param instance - the instance that triggered the ISR.
 *
 * @return None.
*******************************************************************************/
void axi_dmac_dev_to_mem_isr(void* instance)
{
    struct axi_dmac* dmac = (struct axi_dmac*)instance;
    uint32_t burst_size;
    uint32_t reg_val = 0;

    /* Get interrupt sources and clear interrupts. */
    axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
    axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

    if(reg_val & AXI_DMAC_IRQ_SOT)
    {
        if(dmac->remaining_size)
        {
            /* See if remaining size is bigger than max transfer size and
             * set burst size. */
            if(dmac->remaining_size > dmac->max_length)
                burst_size = dmac->max_length;
            else
                burst_size = dmac->remaining_size - 1;

            /* The current transfer was started; set up the new one. */
            axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, dmac->next_dest_addr);
            axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
            axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, burst_size);
            axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

            /* Compute size of the next transfer. */
            dmac->remaining_size = dmac->remaining_size - (burst_size + 1);
            /* Address must advance with +1 since the DMAC writes X_LENGTH+1. */
            dmac->next_dest_addr = dmac->next_dest_addr + (burst_size + 1);

            /* Trigger the next transfer. */
            axi_dmac_write(dmac, AXI_DMAC_REG_TRANSFER_SUBMIT, AXI_DMAC_TRANSFER_SUBMIT);
        }
    }

    if(reg_val & AXI_DMAC_IRQ_EOT)
    {
        if(!dmac->remaining_size)
        {
            dmac->transfer.transfer_done = true;
            dmac->next_dest_addr = 0;
        }
    }
}

/*******************************************************************************
 * @brief ISR for mem DMA to dev transfer. It computes the next transfer params,
 *			if any, and sets the transfer structure fields accordingly.
 *
 * @param instance - the instance that triggered the ISR.
 *
 * @return None.
*******************************************************************************/
void axi_dmac_mem_to_dev_isr(void* instance)
{
    struct axi_dmac* dmac = (struct axi_dmac*)instance;
    uint32_t burst_size;
    uint32_t reg_val = 0;

    /* Get interrupt sources and clear interrupts. */
    axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
    axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

    if(reg_val & AXI_DMAC_IRQ_SOT)
    {
        if((dmac->transfer.cyclic == CYCLIC) && (dmac->next_src_addr >= (dmac->init_addr + dmac->transfer.size - 1)))
        {
            dmac->remaining_size = dmac->transfer.size;
            dmac->next_src_addr = dmac->init_addr;
        }

        if(dmac->remaining_size)
        {
            /** See if remaining size is bigger than max transfer size and
             * set burst size. */
            if(dmac->remaining_size > dmac->max_length)
                burst_size = dmac->max_length;
            else
                burst_size = dmac->remaining_size - 1;

            /* The current transfer was started; set up the new one. */
            axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, dmac->next_src_addr);
            axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
            axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, burst_size);
            axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

            /* Compute parameters for the next transfer. */
            dmac->remaining_size = dmac->remaining_size - (burst_size + 1);
            /* Address must advance with +1 since the DMAC writes X_LENGTH+1. */
            dmac->next_src_addr = dmac->next_src_addr + (burst_size + 1);

            /* Trigger the current transfer */
            axi_dmac_write(dmac, AXI_DMAC_REG_TRANSFER_SUBMIT, AXI_DMAC_TRANSFER_SUBMIT);
        }
    }

    if(reg_val & AXI_DMAC_IRQ_EOT)
    {
        if((!dmac->remaining_size) && (dmac->transfer.cyclic != CYCLIC))
        {
            dmac->transfer.transfer_done = true;
            dmac->next_src_addr = 0;
        }
    }
}

/*******************************************************************************
 * @brief ISR for mem DMA to mem DMA transfer. It computes the next transfer
 *			params, if any, and sets the transfer structure fields accordingly.
 *
 * @param instance - the instance that triggered the ISR.
 *
 * @return None.
*******************************************************************************/
void axi_dmac_mem_to_mem_isr(void* instance)
{
    struct axi_dmac* dmac = (struct axi_dmac*)instance;
    uint32_t burst_size;
    uint32_t reg_val = 0;

    /* Get interrupt sources and clear interrupts. */
    axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
    axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_PENDING, reg_val);

    if(reg_val & AXI_DMAC_IRQ_SOT)
    {
        if(dmac->remaining_size)
        {
            /** See if remaining size is bigger than max transfer size and
             * set burst size. */
            if(dmac->remaining_size > dmac->max_length)
                burst_size = dmac->max_length;
            else
                burst_size = dmac->remaining_size - 1;

            /* The current transfer was started; set up the new one. */
            axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, dmac->next_src_addr);
            axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
            axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, dmac->next_dest_addr);
            axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
            axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, burst_size);
            axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);

            /* Compute parameters for the next transfer. */
            dmac->remaining_size = dmac->remaining_size - (burst_size + 1);
            /* Address must advance with +1 since the DMAC writes X_LENGTH+1. */
            dmac->next_src_addr = dmac->next_src_addr + (burst_size + 1);
            dmac->next_dest_addr = dmac->next_dest_addr + (burst_size + 1);

            /* If transfer finished advance src address so that transfer_done can be set*/
            if(!dmac->remaining_size)
                dmac->next_src_addr = dmac->next_src_addr + (burst_size + 1);

            /* Trigger the current transfer */
            axi_dmac_write(dmac, AXI_DMAC_REG_TRANSFER_SUBMIT, AXI_DMAC_TRANSFER_SUBMIT);
        }
    }

    if(reg_val & AXI_DMAC_IRQ_EOT)
    {
        if(!dmac->remaining_size)
        {
            if(dmac->next_src_addr > (dmac->init_addr + dmac->transfer.size))
            {
                dmac->transfer.transfer_done = true;
                dmac->next_src_addr = 0;
                dmac->next_dest_addr = 0;
            }
        }
    }
}

/*******************************************************************************
 * @brief Wrapper for AXI IO through UIO/devmem read function.
 *
 * @param dmac - DMAC istance containing UIO index (/dev/uioX)/base address.
 * @param reg_addr - Address to read from.
 * @param reg_data - Location where read data will be stored.
 *
 * @return 0 - success.
*******************************************************************************/
int32_t axi_dmac_read(struct axi_dmac* dmac, uint32_t reg_addr, uint32_t* reg_data)
{
    if(dmac == NULL)
        return -1;

    if(reg_data == NULL)
        return -1;

    *reg_data = (uint32_t)*(volatile uint32_t *)((uintptr_t)dmac->base + reg_addr);

    return 0;
}

/*******************************************************************************
 * @brief Wrapper for AXI IO through UIO/devmem wrrite function.
 *
 * @param dmac - DMAC istance containing UIO index (/dev/uioX)/base address.
 * @param reg_addr - Address to write to.
 * @param reg_data - Data to be written.
 *
 * @return 0 - success.
*******************************************************************************/
int32_t axi_dmac_write(struct axi_dmac* dmac, uint32_t reg_addr, uint32_t reg_data)
{
    if(dmac == NULL)
        return -1;

    *(volatile uint32_t *)((uintptr_t)dmac->base + reg_addr) = reg_data;

    return 0;
}

/*******************************************************************************
 * @brief Get transfer status (value of dmac->transfer.transfer_done flag).
 *
 * @note This function should not be used if DMA IRQ is not used.
 *
 * @param dmac - DMAC istance.
 * @param rdy - Transfer status (0 - not ready, 1 - ready).
 *
 * @return 0 - success.
*******************************************************************************/
int32_t axi_dmac_is_transfer_ready(struct axi_dmac* dmac, bool* rdy)
{
    *rdy = dmac->transfer.transfer_done;

    return 0;
}

/*******************************************************************************
 * @brief Get DMAC capabilities.
 *
 * @param dmac - DMAC istance.
 *
 * @return 0 for success, -1 in case of failure.
*******************************************************************************/
static int32_t axi_dmac_detect_caps(struct axi_dmac* dmac)
{
    uint32_t reg_val, initial_reg_val = 0;
    uint32_t src_mem_mapped = 0;
    uint32_t dest_mem_mapped = 0;

    dmac->max_length = -1;
    dmac->direction = INVALID_DIR;

    /* Check if HW cyclic possible */
    axi_dmac_read(dmac, AXI_DMAC_REG_FLAGS, &initial_reg_val);
    axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, DMA_CYCLIC);
    axi_dmac_read(dmac, AXI_DMAC_REG_FLAGS, &reg_val);

    if(reg_val == DMA_CYCLIC)
        dmac->hw_cyclic = true;

    /* Restore initial value for AXI_DMAC_REG_FLAGS register */
    axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, initial_reg_val);

    /* Get maximum burst size and set value. */
    axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, dmac->max_length);
    axi_dmac_read(dmac, AXI_DMAC_REG_X_LENGTH, &dmac->max_length);

    /* Get transfer direction and set value. */
    axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, 0xFFFFFFFF);
    axi_dmac_read(dmac, AXI_DMAC_REG_DEST_ADDRESS, &dest_mem_mapped);
    axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, 0xFFFFFFFF);
    axi_dmac_read(dmac, AXI_DMAC_REG_SRC_ADDRESS, &src_mem_mapped);

    if(dest_mem_mapped && !src_mem_mapped)
    {
        dmac->direction = DMA_DEV_TO_MEM;
    }
    else if(!dest_mem_mapped && src_mem_mapped)
    {
        dmac->direction = DMA_MEM_TO_DEV;
    }
    else if(dest_mem_mapped && src_mem_mapped)
    {
        dmac->direction = DMA_MEM_TO_MEM;
    }
    else
    {
        DBGPRINTLN_CTX("Destination and source memory-mapped interfaces not supported.");

        return -1;
    }

    return 0;
}

/*******************************************************************************
 * @brief Get transfer status (value of dmac->transfer.transfer_done flag).
 *
 * @param dmac_core - DMAC istance.
 * @param init - Structure containing initializing data.
 *
 * @return 0 for success, -1 in case of failure.
*******************************************************************************/
int32_t axi_dmac_init(struct axi_dmac** dmac_core, const struct axi_dmac_init* init)
{
    struct axi_dmac* dmac;

    dmac = (struct axi_dmac*)calloc(1, sizeof(*dmac));

    if(!dmac)
        return -1;

    dmac->name = init->name;
    dmac->base = init->base;
    dmac->irq_option = init->irq_option;

    *dmac_core = dmac;

    int32_t status = axi_dmac_detect_caps(*dmac_core);

    if(status < 0)
        return -1;

    return 0;
}

/*******************************************************************************
 * @brief Remove the DMAC instance.
 *
 * @param dmac - DMAC istance.
 *
 * @return 0 for success, -1 in case of failure.
*******************************************************************************/
int32_t axi_dmac_remove(struct axi_dmac* dmac)
{
    if(!dmac)
        return -1;

    free(dmac);

    return 0;
}

/*******************************************************************************
 * @brief Start a DMA transfer.
 *
 * @param dmac - DMAC istance.
 * @param dma_transfer - Structure containing transfer details.
 *
 * @return 0 for success, -1 in case of failure.
*******************************************************************************/
int32_t axi_dmac_transfer_start(struct axi_dmac* dmac, struct axi_dma_transfer* dma_transfer)
{
    uint32_t reg_val, burst_size;

    if(dma_transfer->size == 0)
        return 0; /* Nothing to do. */

    /* Set current transfer parameters. */
    dmac->transfer.size = dma_transfer->size;
    dmac->transfer.cyclic = dma_transfer->cyclic;
    dmac->transfer.dest_addr = dma_transfer->dest_addr;
    dmac->transfer.src_addr = dma_transfer->src_addr;

    dmac->remaining_size = dma_transfer->size;
    dmac->next_dest_addr = dma_transfer->dest_addr;
    dmac->next_src_addr = dma_transfer->src_addr;

    /* If HW cyclic transfer selected and not available, show error */
    /* HW cyclic transfer available only for MEM to DEV transfers. */
    if((!dmac->hw_cyclic) && (dma_transfer->cyclic == CYCLIC))
    {
        DBGPRINTLN_CTX("Transfer mode not supported!");

        return -1;
    }

    /* Cyclic transfers not possible for DEV_TO_MEM and MEM_TO_MEM transmissions. */
    if((dmac->direction == DMA_DEV_TO_MEM) || (dmac->direction == DMA_MEM_TO_MEM))
    {
        if(dma_transfer->cyclic == CYCLIC)
        {
            DBGPRINTLN_CTX("Transfer mode not supported!");

            return -1;
        }
    }

    /* Cyclic transfers set to HW or SW depending on size for MEM to DEV. */
    if((dmac->direction == DMA_MEM_TO_DEV) && (dmac->transfer.cyclic == CYCLIC))
    {
        if((dmac->remaining_size - 1) <= dmac->max_length)
        {
            if(dmac->hw_cyclic)
            {
                axi_dmac_read(dmac, AXI_DMAC_REG_FLAGS, &reg_val);
                reg_val = reg_val | DMA_CYCLIC;
                axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, reg_val);
            }
        }
        else
        {
            axi_dmac_read(dmac, AXI_DMAC_REG_FLAGS, &reg_val);
            reg_val = reg_val & ~DMA_CYCLIC;
            axi_dmac_write(dmac, AXI_DMAC_REG_FLAGS, reg_val);
        }
    }

    /* Enable DMA if not already enabled. */
    axi_dmac_read(dmac, AXI_DMAC_REG_CTRL, &reg_val);

    if(!(reg_val & AXI_DMAC_CTRL_ENABLE))
    {
        axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, 0x0);
        axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);
        axi_dmac_write(dmac, AXI_DMAC_REG_IRQ_MASK, 0x0);
    }

    axi_dmac_read(dmac, AXI_DMAC_REG_TRANSFER_SUBMIT, &reg_val);
    /* If we don't have a start of transfer then start compute
     * values and trigger next transfer. */
    if(!(reg_val & AXI_DMAC_QUEUE_FULL))
    {
        switch(dmac->direction)
        {
            case DMA_DEV_TO_MEM:
                dmac->init_addr = dmac->next_dest_addr;
                axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, dmac->next_dest_addr);
                axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
            break;
            case DMA_MEM_TO_DEV:
                dmac->init_addr = dmac->next_src_addr;
                axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, dmac->next_src_addr);
                axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
            break;
            case DMA_MEM_TO_MEM:
                dmac->init_addr = dmac->next_src_addr;
                axi_dmac_write(dmac, AXI_DMAC_REG_DEST_ADDRESS, dmac->next_dest_addr);
                axi_dmac_write(dmac, AXI_DMAC_REG_DEST_STRIDE, 0x0);
                axi_dmac_write(dmac, AXI_DMAC_REG_SRC_ADDRESS, dmac->next_src_addr);
                axi_dmac_write(dmac, AXI_DMAC_REG_SRC_STRIDE, 0x0);
            break;
            default:
                return -1; /* Other directions are not supported yet. */
            break;
        }

        /* Compute the burst size. */
        if(dmac->remaining_size > dmac->max_length)
            burst_size = dmac->max_length;
        else
            burst_size = dmac->remaining_size - 1;

        /* Compute next address (src or dest, or both, depending on type of transfer). */
        switch(dmac->direction)
        {
            case DMA_DEV_TO_MEM:
                dmac->next_dest_addr = dmac->next_dest_addr + (burst_size + 1);
            break;
            case DMA_MEM_TO_DEV:
                dmac->next_src_addr = dmac->next_src_addr + (burst_size + 1);
            break;
            case DMA_MEM_TO_MEM:
                dmac->next_dest_addr = dmac->next_dest_addr + (burst_size + 1);
                dmac->next_src_addr = dmac->next_src_addr + (burst_size + 1);
            break;
            default:
                return -1;
            break;
        }

        /* Compute remaining size. */
        dmac->remaining_size = dmac->remaining_size - (burst_size + 1);

        /* Specify the length of the transfer and trigger transfer. */
        axi_dmac_write(dmac, AXI_DMAC_REG_X_LENGTH, burst_size);
        axi_dmac_write(dmac, AXI_DMAC_REG_Y_LENGTH, 0x0);
        axi_dmac_write(dmac, AXI_DMAC_REG_TRANSFER_SUBMIT, AXI_DMAC_TRANSFER_SUBMIT);
    }
    else
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * @brief Wait for DMA transfer to be completed.
 *
 * @param dmac - DMAC istance.
 * @param timeout_ms - Number of ms to wait for completion of transfer.
 *
 * @return 0 for success, -1 in case trasnfer not completed in specified time.
*******************************************************************************/
int32_t axi_dmac_transfer_wait_completion(struct axi_dmac* dmac, uint32_t timeout_ms)
{
    uint32_t timeout = 0;
    uint32_t reg_val = 0;

    if(dmac->irq_option == IRQ_ENABLED)
    {
        while(!dmac->transfer.transfer_done)
        {
            timeout++;

            usleep(1000);

            if(timeout == timeout_ms)
            {
                DBGPRINTLN_CTX("Error transferring data using DMA.");

                return -1;
            }
        }
    }
    else if(dmac->irq_option == IRQ_DISABLED)
    {
        axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);

        while(reg_val != (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT))
        {
            timeout++;

            usleep(1000);

            if(timeout == timeout_ms)
            {
                DBGPRINTLN_CTX("Error transferring data using DMA.");

                return -1;
            }

            axi_dmac_read(dmac, AXI_DMAC_REG_IRQ_PENDING, &reg_val);
        }
    }

    return 0;
}

/*******************************************************************************
 * @brief Stop a DMA transfer.
 *
 * @param dmac - DMAC istance.
 *
 * @return None
*******************************************************************************/
void axi_dmac_transfer_stop(struct axi_dmac* dmac)
{
    axi_dmac_write(dmac, AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_DISABLE);
}