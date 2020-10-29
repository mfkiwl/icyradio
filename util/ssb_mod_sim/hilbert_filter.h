static const uint32_t hilbert_filter_order = 400;
static const uint32_t hilbert_filter_taps_size = 200;
static const int16_t hilbert_filter_taps[] = {
    -11,
    -3,
    -3,
    -4,
    -4,
    -5,
    -5,
    -6,
    -6,
    -7,
    -8,
    -9,
    -9,
    -10,
    -11,
    -12,
    -13,
    -14,
    -15,
    -16,
    -18,
    -19,
    -21,
    -22,
    -24,
    -25,
    -27,
    -29,
    -31,
    -33,
    -35,
    -37,
    -39,
    -42,
    -44,
    -47,
    -50,
    -53,
    -56,
    -59,
    -62,
    -66,
    -70,
    -73,
    -77,
    -82,
    -86,
    -90,
    -95,
    -100,
    -105,
    -111,
    -116,
    -122,
    -128,
    -134,
    -141,
    -148,
    -155,
    -163,
    -171,
    -180,
    -188,
    -198,
    -207,
    -218,
    -229,
    -240,
    -252,
    -265,
    -279,
    -293,
    -309,
    -325,
    -343,
    -362,
    -382,
    -404,
    -428,
    -454,
    -483,
    -514,
    -549,
    -587,
    -631,
    -680,
    -736,
    -800,
    -875,
    -964,
    -1072,
    -1203,
    -1370,
    -1586,
    -1881,
    -2305,
    -2970,
    -4165,
    -6949,
    -20859,
    20859,
    6949,
    4165,
    2970,
    2305,
    1881,
    1586,
    1370,
    1203,
    1072,
    964,
    875,
    800,
    736,
    680,
    631,
    587,
    549,
    514,
    483,
    454,
    428,
    404,
    382,
    362,
    343,
    325,
    309,
    293,
    279,
    265,
    252,
    240,
    229,
    218,
    207,
    198,
    188,
    180,
    171,
    163,
    155,
    148,
    141,
    134,
    128,
    122,
    116,
    111,
    105,
    100,
    95,
    90,
    86,
    82,
    77,
    73,
    70,
    66,
    62,
    59,
    56,
    53,
    50,
    47,
    44,
    42,
    39,
    37,
    35,
    33,
    31,
    29,
    27,
    25,
    24,
    22,
    21,
    19,
    18,
    16,
    15,
    14,
    13,
    12,
    11,
    10,
    9,
    9,
    8,
    7,
    6,
    6,
    5,
    5,
    4,
    4,
    3,
    3,
    11
};
static const int32_t hilbert_filter_tap_delay[] = {
    1,
    3,
    5,
    7,
    9,
    11,
    13,
    15,
    17,
    19,
    21,
    23,
    25,
    27,
    29,
    31,
    33,
    35,
    37,
    39,
    41,
    43,
    45,
    47,
    49,
    51,
    53,
    55,
    57,
    59,
    61,
    63,
    65,
    67,
    69,
    71,
    73,
    75,
    77,
    79,
    81,
    83,
    85,
    87,
    89,
    91,
    93,
    95,
    97,
    99,
    101,
    103,
    105,
    107,
    109,
    111,
    113,
    115,
    117,
    119,
    121,
    123,
    125,
    127,
    129,
    131,
    133,
    135,
    137,
    139,
    141,
    143,
    145,
    147,
    149,
    151,
    153,
    155,
    157,
    159,
    161,
    163,
    165,
    167,
    169,
    171,
    173,
    175,
    177,
    179,
    181,
    183,
    185,
    187,
    189,
    191,
    193,
    195,
    197,
    199,
    201,
    203,
    205,
    207,
    209,
    211,
    213,
    215,
    217,
    219,
    221,
    223,
    225,
    227,
    229,
    231,
    233,
    235,
    237,
    239,
    241,
    243,
    245,
    247,
    249,
    251,
    253,
    255,
    257,
    259,
    261,
    263,
    265,
    267,
    269,
    271,
    273,
    275,
    277,
    279,
    281,
    283,
    285,
    287,
    289,
    291,
    293,
    295,
    297,
    299,
    301,
    303,
    305,
    307,
    309,
    311,
    313,
    315,
    317,
    319,
    321,
    323,
    325,
    327,
    329,
    331,
    333,
    335,
    337,
    339,
    341,
    343,
    345,
    347,
    349,
    351,
    353,
    355,
    357,
    359,
    361,
    363,
    365,
    367,
    369,
    371,
    373,
    375,
    377,
    379,
    381,
    383,
    385,
    387,
    389,
    391,
    393,
    395,
    397,
    399
};