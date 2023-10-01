
#include "axi_dac_core.h"

const uint16_t sine_lut[128] =
{
    0x000, 0x064, 0x0C8, 0x12C, 0x18F, 0x1F1, 0x252, 0x2B1,
    0x30F, 0x36B, 0x3C5, 0x41C, 0x471, 0x4C3, 0x512, 0x55F,
    0x5A7, 0x5ED, 0x62E, 0x66C, 0x6A6, 0x6DC, 0x70D, 0x73A,
    0x763, 0x787, 0x7A7, 0x7C2, 0x7D8, 0x7E9, 0x7F5, 0x7FD,
    0x7FF, 0x7FD, 0x7F5, 0x7E9, 0x7D8, 0x7C2, 0x7A7, 0x787,
    0x763, 0x73A, 0x70D, 0x6DC, 0x6A6, 0x66C, 0x62E, 0x5ED,
    0x5A7, 0x55F, 0x512, 0x4C3, 0x471, 0x41C, 0x3C5, 0x36B,
    0x30F, 0x2B1, 0x252, 0x1F1, 0x18F, 0x12C, 0xC8,  0x64,
    0x000, 0xF9B, 0xF37, 0xED3, 0xE70, 0xE0E, 0xDAD, 0xD4E,
    0xCF0, 0xC94, 0xC3A, 0xBE3, 0xB8E, 0xB3C, 0xAED, 0xAA0,
    0xA58, 0xA12, 0x9D1, 0x993, 0x959, 0x923, 0x8F2, 0x8C5,
    0x89C, 0x878, 0x858, 0x83D, 0x827, 0x816, 0x80A, 0x802,
    0x800, 0x802, 0x80A, 0x816, 0x827, 0x83D, 0x858, 0x878,
    0x89C, 0x8C5, 0x8F2, 0x923, 0x959, 0x993, 0x9D1, 0xA12,
    0xA58, 0xAA0, 0xAED, 0xB3C, 0xB8E, 0xBE3, 0xC3A, 0xC94,
    0xCF0, 0xD4E, 0xDAD, 0xE0E, 0xE70, 0xED3, 0xF37, 0xF9B
};

const uint32_t sine_lut_iq[1024] =
{
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A,
    0x00002666, 0x01E2265A, 0x03C32636, 0x05A225FB, 0x077D25A9,
    0x0954253F, 0x0B2524BE, 0x0CEF2427, 0x0EB12379, 0x106A22B6,
    0x121921DD, 0x13BD20EF, 0x15551FED, 0x16DF1ED7, 0x185C1DAE,
    0x19C91C73, 0x1B261B26, 0x1C7319C9, 0x1DAE185C, 0x1ED716DF,
    0x1FED1555, 0x20EF13BD, 0x21DD1219, 0x22B6106A, 0x23790EB1,
    0x24270CEF, 0x24BE0B25, 0x253F0954, 0x25A9077D, 0x25FB05A2,
    0x263603C3, 0x265A01E2, 0x26660000, 0x265AFE1E, 0x2636FC3D,
    0x25FBFA5E, 0x25A9F883, 0x253FF6AC, 0x24BEF4DB, 0x2427F311,
    0x2379F14F, 0x22B6EF96, 0x21DDEDE7, 0x20EFEC43, 0x1FEDEAAB,
    0x1ED7E921, 0x1DAEE7A4, 0x1C73E637, 0x1B26E4DA, 0x19C9E38D,
    0x185CE252, 0x16DFE129, 0x1555E013, 0x13BDDF11, 0x1219DE23,
    0x106ADD4A, 0x0EB1DC87, 0x0CEFDBD9, 0x0B25DB42, 0x0954DAC1,
    0x077DDA57, 0x05A2DA05, 0x03C3D9CA, 0x01E2D9A6, 0x0000D99A,
    0xFE1ED9A6, 0xFC3DD9CA, 0xFA5EDA05, 0xF883DA57, 0xF6ACDAC1,
    0xF4DBDB42, 0xF311DBD9, 0xF14FDC87, 0xEF96DD4A, 0xEDE7DE23,
    0xEC43DF11, 0xEAABE013, 0xE921E129, 0xE7A4E252, 0xE637E38D,
    0xE4DAE4DA, 0xE38DE637, 0xE252E7A4, 0xE129E921, 0xE013EAAB,
    0xDF11EC43, 0xDE23EDE7, 0xDD4AEF96, 0xDC87F14F, 0xDBD9F311,
    0xDB42F4DB, 0xDAC1F6AC, 0xDA57F883, 0xDA05FA5E, 0xD9CAFC3D,
    0xD9A6FE1E, 0xD99A0000, 0xD9A601E2, 0xD9CA03C3, 0xDA0505A2,
    0xDA57077D, 0xDAC10954, 0xDB420B25, 0xDBD90CEF, 0xDC870EB1,
    0xDD4A106A, 0xDE231219, 0xDF1113BD, 0xE0131555, 0xE12916DF,
    0xE252185C, 0xE38D19C9, 0xE4DA1B26, 0xE6371C73, 0xE7A41DAE,
    0xE9211ED7, 0xEAAB1FED, 0xEC4320EF, 0xEDE721DD, 0xEF9622B6,
    0xF14F2379, 0xF3112427, 0xF4DB24BE, 0xF6AC253F, 0xF88325A9,
    0xFA5E25FB, 0xFC3D2636, 0xFE1E265A
};

/**
 * @brief AXI DAC Data Read.
 * @param dac - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Data value to be written.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_read(struct axi_dac* dac, uint32_t reg_addr, uint32_t* reg_data)
{
    if(dac == NULL)
        return -1;

    if(reg_data == NULL)
        return -1;

    *reg_data = (uint32_t)*(volatile uint32_t *)((uintptr_t)dac->base + reg_addr);

    return 0;
}

/**
 * @brief AXI DAC Data Write.
 * @param dac - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Data value to be written.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_write(struct axi_dac* dac, uint32_t reg_addr, uint32_t reg_data)
{
    if(dac == NULL)
        return -1;

    *(volatile uint32_t *)((uintptr_t)dac->base + reg_addr) = reg_data;

    return 0;
}

/**
 * @brief AXI DAC Set Data type for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param sel - Data type.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_set_datasel(struct axi_dac* dac, int32_t chan, enum axi_dac_data_sel sel)
{
    int32_t i;

    if(chan < 0)
        for(i = 0; i < dac->num_channels; i++)
            axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_7(i), sel);
    else
        axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_7(chan), sel);

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    return 0;
}

/**
 * @brief AXI DAC Set DDS frequency for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param freq_hz - The frequency to be set in Hz.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_set_frequency(struct axi_dac* dac, uint32_t chan, uint32_t freq_hz)
{
    uint64_t val64;
    uint32_t reg;

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
    axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);

    val64 = (uint64_t)freq_hz * 0xFFFFULL;
    val64 = val64 / dac->clock_hz;
    reg = (reg & ~AXI_DAC_DDS_INCR(~0)) | AXI_DAC_DDS_INCR(val64) | 1;

    axi_dac_write(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), reg);
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    return 0;
}

/**
 * @brief AXI DAC Get DDS frequency for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param freq - The frequency read in Hz.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_get_frequency(struct axi_dac* dac, uint32_t chan, uint32_t* freq)
{
    uint32_t reg = 0;
    uint64_t val64;

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
    axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    reg = (reg & AXI_DAC_DDS_INCR(~0));
    val64 = (uint64_t)reg * dac->clock_hz;
    utils_do_div(&val64, 0xFFFF);
    *freq = val64;

    return 0;
}

/**
 * @brief AXI DAC Set DDS phase for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param phase - The phase to be set. Phase is in milli angles scaled to 1000
 * 		  (i.e. 90*1000 is 90 degrees (pi/2))
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_set_phase(struct axi_dac* dac, uint32_t chan, uint32_t phase)
{
    uint64_t val64;
    uint32_t reg;

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
    axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);

    val64 = (uint64_t)phase * 0x10000ULL + (360000 / 2);
    val64 = val64 / 360000;
    reg = (reg & ~AXI_DAC_DDS_INIT(~0)) | AXI_DAC_DDS_INIT(val64);

    axi_dac_write(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), reg);
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    return 0;
}

/**
 * @brief AXI DAC Get DDS phase for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param phase - The phase read. Phase is in milli angles scaled to 1000
 * 		  (i.e. 90*1000 is 90 degrees (pi/2))
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_get_phase(struct axi_dac* dac, uint32_t chan, uint32_t* phase)
{
    uint64_t val64;
    uint32_t reg = 0;

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
    axi_dac_read(dac, AXI_DAC_REG_DDS_INIT_INCR(chan), &reg);
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    reg = (reg & AXI_DAC_DDS_INIT(~0));
    reg = AXI_DAC_TO_DDS_INIT(reg);
    val64 = reg * 360000ULL + (0x10000 / 2);
    utils_do_div(&val64, 0x10000);
    *phase = val64;

    return 0;
}

/**
 * @brief AXI DAC Set DDS scale for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param scale_micro_units - The scale to be set. Scale is in micro units
 * 			      (i.e. 1*1000*1000 is 1.0)
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_set_scale(struct axi_dac* dac, uint32_t chan, int32_t scale_micro_units)
{
    uint32_t scale_reg;

    scale_reg = scale_micro_units;

    if(scale_micro_units < 0)
        scale_reg = scale_micro_units * -1;
    if(scale_reg >= 1999000)
        scale_reg = 1999000;

    scale_reg = (uint32_t)(((uint64_t)scale_reg * 0x4000) / 1000000);

    if(scale_micro_units < 0)
        scale_reg = scale_reg | 0x8000;

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
    axi_dac_write(dac, AXI_DAC_REG_DDS_SCALE(chan), AXI_DAC_DDS_SCALE(scale_reg));
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    return 0;
}

/**
 * @brief AXI DAC Get DDS scale for specific channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param scale_micro_units - The scale read. Scale is in micro units
 * 			      (i.e. 1*1000*1000 is 1.0)
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_get_scale(struct axi_dac* dac, uint32_t chan, int32_t* scale_micro_units)
{
    int32_t sign;
    uint32_t scale_reg = 0;

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, 0);
    axi_dac_read(dac, AXI_DAC_REG_DDS_SCALE(chan), &scale_reg);
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    scale_reg = AXI_DAC_TO_DDS_SCALE(scale_reg);
    sign = (scale_reg & 0x8000) ? -1 : 1;
    scale_reg &= ~0x8000;
    scale_reg = ((uint64_t)scale_reg * 1000000) / 0x4000;
    *scale_micro_units = (int32_t)scale_reg * sign;

    return 0;
}

/**
 * @brief AXI DAC Convert to signed magnitude format.
 * @param val - integer part
 * @param val2 - fractional part
 * @return Returns converted value in case of success or negative error code
 * 	   otherwise.
 */
uint32_t axi_dac_dds_to_signed_mag_fmt(int32_t val, int32_t val2)
{
    uint32_t i;
    uint64_t val64;

    switch(val)
    {
        case 1:
            i = 0x4000;
        break;
        case -1:
            i = 0xC000;
        break;
        case 0:
            i = 0;

            if(val2 < 0)
            {
                i = 0x8000;
                val2 *= -1;
            }
        break;
        default:
            i = 0;
        break;
    }

    val64 = (uint64_t)val2 * 0x4000UL + (1000000UL / 2);
    utils_do_div(&val64, 1000000UL);

    return i | val64;
}

/**
 * @brief AXI DAC Convert from signed magnitude format.
 * @param val - input value
 * @param r_val - integer part
 * @param r_val2 - fractional part
 */
void axi_dac_dds_from_signed_mag_fmt(uint32_t val, int32_t* r_val, int32_t* r_val2)
{
    uint64_t val64;
    int32_t sign;

    if(val & 0x8000)
        sign = -1;
    else
        sign = 1;

    if(val & 0x4000)
        *r_val = 1 * sign;
    else
        *r_val = 0;

    val &= ~0xC000;

    val64 = val * 1000000ULL + (0x4000 / 2);
    utils_do_div(&val64, 0x4000);

    if(*r_val == 0)
        *r_val2 = val64 * sign;
    else
        *r_val2 = val64;
}

/**
 * @brief Calibrate phase scale for specific AXI DAC channel
 * @param dac - The device structure.
 * @param phase - Phase value.
 * @param chan - The DAC channel.
 * @param val - the integer part.
 * @param val2 - the fractional part.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_set_calib_phase_scale(struct axi_dac* dac, uint32_t phase, uint32_t chan, int32_t val, int32_t val2)
{
    uint32_t reg = 0;
    uint32_t i;

    i = axi_dac_dds_to_signed_mag_fmt(val, val2);

    axi_dac_read(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), &reg);

    if(!((chan + phase) % 2))
    {
        reg &= ~AXI_DAC_IQCOR_COEFF_1(~0);
        reg |= AXI_DAC_IQCOR_COEFF_1(i);
    }
    else
    {
        reg &= ~AXI_DAC_IQCOR_COEFF_2(~0);
        reg |= AXI_DAC_IQCOR_COEFF_2(i);
    }

    axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), reg);
    axi_dac_write(dac, AXI_DAC_REG_CHAN_CNTRL_6(chan), AXI_DAC_IQCOR_ENB);

    return 0;
}

/**
 * @brief Get the phase scale calibration values for AXI DAC channel
 * @param dac - The device structure.
 * @param phase - Phase value.
 * @param chan - The DAC channel.
 * @param val - the integer part.
 * @param val2 - the fractional part.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_get_calib_phase_scale(struct axi_dac* dac, uint32_t phase, uint32_t chan, int32_t* val, int32_t* val2)
{
    uint32_t reg = 0;

    axi_dac_read(dac, AXI_DAC_REG_CHAN_CNTRL_8(chan), &reg);

    if(!((phase + chan) % 2))
        reg = AXI_DAC_TO_IQCOR_COEFF_1(reg);
    else
        reg = AXI_DAC_TO_IQCOR_COEFF_2(reg);

    axi_dac_dds_from_signed_mag_fmt(reg, val, val2);

    return 0;
}

/**
 * @brief Calibrate scale for specific AXI DAC channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param val - the integer part.
 * @param val2 - the fractional part.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_set_calib_scale(struct axi_dac* dac, uint32_t chan, int32_t val, int32_t val2)
{
    return axi_dac_dds_set_calib_phase_scale(dac, 0, chan, val, val2);
}

/**
 * @brief Get the scale calibration values for AXI DAC channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param val - the integer part.
 * @param val2 - the fractional part.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_get_calib_scale(struct axi_dac* dac, uint32_t chan, int32_t* val, int32_t* val2)
{
    return axi_dac_dds_get_calib_phase_scale(dac, 0, chan, val, val2);
}

/**
 * @brief Calibrate phase for specific AXI DAC channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param val - the integer part.
 * @param val2 - the fractional part.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_set_calib_phase(struct axi_dac* dac, uint32_t chan, int32_t val, int32_t val2)
{
    return axi_dac_dds_set_calib_phase_scale(dac, 1, chan, val, val2);
}

/**
 * @brief Get the phase calibration values for AXI DAC channel
 * @param dac - The device structure.
 * @param chan - The DAC channel.
 * @param val - the integer part.
 * @param val2 - the fractional part.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_dds_get_calib_phase(struct axi_dac* dac, uint32_t chan, int32_t* val, int32_t* val2)
{
    return axi_dac_dds_get_calib_phase_scale(dac, 1, chan, val, val2);
}

/**
 * @brief AXI DAC Set data based on a Sine Lookup Table
 * @param dac - The device structure.
 * @param address - Address of the sine lut.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
uint32_t axi_dac_set_sine_lut(struct axi_dac* dac, uint32_t address)
{
    uint32_t length;
    uint32_t tx_count;
    uint32_t index_mem;
    uint32_t index;
    uint32_t index_i1;
    uint32_t index_q1;
    uint32_t index_i2;
    uint32_t index_q2;
    uint32_t data_i1;
    uint32_t data_q1;
    uint32_t data_i2;
    uint32_t data_q2;

    tx_count = sizeof(sine_lut) / sizeof(uint16_t);

    if(dac->num_channels == 4)
    {
        for(index = 0, index_mem = 0; index < (tx_count * 2); index += 2, index_mem += 2)
        {
            index_i1 = index;
            index_q1 = index + (tx_count / 2);

            if(index_q1 >= (tx_count * 2))
                index_q1 -= (tx_count * 2);

            data_i1 = (sine_lut[index_i1 / 2] << 20);
            data_q1 = (sine_lut[index_q1 / 2] << 4);

            // TODO: no_os_axi_io_write(address, index_mem * 4, data_i1 | data_q1);

            index_i2 = index_i1;
            index_q2 = index_q1;

            if(index_i2 >= (tx_count * 2))
                index_i2 -= (tx_count * 2);
            if(index_q2 >= (tx_count * 2))
                index_q2 -= (tx_count * 2);

            data_i2 = (sine_lut[index_i2 / 2] << 20);
            data_q2 = (sine_lut[index_q2 / 2] << 4);

            // TODO: no_os_axi_io_write(address, (index_mem + 1) * 4, data_i2 | data_q2);

        }
    }
    else
    {
        for(index = 0; index < tx_count; index += 1)
        {
            index_i1 = index;
            index_q1 = index + (tx_count / 4);

            if(index_q1 >= tx_count)
                index_q1 -= tx_count;

            data_i1 = (sine_lut[index_i1] << 20);
            data_q1 = (sine_lut[index_q1] << 4);

            // TODO: no_os_axi_io_write(address, index * 4, data_i1 | data_q1);
        }
    }

    length = tx_count * dac->num_channels * 2;

    return length;
}

/**
 * @brief AXI DAC Set data buffer.
 * @param dac - The device structure.
 * @param address - Base Address.
 * @param buff - The buffer to be set.
 * @param buff_size - The buffer size.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_set_buff(struct axi_dac* dac, uint32_t address, uint16_t* buff, uint32_t buff_size)
{
    uint32_t index;
    uint32_t data_i;
    uint32_t data_q;

    for(index = 0; index < buff_size; index += 2)
    {
        data_i = (buff[index]);
        data_q = (buff[index + 1] << 16);

        // TODO: no_os_axi_io_write(address, index * 2, data_i | data_q);
    }

    return 0;
}

/**
 * @brief AXI DAC Load custom data.
 * @param dac - The device structure.
 * @param custom_data_iq - The custom data array in I/Q format.
 * @param custom_tx_count - The custom data array size.
 * @param address - The address where the data is loaded.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_load_custom_data(struct axi_dac* dac, const uint32_t* custom_data_iq, uint32_t custom_tx_count, uint32_t address)
{
    uint32_t index, index_mem = 0;
    uint8_t chan;
    uint8_t num_tx_channels = dac->num_channels / 2;

    for(index = 0; index < custom_tx_count; index++)
    {
        /* Send the same data on all the channels */
        for(chan = 0; chan < num_tx_channels; chan++)
        {

            // TODO: no_os_axi_io_write(address, index_mem * sizeof(uint32_t), custom_data_iq[index]);

            index_mem++;
        }
    }

    for(chan = 0; chan < dac->num_channels; chan++)
    {
        axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((chan * 2) + 0), 0x2);
        axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((chan * 2) + 1), 0x2);
    }

    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    return 0;
}

/**
 * @brief Begin AXI DAC Initialization.
 * @param dac_core - The device structure.
 * @param init - Initialization parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_init_begin(struct axi_dac** dac_core, const struct axi_dac_init* init)
{
    struct axi_dac* dac;

    dac = (struct axi_dac*)malloc(sizeof(*dac));

    if(!dac)
        return -1;

    dac->name = init->name;
    dac->base = init->base;
    dac->num_channels = init->num_channels;
    dac->channels = init->channels;

    *dac_core = dac;

    return 0;
}

/**
 * @brief Begin AXI DAC Initialization.
 * @param dac - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_init_finish(struct axi_dac* dac)
{
    uint32_t reg_data = 0;
    uint32_t freq = 0;
    uint32_t ratio = 0;

    axi_dac_read(dac, AXI_DAC_REG_STATUS, &reg_data);

    if(reg_data == 0x0)
    {
        DBGPRINTLN_CTX("%s: Status errors", dac->name);

        return -1;
    }

    axi_dac_read(dac, AXI_DAC_REG_CLK_FREQ, &freq);
    axi_dac_read(dac, AXI_DAC_REG_CLK_RATIO, &ratio);

    dac->clock_hz = freq * ratio;
    dac->clock_hz = (dac->clock_hz * 125000000UL) >> 16;

    DBGPRINTLN_CTX("%s: Successfully initialized (%"PRIu64" Hz)", dac->name, dac->clock_hz);

    return 0;
}

/**
 * @brief AXI DAC Main Initialization.
 * @param dac_core - The device structure.
 * @param init - Initialization parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_init(struct axi_dac** dac_core, const struct axi_dac_init* init)
{
    struct axi_dac* dac;
    int32_t ret;

    ret = axi_dac_init_begin(&dac, init);

    if(ret)
        return ret;

    axi_dac_write(dac, AXI_DAC_REG_RSTN, 0);
    axi_dac_write(dac, AXI_DAC_REG_RSTN, AXI_DAC_MMCM_RSTN | AXI_DAC_RSTN);

    axi_dac_write(dac, AXI_DAC_REG_RATECNTRL, AXI_DAC_RATE(init->rate));

    usleep(100000);

    ret = axi_dac_init_finish(dac);

    if(ret)
    {
        free(dac);

        return -1;
    }

    axi_dac_data_setup(dac);
    axi_dac_write(dac, AXI_DAC_REG_SYNC_CONTROL, AXI_DAC_SYNC);

    *dac_core = dac;

    return 0;
}

/**
 * @brief AXI DAC Data Setup.
 * @param dac - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_data_setup(struct axi_dac* dac)
{
    struct axi_dac_channel* chan;

    if(dac->channels)
    {
        for(uint32_t i = 0; i < dac->num_channels; i++)
        {
            chan = &dac->channels[i];

            if(chan->sel == AXI_DAC_DATA_SEL_DDS)
            {
                axi_dac_dds_set_frequency(dac, ((i * 2) + 0), chan->dds_frequency_0);
                axi_dac_dds_set_phase(dac, ((i * 2) + 0), chan->dds_phase_0);
                axi_dac_dds_set_scale(dac, ((i * 2) + 0), chan->dds_scale_0);

                if(chan->dds_dual_tone == 0)
                {
                    axi_dac_dds_set_frequency(dac, ((i * 2) + 1), chan->dds_frequency_0);
                    axi_dac_dds_set_phase(dac, ((i * 2) + 1), chan->dds_phase_0);
                    axi_dac_dds_set_scale(dac, ((i * 2) + 1), chan->dds_scale_0);
                }
                else
                {
                    axi_dac_dds_set_frequency(dac, ((i * 2) + 1), chan->dds_frequency_1);
                    axi_dac_dds_set_phase(dac, ((i * 2) + 1), chan->dds_phase_1);
                    axi_dac_dds_set_scale(dac, ((i * 2) + 1), chan->dds_scale_1);
                }
            }

            axi_dac_write(dac, DAC_REG_DATA_PATTERN(i), chan->pat_data);
            axi_dac_set_datasel(dac, i, chan->sel);
        }
    }
    else
    {
        for(uint32_t i = 0; i < dac->num_channels; i++)
        {
            axi_dac_dds_set_frequency(dac, ((i * 2) + 0), 3 * 1000 * 1000);
            axi_dac_dds_set_frequency(dac, ((i * 2) + 1), 3 * 1000 * 1000);
            axi_dac_dds_set_phase(dac, ((i * 2) + 0), (i % 2) ? 0 : 90000);
            axi_dac_dds_set_phase(dac, ((i * 2) + 1), (i % 2) ? 0 : 90000);
            axi_dac_dds_set_scale(dac, ((i * 2) + 0), 50 * 1000);
            axi_dac_dds_set_scale(dac, ((i * 2) + 1), 50 * 1000);
            axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((i * 2) + 0), 0);
            axi_dac_write(dac, AXI_DAC_REG_DATA_SELECT((i * 2) + 1), 0);
        }
    }
    return 0;
}

/**
 * @brief AXI DAC Resources deallocation.
 * @param dac - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_dac_remove(struct axi_dac* dac)
{
    free(dac);

    return 0;
}
