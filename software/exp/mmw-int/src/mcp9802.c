#include "mcp9802.h"

static uint8_t mcp9802_read_register(uint8_t ubAddress, uint8_t ubRegister)
{
    uint8_t ubValue;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        i2c2_write_byte(MCP9802_I2C_ADDR | (ubAddress & 0x07), ubRegister, I2C_RESTART);
        ubValue = i2c2_read_byte(MCP9802_I2C_ADDR | (ubAddress & 0x07), I2C_STOP);
    }

    return ubValue;
}
static uint16_t mcp9802_read_register16(uint8_t ubAddress, uint8_t ubRegister)
{
    uint16_t usValue;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        i2c2_write_byte(MCP9802_I2C_ADDR | (ubAddress & 0x07), ubRegister, I2C_RESTART);
        i2c2_read(MCP9802_I2C_ADDR | (ubAddress & 0x07), (uint8_t *)&usValue, 2, I2C_STOP);
    }

    return (usValue >> 8) | (usValue << 8);
}
static void mcp9802_write_register(uint8_t ubAddress, uint8_t ubRegister, uint8_t ubValue)
{
    uint8_t pubBuffer[2];

    pubBuffer[0] = ubRegister;
    pubBuffer[1] = ubValue;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        i2c2_write(MCP9802_I2C_ADDR | (ubAddress & 0x07), pubBuffer, 2, I2C_STOP);
    }
}
static void mcp9802_write_register16(uint8_t ubAddress, uint8_t ubRegister, uint16_t usValue)
{
    uint8_t pubBuffer[3];

    pubBuffer[0] = ubRegister;
    pubBuffer[1] = usValue >> 8;
    pubBuffer[2] = usValue >> 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        i2c2_write(MCP9802_I2C_ADDR | (ubAddress & 0x07), pubBuffer, 3, I2C_STOP);
    }
}
static void mcp9802_rmw_register(uint8_t ubAddress, uint8_t ubRegister, uint8_t ubMask, uint8_t ubValue)
{
    mcp9802_write_register(ubAddress, ubRegister, (mcp9802_read_register(ubAddress, ubRegister) & ubMask) | ubValue);
}
static void mcp9802_rmw_register16(uint8_t ubAddress, uint8_t ubRegister, uint16_t usMask, uint16_t usValue)
{
    mcp9802_write_register16(ubAddress, ubRegister, (mcp9802_read_register16(ubAddress, ubRegister) & usMask) | usValue);
}

uint8_t mcp9802_init(uint8_t ubAddress)
{
    if(!i2c2_write(MCP9802_I2C_ADDR | (ubAddress & 0x07), NULL, 0, I2C_STOP))
        return 0;

    mcp9802_set_config(ubAddress, MCP9802_REG_CFG_RES_12BIT | MCP9802_REG_CFG_FAULT_Q1);

    return 1;
}

void mcp9802_set_config(uint8_t ubAddress, uint8_t ubConfig)
{
    mcp9802_write_register(ubAddress, MCP9802_REG_CFG, ubConfig);
}
uint8_t mcp9802_get_config(uint8_t ubAddress)
{
    return mcp9802_read_register(ubAddress, MCP9802_REG_CFG);
}

void mcp9802_do_oneshot(uint8_t ubAddress)
{
    uint8_t ubConfig = mcp9802_get_config(ubAddress);

    if(!(ubConfig & MCP9802_REG_CFG_SHUTDOWN))
        return; // Device not in shutdown mode

    ubConfig |= MCP9802_REG_CFG_ONE_SHOT;

    mcp9802_set_config(ubAddress, ubConfig);

    while(mcp9802_get_config(ubAddress) & MCP9802_REG_CFG_ONE_SHOT)
        delay_ms(5);
}

uint16_t mcp9802_get_ambient_temp_reg(uint8_t ubAddress)
{
    return mcp9802_read_register16(ubAddress, MCP9802_REG_TAMB);
}
float mcp9802_get_ambient_temp(uint8_t ubAddress)
{
    int16_t sValue = mcp9802_get_ambient_temp_reg(ubAddress);

    return (float)sValue / 256.f;
}
void mcp9802_set_hyst_temp_reg(uint8_t ubAddress, uint16_t usTemp)
{
    mcp9802_write_register16(ubAddress, MCP9802_REG_THYST, usTemp & 0xFF80);
}
void mcp9802_set_hyst_temp(uint8_t ubAddress, float fTemp)
{
    int16_t sValue = fTemp * 256.f;

    mcp9802_set_hyst_temp_reg(ubAddress, (uint16_t)sValue & 0xFF80);
}
uint16_t mcp9802_get_hyst_temp_reg(uint8_t ubAddress)
{
    return mcp9802_read_register16(ubAddress, MCP9802_REG_THYST);
}
float mcp9802_get_hyst_temp(uint8_t ubAddress)
{
    int16_t sValue = mcp9802_get_hyst_temp_reg(ubAddress);

    return (float)sValue / 256.f;
}
void mcp9802_set_limit_temp_reg(uint8_t ubAddress, uint16_t usTemp)
{
    mcp9802_write_register16(ubAddress, MCP9802_REG_TLIMIT, usTemp & 0xFF80);
}
void mcp9802_set_limit_temp(uint8_t ubAddress, float fTemp)
{
    int16_t sValue = fTemp * 256.f;

    mcp9802_set_limit_temp_reg(ubAddress, (uint16_t)sValue & 0xFF80);
}
uint16_t mcp9802_get_limit_temp_reg(uint8_t ubAddress)
{
    return mcp9802_read_register16(ubAddress, MCP9802_REG_TLIMIT);
}
float mcp9802_get_limit_temp(uint8_t ubAddress)
{
    int16_t sValue = mcp9802_get_limit_temp_reg(ubAddress);

    return (float)sValue / 256.f;
}