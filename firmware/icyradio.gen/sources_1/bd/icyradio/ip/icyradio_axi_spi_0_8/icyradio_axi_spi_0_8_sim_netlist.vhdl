-- Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2021.1 (lin64) Build 3247384 Thu Jun 10 19:36:07 MDT 2021
-- Date        : Fri Dec 22 02:37:54 2023
-- Host        : node4-dev running 64-bit Ubuntu 22.04.3 LTS
-- Command     : write_vhdl -force -mode funcsim
--               /home/joao/icyradio/firmware/icyradio.gen/sources_1/bd/icyradio/ip/icyradio_axi_spi_0_8/icyradio_axi_spi_0_8_sim_netlist.vhdl
-- Design      : icyradio_axi_spi_0_8
-- Purpose     : This VHDL netlist is a functional simulation representation of the design and should not be modified or
--               synthesized. This netlist cannot be used for SDF annotated simulation.
-- Device      : xc7a100tfgg484-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity icyradio_axi_spi_0_8_axi_spi is
  port (
    s_axi_wready : out STD_LOGIC;
    s_axi_rdata : out STD_LOGIC_VECTOR ( 19 downto 0 );
    spi_io0_o : out STD_LOGIC;
    spi_io1_o : out STD_LOGIC;
    Q : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_rvalid_reg_0 : out STD_LOGIC;
    s_axi_arready : out STD_LOGIC;
    spi_ss_o : out STD_LOGIC_VECTOR ( 0 to 0 );
    spi_sck_o : out STD_LOGIC;
    s_axi_bvalid : out STD_LOGIC;
    s_axi_wstrb : in STD_LOGIC_VECTOR ( 1 downto 0 );
    aclk : in STD_LOGIC;
    s_axi_wdata : in STD_LOGIC_VECTOR ( 9 downto 0 );
    s_axi_awaddr : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_arvalid : in STD_LOGIC;
    s_axi_rready : in STD_LOGIC;
    aresetn : in STD_LOGIC;
    s_axi_araddr : in STD_LOGIC_VECTOR ( 2 downto 0 );
    spi_io1_i : in STD_LOGIC;
    spi_io0_i : in STD_LOGIC;
    s_axi_wvalid : in STD_LOGIC;
    s_axi_awvalid : in STD_LOGIC;
    s_axi_bready : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of icyradio_axi_spi_0_8_axi_spi : entity is "axi_spi";
end icyradio_axi_spi_0_8_axi_spi;

architecture STRUCTURE of icyradio_axi_spi_0_8_axi_spi is
  signal \FSM_onehot_spi_fsm_state[2]_i_2_n_0\ : STD_LOGIC;
  signal \FSM_onehot_spi_fsm_state_reg_n_0_[0]\ : STD_LOGIC;
  signal \FSM_onehot_spi_fsm_state_reg_n_0_[1]\ : STD_LOGIC;
  signal \FSM_onehot_spi_fsm_state_reg_n_0_[2]\ : STD_LOGIC;
  signal data3 : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal p_0_in : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal p_1_in : STD_LOGIC_VECTOR ( 0 to 0 );
  signal p_2_in : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal s_axi_awready0 : STD_LOGIC;
  signal s_axi_awready_i_1_n_0 : STD_LOGIC;
  signal \^s_axi_bvalid\ : STD_LOGIC;
  signal s_axi_bvalid_i_1_n_0 : STD_LOGIC;
  signal s_axi_rd_en : STD_LOGIC;
  signal \s_axi_rdata[0]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[1]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[2]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[31]_i_1_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[3]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[4]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[5]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[6]_i_2_n_0\ : STD_LOGIC;
  signal \s_axi_rdata[7]_i_2_n_0\ : STD_LOGIC;
  signal s_axi_rdata_1 : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal s_axi_rvalid_i_1_n_0 : STD_LOGIC;
  signal \^s_axi_rvalid_reg_0\ : STD_LOGIC;
  signal \^s_axi_wready\ : STD_LOGIC;
  signal spi_cpol : STD_LOGIC;
  signal spi_dir_i_1_n_0 : STD_LOGIC;
  signal spi_dir_reg_n_0 : STD_LOGIC;
  signal spi_en : STD_LOGIC;
  signal spi_en_i_1_n_0 : STD_LOGIC;
  signal spi_fsm_state : STD_LOGIC;
  signal \spi_fsm_state[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_fsm_state[1]_i_1_n_0\ : STD_LOGIC;
  signal \spi_fsm_state_reg_n_0_[0]\ : STD_LOGIC;
  signal \spi_fsm_state_reg_n_0_[1]\ : STD_LOGIC;
  signal \^spi_io1_o\ : STD_LOGIC;
  signal spi_io_mode : STD_LOGIC_VECTOR ( 2 downto 0 );
  signal \spi_io_mode[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_mode[1]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_mode[2]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_mode_q0__0\ : STD_LOGIC;
  signal \spi_io_mode_q1__8\ : STD_LOGIC;
  signal \spi_io_mode_q[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_mode_q[1]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_mode_q[2]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_mode_q[2]_i_2_n_0\ : STD_LOGIC;
  signal \spi_io_mode_q[2]_i_3_n_0\ : STD_LOGIC;
  signal \spi_io_mode_q_reg_n_0_[0]\ : STD_LOGIC;
  signal \spi_io_mode_q_reg_n_0_[1]\ : STD_LOGIC;
  signal \spi_io_mode_q_reg_n_0_[2]\ : STD_LOGIC;
  signal spi_io_o : STD_LOGIC_VECTOR ( 1 downto 0 );
  signal \spi_io_o1__3\ : STD_LOGIC;
  signal \spi_io_o[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_o[0]_i_2_n_0\ : STD_LOGIC;
  signal \spi_io_o[0]_i_5_n_0\ : STD_LOGIC;
  signal \spi_io_o[0]_i_6_n_0\ : STD_LOGIC;
  signal \spi_io_o[1]_i_1_n_0\ : STD_LOGIC;
  signal \spi_io_o[1]_i_3_n_0\ : STD_LOGIC;
  signal spi_io_t : STD_LOGIC_VECTOR ( 1 downto 0 );
  signal \spi_io_t[0]_i_2_n_0\ : STD_LOGIC;
  signal \spi_io_t[1]_i_2_n_0\ : STD_LOGIC;
  signal spi_lsb_first_i_1_n_0 : STD_LOGIC;
  signal spi_lsb_first_reg_n_0 : STD_LOGIC;
  signal \spi_mode[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_mode[1]_i_1_n_0\ : STD_LOGIC;
  signal \spi_mode[1]_i_2_n_0\ : STD_LOGIC;
  signal \spi_mode_reg_n_0_[0]\ : STD_LOGIC;
  signal spi_rd_req : STD_LOGIC;
  signal spi_rd_req_ack : STD_LOGIC;
  signal \spi_rd_req_ack0__0\ : STD_LOGIC;
  signal spi_rd_req_ack_i_3_n_0 : STD_LOGIC;
  signal spi_rd_req_ack_reg_n_0 : STD_LOGIC;
  signal spi_rd_req_i_1_n_0 : STD_LOGIC;
  signal spi_rd_req_i_2_n_0 : STD_LOGIC;
  signal spi_sck_div : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal spi_sck_div_0 : STD_LOGIC;
  signal \spi_sck_div_cnt[7]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sck_div_cnt[7]_i_4_n_0\ : STD_LOGIC;
  signal \spi_sck_div_cnt[7]_i_5_n_0\ : STD_LOGIC;
  signal \spi_sck_div_cnt[7]_i_6_n_0\ : STD_LOGIC;
  signal \spi_sck_div_cnt[7]_i_7_n_0\ : STD_LOGIC;
  signal \spi_sck_div_cnt[7]_i_8_n_0\ : STD_LOGIC;
  signal spi_sck_div_cnt_reg : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal spi_sck_div_en_i_1_n_0 : STD_LOGIC;
  signal spi_sck_div_en_reg_n_0 : STD_LOGIC;
  signal spi_sck_int : STD_LOGIC;
  signal spi_sck_int_i_1_n_0 : STD_LOGIC;
  signal spi_sck_oe : STD_LOGIC;
  signal spi_sck_oe_reg_n_0 : STD_LOGIC;
  signal spi_sck_toggle : STD_LOGIC;
  signal spi_sr : STD_LOGIC_VECTOR ( 5 downto 4 );
  signal spi_sr1_in : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal spi_sr3_in : STD_LOGIC_VECTOR ( 6 downto 0 );
  signal \spi_sr[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[0]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[1]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[1]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[2]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[2]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[3]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[3]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[4]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[4]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[5]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[5]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[6]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[6]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[7]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr[7]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr[7]_i_3_n_0\ : STD_LOGIC;
  signal spi_sr_bit_cnt : STD_LOGIC_VECTOR ( 4 downto 1 );
  signal \spi_sr_bit_cnt[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[2]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[3]_i_2_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[3]_i_4_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[3]_i_5_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[4]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[4]_i_3_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt[4]_i_4_n_0\ : STD_LOGIC;
  signal \spi_sr_bit_cnt_reg_n_0_[0]\ : STD_LOGIC;
  signal \spi_sr_bit_cnt_reg_n_0_[1]\ : STD_LOGIC;
  signal \spi_sr_bit_cnt_reg_n_0_[2]\ : STD_LOGIC;
  signal \spi_sr_bit_cnt_reg_n_0_[3]\ : STD_LOGIC;
  signal \spi_sr_bit_cnt_reg_n_0_[4]\ : STD_LOGIC;
  signal spi_sr_in_buf : STD_LOGIC;
  signal spi_sr_in_buf1_in : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal \spi_sr_in_buf[7]_i_3_n_0\ : STD_LOGIC;
  signal \spi_sr_in_buf[7]_i_5_n_0\ : STD_LOGIC;
  signal spi_sr_in_buf_ready_i_1_n_0 : STD_LOGIC;
  signal spi_sr_in_buf_ready_reg_n_0 : STD_LOGIC;
  signal spi_sr_in_buf_valid_i_1_n_0 : STD_LOGIC;
  signal spi_sr_in_buf_valid_i_2_n_0 : STD_LOGIC;
  signal spi_sr_in_buf_valid_i_3_n_0 : STD_LOGIC;
  signal spi_sr_in_buf_valid_reg_n_0 : STD_LOGIC;
  signal spi_sr_out_buf : STD_LOGIC_VECTOR ( 7 downto 0 );
  signal \spi_sr_out_buf[7]_i_1_n_0\ : STD_LOGIC;
  signal \spi_sr_out_buf[7]_i_3_n_0\ : STD_LOGIC;
  signal \spi_sr_out_buf[7]_i_4_n_0\ : STD_LOGIC;
  signal \spi_sr_out_buf[7]_i_5_n_0\ : STD_LOGIC;
  signal \spi_sr_out_buf[7]_i_6_n_0\ : STD_LOGIC;
  signal spi_sr_out_buf_ready_i_1_n_0 : STD_LOGIC;
  signal spi_sr_out_buf_ready_reg_n_0 : STD_LOGIC;
  signal spi_sr_out_buf_valid : STD_LOGIC;
  signal spi_sr_out_buf_valid_i_1_n_0 : STD_LOGIC;
  signal \spi_sr_reg_n_0_[0]\ : STD_LOGIC;
  signal \spi_sr_reg_n_0_[1]\ : STD_LOGIC;
  signal \spi_sr_reg_n_0_[2]\ : STD_LOGIC;
  signal \spi_sr_reg_n_0_[3]\ : STD_LOGIC;
  signal \spi_sr_reg_n_0_[6]\ : STD_LOGIC;
  signal \^spi_ss_o\ : STD_LOGIC_VECTOR ( 0 to 0 );
  signal \spi_ss_o[0]_i_1_n_0\ : STD_LOGIC;
  signal \spi_ss_o[0]_i_2_n_0\ : STD_LOGIC;
  attribute SOFT_HLUTNM : string;
  attribute SOFT_HLUTNM of \FSM_onehot_spi_fsm_state[2]_i_2\ : label is "soft_lutpair14";
  attribute SOFT_HLUTNM of \FSM_onehot_spi_fsm_state[2]_i_3\ : label is "soft_lutpair15";
  attribute FSM_ENCODED_STATES : string;
  attribute FSM_ENCODED_STATES of \FSM_onehot_spi_fsm_state_reg[0]\ : label is "SPI_FSM_STATE_WAIT_XFER_REQ:001,SPI_FSM_STATE_WAIT_SCK_SYNC:010,SPI_FSM_STATE_ACTIVE:100,";
  attribute FSM_ENCODED_STATES of \FSM_onehot_spi_fsm_state_reg[1]\ : label is "SPI_FSM_STATE_WAIT_XFER_REQ:001,SPI_FSM_STATE_WAIT_SCK_SYNC:010,SPI_FSM_STATE_ACTIVE:100,";
  attribute FSM_ENCODED_STATES of \FSM_onehot_spi_fsm_state_reg[2]\ : label is "SPI_FSM_STATE_WAIT_XFER_REQ:001,SPI_FSM_STATE_WAIT_SCK_SYNC:010,SPI_FSM_STATE_ACTIVE:100,";
  attribute SOFT_HLUTNM of s_axi_arready_INST_0 : label is "soft_lutpair31";
  attribute SOFT_HLUTNM of s_axi_awready_i_2 : label is "soft_lutpair11";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of s_axi_awready_reg : label is "xilinx.com:interface:aximm:1.0 s_axi AWREADY";
  attribute SOFT_HLUTNM of s_axi_bvalid_i_1 : label is "soft_lutpair11";
  attribute X_INTERFACE_INFO of s_axi_bvalid_reg : label is "xilinx.com:interface:aximm:1.0 s_axi BVALID";
  attribute SOFT_HLUTNM of \s_axi_rdata[16]_i_1\ : label is "soft_lutpair0";
  attribute SOFT_HLUTNM of \s_axi_rdata[17]_i_1\ : label is "soft_lutpair20";
  attribute SOFT_HLUTNM of \s_axi_rdata[20]_i_1\ : label is "soft_lutpair17";
  attribute SOFT_HLUTNM of \s_axi_rdata[21]_i_1\ : label is "soft_lutpair17";
  attribute SOFT_HLUTNM of \s_axi_rdata[22]_i_1\ : label is "soft_lutpair18";
  attribute SOFT_HLUTNM of \s_axi_rdata[23]_i_1\ : label is "soft_lutpair18";
  attribute SOFT_HLUTNM of \s_axi_rdata[28]_i_1\ : label is "soft_lutpair19";
  attribute SOFT_HLUTNM of \s_axi_rdata[29]_i_1\ : label is "soft_lutpair19";
  attribute SOFT_HLUTNM of \s_axi_rdata[30]_i_1\ : label is "soft_lutpair20";
  attribute SOFT_HLUTNM of \s_axi_rdata[31]_i_3\ : label is "soft_lutpair21";
  attribute SOFT_HLUTNM of \s_axi_rdata[8]_i_1\ : label is "soft_lutpair0";
  attribute SOFT_HLUTNM of \s_axi_rdata[9]_i_1\ : label is "soft_lutpair21";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[0]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[16]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[17]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[1]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[20]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[21]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[22]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[23]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[28]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[29]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[2]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[30]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[31]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[3]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[4]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[5]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[6]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[7]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[8]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of \s_axi_rdata_reg[9]\ : label is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute SOFT_HLUTNM of s_axi_rvalid_i_1 : label is "soft_lutpair31";
  attribute X_INTERFACE_INFO of s_axi_rvalid_reg : label is "xilinx.com:interface:aximm:1.0 s_axi RVALID";
  attribute SOFT_HLUTNM of \spi_fsm_state[0]_i_1\ : label is "soft_lutpair33";
  attribute SOFT_HLUTNM of \spi_fsm_state[1]_i_1\ : label is "soft_lutpair33";
  attribute SOFT_HLUTNM of \spi_io_mode_q[2]_i_2\ : label is "soft_lutpair15";
  attribute SOFT_HLUTNM of \spi_io_mode_q[2]_i_3\ : label is "soft_lutpair29";
  attribute SOFT_HLUTNM of \spi_io_o[0]_i_4\ : label is "soft_lutpair28";
  attribute SOFT_HLUTNM of \spi_io_o[1]_i_3\ : label is "soft_lutpair9";
  attribute X_INTERFACE_INFO of \spi_io_o_reg[0]\ : label is "xilinx.com:interface:spi_rtl:1.0 spi IO0_O";
  attribute X_INTERFACE_INFO of \spi_io_o_reg[1]\ : label is "xilinx.com:interface:spi_rtl:1.0 spi IO0_O";
  attribute SOFT_HLUTNM of \spi_io_t[0]_i_2\ : label is "soft_lutpair9";
  attribute SOFT_HLUTNM of \spi_io_t[1]_i_2\ : label is "soft_lutpair8";
  attribute X_INTERFACE_INFO of \spi_io_t_reg[0]\ : label is "xilinx.com:interface:spi_rtl:1.0 spi IO0_T";
  attribute X_INTERFACE_INFO of \spi_io_t_reg[1]\ : label is "xilinx.com:interface:spi_rtl:1.0 spi IO0_T";
  attribute SOFT_HLUTNM of \spi_mode[1]_i_2\ : label is "soft_lutpair27";
  attribute SOFT_HLUTNM of spi_rd_req_ack_i_3 : label is "soft_lutpair29";
  attribute SOFT_HLUTNM of spi_rd_req_i_1 : label is "soft_lutpair12";
  attribute SOFT_HLUTNM of spi_rd_req_i_2 : label is "soft_lutpair22";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[0]_i_1\ : label is "soft_lutpair32";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[1]_i_1\ : label is "soft_lutpair32";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[2]_i_1\ : label is "soft_lutpair16";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[3]_i_1\ : label is "soft_lutpair10";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[4]_i_1\ : label is "soft_lutpair10";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[6]_i_1\ : label is "soft_lutpair30";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[7]_i_2\ : label is "soft_lutpair30";
  attribute SOFT_HLUTNM of \spi_sck_div_cnt[7]_i_7\ : label is "soft_lutpair16";
  attribute SOFT_HLUTNM of spi_sck_int_i_1 : label is "soft_lutpair28";
  attribute SOFT_HLUTNM of \spi_sr[0]_i_2\ : label is "soft_lutpair23";
  attribute SOFT_HLUTNM of \spi_sr[1]_i_2\ : label is "soft_lutpair23";
  attribute SOFT_HLUTNM of \spi_sr[2]_i_2\ : label is "soft_lutpair24";
  attribute SOFT_HLUTNM of \spi_sr[3]_i_2\ : label is "soft_lutpair24";
  attribute SOFT_HLUTNM of \spi_sr[4]_i_2\ : label is "soft_lutpair25";
  attribute SOFT_HLUTNM of \spi_sr[5]_i_2\ : label is "soft_lutpair25";
  attribute SOFT_HLUTNM of \spi_sr[6]_i_2\ : label is "soft_lutpair26";
  attribute SOFT_HLUTNM of \spi_sr[7]_i_2\ : label is "soft_lutpair26";
  attribute SOFT_HLUTNM of \spi_sr_bit_cnt[0]_i_1\ : label is "soft_lutpair13";
  attribute SOFT_HLUTNM of \spi_sr_bit_cnt[3]_i_3\ : label is "soft_lutpair12";
  attribute SOFT_HLUTNM of \spi_sr_bit_cnt[3]_i_4\ : label is "soft_lutpair13";
  attribute SOFT_HLUTNM of \spi_sr_bit_cnt[4]_i_3\ : label is "soft_lutpair14";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[0]_i_2\ : label is "soft_lutpair2";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[0]_i_3\ : label is "soft_lutpair7";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[1]_i_2\ : label is "soft_lutpair8";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[1]_i_3\ : label is "soft_lutpair6";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[2]_i_2\ : label is "soft_lutpair7";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[2]_i_3\ : label is "soft_lutpair5";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[3]_i_2\ : label is "soft_lutpair6";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[3]_i_3\ : label is "soft_lutpair4";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[4]_i_2\ : label is "soft_lutpair5";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[4]_i_3\ : label is "soft_lutpair3";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[5]_i_2\ : label is "soft_lutpair4";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[5]_i_3\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[6]_i_2\ : label is "soft_lutpair3";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[7]_i_3\ : label is "soft_lutpair2";
  attribute SOFT_HLUTNM of \spi_sr_in_buf[7]_i_4\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \spi_sr_out_buf[7]_i_3\ : label is "soft_lutpair27";
  attribute SOFT_HLUTNM of \spi_sr_out_buf[7]_i_4\ : label is "soft_lutpair22";
  attribute X_INTERFACE_INFO of \spi_ss_o_reg[0]\ : label is "xilinx.com:interface:spi_rtl:1.0 spi SS_O";
begin
  s_axi_bvalid <= \^s_axi_bvalid\;
  s_axi_rvalid_reg_0 <= \^s_axi_rvalid_reg_0\;
  s_axi_wready <= \^s_axi_wready\;
  spi_io1_o <= \^spi_io1_o\;
  spi_ss_o(0) <= \^spi_ss_o\(0);
\FSM_onehot_spi_fsm_state[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EAEAEAEAFFEAEAEA"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state[2]_i_2_n_0\,
      I1 => \spi_io_mode_q1__8\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      I3 => \spi_io_o1__3\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I5 => \spi_io_o[0]_i_5_n_0\,
      O => spi_fsm_state
    );
\FSM_onehot_spi_fsm_state[2]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"F200"
    )
        port map (
      I0 => spi_rd_req,
      I1 => spi_rd_req_ack_reg_n_0,
      I2 => spi_sr_out_buf_valid,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      O => \FSM_onehot_spi_fsm_state[2]_i_2_n_0\
    );
\FSM_onehot_spi_fsm_state[2]_i_3\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"90"
    )
        port map (
      I0 => spi_sck_int,
      I1 => \spi_mode_reg_n_0_[0]\,
      I2 => spi_sck_toggle,
      O => \spi_io_mode_q1__8\
    );
\FSM_onehot_spi_fsm_state_reg[0]\: unisim.vcomponents.FDSE
    generic map(
      INIT => '1'
    )
        port map (
      C => aclk,
      CE => spi_fsm_state,
      D => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      Q => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      S => \spi_io_o[0]_i_1_n_0\
    );
\FSM_onehot_spi_fsm_state_reg[1]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => aclk,
      CE => spi_fsm_state,
      D => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      Q => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\FSM_onehot_spi_fsm_state_reg[2]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => aclk,
      CE => spi_fsm_state,
      D => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      Q => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
s_axi_arready_INST_0: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => \^s_axi_rvalid_reg_0\,
      O => s_axi_arready
    );
s_axi_awready_i_1: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => aresetn,
      O => s_axi_awready_i_1_n_0
    );
s_axi_awready_i_2: unisim.vcomponents.LUT5
    generic map(
      INIT => X"08080008"
    )
        port map (
      I0 => s_axi_wvalid,
      I1 => s_axi_awvalid,
      I2 => \^s_axi_wready\,
      I3 => \^s_axi_bvalid\,
      I4 => s_axi_bready,
      O => s_axi_awready0
    );
s_axi_awready_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => s_axi_awready0,
      Q => \^s_axi_wready\,
      R => s_axi_awready_i_1_n_0
    );
s_axi_bvalid_i_1: unisim.vcomponents.LUT3
    generic map(
      INIT => X"BA"
    )
        port map (
      I0 => \^s_axi_wready\,
      I1 => s_axi_bready,
      I2 => \^s_axi_bvalid\,
      O => s_axi_bvalid_i_1_n_0
    );
s_axi_bvalid_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => s_axi_bvalid_i_1_n_0,
      Q => \^s_axi_bvalid\,
      R => s_axi_awready_i_1_n_0
    );
\s_axi_rdata[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFF642000006420"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => spi_sck_div(0),
      I3 => spi_sck_div_en_reg_n_0,
      I4 => s_axi_araddr(2),
      I5 => \s_axi_rdata[0]_i_2_n_0\,
      O => s_axi_rdata_1(0)
    );
\s_axi_rdata[0]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"CAFFCAF0CA0FCA00"
    )
        port map (
      I0 => data3(0),
      I1 => \^spi_ss_o\(0),
      I2 => s_axi_araddr(1),
      I3 => s_axi_araddr(0),
      I4 => spi_sr_out_buf(0),
      I5 => \spi_sr_reg_n_0_[0]\,
      O => \s_axi_rdata[0]_i_2_n_0\
    );
\s_axi_rdata[16]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0023"
    )
        port map (
      I0 => spi_sr_out_buf_valid,
      I1 => s_axi_araddr(2),
      I2 => s_axi_araddr(0),
      I3 => s_axi_araddr(1),
      O => s_axi_rdata_1(16)
    );
\s_axi_rdata[17]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0008"
    )
        port map (
      I0 => spi_sr_in_buf_valid_reg_n_0,
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => s_axi_araddr(2),
      O => s_axi_rdata_1(17)
    );
\s_axi_rdata[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF14100400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(1),
      I2 => s_axi_araddr(0),
      I3 => spi_sck_div(1),
      I4 => spi_en,
      I5 => \s_axi_rdata[1]_i_2_n_0\,
      O => s_axi_rdata_1(1)
    );
\s_axi_rdata[1]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(1),
      I3 => \spi_sr_reg_n_0_[1]\,
      I4 => spi_sr_out_buf(1),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[1]_i_2_n_0\
    );
\s_axi_rdata[20]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      O => s_axi_rdata_1(20)
    );
\s_axi_rdata[21]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => spi_dir_reg_n_0,
      O => s_axi_rdata_1(21)
    );
\s_axi_rdata[22]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => \spi_fsm_state_reg_n_0_[0]\,
      O => s_axi_rdata_1(22)
    );
\s_axi_rdata[23]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => \spi_fsm_state_reg_n_0_[1]\,
      O => s_axi_rdata_1(23)
    );
\s_axi_rdata[28]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      O => s_axi_rdata_1(28)
    );
\s_axi_rdata[29]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => \spi_io_mode_q_reg_n_0_[1]\,
      O => s_axi_rdata_1(29)
    );
\s_axi_rdata[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF14100400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(1),
      I2 => s_axi_araddr(0),
      I3 => spi_sck_div(2),
      I4 => \spi_mode_reg_n_0_[0]\,
      I5 => \s_axi_rdata[2]_i_2_n_0\,
      O => s_axi_rdata_1(2)
    );
\s_axi_rdata[2]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(2),
      I3 => \spi_sr_reg_n_0_[2]\,
      I4 => spi_sr_out_buf(2),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[2]_i_2_n_0\
    );
\s_axi_rdata[30]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      O => s_axi_rdata_1(30)
    );
\s_axi_rdata[31]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"D0FF"
    )
        port map (
      I0 => s_axi_arvalid,
      I1 => \^s_axi_rvalid_reg_0\,
      I2 => s_axi_rready,
      I3 => aresetn,
      O => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata[31]_i_2\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"2"
    )
        port map (
      I0 => s_axi_arvalid,
      I1 => \^s_axi_rvalid_reg_0\,
      O => s_axi_rd_en
    );
\s_axi_rdata[31]_i_3\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0080"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => spi_sr_in_buf_valid_reg_n_0,
      I2 => s_axi_araddr(0),
      I3 => s_axi_araddr(1),
      O => s_axi_rdata_1(31)
    );
\s_axi_rdata[3]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF14100400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(1),
      I2 => s_axi_araddr(0),
      I3 => spi_sck_div(3),
      I4 => spi_cpol,
      I5 => \s_axi_rdata[3]_i_2_n_0\,
      O => s_axi_rdata_1(3)
    );
\s_axi_rdata[3]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(3),
      I3 => \spi_sr_reg_n_0_[3]\,
      I4 => spi_sr_out_buf(3),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[3]_i_2_n_0\
    );
\s_axi_rdata[4]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF14100400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(1),
      I2 => s_axi_araddr(0),
      I3 => spi_sck_div(4),
      I4 => spi_io_mode(0),
      I5 => \s_axi_rdata[4]_i_2_n_0\,
      O => s_axi_rdata_1(4)
    );
\s_axi_rdata[4]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(4),
      I3 => spi_sr(4),
      I4 => spi_sr_out_buf(4),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[4]_i_2_n_0\
    );
\s_axi_rdata[5]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF14100400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(1),
      I2 => s_axi_araddr(0),
      I3 => spi_sck_div(5),
      I4 => spi_io_mode(1),
      I5 => \s_axi_rdata[5]_i_2_n_0\,
      O => s_axi_rdata_1(5)
    );
\s_axi_rdata[5]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(5),
      I3 => spi_sr(5),
      I4 => spi_sr_out_buf(5),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[5]_i_2_n_0\
    );
\s_axi_rdata[6]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF14100400"
    )
        port map (
      I0 => s_axi_araddr(2),
      I1 => s_axi_araddr(1),
      I2 => s_axi_araddr(0),
      I3 => spi_sck_div(6),
      I4 => spi_io_mode(2),
      I5 => \s_axi_rdata[6]_i_2_n_0\,
      O => s_axi_rdata_1(6)
    );
\s_axi_rdata[6]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(6),
      I3 => \spi_sr_reg_n_0_[6]\,
      I4 => spi_sr_out_buf(6),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[6]_i_2_n_0\
    );
\s_axi_rdata[7]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"AAAAAEAA"
    )
        port map (
      I0 => \s_axi_rdata[7]_i_2_n_0\,
      I1 => spi_sck_div(7),
      I2 => s_axi_araddr(0),
      I3 => s_axi_araddr(1),
      I4 => s_axi_araddr(2),
      O => s_axi_rdata_1(7)
    );
\s_axi_rdata[7]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7351624000000000"
    )
        port map (
      I0 => s_axi_araddr(1),
      I1 => s_axi_araddr(0),
      I2 => data3(7),
      I3 => p_1_in(0),
      I4 => spi_sr_out_buf(7),
      I5 => s_axi_araddr(2),
      O => \s_axi_rdata[7]_i_2_n_0\
    );
\s_axi_rdata[8]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"03080008"
    )
        port map (
      I0 => spi_lsb_first_reg_n_0,
      I1 => s_axi_araddr(0),
      I2 => s_axi_araddr(1),
      I3 => s_axi_araddr(2),
      I4 => spi_sr_out_buf_valid,
      O => s_axi_rdata_1(8)
    );
\s_axi_rdata[9]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0008"
    )
        port map (
      I0 => spi_rd_req,
      I1 => s_axi_araddr(2),
      I2 => s_axi_araddr(1),
      I3 => s_axi_araddr(0),
      O => s_axi_rdata_1(9)
    );
\s_axi_rdata_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(0),
      Q => s_axi_rdata(0),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[16]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(16),
      Q => s_axi_rdata(10),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[17]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(17),
      Q => s_axi_rdata(11),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(1),
      Q => s_axi_rdata(1),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[20]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(20),
      Q => s_axi_rdata(12),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[21]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(21),
      Q => s_axi_rdata(13),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[22]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(22),
      Q => s_axi_rdata(14),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[23]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(23),
      Q => s_axi_rdata(15),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[28]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(28),
      Q => s_axi_rdata(16),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[29]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(29),
      Q => s_axi_rdata(17),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(2),
      Q => s_axi_rdata(2),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[30]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(30),
      Q => s_axi_rdata(18),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[31]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(31),
      Q => s_axi_rdata(19),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(3),
      Q => s_axi_rdata(3),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(4),
      Q => s_axi_rdata(4),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(5),
      Q => s_axi_rdata(5),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(6),
      Q => s_axi_rdata(6),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(7),
      Q => s_axi_rdata(7),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(8),
      Q => s_axi_rdata(8),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
\s_axi_rdata_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => s_axi_rd_en,
      D => s_axi_rdata_1(9),
      Q => s_axi_rdata(9),
      R => \s_axi_rdata[31]_i_1_n_0\
    );
s_axi_rvalid_i_1: unisim.vcomponents.LUT3
    generic map(
      INIT => X"4E"
    )
        port map (
      I0 => \^s_axi_rvalid_reg_0\,
      I1 => s_axi_arvalid,
      I2 => s_axi_rready,
      O => s_axi_rvalid_i_1_n_0
    );
s_axi_rvalid_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => s_axi_rvalid_i_1_n_0,
      Q => \^s_axi_rvalid_reg_0\,
      R => s_axi_awready_i_1_n_0
    );
spi_dir_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0F0FBFBF0F000400"
    )
        port map (
      I0 => \spi_io_mode_q[2]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf_valid,
      I3 => \spi_io_mode_q0__0\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I5 => spi_dir_reg_n_0,
      O => spi_dir_i_1_n_0
    );
spi_dir_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_dir_i_1_n_0,
      Q => spi_dir_reg_n_0,
      R => \spi_io_o[0]_i_1_n_0\
    );
spi_en_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FBFBFBFF08080800"
    )
        port map (
      I0 => s_axi_wdata(1),
      I1 => \spi_mode[1]_i_2_n_0\,
      I2 => \spi_ss_o[0]_i_2_n_0\,
      I3 => spi_sck_div_en_reg_n_0,
      I4 => s_axi_wdata(0),
      I5 => spi_en,
      O => spi_en_i_1_n_0
    );
spi_en_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_en_i_1_n_0,
      Q => spi_en,
      R => s_axi_awready_i_1_n_0
    );
\spi_fsm_state[0]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => \spi_fsm_state_reg_n_0_[0]\,
      I1 => \spi_fsm_state_reg_n_0_[1]\,
      O => \spi_fsm_state[0]_i_1_n_0\
    );
\spi_fsm_state[1]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"2"
    )
        port map (
      I0 => \spi_fsm_state_reg_n_0_[0]\,
      I1 => \spi_fsm_state_reg_n_0_[1]\,
      O => \spi_fsm_state[1]_i_1_n_0\
    );
\spi_fsm_state_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_fsm_state,
      D => \spi_fsm_state[0]_i_1_n_0\,
      Q => \spi_fsm_state_reg_n_0_[0]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_fsm_state_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_fsm_state,
      D => \spi_fsm_state[1]_i_1_n_0\,
      Q => \spi_fsm_state_reg_n_0_[1]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_mode[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFDF00000008"
    )
        port map (
      I0 => \spi_mode[1]_i_2_n_0\,
      I1 => s_axi_wdata(4),
      I2 => s_axi_wdata(5),
      I3 => \spi_ss_o[0]_i_2_n_0\,
      I4 => s_axi_wdata(6),
      I5 => spi_io_mode(0),
      O => \spi_io_mode[0]_i_1_n_0\
    );
\spi_io_mode[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFF700000020"
    )
        port map (
      I0 => \spi_mode[1]_i_2_n_0\,
      I1 => s_axi_wdata(4),
      I2 => s_axi_wdata(5),
      I3 => \spi_ss_o[0]_i_2_n_0\,
      I4 => s_axi_wdata(6),
      I5 => spi_io_mode(1),
      O => \spi_io_mode[1]_i_1_n_0\
    );
\spi_io_mode[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFD700000000"
    )
        port map (
      I0 => \spi_mode[1]_i_2_n_0\,
      I1 => s_axi_wdata(4),
      I2 => s_axi_wdata(5),
      I3 => \spi_ss_o[0]_i_2_n_0\,
      I4 => s_axi_wdata(6),
      I5 => spi_io_mode(2),
      O => \spi_io_mode[2]_i_1_n_0\
    );
\spi_io_mode_q[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AAFFEFFFAA002000"
    )
        port map (
      I0 => spi_io_mode(0),
      I1 => \spi_io_mode_q[2]_i_2_n_0\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I3 => \spi_io_mode_q[2]_i_3_n_0\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I5 => \spi_io_mode_q_reg_n_0_[0]\,
      O => \spi_io_mode_q[0]_i_1_n_0\
    );
\spi_io_mode_q[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AAFFEFFFAA002000"
    )
        port map (
      I0 => spi_io_mode(1),
      I1 => \spi_io_mode_q[2]_i_2_n_0\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I3 => \spi_io_mode_q[2]_i_3_n_0\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I5 => \spi_io_mode_q_reg_n_0_[1]\,
      O => \spi_io_mode_q[1]_i_1_n_0\
    );
\spi_io_mode_q[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AAFFEFFFAA002000"
    )
        port map (
      I0 => spi_io_mode(2),
      I1 => \spi_io_mode_q[2]_i_2_n_0\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I3 => \spi_io_mode_q[2]_i_3_n_0\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I5 => \spi_io_mode_q_reg_n_0_[2]\,
      O => \spi_io_mode_q[2]_i_1_n_0\
    );
\spi_io_mode_q[2]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"7DFF"
    )
        port map (
      I0 => spi_sck_toggle,
      I1 => \spi_mode_reg_n_0_[0]\,
      I2 => spi_sck_int,
      I3 => \spi_sr_bit_cnt[3]_i_4_n_0\,
      O => \spi_io_mode_q[2]_i_2_n_0\
    );
\spi_io_mode_q[2]_i_3\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"BA"
    )
        port map (
      I0 => spi_sr_out_buf_valid,
      I1 => spi_rd_req_ack_reg_n_0,
      I2 => spi_rd_req,
      O => \spi_io_mode_q[2]_i_3_n_0\
    );
\spi_io_mode_q_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_mode_q[0]_i_1_n_0\,
      Q => \spi_io_mode_q_reg_n_0_[0]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_mode_q_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_mode_q[1]_i_1_n_0\,
      Q => \spi_io_mode_q_reg_n_0_[1]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_mode_q_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_mode_q[2]_i_1_n_0\,
      Q => \spi_io_mode_q_reg_n_0_[2]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_mode_reg[0]\: unisim.vcomponents.FDSE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_mode[0]_i_1_n_0\,
      Q => spi_io_mode(0),
      S => s_axi_awready_i_1_n_0
    );
\spi_io_mode_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_mode[1]_i_1_n_0\,
      Q => spi_io_mode(1),
      R => s_axi_awready_i_1_n_0
    );
\spi_io_mode_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_mode[2]_i_1_n_0\,
      Q => spi_io_mode(2),
      R => s_axi_awready_i_1_n_0
    );
\spi_io_o[0]_i_1\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => spi_en,
      O => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_o[0]_i_2\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"F8"
    )
        port map (
      I0 => \spi_io_o1__3\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      O => \spi_io_o[0]_i_2_n_0\
    );
\spi_io_o[0]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"88888000"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I1 => \spi_io_o[0]_i_5_n_0\,
      I2 => \spi_sr_reg_n_0_[0]\,
      I3 => spi_lsb_first_reg_n_0,
      I4 => \spi_io_o[0]_i_6_n_0\,
      O => spi_io_o(0)
    );
\spi_io_o[0]_i_4\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"28"
    )
        port map (
      I0 => spi_sck_toggle,
      I1 => spi_sck_int,
      I2 => \spi_mode_reg_n_0_[0]\,
      O => \spi_io_o1__3\
    );
\spi_io_o[0]_i_5\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFFFFFFE"
    )
        port map (
      I0 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      I1 => \spi_sr_bit_cnt_reg_n_0_[1]\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[2]\,
      I3 => \spi_sr_bit_cnt_reg_n_0_[4]\,
      I4 => \spi_sr_bit_cnt_reg_n_0_[3]\,
      O => \spi_io_o[0]_i_5_n_0\
    );
\spi_io_o[0]_i_6\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000020203000000"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[6]\,
      I1 => spi_lsb_first_reg_n_0,
      I2 => \spi_io_mode_q_reg_n_0_[2]\,
      I3 => p_1_in(0),
      I4 => \spi_io_mode_q_reg_n_0_[0]\,
      I5 => \spi_io_mode_q_reg_n_0_[1]\,
      O => \spi_io_o[0]_i_6_n_0\
    );
\spi_io_o[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AAAABFFFAAAA8000"
    )
        port map (
      I0 => spi_io_o(1),
      I1 => \spi_io_o[1]_i_3_n_0\,
      I2 => \spi_io_o1__3\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I5 => \^spi_io1_o\,
      O => \spi_io_o[1]_i_1_n_0\
    );
\spi_io_o[1]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"80808080808C8080"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[1]\,
      I1 => spi_sck_oe,
      I2 => spi_lsb_first_reg_n_0,
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => p_1_in(0),
      I5 => \spi_io_t[1]_i_2_n_0\,
      O => spi_io_o(1)
    );
\spi_io_o[1]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FEFFFFFF"
    )
        port map (
      I0 => spi_lsb_first_reg_n_0,
      I1 => \spi_io_mode_q_reg_n_0_[2]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_io_o[0]_i_5_n_0\,
      O => \spi_io_o[1]_i_3_n_0\
    );
\spi_io_o_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_io_o[0]_i_2_n_0\,
      D => spi_io_o(0),
      Q => spi_io0_o,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_o_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_io_o[1]_i_1_n_0\,
      Q => \^spi_io1_o\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_t[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFFFFD50000"
    )
        port map (
      I0 => \spi_io_o[0]_i_5_n_0\,
      I1 => \spi_io_mode_q_reg_n_0_[1]\,
      I2 => spi_dir_reg_n_0,
      I3 => \spi_io_t[0]_i_2_n_0\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I5 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      O => spi_io_t(0)
    );
\spi_io_t[0]_i_2\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"EB"
    )
        port map (
      I0 => \spi_io_mode_q_reg_n_0_[2]\,
      I1 => \spi_io_mode_q_reg_n_0_[0]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      O => \spi_io_t[0]_i_2_n_0\
    );
\spi_io_t[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFFFEFF0000"
    )
        port map (
      I0 => \spi_io_t[1]_i_2_n_0\,
      I1 => \spi_io_mode_q_reg_n_0_[2]\,
      I2 => spi_dir_reg_n_0,
      I3 => \spi_io_o[0]_i_5_n_0\,
      I4 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I5 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      O => spi_io_t(1)
    );
\spi_io_t[1]_i_2\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"B"
    )
        port map (
      I0 => \spi_io_mode_q_reg_n_0_[0]\,
      I1 => \spi_io_mode_q_reg_n_0_[1]\,
      O => \spi_io_t[1]_i_2_n_0\
    );
\spi_io_t_reg[0]\: unisim.vcomponents.FDSE
     port map (
      C => aclk,
      CE => \spi_io_o[0]_i_2_n_0\,
      D => spi_io_t(0),
      Q => Q(0),
      S => \spi_io_o[0]_i_1_n_0\
    );
\spi_io_t_reg[1]\: unisim.vcomponents.FDSE
     port map (
      C => aclk,
      CE => \spi_io_o[0]_i_2_n_0\,
      D => spi_io_t(1),
      Q => Q(1),
      S => \spi_io_o[0]_i_1_n_0\
    );
spi_lsb_first_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"BFFFFFFF80000000"
    )
        port map (
      I0 => s_axi_wdata(8),
      I1 => s_axi_awaddr(0),
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => s_axi_wstrb(1),
      I4 => \spi_mode[1]_i_2_n_0\,
      I5 => spi_lsb_first_reg_n_0,
      O => spi_lsb_first_i_1_n_0
    );
spi_lsb_first_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_lsb_first_i_1_n_0,
      Q => spi_lsb_first_reg_n_0,
      R => s_axi_awready_i_1_n_0
    );
\spi_mode[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"BFFFFFFF80000000"
    )
        port map (
      I0 => s_axi_wdata(2),
      I1 => s_axi_awaddr(0),
      I2 => s_axi_wstrb(0),
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I4 => \spi_mode[1]_i_2_n_0\,
      I5 => \spi_mode_reg_n_0_[0]\,
      O => \spi_mode[0]_i_1_n_0\
    );
\spi_mode[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"BFFFFFFF80000000"
    )
        port map (
      I0 => s_axi_wdata(3),
      I1 => s_axi_awaddr(0),
      I2 => s_axi_wstrb(0),
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I4 => \spi_mode[1]_i_2_n_0\,
      I5 => spi_cpol,
      O => \spi_mode[1]_i_1_n_0\
    );
\spi_mode[1]_i_2\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"02"
    )
        port map (
      I0 => \^s_axi_wready\,
      I1 => s_axi_awaddr(2),
      I2 => s_axi_awaddr(1),
      O => \spi_mode[1]_i_2_n_0\
    );
\spi_mode_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_mode[0]_i_1_n_0\,
      Q => \spi_mode_reg_n_0_[0]\,
      R => s_axi_awready_i_1_n_0
    );
\spi_mode_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_mode[1]_i_1_n_0\,
      Q => spi_cpol,
      R => s_axi_awready_i_1_n_0
    );
spi_rd_req_ack_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEAAFEFAFC00CC00"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I3 => \spi_rd_req_ack0__0\,
      I4 => \spi_io_mode_q[2]_i_2_n_0\,
      I5 => spi_rd_req_ack_i_3_n_0,
      O => spi_rd_req_ack
    );
spi_rd_req_ack_i_2: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => spi_rd_req,
      I1 => spi_rd_req_ack_reg_n_0,
      O => \spi_rd_req_ack0__0\
    );
spi_rd_req_ack_i_3: unisim.vcomponents.LUT3
    generic map(
      INIT => X"D0"
    )
        port map (
      I0 => spi_sr_out_buf_valid,
      I1 => spi_rd_req_ack_reg_n_0,
      I2 => spi_rd_req,
      O => spi_rd_req_ack_i_3_n_0
    );
spi_rd_req_ack_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_rd_req_ack,
      Q => spi_rd_req_ack_reg_n_0,
      R => \spi_io_o[0]_i_1_n_0\
    );
spi_rd_req_i_1: unisim.vcomponents.LUT5
    generic map(
      INIT => X"8F888888"
    )
        port map (
      I0 => spi_rd_req_i_2_n_0,
      I1 => \^s_axi_wready\,
      I2 => spi_rd_req_ack_reg_n_0,
      I3 => spi_rd_req,
      I4 => spi_en,
      O => spi_rd_req_i_1_n_0
    );
spi_rd_req_i_2: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0400"
    )
        port map (
      I0 => s_axi_wdata(8),
      I1 => s_axi_wdata(9),
      I2 => spi_rd_req,
      I3 => \spi_sr_out_buf[7]_i_5_n_0\,
      O => spi_rd_req_i_2_n_0
    );
spi_rd_req_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_rd_req_i_1_n_0,
      Q => spi_rd_req,
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div[7]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0010000000000000"
    )
        port map (
      I0 => s_axi_awaddr(2),
      I1 => spi_sck_div_en_reg_n_0,
      I2 => s_axi_awaddr(1),
      I3 => s_axi_awaddr(0),
      I4 => \^s_axi_wready\,
      I5 => s_axi_wstrb(0),
      O => spi_sck_div_0
    );
\spi_sck_div_cnt[0]_i_1\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(0),
      O => p_0_in(0)
    );
\spi_sck_div_cnt[1]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"6"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(0),
      I1 => spi_sck_div_cnt_reg(1),
      O => p_0_in(1)
    );
\spi_sck_div_cnt[2]_i_1\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"78"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(0),
      I1 => spi_sck_div_cnt_reg(1),
      I2 => spi_sck_div_cnt_reg(2),
      O => p_0_in(2)
    );
\spi_sck_div_cnt[3]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"7F80"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(1),
      I1 => spi_sck_div_cnt_reg(0),
      I2 => spi_sck_div_cnt_reg(2),
      I3 => spi_sck_div_cnt_reg(3),
      O => p_0_in(3)
    );
\spi_sck_div_cnt[4]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"7FFF8000"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(2),
      I1 => spi_sck_div_cnt_reg(0),
      I2 => spi_sck_div_cnt_reg(1),
      I3 => spi_sck_div_cnt_reg(3),
      I4 => spi_sck_div_cnt_reg(4),
      O => p_0_in(4)
    );
\spi_sck_div_cnt[5]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"7FFFFFFF80000000"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(3),
      I1 => spi_sck_div_cnt_reg(1),
      I2 => spi_sck_div_cnt_reg(0),
      I3 => spi_sck_div_cnt_reg(2),
      I4 => spi_sck_div_cnt_reg(4),
      I5 => spi_sck_div_cnt_reg(5),
      O => p_0_in(5)
    );
\spi_sck_div_cnt[6]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"6"
    )
        port map (
      I0 => \spi_sck_div_cnt[7]_i_4_n_0\,
      I1 => spi_sck_div_cnt_reg(6),
      O => p_0_in(6)
    );
\spi_sck_div_cnt[7]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"B"
    )
        port map (
      I0 => spi_sck_toggle,
      I1 => spi_sck_div_en_reg_n_0,
      O => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt[7]_i_2\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"78"
    )
        port map (
      I0 => \spi_sck_div_cnt[7]_i_4_n_0\,
      I1 => spi_sck_div_cnt_reg(6),
      I2 => spi_sck_div_cnt_reg(7),
      O => p_0_in(7)
    );
\spi_sck_div_cnt[7]_i_3\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0002"
    )
        port map (
      I0 => \spi_sck_div_cnt[7]_i_5_n_0\,
      I1 => \spi_sck_div_cnt[7]_i_6_n_0\,
      I2 => \spi_sck_div_cnt[7]_i_7_n_0\,
      I3 => \spi_sck_div_cnt[7]_i_8_n_0\,
      O => spi_sck_toggle
    );
\spi_sck_div_cnt[7]_i_4\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"8000000000000000"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(5),
      I1 => spi_sck_div_cnt_reg(3),
      I2 => spi_sck_div_cnt_reg(1),
      I3 => spi_sck_div_cnt_reg(0),
      I4 => spi_sck_div_cnt_reg(2),
      I5 => spi_sck_div_cnt_reg(4),
      O => \spi_sck_div_cnt[7]_i_4_n_0\
    );
\spi_sck_div_cnt[7]_i_5\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"82000082"
    )
        port map (
      I0 => spi_sck_div_en_reg_n_0,
      I1 => spi_sck_div(6),
      I2 => spi_sck_div_cnt_reg(6),
      I3 => spi_sck_div(7),
      I4 => spi_sck_div_cnt_reg(7),
      O => \spi_sck_div_cnt[7]_i_5_n_0\
    );
\spi_sck_div_cnt[7]_i_6\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"6FF6"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(5),
      I1 => spi_sck_div(5),
      I2 => spi_sck_div_cnt_reg(2),
      I3 => spi_sck_div(2),
      O => \spi_sck_div_cnt[7]_i_6_n_0\
    );
\spi_sck_div_cnt[7]_i_7\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"6FF6"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(1),
      I1 => spi_sck_div(1),
      I2 => spi_sck_div_cnt_reg(0),
      I3 => spi_sck_div(0),
      O => \spi_sck_div_cnt[7]_i_7_n_0\
    );
\spi_sck_div_cnt[7]_i_8\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"6FF6"
    )
        port map (
      I0 => spi_sck_div_cnt_reg(4),
      I1 => spi_sck_div(4),
      I2 => spi_sck_div_cnt_reg(3),
      I3 => spi_sck_div(3),
      O => \spi_sck_div_cnt[7]_i_8_n_0\
    );
\spi_sck_div_cnt_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(0),
      Q => spi_sck_div_cnt_reg(0),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(1),
      Q => spi_sck_div_cnt_reg(1),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(2),
      Q => spi_sck_div_cnt_reg(2),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(3),
      Q => spi_sck_div_cnt_reg(3),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(4),
      Q => spi_sck_div_cnt_reg(4),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(5),
      Q => spi_sck_div_cnt_reg(5),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(6),
      Q => spi_sck_div_cnt_reg(6),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
\spi_sck_div_cnt_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => p_0_in(7),
      Q => spi_sck_div_cnt_reg(7),
      R => \spi_sck_div_cnt[7]_i_1_n_0\
    );
spi_sck_div_en_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFBFBFB00080808"
    )
        port map (
      I0 => s_axi_wdata(0),
      I1 => \spi_mode[1]_i_2_n_0\,
      I2 => \spi_ss_o[0]_i_2_n_0\,
      I3 => spi_en,
      I4 => s_axi_wdata(1),
      I5 => spi_sck_div_en_reg_n_0,
      O => spi_sck_div_en_i_1_n_0
    );
spi_sck_div_en_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_sck_div_en_i_1_n_0,
      Q => spi_sck_div_en_reg_n_0,
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(0),
      Q => spi_sck_div(0),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(1),
      Q => spi_sck_div(1),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(2),
      Q => spi_sck_div(2),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(3),
      Q => spi_sck_div(3),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(4),
      Q => spi_sck_div(4),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(5),
      Q => spi_sck_div(5),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(6),
      Q => spi_sck_div(6),
      R => s_axi_awready_i_1_n_0
    );
\spi_sck_div_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sck_div_0,
      D => s_axi_wdata(7),
      Q => spi_sck_div(7),
      R => s_axi_awready_i_1_n_0
    );
spi_sck_int_i_1: unisim.vcomponents.LUT3
    generic map(
      INIT => X"60"
    )
        port map (
      I0 => spi_sck_int,
      I1 => spi_sck_toggle,
      I2 => spi_sck_div_en_reg_n_0,
      O => spi_sck_int_i_1_n_0
    );
spi_sck_int_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_sck_int_i_1_n_0,
      Q => spi_sck_int,
      R => '0'
    );
spi_sck_o_INST_0: unisim.vcomponents.LUT3
    generic map(
      INIT => X"6A"
    )
        port map (
      I0 => spi_cpol,
      I1 => spi_sck_oe_reg_n_0,
      I2 => spi_sck_int,
      O => spi_sck_o
    );
spi_sck_oe_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFE00000000"
    )
        port map (
      I0 => \spi_sr_bit_cnt_reg_n_0_[3]\,
      I1 => \spi_sr_bit_cnt_reg_n_0_[4]\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[2]\,
      I3 => \spi_sr_bit_cnt_reg_n_0_[1]\,
      I4 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      I5 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      O => spi_sck_oe
    );
spi_sck_oe_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_io_o[0]_i_2_n_0\,
      D => spi_sck_oe,
      Q => spi_sck_oe_reg_n_0,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[0]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(0),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(0),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[0]_i_1_n_0\
    );
\spi_sr[0]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(0),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[0]_i_2_n_0\
    );
\spi_sr[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[1]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(1),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(1),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[1]_i_1_n_0\
    );
\spi_sr[1]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(1),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[1]_i_2_n_0\
    );
\spi_sr[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[2]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(2),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(2),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[2]_i_1_n_0\
    );
\spi_sr[2]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(2),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[2]_i_2_n_0\
    );
\spi_sr[3]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[3]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(3),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(3),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[3]_i_1_n_0\
    );
\spi_sr[3]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(3),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[3]_i_2_n_0\
    );
\spi_sr[4]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[4]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(4),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(4),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[4]_i_1_n_0\
    );
\spi_sr[4]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(4),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[4]_i_2_n_0\
    );
\spi_sr[5]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[5]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(5),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(5),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[5]_i_1_n_0\
    );
\spi_sr[5]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(5),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[5]_i_2_n_0\
    );
\spi_sr[6]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[6]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(6),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(6),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[6]_i_1_n_0\
    );
\spi_sr[6]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(6),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[6]_i_2_n_0\
    );
\spi_sr[7]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"EEEEEAAAEAAAEAAA"
    )
        port map (
      I0 => \spi_sr[7]_i_2_n_0\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf(7),
      I3 => \spi_sr[7]_i_3_n_0\,
      I4 => spi_sr_in_buf1_in(7),
      I5 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      O => \spi_sr[7]_i_1_n_0\
    );
\spi_sr[7]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FF80"
    )
        port map (
      I0 => spi_sr_out_buf(7),
      I1 => spi_sr_out_buf_valid,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I3 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      O => \spi_sr[7]_i_2_n_0\
    );
\spi_sr[7]_i_3\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \spi_sr_bit_cnt[3]_i_4_n_0\,
      I1 => spi_sr_out_buf_valid,
      O => \spi_sr[7]_i_3_n_0\
    );
\spi_sr_bit_cnt[0]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0880"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I1 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      I2 => \spi_io_mode_q_reg_n_0_[0]\,
      I3 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      O => \spi_sr_bit_cnt[0]_i_1_n_0\
    );
\spi_sr_bit_cnt[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0880800808800880"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I1 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[1]\,
      I4 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      I5 => \spi_io_mode_q_reg_n_0_[0]\,
      O => spi_sr_bit_cnt(1)
    );
\spi_sr_bit_cnt[2]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"08808008"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I1 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[2]\,
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_sr_bit_cnt[2]_i_2_n_0\,
      O => spi_sr_bit_cnt(2)
    );
\spi_sr_bit_cnt[2]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"DF45"
    )
        port map (
      I0 => \spi_io_mode_q_reg_n_0_[1]\,
      I1 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      I2 => \spi_io_mode_q_reg_n_0_[0]\,
      I3 => \spi_sr_bit_cnt_reg_n_0_[1]\,
      O => \spi_sr_bit_cnt[2]_i_2_n_0\
    );
\spi_sr_bit_cnt[3]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFEEEEEAAAAAAAA"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I1 => \spi_sr_bit_cnt[3]_i_2_n_0\,
      I2 => \spi_io_mode_q0__0\,
      I3 => spi_sr_out_buf_valid,
      I4 => \spi_sr_bit_cnt[3]_i_4_n_0\,
      I5 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      O => spi_sr_bit_cnt(3)
    );
\spi_sr_bit_cnt[3]_i_2\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"8E71"
    )
        port map (
      I0 => \spi_sr_bit_cnt_reg_n_0_[2]\,
      I1 => \spi_sr_bit_cnt[2]_i_2_n_0\,
      I2 => \spi_io_mode_q_reg_n_0_[2]\,
      I3 => \spi_sr_bit_cnt_reg_n_0_[3]\,
      O => \spi_sr_bit_cnt[3]_i_2_n_0\
    );
\spi_sr_bit_cnt[3]_i_3\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"2"
    )
        port map (
      I0 => spi_rd_req,
      I1 => spi_rd_req_ack_reg_n_0,
      O => \spi_io_mode_q0__0\
    );
\spi_sr_bit_cnt[3]_i_4\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"82"
    )
        port map (
      I0 => \spi_sr_bit_cnt[3]_i_5_n_0\,
      I1 => \spi_io_mode_q_reg_n_0_[0]\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      O => \spi_sr_bit_cnt[3]_i_4_n_0\
    );
\spi_sr_bit_cnt[3]_i_5\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"1001000000001001"
    )
        port map (
      I0 => \spi_sr_bit_cnt_reg_n_0_[3]\,
      I1 => \spi_sr_bit_cnt_reg_n_0_[4]\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[2]\,
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_io_mode_q_reg_n_0_[1]\,
      I5 => \spi_sr_bit_cnt_reg_n_0_[1]\,
      O => \spi_sr_bit_cnt[3]_i_5_n_0\
    );
\spi_sr_bit_cnt[4]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"F8FFF8F888888888"
    )
        port map (
      I0 => \spi_io_mode_q1__8\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I2 => spi_sr_out_buf_valid,
      I3 => spi_rd_req_ack_reg_n_0,
      I4 => spi_rd_req,
      I5 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      O => \spi_sr_bit_cnt[4]_i_1_n_0\
    );
\spi_sr_bit_cnt[4]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"80808008"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I1 => \spi_sr_bit_cnt[4]_i_3_n_0\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[4]\,
      I3 => \spi_sr_bit_cnt[4]_i_4_n_0\,
      I4 => \spi_sr_bit_cnt_reg_n_0_[3]\,
      O => spi_sr_bit_cnt(4)
    );
\spi_sr_bit_cnt[4]_i_3\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0DFF"
    )
        port map (
      I0 => spi_rd_req,
      I1 => spi_rd_req_ack_reg_n_0,
      I2 => spi_sr_out_buf_valid,
      I3 => \spi_sr_bit_cnt[3]_i_4_n_0\,
      O => \spi_sr_bit_cnt[4]_i_3_n_0\
    );
\spi_sr_bit_cnt[4]_i_4\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"F7FF757751551011"
    )
        port map (
      I0 => \spi_io_mode_q_reg_n_0_[2]\,
      I1 => \spi_io_mode_q_reg_n_0_[1]\,
      I2 => \spi_sr_bit_cnt_reg_n_0_[0]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_bit_cnt_reg_n_0_[1]\,
      I5 => \spi_sr_bit_cnt_reg_n_0_[2]\,
      O => \spi_sr_bit_cnt[4]_i_4_n_0\
    );
\spi_sr_bit_cnt_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr_bit_cnt[0]_i_1_n_0\,
      Q => \spi_sr_bit_cnt_reg_n_0_[0]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_bit_cnt_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => spi_sr_bit_cnt(1),
      Q => \spi_sr_bit_cnt_reg_n_0_[1]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_bit_cnt_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => spi_sr_bit_cnt(2),
      Q => \spi_sr_bit_cnt_reg_n_0_[2]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_bit_cnt_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => spi_sr_bit_cnt(3),
      Q => \spi_sr_bit_cnt_reg_n_0_[3]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_bit_cnt_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => spi_sr_bit_cnt(4),
      Q => \spi_sr_bit_cnt_reg_n_0_[4]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf[0]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(0),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(0),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_sr_reg_n_0_[0]\,
      O => spi_sr_in_buf1_in(0)
    );
\spi_sr_in_buf[0]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"AFCAA0CA"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[0]\,
      I1 => spi_io0_i,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => spi_io1_i,
      O => spi_sr1_in(0)
    );
\spi_sr_in_buf[0]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"AFCAA0CA"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[0]\,
      I1 => \spi_sr_reg_n_0_[2]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[1]\,
      O => spi_sr3_in(0)
    );
\spi_sr_in_buf[1]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(1),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(1),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_sr_reg_n_0_[1]\,
      O => spi_sr_in_buf1_in(1)
    );
\spi_sr_in_buf[1]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"CFACC0AC"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[0]\,
      I1 => \spi_sr_reg_n_0_[1]\,
      I2 => \spi_io_mode_q_reg_n_0_[0]\,
      I3 => \spi_io_mode_q_reg_n_0_[1]\,
      I4 => spi_io1_i,
      O => spi_sr1_in(1)
    );
\spi_sr_in_buf[1]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"AFCAA0CA"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[1]\,
      I1 => \spi_sr_reg_n_0_[3]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[2]\,
      O => spi_sr3_in(1)
    );
\spi_sr_in_buf[2]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(2),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(2),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_sr_reg_n_0_[2]\,
      O => spi_sr_in_buf1_in(2)
    );
\spi_sr_in_buf[2]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[1]\,
      I1 => \spi_sr_reg_n_0_[0]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[2]\,
      O => spi_sr1_in(2)
    );
\spi_sr_in_buf[2]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"AFCAA0CA"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[2]\,
      I1 => spi_sr(4),
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[3]\,
      O => spi_sr3_in(2)
    );
\spi_sr_in_buf[3]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(3),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(3),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_sr_reg_n_0_[3]\,
      O => spi_sr_in_buf1_in(3)
    );
\spi_sr_in_buf[3]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[2]\,
      I1 => \spi_sr_reg_n_0_[1]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[3]\,
      O => spi_sr1_in(3)
    );
\spi_sr_in_buf[3]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"AFCAA0CA"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[3]\,
      I1 => spi_sr(5),
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => spi_sr(4),
      O => spi_sr3_in(3)
    );
\spi_sr_in_buf[4]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(4),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(4),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => spi_sr(4),
      O => spi_sr_in_buf1_in(4)
    );
\spi_sr_in_buf[4]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[3]\,
      I1 => \spi_sr_reg_n_0_[2]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => spi_sr(4),
      O => spi_sr1_in(4)
    );
\spi_sr_in_buf[4]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"CFACC0AC"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[6]\,
      I1 => spi_sr(4),
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => spi_sr(5),
      O => spi_sr3_in(4)
    );
\spi_sr_in_buf[5]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(5),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(5),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => spi_sr(5),
      O => spi_sr_in_buf1_in(5)
    );
\spi_sr_in_buf[5]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => spi_sr(4),
      I1 => \spi_sr_reg_n_0_[3]\,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => spi_sr(5),
      O => spi_sr1_in(5)
    );
\spi_sr_in_buf[5]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"CFACC0AC"
    )
        port map (
      I0 => p_1_in(0),
      I1 => spi_sr(5),
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[6]\,
      O => spi_sr3_in(5)
    );
\spi_sr_in_buf[6]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFE200E2"
    )
        port map (
      I0 => spi_sr1_in(6),
      I1 => spi_lsb_first_reg_n_0,
      I2 => spi_sr3_in(6),
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => \spi_sr_reg_n_0_[6]\,
      O => spi_sr_in_buf1_in(6)
    );
\spi_sr_in_buf[6]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => spi_sr(5),
      I1 => spi_sr(4),
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[6]\,
      O => spi_sr1_in(6)
    );
\spi_sr_in_buf[6]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => p_1_in(0),
      I1 => spi_io0_i,
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_sr_reg_n_0_[6]\,
      O => spi_sr3_in(6)
    );
\spi_sr_in_buf[7]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000AA0002"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I1 => \spi_sr_in_buf[7]_i_3_n_0\,
      I2 => \spi_io_mode_q_reg_n_0_[2]\,
      I3 => spi_sr_in_buf_valid_reg_n_0,
      I4 => spi_dir_reg_n_0,
      I5 => \spi_io_mode_q[2]_i_2_n_0\,
      O => spi_sr_in_buf
    );
\spi_sr_in_buf[7]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFF2F0F2"
    )
        port map (
      I0 => spi_sr1_in(7),
      I1 => spi_lsb_first_reg_n_0,
      I2 => \spi_sr_in_buf[7]_i_5_n_0\,
      I3 => \spi_io_mode_q_reg_n_0_[2]\,
      I4 => p_1_in(0),
      O => spi_sr_in_buf1_in(7)
    );
\spi_sr_in_buf[7]_i_3\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"B"
    )
        port map (
      I0 => \spi_io_mode_q_reg_n_0_[1]\,
      I1 => \spi_io_mode_q_reg_n_0_[0]\,
      O => \spi_sr_in_buf[7]_i_3_n_0\
    );
\spi_sr_in_buf[7]_i_4\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FACF0AC0"
    )
        port map (
      I0 => \spi_sr_reg_n_0_[6]\,
      I1 => spi_sr(5),
      I2 => \spi_io_mode_q_reg_n_0_[1]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => p_1_in(0),
      O => spi_sr1_in(7)
    );
\spi_sr_in_buf[7]_i_5\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"4044444040000040"
    )
        port map (
      I0 => \spi_io_mode_q_reg_n_0_[2]\,
      I1 => spi_lsb_first_reg_n_0,
      I2 => p_1_in(0),
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_io_mode_q_reg_n_0_[1]\,
      I5 => spi_io1_i,
      O => \spi_sr_in_buf[7]_i_5_n_0\
    );
spi_sr_in_buf_ready_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"2000DFFF20000000"
    )
        port map (
      I0 => s_axi_araddr(0),
      I1 => s_axi_araddr(1),
      I2 => s_axi_rd_en,
      I3 => s_axi_araddr(2),
      I4 => spi_sr_in_buf_valid_reg_n_0,
      I5 => spi_sr_in_buf_ready_reg_n_0,
      O => spi_sr_in_buf_ready_i_1_n_0
    );
spi_sr_in_buf_ready_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_sr_in_buf_ready_i_1_n_0,
      Q => spi_sr_in_buf_ready_reg_n_0,
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_in_buf_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(0),
      Q => data3(0),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(1),
      Q => data3(1),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(2),
      Q => data3(2),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(3),
      Q => data3(3),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(4),
      Q => data3(4),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(5),
      Q => data3(5),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(6),
      Q => data3(6),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_in_buf_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => spi_sr_in_buf,
      D => spi_sr_in_buf1_in(7),
      Q => data3(7),
      R => \spi_io_o[0]_i_1_n_0\
    );
spi_sr_in_buf_valid_i_1: unisim.vcomponents.LUT4
    generic map(
      INIT => X"7530"
    )
        port map (
      I0 => spi_sr_in_buf_valid_i_2_n_0,
      I1 => \spi_io_mode_q[2]_i_2_n_0\,
      I2 => spi_sr_in_buf_valid_i_3_n_0,
      I3 => spi_sr_in_buf_valid_reg_n_0,
      O => spi_sr_in_buf_valid_i_1_n_0
    );
spi_sr_in_buf_valid_i_2: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FE000000"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I3 => spi_sr_in_buf_ready_reg_n_0,
      I4 => spi_sr_in_buf_valid_reg_n_0,
      O => spi_sr_in_buf_valid_i_2_n_0
    );
spi_sr_in_buf_valid_i_3: unisim.vcomponents.LUT6
    generic map(
      INIT => X"2222232200000000"
    )
        port map (
      I0 => spi_dir_reg_n_0,
      I1 => spi_sr_in_buf_valid_reg_n_0,
      I2 => \spi_io_mode_q_reg_n_0_[2]\,
      I3 => \spi_io_mode_q_reg_n_0_[0]\,
      I4 => \spi_io_mode_q_reg_n_0_[1]\,
      I5 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      O => spi_sr_in_buf_valid_i_3_n_0
    );
spi_sr_in_buf_valid_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_sr_in_buf_valid_i_1_n_0,
      Q => spi_sr_in_buf_valid_reg_n_0,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_out_buf[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(0),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(0)
    );
\spi_sr_out_buf[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(1),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(1)
    );
\spi_sr_out_buf[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(2),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(2)
    );
\spi_sr_out_buf[3]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(3),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(3)
    );
\spi_sr_out_buf[4]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(4),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(4)
    );
\spi_sr_out_buf[5]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(5),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(5)
    );
\spi_sr_out_buf[6]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(6),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(6)
    );
\spi_sr_out_buf[7]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"00800000FFFFFFFF"
    )
        port map (
      I0 => s_axi_wstrb(0),
      I1 => s_axi_wstrb(1),
      I2 => \spi_sr_out_buf[7]_i_3_n_0\,
      I3 => \spi_sr_out_buf[7]_i_4_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => \spi_sr_out_buf[7]_i_1_n_0\
    );
\spi_sr_out_buf[7]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFE0FFFFFF200000"
    )
        port map (
      I0 => s_axi_wdata(7),
      I1 => \spi_sr_out_buf[7]_i_4_n_0\,
      I2 => \spi_sr_out_buf[7]_i_5_n_0\,
      I3 => \spi_sr_out_buf[7]_i_6_n_0\,
      I4 => \^s_axi_wready\,
      I5 => spi_en,
      O => p_2_in(7)
    );
\spi_sr_out_buf[7]_i_3\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"04"
    )
        port map (
      I0 => s_axi_awaddr(1),
      I1 => s_axi_awaddr(2),
      I2 => s_axi_awaddr(0),
      O => \spi_sr_out_buf[7]_i_3_n_0\
    );
\spi_sr_out_buf[7]_i_4\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"EF"
    )
        port map (
      I0 => spi_sr_out_buf_valid,
      I1 => s_axi_wdata(9),
      I2 => s_axi_wdata(8),
      O => \spi_sr_out_buf[7]_i_4_n_0\
    );
\spi_sr_out_buf[7]_i_5\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000080000000000"
    )
        port map (
      I0 => s_axi_wstrb(0),
      I1 => s_axi_wstrb(1),
      I2 => s_axi_awaddr(1),
      I3 => s_axi_awaddr(2),
      I4 => s_axi_awaddr(0),
      I5 => spi_en,
      O => \spi_sr_out_buf[7]_i_5_n_0\
    );
\spi_sr_out_buf[7]_i_6\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AA8AAAAAAAAAAAAA"
    )
        port map (
      I0 => spi_en,
      I1 => s_axi_awaddr(0),
      I2 => s_axi_awaddr(2),
      I3 => s_axi_awaddr(1),
      I4 => s_axi_wstrb(0),
      I5 => s_axi_wstrb(1),
      O => \spi_sr_out_buf[7]_i_6_n_0\
    );
spi_sr_out_buf_ready_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"ABFBFFFFAAFA0000"
    )
        port map (
      I0 => \FSM_onehot_spi_fsm_state_reg_n_0_[0]\,
      I1 => \FSM_onehot_spi_fsm_state_reg_n_0_[1]\,
      I2 => \FSM_onehot_spi_fsm_state_reg_n_0_[2]\,
      I3 => \spi_io_mode_q[2]_i_2_n_0\,
      I4 => spi_sr_out_buf_valid,
      I5 => spi_sr_out_buf_ready_reg_n_0,
      O => spi_sr_out_buf_ready_i_1_n_0
    );
spi_sr_out_buf_ready_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_sr_out_buf_ready_i_1_n_0,
      Q => spi_sr_out_buf_ready_reg_n_0,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_out_buf_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(0),
      Q => spi_sr_out_buf(0),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(1),
      Q => spi_sr_out_buf(1),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(2),
      Q => spi_sr_out_buf(2),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(3),
      Q => spi_sr_out_buf(3),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(4),
      Q => spi_sr_out_buf(4),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(5),
      Q => spi_sr_out_buf(5),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(6),
      Q => spi_sr_out_buf(6),
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_out_buf_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_out_buf[7]_i_1_n_0\,
      D => p_2_in(7),
      Q => spi_sr_out_buf(7),
      R => s_axi_awready_i_1_n_0
    );
spi_sr_out_buf_valid_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"20FF202020202020"
    )
        port map (
      I0 => spi_sr_out_buf_valid,
      I1 => spi_sr_out_buf_ready_reg_n_0,
      I2 => spi_en,
      I3 => \spi_sr_out_buf[7]_i_4_n_0\,
      I4 => \spi_sr_out_buf[7]_i_5_n_0\,
      I5 => \^s_axi_wready\,
      O => spi_sr_out_buf_valid_i_1_n_0
    );
spi_sr_out_buf_valid_reg: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => '1',
      D => spi_sr_out_buf_valid_i_1_n_0,
      Q => spi_sr_out_buf_valid,
      R => s_axi_awready_i_1_n_0
    );
\spi_sr_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[0]_i_1_n_0\,
      Q => \spi_sr_reg_n_0_[0]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[1]_i_1_n_0\,
      Q => \spi_sr_reg_n_0_[1]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[2]_i_1_n_0\,
      Q => \spi_sr_reg_n_0_[2]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[3]_i_1_n_0\,
      Q => \spi_sr_reg_n_0_[3]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[4]_i_1_n_0\,
      Q => spi_sr(4),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[5]_i_1_n_0\,
      Q => spi_sr(5),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[6]_i_1_n_0\,
      Q => \spi_sr_reg_n_0_[6]\,
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_sr_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => aclk,
      CE => \spi_sr_bit_cnt[4]_i_1_n_0\,
      D => \spi_sr[7]_i_1_n_0\,
      Q => p_1_in(0),
      R => \spi_io_o[0]_i_1_n_0\
    );
\spi_ss_o[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFBFFF00008000"
    )
        port map (
      I0 => s_axi_wdata(0),
      I1 => s_axi_awaddr(1),
      I2 => s_axi_awaddr(2),
      I3 => \^s_axi_wready\,
      I4 => \spi_ss_o[0]_i_2_n_0\,
      I5 => \^spi_ss_o\(0),
      O => \spi_ss_o[0]_i_1_n_0\
    );
\spi_ss_o[0]_i_2\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"7"
    )
        port map (
      I0 => s_axi_awaddr(0),
      I1 => s_axi_wstrb(0),
      O => \spi_ss_o[0]_i_2_n_0\
    );
\spi_ss_o_reg[0]\: unisim.vcomponents.FDSE
     port map (
      C => aclk,
      CE => '1',
      D => \spi_ss_o[0]_i_1_n_0\,
      Q => \^spi_ss_o\(0),
      S => s_axi_awready_i_1_n_0
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity icyradio_axi_spi_0_8 is
  port (
    aclk : in STD_LOGIC;
    aresetn : in STD_LOGIC;
    s_axi_awaddr : in STD_LOGIC_VECTOR ( 4 downto 0 );
    s_axi_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_awvalid : in STD_LOGIC;
    s_axi_awready : out STD_LOGIC;
    s_axi_wdata : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_wstrb : in STD_LOGIC_VECTOR ( 3 downto 0 );
    s_axi_wvalid : in STD_LOGIC;
    s_axi_wready : out STD_LOGIC;
    s_axi_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_bvalid : out STD_LOGIC;
    s_axi_bready : in STD_LOGIC;
    s_axi_araddr : in STD_LOGIC_VECTOR ( 4 downto 0 );
    s_axi_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_arvalid : in STD_LOGIC;
    s_axi_arready : out STD_LOGIC;
    s_axi_rdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_rvalid : out STD_LOGIC;
    s_axi_rready : in STD_LOGIC;
    spi_sck_i : in STD_LOGIC;
    spi_sck_o : out STD_LOGIC;
    spi_sck_t : out STD_LOGIC;
    spi_ss_i : in STD_LOGIC_VECTOR ( 0 to 0 );
    spi_ss_o : out STD_LOGIC_VECTOR ( 0 to 0 );
    spi_ss_t : out STD_LOGIC;
    spi_io0_i : in STD_LOGIC;
    spi_io0_o : out STD_LOGIC;
    spi_io0_t : out STD_LOGIC;
    spi_io1_i : in STD_LOGIC;
    spi_io1_o : out STD_LOGIC;
    spi_io1_t : out STD_LOGIC
  );
  attribute NotValidForBitStream : boolean;
  attribute NotValidForBitStream of icyradio_axi_spi_0_8 : entity is true;
  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of icyradio_axi_spi_0_8 : entity is "icyradio_axi_spi_0_8,axi_spi,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of icyradio_axi_spi_0_8 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of icyradio_axi_spi_0_8 : entity is "module_ref";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of icyradio_axi_spi_0_8 : entity is "axi_spi,Vivado 2021.1";
end icyradio_axi_spi_0_8;

architecture STRUCTURE of icyradio_axi_spi_0_8 is
  signal \<const0>\ : STD_LOGIC;
  signal \^s_axi_rdata\ : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \^s_axi_wready\ : STD_LOGIC;
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of aclk : signal is "xilinx.com:signal:clock:1.0 aclk CLK";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of aclk : signal is "XIL_INTERFACENAME aclk, ASSOCIATED_BUSIF s_axi, ASSOCIATED_RESET aresetn, FREQ_HZ 125000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN icyradio_axi_pcie_0_0_axi_aclk_out, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of aresetn : signal is "xilinx.com:signal:reset:1.0 aresetn RST";
  attribute X_INTERFACE_PARAMETER of aresetn : signal is "XIL_INTERFACENAME aresetn, POLARITY ACTIVE_LOW, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of s_axi_arready : signal is "xilinx.com:interface:aximm:1.0 s_axi ARREADY";
  attribute X_INTERFACE_INFO of s_axi_arvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi ARVALID";
  attribute X_INTERFACE_INFO of s_axi_awready : signal is "xilinx.com:interface:aximm:1.0 s_axi AWREADY";
  attribute X_INTERFACE_INFO of s_axi_awvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi AWVALID";
  attribute X_INTERFACE_INFO of s_axi_bready : signal is "xilinx.com:interface:aximm:1.0 s_axi BREADY";
  attribute X_INTERFACE_INFO of s_axi_bvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi BVALID";
  attribute X_INTERFACE_INFO of s_axi_rready : signal is "xilinx.com:interface:aximm:1.0 s_axi RREADY";
  attribute X_INTERFACE_PARAMETER of s_axi_rready : signal is "XIL_INTERFACENAME s_axi, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 125000000, ID_WIDTH 0, ADDR_WIDTH 5, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN icyradio_axi_pcie_0_0_axi_aclk_out, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of s_axi_rvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi RVALID";
  attribute X_INTERFACE_INFO of s_axi_wready : signal is "xilinx.com:interface:aximm:1.0 s_axi WREADY";
  attribute X_INTERFACE_INFO of s_axi_wvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi WVALID";
  attribute X_INTERFACE_INFO of spi_io0_i : signal is "xilinx.com:interface:spi:1.0 spi IO0_I";
  attribute X_INTERFACE_INFO of spi_io0_o : signal is "xilinx.com:interface:spi:1.0 spi IO0_O";
  attribute X_INTERFACE_INFO of spi_io0_t : signal is "xilinx.com:interface:spi:1.0 spi IO0_T";
  attribute X_INTERFACE_INFO of spi_io1_i : signal is "xilinx.com:interface:spi:1.0 spi IO1_I";
  attribute X_INTERFACE_INFO of spi_io1_o : signal is "xilinx.com:interface:spi:1.0 spi IO1_O";
  attribute X_INTERFACE_INFO of spi_io1_t : signal is "xilinx.com:interface:spi:1.0 spi IO1_T";
  attribute X_INTERFACE_INFO of spi_sck_i : signal is "xilinx.com:interface:spi:1.0 spi SCK_I";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of spi_sck_i : signal is "Master";
  attribute X_INTERFACE_INFO of spi_sck_o : signal is "xilinx.com:interface:spi:1.0 spi SCK_O";
  attribute X_INTERFACE_INFO of spi_sck_t : signal is "xilinx.com:interface:spi:1.0 spi SCK_T";
  attribute X_INTERFACE_INFO of spi_ss_t : signal is "xilinx.com:interface:spi:1.0 spi SS_T";
  attribute X_INTERFACE_INFO of s_axi_araddr : signal is "xilinx.com:interface:aximm:1.0 s_axi ARADDR";
  attribute X_INTERFACE_INFO of s_axi_arprot : signal is "xilinx.com:interface:aximm:1.0 s_axi ARPROT";
  attribute X_INTERFACE_INFO of s_axi_awaddr : signal is "xilinx.com:interface:aximm:1.0 s_axi AWADDR";
  attribute X_INTERFACE_INFO of s_axi_awprot : signal is "xilinx.com:interface:aximm:1.0 s_axi AWPROT";
  attribute X_INTERFACE_INFO of s_axi_bresp : signal is "xilinx.com:interface:aximm:1.0 s_axi BRESP";
  attribute X_INTERFACE_INFO of s_axi_rdata : signal is "xilinx.com:interface:aximm:1.0 s_axi RDATA";
  attribute X_INTERFACE_INFO of s_axi_rresp : signal is "xilinx.com:interface:aximm:1.0 s_axi RRESP";
  attribute X_INTERFACE_INFO of s_axi_wdata : signal is "xilinx.com:interface:aximm:1.0 s_axi WDATA";
  attribute X_INTERFACE_INFO of s_axi_wstrb : signal is "xilinx.com:interface:aximm:1.0 s_axi WSTRB";
  attribute X_INTERFACE_INFO of spi_ss_i : signal is "xilinx.com:interface:spi:1.0 spi SS_I";
  attribute X_INTERFACE_INFO of spi_ss_o : signal is "xilinx.com:interface:spi:1.0 spi SS_O";
begin
  s_axi_awready <= \^s_axi_wready\;
  s_axi_bresp(1) <= \<const0>\;
  s_axi_bresp(0) <= \<const0>\;
  s_axi_rdata(31 downto 28) <= \^s_axi_rdata\(31 downto 28);
  s_axi_rdata(27) <= \<const0>\;
  s_axi_rdata(26) <= \<const0>\;
  s_axi_rdata(25) <= \<const0>\;
  s_axi_rdata(24) <= \<const0>\;
  s_axi_rdata(23 downto 20) <= \^s_axi_rdata\(23 downto 20);
  s_axi_rdata(19) <= \<const0>\;
  s_axi_rdata(18) <= \<const0>\;
  s_axi_rdata(17 downto 16) <= \^s_axi_rdata\(17 downto 16);
  s_axi_rdata(15) <= \<const0>\;
  s_axi_rdata(14) <= \<const0>\;
  s_axi_rdata(13) <= \<const0>\;
  s_axi_rdata(12) <= \<const0>\;
  s_axi_rdata(11) <= \<const0>\;
  s_axi_rdata(10) <= \<const0>\;
  s_axi_rdata(9 downto 0) <= \^s_axi_rdata\(9 downto 0);
  s_axi_rresp(1) <= \<const0>\;
  s_axi_rresp(0) <= \<const0>\;
  s_axi_wready <= \^s_axi_wready\;
  spi_sck_t <= \<const0>\;
  spi_ss_t <= \<const0>\;
GND: unisim.vcomponents.GND
     port map (
      G => \<const0>\
    );
inst: entity work.icyradio_axi_spi_0_8_axi_spi
     port map (
      Q(1) => spi_io1_t,
      Q(0) => spi_io0_t,
      aclk => aclk,
      aresetn => aresetn,
      s_axi_araddr(2 downto 0) => s_axi_araddr(4 downto 2),
      s_axi_arready => s_axi_arready,
      s_axi_arvalid => s_axi_arvalid,
      s_axi_awaddr(2 downto 0) => s_axi_awaddr(4 downto 2),
      s_axi_awvalid => s_axi_awvalid,
      s_axi_bready => s_axi_bready,
      s_axi_bvalid => s_axi_bvalid,
      s_axi_rdata(19 downto 16) => \^s_axi_rdata\(31 downto 28),
      s_axi_rdata(15 downto 12) => \^s_axi_rdata\(23 downto 20),
      s_axi_rdata(11 downto 10) => \^s_axi_rdata\(17 downto 16),
      s_axi_rdata(9 downto 0) => \^s_axi_rdata\(9 downto 0),
      s_axi_rready => s_axi_rready,
      s_axi_rvalid_reg_0 => s_axi_rvalid,
      s_axi_wdata(9 downto 0) => s_axi_wdata(9 downto 0),
      s_axi_wready => \^s_axi_wready\,
      s_axi_wstrb(1 downto 0) => s_axi_wstrb(1 downto 0),
      s_axi_wvalid => s_axi_wvalid,
      spi_io0_i => spi_io0_i,
      spi_io0_o => spi_io0_o,
      spi_io1_i => spi_io1_i,
      spi_io1_o => spi_io1_o,
      spi_sck_o => spi_sck_o,
      spi_ss_o(0) => spi_ss_o(0)
    );
end STRUCTURE;