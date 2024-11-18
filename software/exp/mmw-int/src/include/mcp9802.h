#ifndef __MCP9802_H__
#define __MCP9802_H__

#include <em_device.h>
#include "i2c.h"
#include "atomic.h"
#include "systick.h"

#define MCP9802_I2C_ADDR 0x48 // 0x48 thru 0x4F

// Commands
#define MCP9802_REG_TAMB     0x00
#define MCP9802_REG_CFG      0x01
#define MCP9802_REG_THYST    0x02
#define MCP9802_REG_TLIMIT   0x03

// MCP9802_REG_CFG
#define MCP9802_REG_CFG_ONE_SHOT    BIT(7)
#define MCP9802_REG_CFG_RES_9BIT    0x00
#define MCP9802_REG_CFG_RES_10BIT   0x20
#define MCP9802_REG_CFG_RES_11BIT   0x40
#define MCP9802_REG_CFG_RES_12BIT   0x60
#define MCP9802_REG_CFG_FAULT_Q1    0x00
#define MCP9802_REG_CFG_FAULT_Q2    0x08
#define MCP9802_REG_CFG_FAULT_Q3    0x10
#define MCP9802_REG_CFG_FAULT_Q4    0x18
#define MCP9802_REG_CFG_ALERT_POL   BIT(2)
#define MCP9802_REG_CFG_COMP_INT    BIT(1)
#define MCP9802_REG_CFG_SHUTDOWN    BIT(0)


uint8_t mcp9802_init(uint8_t ubAddress);

void mcp9802_set_config(uint8_t ubAddress, uint8_t ubConfig);
uint8_t mcp9802_get_config(uint8_t ubAddress);

void mcp9802_do_oneshot(uint8_t ubAddress);

uint16_t mcp9802_get_ambient_temp_reg(uint8_t ubAddress);
float mcp9802_get_ambient_temp(uint8_t ubAddress);
void mcp9802_set_hyst_temp_reg(uint8_t ubAddress, uint16_t usTemp);
void mcp9802_set_hyst_temp(uint8_t ubAddress, float fTemp);
uint16_t mcp9802_get_hyst_temp_reg(uint8_t ubAddress);
float mcp9802_get_hyst_temp(uint8_t ubAddress);
void mcp9802_set_limit_temp_reg(uint8_t ubAddress, uint16_t usTemp);
void mcp9802_set_limit_temp(uint8_t ubAddress, float fTemp);
uint16_t mcp9802_get_limit_temp_reg(uint8_t ubAddress);
float mcp9802_get_limit_temp(uint8_t ubAddress);

#endif  // __MCP9802_H__
