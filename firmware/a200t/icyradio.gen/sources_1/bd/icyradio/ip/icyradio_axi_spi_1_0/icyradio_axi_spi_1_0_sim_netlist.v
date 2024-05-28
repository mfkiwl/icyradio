// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2023.2 (lin64) Build 4029153 Fri Oct 13 20:13:54 MDT 2023
// Date        : Sun May 26 20:14:09 2024
// Host        : node4-dev running 64-bit Ubuntu 22.04.4 LTS
// Command     : write_verilog -force -mode funcsim
//               /home/joao/icyradio/firmware/a200t/icyradio.gen/sources_1/bd/icyradio/ip/icyradio_axi_spi_1_0/icyradio_axi_spi_1_0_sim_netlist.v
// Design      : icyradio_axi_spi_1_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7a200tfbg484-2
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "icyradio_axi_spi_1_0,axi_spi,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "module_ref" *) 
(* X_CORE_INFO = "axi_spi,Vivado 2023.2" *) 
(* NotValidForBitStream *)
module icyradio_axi_spi_1_0
   (aclk,
    aresetn,
    s_axi_awaddr,
    s_axi_awprot,
    s_axi_awvalid,
    s_axi_awready,
    s_axi_wdata,
    s_axi_wstrb,
    s_axi_wvalid,
    s_axi_wready,
    s_axi_bresp,
    s_axi_bvalid,
    s_axi_bready,
    s_axi_araddr,
    s_axi_arprot,
    s_axi_arvalid,
    s_axi_arready,
    s_axi_rdata,
    s_axi_rresp,
    s_axi_rvalid,
    s_axi_rready,
    irq,
    spi_sck_i,
    spi_sck_o,
    spi_sck_t,
    spi_ss_i,
    spi_ss_o,
    spi_ss_t,
    spi_io0_i,
    spi_io0_o,
    spi_io0_t,
    spi_io1_i,
    spi_io1_o,
    spi_io1_t);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 aclk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME aclk, ASSOCIATED_BUSIF s_axi, ASSOCIATED_RESET aresetn, FREQ_HZ 125000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN icyradio_axi_pcie_0_0_axi_aclk_out, INSERT_VIP 0" *) input aclk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 aresetn RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME aresetn, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input aresetn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWADDR" *) input [5:0]s_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWPROT" *) input [2:0]s_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWVALID" *) input s_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWREADY" *) output s_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi WDATA" *) input [31:0]s_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi WSTRB" *) input [3:0]s_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi WVALID" *) input s_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi WREADY" *) output s_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi BRESP" *) output [1:0]s_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi BVALID" *) output s_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi BREADY" *) input s_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARADDR" *) input [5:0]s_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARPROT" *) input [2:0]s_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARVALID" *) input s_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARREADY" *) output s_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) output [31:0]s_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RRESP" *) output [1:0]s_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RVALID" *) output s_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s_axi, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 125000000, ID_WIDTH 0, ADDR_WIDTH 6, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN icyradio_axi_pcie_0_0_axi_aclk_out, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input s_axi_rready;
  (* X_INTERFACE_INFO = "xilinx.com:signal:interrupt:1.0 irq INTERRUPT" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME irq, SENSITIVITY LEVEL_HIGH, PortWidth 1" *) output irq;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi SCK_I" *) (* X_INTERFACE_MODE = "Master" *) input spi_sck_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi SCK_O" *) output spi_sck_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi SCK_T" *) output spi_sck_t;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi SS_I" *) input [0:0]spi_ss_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi SS_O" *) output [0:0]spi_ss_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi SS_T" *) output spi_ss_t;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi IO0_I" *) input spi_io0_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi IO0_O" *) output spi_io0_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi IO0_T" *) output spi_io0_t;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi IO1_I" *) input spi_io1_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi IO1_O" *) output spi_io1_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi:1.0 spi IO1_T" *) output spi_io1_t;

  wire \<const0> ;
  wire aclk;
  wire aresetn;
  wire irq;
  wire [5:0]s_axi_araddr;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire [5:0]s_axi_awaddr;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire s_axi_bvalid;
  wire [31:0]\^s_axi_rdata ;
  wire s_axi_rready;
  wire s_axi_rvalid;
  wire [31:0]s_axi_wdata;
  wire s_axi_wready;
  wire [3:0]s_axi_wstrb;
  wire s_axi_wvalid;
  wire spi_io0_i;
  wire spi_io0_o;
  wire spi_io0_t;
  wire spi_io1_i;
  wire spi_io1_o;
  wire spi_io1_t;
  wire spi_sck_o;
  wire [0:0]spi_ss_o;

  assign s_axi_awready = s_axi_wready;
  assign s_axi_bresp[1] = \<const0> ;
  assign s_axi_bresp[0] = \<const0> ;
  assign s_axi_rdata[31:28] = \^s_axi_rdata [31:28];
  assign s_axi_rdata[27] = \<const0> ;
  assign s_axi_rdata[26] = \<const0> ;
  assign s_axi_rdata[25] = \<const0> ;
  assign s_axi_rdata[24] = \<const0> ;
  assign s_axi_rdata[23:20] = \^s_axi_rdata [23:20];
  assign s_axi_rdata[19] = \<const0> ;
  assign s_axi_rdata[18] = \<const0> ;
  assign s_axi_rdata[17:16] = \^s_axi_rdata [17:16];
  assign s_axi_rdata[15] = \<const0> ;
  assign s_axi_rdata[14] = \<const0> ;
  assign s_axi_rdata[13] = \<const0> ;
  assign s_axi_rdata[12] = \<const0> ;
  assign s_axi_rdata[11] = \<const0> ;
  assign s_axi_rdata[10] = \<const0> ;
  assign s_axi_rdata[9:0] = \^s_axi_rdata [9:0];
  assign s_axi_rresp[1] = \<const0> ;
  assign s_axi_rresp[0] = \<const0> ;
  assign spi_sck_t = \<const0> ;
  assign spi_ss_t = \<const0> ;
  GND GND
       (.G(\<const0> ));
  icyradio_axi_spi_1_0_axi_spi inst
       (.D({spi_io1_i,spi_io0_i}),
        .aclk(aclk),
        .aresetn(aresetn),
        .irq(irq),
        .s_axi_araddr(s_axi_araddr[5:2]),
        .s_axi_arready(s_axi_arready),
        .s_axi_arvalid(s_axi_arvalid),
        .s_axi_awaddr(s_axi_awaddr[5:2]),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_bready(s_axi_bready),
        .s_axi_bvalid(s_axi_bvalid),
        .s_axi_rdata({\^s_axi_rdata [31:28],\^s_axi_rdata [23:20],\^s_axi_rdata [17:16],\^s_axi_rdata [9:0]}),
        .s_axi_rready(s_axi_rready),
        .s_axi_rvalid_reg_0(s_axi_rvalid),
        .s_axi_wdata(s_axi_wdata[9:0]),
        .s_axi_wready(s_axi_wready),
        .s_axi_wstrb(s_axi_wstrb[1:0]),
        .s_axi_wvalid(s_axi_wvalid),
        .spi_io0_o(spi_io0_o),
        .spi_io0_t(spi_io0_t),
        .spi_io1_o(spi_io1_o),
        .spi_io1_t(spi_io1_t),
        .spi_sck_o(spi_sck_o),
        .spi_ss_o(spi_ss_o));
endmodule

(* ORIG_REF_NAME = "axi_spi" *) 
module icyradio_axi_spi_1_0_axi_spi
   (s_axi_wready,
    s_axi_rdata,
    spi_io1_o,
    spi_io0_o,
    spi_io1_t,
    spi_io0_t,
    s_axi_rvalid_reg_0,
    s_axi_arready,
    spi_ss_o,
    s_axi_bvalid,
    irq,
    spi_sck_o,
    s_axi_wdata,
    s_axi_awaddr,
    aclk,
    D,
    s_axi_rready,
    s_axi_arvalid,
    aresetn,
    s_axi_wstrb,
    s_axi_araddr,
    s_axi_awvalid,
    s_axi_wvalid,
    s_axi_bready);
  output s_axi_wready;
  output [19:0]s_axi_rdata;
  output spi_io1_o;
  output spi_io0_o;
  output spi_io1_t;
  output spi_io0_t;
  output s_axi_rvalid_reg_0;
  output s_axi_arready;
  output [0:0]spi_ss_o;
  output s_axi_bvalid;
  output irq;
  output spi_sck_o;
  input [9:0]s_axi_wdata;
  input [3:0]s_axi_awaddr;
  input aclk;
  input [1:0]D;
  input s_axi_rready;
  input s_axi_arvalid;
  input aresetn;
  input [1:0]s_axi_wstrb;
  input [3:0]s_axi_araddr;
  input s_axi_awvalid;
  input s_axi_wvalid;
  input s_axi_bready;

  wire [1:0]D;
  wire \FSM_onehot_spi_fsm_state[2]_i_2_n_0 ;
  wire \FSM_onehot_spi_fsm_state[2]_i_3_n_0 ;
  wire \FSM_onehot_spi_fsm_state_reg_n_0_[0] ;
  wire \FSM_onehot_spi_fsm_state_reg_n_0_[1] ;
  wire \FSM_onehot_spi_fsm_state_reg_n_0_[2] ;
  wire aclk;
  wire aresetn;
  wire [7:0]data5;
  wire irq;
  wire irq_enabled;
  wire \irq_enabled_reg_n_0_[0] ;
  wire \irq_enabled_reg_n_0_[1] ;
  wire \irq_enabled_reg_n_0_[2] ;
  wire \irq_enabled_reg_n_0_[3] ;
  wire \irq_enabled_reg_n_0_[4] ;
  wire \irq_enabled_reg_n_0_[5] ;
  wire \irq_enabled_reg_n_0_[6] ;
  wire \irq_pend[0]_i_1_n_0 ;
  wire \irq_pend[1]_i_1_n_0 ;
  wire \irq_pend[2]_i_1_n_0 ;
  wire \irq_pend[2]_i_2_n_0 ;
  wire \irq_pend_reg_n_0_[0] ;
  wire \irq_pend_reg_n_0_[1] ;
  wire \irq_pend_reg_n_0_[2] ;
  wire [7:0]p_0_in;
  wire [0:0]p_1_in;
  wire [7:0]p_2_in;
  wire [3:0]s_axi_araddr;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire [3:0]s_axi_awaddr;
  wire s_axi_awready0;
  wire s_axi_awready_i_1_n_0;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire s_axi_bvalid;
  wire s_axi_bvalid_i_1_n_0;
  wire s_axi_rd_en;
  wire [19:0]s_axi_rdata;
  wire \s_axi_rdata[0]_i_2_n_0 ;
  wire \s_axi_rdata[0]_i_3_n_0 ;
  wire \s_axi_rdata[0]_i_4_n_0 ;
  wire \s_axi_rdata[0]_i_5_n_0 ;
  wire \s_axi_rdata[1]_i_2_n_0 ;
  wire \s_axi_rdata[1]_i_3_n_0 ;
  wire \s_axi_rdata[2]_i_2_n_0 ;
  wire \s_axi_rdata[2]_i_3_n_0 ;
  wire \s_axi_rdata[2]_i_4_n_0 ;
  wire \s_axi_rdata[31]_i_1_n_0 ;
  wire \s_axi_rdata[3]_i_2_n_0 ;
  wire \s_axi_rdata[3]_i_3_n_0 ;
  wire \s_axi_rdata[4]_i_2_n_0 ;
  wire \s_axi_rdata[4]_i_3_n_0 ;
  wire \s_axi_rdata[5]_i_2_n_0 ;
  wire \s_axi_rdata[5]_i_3_n_0 ;
  wire \s_axi_rdata[6]_i_2_n_0 ;
  wire \s_axi_rdata[6]_i_3_n_0 ;
  wire \s_axi_rdata[6]_i_4_n_0 ;
  wire \s_axi_rdata[6]_i_5_n_0 ;
  wire \s_axi_rdata[7]_i_2_n_0 ;
  wire \s_axi_rdata[7]_i_3_n_0 ;
  wire \s_axi_rdata[7]_i_4_n_0 ;
  wire [31:0]s_axi_rdata_1;
  wire s_axi_rready;
  wire s_axi_rvalid_i_1_n_0;
  wire s_axi_rvalid_reg_0;
  wire [9:0]s_axi_wdata;
  wire s_axi_wready;
  wire [1:0]s_axi_wstrb;
  wire s_axi_wvalid;
  wire spi_cpol;
  wire spi_dir_i_1_n_0;
  wire spi_dir_reg_n_0;
  wire spi_en;
  wire spi_en_i_1_n_0;
  wire spi_fsm_state;
  wire \spi_fsm_state[0]_i_1_n_0 ;
  wire \spi_fsm_state[1]_i_1_n_0 ;
  wire \spi_fsm_state_reg_n_0_[0] ;
  wire \spi_fsm_state_reg_n_0_[1] ;
  wire spi_idle_q;
  wire spi_io0_o;
  wire spi_io0_t;
  wire spi_io1_o;
  wire spi_io1_t;
  wire \spi_io_i_reg_n_0_[0] ;
  wire \spi_io_i_reg_n_0_[1] ;
  wire [2:0]spi_io_mode;
  wire \spi_io_mode[0]_i_1_n_0 ;
  wire \spi_io_mode[1]_i_1_n_0 ;
  wire \spi_io_mode[2]_i_1_n_0 ;
  wire spi_io_mode_q;
  wire spi_io_mode_q1__8;
  wire \spi_io_mode_q[2]_i_2_n_0 ;
  wire \spi_io_mode_q[2]_i_3_n_0 ;
  wire \spi_io_mode_q_reg_n_0_[0] ;
  wire \spi_io_mode_q_reg_n_0_[1] ;
  wire \spi_io_mode_q_reg_n_0_[2] ;
  wire [1:1]spi_io_o;
  wire \spi_io_o[0]_i_1_n_0 ;
  wire \spi_io_o[0]_i_2_n_0 ;
  wire \spi_io_o[0]_i_3_n_0 ;
  wire \spi_io_o[0]_i_4_n_0 ;
  wire \spi_io_o[0]_i_5_n_0 ;
  wire \spi_io_o[0]_i_6_n_0 ;
  wire \spi_io_o[1]_i_1_n_0 ;
  wire \spi_io_o[1]_i_4_n_0 ;
  wire \spi_io_o[1]_i_5_n_0 ;
  wire \spi_io_o[1]_i_6_n_0 ;
  wire \spi_io_o[1]_i_7_n_0 ;
  wire \spi_io_o[1]_i_8_n_0 ;
  wire \spi_io_o[1]_i_9_n_0 ;
  wire \spi_io_t[0]_i_1_n_0 ;
  wire \spi_io_t[0]_i_2_n_0 ;
  wire \spi_io_t[1]_i_1_n_0 ;
  wire \spi_io_t[1]_i_2_n_0 ;
  wire \spi_io_t[1]_i_3_n_0 ;
  wire spi_lsb_first_i_1_n_0;
  wire spi_lsb_first_i_2_n_0;
  wire spi_lsb_first_reg_n_0;
  wire \spi_mode[0]_i_1_n_0 ;
  wire \spi_mode[1]_i_1_n_0 ;
  wire \spi_mode[1]_i_2_n_0 ;
  wire \spi_mode_reg_n_0_[0] ;
  wire spi_rd_req;
  wire spi_rd_req128_out__0;
  wire spi_rd_req_ack;
  wire spi_rd_req_ack_i_2_n_0;
  wire spi_rd_req_ack_reg_n_0;
  wire spi_rd_req_i_1_n_0;
  wire spi_rd_req_i_2_n_0;
  wire [7:0]spi_sck_div;
  wire spi_sck_div_0;
  wire \spi_sck_div_cnt[7]_i_1_n_0 ;
  wire \spi_sck_div_cnt[7]_i_4_n_0 ;
  wire [7:0]spi_sck_div_cnt_reg;
  wire spi_sck_div_en_i_1_n_0;
  wire spi_sck_div_en_reg_n_0;
  wire spi_sck_int;
  wire spi_sck_int_i_1_n_0;
  wire spi_sck_o;
  wire spi_sck_o1__3;
  wire spi_sck_o_i_1_n_0;
  wire spi_sck_o_i_2_n_0;
  wire spi_sck_o_i_3_n_0;
  wire spi_sck_o_i_4_n_0;
  wire spi_sck_o_i_5_n_0;
  wire spi_sck_o_i_6_n_0;
  wire spi_sck_o_i_7_n_0;
  wire spi_sck_o_i_8_n_0;
  wire spi_sck_toggle;
  wire [5:4]spi_sr;
  wire \spi_sr[0]_i_1_n_0 ;
  wire \spi_sr[1]_i_1_n_0 ;
  wire \spi_sr[2]_i_1_n_0 ;
  wire \spi_sr[3]_i_1_n_0 ;
  wire \spi_sr[4]_i_1_n_0 ;
  wire \spi_sr[5]_i_1_n_0 ;
  wire \spi_sr[6]_i_1_n_0 ;
  wire \spi_sr[7]_i_1_n_0 ;
  wire \spi_sr[7]_i_2_n_0 ;
  wire [4:0]spi_sr_bit_cnt;
  wire \spi_sr_bit_cnt[2]_i_2_n_0 ;
  wire \spi_sr_bit_cnt[3]_i_2_n_0 ;
  wire \spi_sr_bit_cnt[4]_i_1_n_0 ;
  wire \spi_sr_bit_cnt[4]_i_3_n_0 ;
  wire \spi_sr_bit_cnt[4]_i_4_n_0 ;
  wire \spi_sr_bit_cnt_reg_n_0_[0] ;
  wire \spi_sr_bit_cnt_reg_n_0_[1] ;
  wire \spi_sr_bit_cnt_reg_n_0_[2] ;
  wire \spi_sr_bit_cnt_reg_n_0_[3] ;
  wire \spi_sr_bit_cnt_reg_n_0_[4] ;
  wire [7:0]spi_sr_in_buf0_in;
  wire \spi_sr_in_buf[0]_i_2_n_0 ;
  wire \spi_sr_in_buf[1]_i_2_n_0 ;
  wire \spi_sr_in_buf[2]_i_2_n_0 ;
  wire \spi_sr_in_buf[3]_i_2_n_0 ;
  wire \spi_sr_in_buf[4]_i_2_n_0 ;
  wire \spi_sr_in_buf[5]_i_2_n_0 ;
  wire \spi_sr_in_buf[6]_i_2_n_0 ;
  wire \spi_sr_in_buf[6]_i_3_n_0 ;
  wire \spi_sr_in_buf[7]_i_3_n_0 ;
  wire \spi_sr_in_buf[7]_i_4_n_0 ;
  wire \spi_sr_in_buf[7]_i_5_n_0 ;
  wire \spi_sr_in_buf[7]_i_6_n_0 ;
  wire spi_sr_in_buf_ready_i_1_n_0;
  wire spi_sr_in_buf_ready_reg_n_0;
  wire spi_sr_in_buf_valid;
  wire spi_sr_in_buf_valid_i_1_n_0;
  wire spi_sr_in_buf_valid_i_2_n_0;
  wire spi_sr_in_buf_valid_i_3_n_0;
  wire spi_sr_in_buf_valid_q;
  wire spi_sr_in_buf_valid_reg_n_0;
  wire [7:0]spi_sr_out_buf;
  wire \spi_sr_out_buf[7]_i_10_n_0 ;
  wire \spi_sr_out_buf[7]_i_11_n_0 ;
  wire \spi_sr_out_buf[7]_i_1_n_0 ;
  wire \spi_sr_out_buf[7]_i_3_n_0 ;
  wire \spi_sr_out_buf[7]_i_4_n_0 ;
  wire \spi_sr_out_buf[7]_i_5_n_0 ;
  wire \spi_sr_out_buf[7]_i_6_n_0 ;
  wire \spi_sr_out_buf[7]_i_7_n_0 ;
  wire \spi_sr_out_buf[7]_i_8_n_0 ;
  wire \spi_sr_out_buf[7]_i_9_n_0 ;
  wire spi_sr_out_buf_ready_i_1_n_0;
  wire spi_sr_out_buf_ready_i_2_n_0;
  wire spi_sr_out_buf_ready_i_3_n_0;
  wire spi_sr_out_buf_ready_reg_n_0;
  wire spi_sr_out_buf_valid;
  wire spi_sr_out_buf_valid_i_1_n_0;
  wire spi_sr_out_buf_valid_i_2_n_0;
  wire \spi_sr_reg_n_0_[0] ;
  wire \spi_sr_reg_n_0_[1] ;
  wire \spi_sr_reg_n_0_[2] ;
  wire \spi_sr_reg_n_0_[3] ;
  wire \spi_sr_reg_n_0_[6] ;
  wire [0:0]spi_ss_o;
  wire \spi_ss_o[0]_i_1_n_0 ;
  wire \spi_ss_o[0]_i_2_n_0 ;
  wire \spi_ss_o[0]_i_3_n_0 ;

  LUT5 #(
    .INIT(32'hFFF8F8F8)) 
    \FSM_onehot_spi_fsm_state[2]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state[2]_i_2_n_0 ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I2(\FSM_onehot_spi_fsm_state[2]_i_3_n_0 ),
        .I3(spi_io_mode_q1__8),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .O(spi_fsm_state));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT3 #(
    .INIT(8'hF4)) 
    \FSM_onehot_spi_fsm_state[2]_i_2 
       (.I0(spi_rd_req_ack_reg_n_0),
        .I1(spi_rd_req),
        .I2(spi_sr_out_buf_valid),
        .O(\FSM_onehot_spi_fsm_state[2]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT5 #(
    .INIT(32'h00000880)) 
    \FSM_onehot_spi_fsm_state[2]_i_3 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I1(spi_sck_toggle),
        .I2(\spi_mode_reg_n_0_[0] ),
        .I3(spi_sck_int),
        .I4(spi_sck_o_i_7_n_0),
        .O(\FSM_onehot_spi_fsm_state[2]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT3 #(
    .INIT(8'h82)) 
    \FSM_onehot_spi_fsm_state[2]_i_4 
       (.I0(spi_sck_toggle),
        .I1(\spi_mode_reg_n_0_[0] ),
        .I2(spi_sck_int),
        .O(spi_io_mode_q1__8));
  (* FSM_ENCODED_STATES = "SPI_FSM_STATE_WAIT_XFER_REQ:001,SPI_FSM_STATE_WAIT_SCK_SYNC:010,SPI_FSM_STATE_ACTIVE:100," *) 
  FDSE #(
    .INIT(1'b1)) 
    \FSM_onehot_spi_fsm_state_reg[0] 
       (.C(aclk),
        .CE(spi_fsm_state),
        .D(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .Q(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .S(\spi_io_o[0]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "SPI_FSM_STATE_WAIT_XFER_REQ:001,SPI_FSM_STATE_WAIT_SCK_SYNC:010,SPI_FSM_STATE_ACTIVE:100," *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_spi_fsm_state_reg[1] 
       (.C(aclk),
        .CE(spi_fsm_state),
        .D(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .Q(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "SPI_FSM_STATE_WAIT_XFER_REQ:001,SPI_FSM_STATE_WAIT_SCK_SYNC:010,SPI_FSM_STATE_ACTIVE:100," *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_spi_fsm_state_reg[2] 
       (.C(aclk),
        .CE(spi_fsm_state),
        .D(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .Q(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    irq_INST_0
       (.I0(\irq_pend_reg_n_0_[0] ),
        .I1(\irq_enabled_reg_n_0_[0] ),
        .I2(\irq_pend_reg_n_0_[1] ),
        .I3(\irq_enabled_reg_n_0_[1] ),
        .I4(\irq_enabled_reg_n_0_[2] ),
        .I5(\irq_pend_reg_n_0_[2] ),
        .O(irq));
  LUT6 #(
    .INIT(64'h0000000000000080)) 
    \irq_enabled[6]_i_1 
       (.I0(s_axi_wstrb[0]),
        .I1(s_axi_wready),
        .I2(s_axi_awaddr[1]),
        .I3(s_axi_awaddr[0]),
        .I4(s_axi_awaddr[3]),
        .I5(s_axi_awaddr[2]),
        .O(irq_enabled));
  FDRE \irq_enabled_reg[0] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[0]),
        .Q(\irq_enabled_reg_n_0_[0] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_enabled_reg[1] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[1]),
        .Q(\irq_enabled_reg_n_0_[1] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_enabled_reg[2] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[2]),
        .Q(\irq_enabled_reg_n_0_[2] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_enabled_reg[3] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[3]),
        .Q(\irq_enabled_reg_n_0_[3] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_enabled_reg[4] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[4]),
        .Q(\irq_enabled_reg_n_0_[4] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_enabled_reg[5] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[5]),
        .Q(\irq_enabled_reg_n_0_[5] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_enabled_reg[6] 
       (.C(aclk),
        .CE(irq_enabled),
        .D(s_axi_wdata[6]),
        .Q(\irq_enabled_reg_n_0_[6] ),
        .R(s_axi_awready_i_1_n_0));
  LUT5 #(
    .INIT(32'hFF553000)) 
    \irq_pend[0]_i_1 
       (.I0(s_axi_wdata[0]),
        .I1(spi_idle_q),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I3(\irq_pend[2]_i_2_n_0 ),
        .I4(\irq_pend_reg_n_0_[0] ),
        .O(\irq_pend[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFF553000)) 
    \irq_pend[1]_i_1 
       (.I0(s_axi_wdata[1]),
        .I1(spi_sr_in_buf_valid_q),
        .I2(spi_sr_in_buf_valid_reg_n_0),
        .I3(\irq_pend[2]_i_2_n_0 ),
        .I4(\irq_pend_reg_n_0_[1] ),
        .O(\irq_pend[1]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hDDDDDDDDC0CC0000)) 
    \irq_pend[2]_i_1 
       (.I0(s_axi_wdata[2]),
        .I1(\irq_pend[2]_i_2_n_0 ),
        .I2(spi_sr_out_buf_ready_reg_n_0),
        .I3(spi_en),
        .I4(spi_sr_out_buf_valid),
        .I5(\irq_pend_reg_n_0_[2] ),
        .O(\irq_pend[2]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFF7FFFFFFFFF)) 
    \irq_pend[2]_i_2 
       (.I0(s_axi_wready),
        .I1(s_axi_wstrb[0]),
        .I2(s_axi_awaddr[1]),
        .I3(s_axi_awaddr[3]),
        .I4(s_axi_awaddr[2]),
        .I5(s_axi_awaddr[0]),
        .O(\irq_pend[2]_i_2_n_0 ));
  FDRE \irq_pend_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\irq_pend[0]_i_1_n_0 ),
        .Q(\irq_pend_reg_n_0_[0] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_pend_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\irq_pend[1]_i_1_n_0 ),
        .Q(\irq_pend_reg_n_0_[1] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \irq_pend_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(\irq_pend[2]_i_1_n_0 ),
        .Q(\irq_pend_reg_n_0_[2] ),
        .R(s_axi_awready_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair18" *) 
  LUT1 #(
    .INIT(2'h1)) 
    s_axi_arready_INST_0
       (.I0(s_axi_rvalid_reg_0),
        .O(s_axi_arready));
  LUT1 #(
    .INIT(2'h1)) 
    s_axi_awready_i_1
       (.I0(aresetn),
        .O(s_axi_awready_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT5 #(
    .INIT(32'h08080008)) 
    s_axi_awready_i_2
       (.I0(s_axi_awvalid),
        .I1(s_axi_wvalid),
        .I2(s_axi_wready),
        .I3(s_axi_bvalid),
        .I4(s_axi_bready),
        .O(s_axi_awready0));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWREADY" *) 
  FDRE s_axi_awready_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_awready0),
        .Q(s_axi_wready),
        .R(s_axi_awready_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT3 #(
    .INIT(8'hBA)) 
    s_axi_bvalid_i_1
       (.I0(s_axi_wready),
        .I1(s_axi_bready),
        .I2(s_axi_bvalid),
        .O(s_axi_bvalid_i_1_n_0));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi BVALID" *) 
  FDRE s_axi_bvalid_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_bvalid_i_1_n_0),
        .Q(s_axi_bvalid),
        .R(s_axi_awready_i_1_n_0));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[0]_i_1 
       (.I0(data5[0]),
        .I1(\s_axi_rdata[7]_i_2_n_0 ),
        .I2(\irq_enabled_reg_n_0_[0] ),
        .I3(\s_axi_rdata[2]_i_2_n_0 ),
        .I4(\s_axi_rdata[0]_i_2_n_0 ),
        .I5(\s_axi_rdata[0]_i_3_n_0 ),
        .O(s_axi_rdata_1[0]));
  LUT6 #(
    .INIT(64'h0B00080000000000)) 
    \s_axi_rdata[0]_i_2 
       (.I0(\spi_sr_reg_n_0_[0] ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_araddr[1]),
        .I4(\irq_pend_reg_n_0_[0] ),
        .I5(s_axi_araddr[0]),
        .O(\s_axi_rdata[0]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFF888)) 
    \s_axi_rdata[0]_i_3 
       (.I0(\s_axi_rdata[6]_i_2_n_0 ),
        .I1(spi_sck_div_en_reg_n_0),
        .I2(\s_axi_rdata[0]_i_4_n_0 ),
        .I3(spi_sr_out_buf[0]),
        .I4(\s_axi_rdata[0]_i_5_n_0 ),
        .O(\s_axi_rdata[0]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair11" *) 
  LUT4 #(
    .INIT(16'h0400)) 
    \s_axi_rdata[0]_i_4 
       (.I0(s_axi_araddr[1]),
        .I1(s_axi_araddr[0]),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_araddr[2]),
        .O(\s_axi_rdata[0]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'h0003020000000200)) 
    \s_axi_rdata[0]_i_5 
       (.I0(spi_sck_div[0]),
        .I1(s_axi_araddr[1]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[2]),
        .I4(s_axi_araddr[3]),
        .I5(spi_ss_o),
        .O(\s_axi_rdata[0]_i_5_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair11" *) 
  LUT5 #(
    .INIT(32'h00020003)) 
    \s_axi_rdata[16]_i_1 
       (.I0(spi_sr_out_buf_valid),
        .I1(s_axi_araddr[3]),
        .I2(s_axi_araddr[2]),
        .I3(s_axi_araddr[1]),
        .I4(s_axi_araddr[0]),
        .O(s_axi_rdata_1[16]));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[17]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(spi_sr_in_buf_valid_reg_n_0),
        .O(s_axi_rdata_1[17]));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[1]_i_1 
       (.I0(data5[1]),
        .I1(\s_axi_rdata[7]_i_2_n_0 ),
        .I2(\irq_enabled_reg_n_0_[1] ),
        .I3(\s_axi_rdata[2]_i_2_n_0 ),
        .I4(\s_axi_rdata[1]_i_2_n_0 ),
        .I5(\s_axi_rdata[1]_i_3_n_0 ),
        .O(s_axi_rdata_1[1]));
  LUT6 #(
    .INIT(64'h00000C000000A000)) 
    \s_axi_rdata[1]_i_2 
       (.I0(\irq_pend_reg_n_0_[1] ),
        .I1(spi_sr_out_buf[1]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[1]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \s_axi_rdata[1]_i_3 
       (.I0(\s_axi_rdata[6]_i_3_n_0 ),
        .I1(spi_sck_div[1]),
        .I2(\s_axi_rdata[6]_i_2_n_0 ),
        .I3(spi_en),
        .I4(\spi_sr_reg_n_0_[1] ),
        .I5(\s_axi_rdata[7]_i_3_n_0 ),
        .O(\s_axi_rdata[1]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair12" *) 
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[20]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .O(s_axi_rdata_1[20]));
  (* SOFT_HLUTNM = "soft_lutpair13" *) 
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[21]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(spi_dir_reg_n_0),
        .O(s_axi_rdata_1[21]));
  (* SOFT_HLUTNM = "soft_lutpair14" *) 
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[22]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(\spi_fsm_state_reg_n_0_[0] ),
        .O(s_axi_rdata_1[22]));
  (* SOFT_HLUTNM = "soft_lutpair15" *) 
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[23]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(\spi_fsm_state_reg_n_0_[1] ),
        .O(s_axi_rdata_1[23]));
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[28]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(\spi_io_mode_q_reg_n_0_[0] ),
        .O(s_axi_rdata_1[28]));
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[29]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(\spi_io_mode_q_reg_n_0_[1] ),
        .O(s_axi_rdata_1[29]));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[2]_i_1 
       (.I0(data5[2]),
        .I1(\s_axi_rdata[7]_i_2_n_0 ),
        .I2(\irq_enabled_reg_n_0_[2] ),
        .I3(\s_axi_rdata[2]_i_2_n_0 ),
        .I4(\s_axi_rdata[2]_i_3_n_0 ),
        .I5(\s_axi_rdata[2]_i_4_n_0 ),
        .O(s_axi_rdata_1[2]));
  (* SOFT_HLUTNM = "soft_lutpair14" *) 
  LUT4 #(
    .INIT(16'h0002)) 
    \s_axi_rdata[2]_i_2 
       (.I0(s_axi_araddr[1]),
        .I1(s_axi_araddr[3]),
        .I2(s_axi_araddr[2]),
        .I3(s_axi_araddr[0]),
        .O(\s_axi_rdata[2]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h00000C000000A000)) 
    \s_axi_rdata[2]_i_3 
       (.I0(\irq_pend_reg_n_0_[2] ),
        .I1(spi_sr_out_buf[2]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[2]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \s_axi_rdata[2]_i_4 
       (.I0(\s_axi_rdata[6]_i_3_n_0 ),
        .I1(spi_sck_div[2]),
        .I2(\s_axi_rdata[6]_i_2_n_0 ),
        .I3(\spi_mode_reg_n_0_[0] ),
        .I4(\spi_sr_reg_n_0_[2] ),
        .I5(\s_axi_rdata[7]_i_3_n_0 ),
        .O(\s_axi_rdata[2]_i_4_n_0 ));
  LUT5 #(
    .INIT(32'h00100000)) 
    \s_axi_rdata[30]_i_1 
       (.I0(s_axi_araddr[3]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(\spi_io_mode_q_reg_n_0_[2] ),
        .O(s_axi_rdata_1[30]));
  LUT4 #(
    .INIT(16'h8AFF)) 
    \s_axi_rdata[31]_i_1 
       (.I0(s_axi_rready),
        .I1(s_axi_rvalid_reg_0),
        .I2(s_axi_arvalid),
        .I3(aresetn),
        .O(\s_axi_rdata[31]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h2)) 
    \s_axi_rdata[31]_i_2 
       (.I0(s_axi_arvalid),
        .I1(s_axi_rvalid_reg_0),
        .O(s_axi_rd_en));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT5 #(
    .INIT(32'h10000000)) 
    \s_axi_rdata[31]_i_3 
       (.I0(s_axi_araddr[0]),
        .I1(s_axi_araddr[3]),
        .I2(s_axi_araddr[2]),
        .I3(s_axi_araddr[1]),
        .I4(spi_sr_in_buf_valid_reg_n_0),
        .O(s_axi_rdata_1[31]));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[3]_i_1 
       (.I0(spi_cpol),
        .I1(\s_axi_rdata[6]_i_2_n_0 ),
        .I2(spi_sck_div[3]),
        .I3(\s_axi_rdata[6]_i_3_n_0 ),
        .I4(\s_axi_rdata[3]_i_2_n_0 ),
        .I5(\s_axi_rdata[3]_i_3_n_0 ),
        .O(s_axi_rdata_1[3]));
  LUT6 #(
    .INIT(64'h0000000000A000C0)) 
    \s_axi_rdata[3]_i_2 
       (.I0(data5[3]),
        .I1(\irq_enabled_reg_n_0_[3] ),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[3]),
        .I4(s_axi_araddr[2]),
        .I5(s_axi_araddr[0]),
        .O(\s_axi_rdata[3]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h0000AC0000000000)) 
    \s_axi_rdata[3]_i_3 
       (.I0(\spi_sr_reg_n_0_[3] ),
        .I1(spi_sr_out_buf[3]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[3]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[4]_i_1 
       (.I0(spi_io_mode[0]),
        .I1(\s_axi_rdata[6]_i_2_n_0 ),
        .I2(spi_sck_div[4]),
        .I3(\s_axi_rdata[6]_i_3_n_0 ),
        .I4(\s_axi_rdata[4]_i_2_n_0 ),
        .I5(\s_axi_rdata[4]_i_3_n_0 ),
        .O(s_axi_rdata_1[4]));
  LUT6 #(
    .INIT(64'h0000000000A000C0)) 
    \s_axi_rdata[4]_i_2 
       (.I0(data5[4]),
        .I1(\irq_enabled_reg_n_0_[4] ),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[3]),
        .I4(s_axi_araddr[2]),
        .I5(s_axi_araddr[0]),
        .O(\s_axi_rdata[4]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h0000AC0000000000)) 
    \s_axi_rdata[4]_i_3 
       (.I0(spi_sr[4]),
        .I1(spi_sr_out_buf[4]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[4]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[5]_i_1 
       (.I0(spi_io_mode[1]),
        .I1(\s_axi_rdata[6]_i_2_n_0 ),
        .I2(spi_sck_div[5]),
        .I3(\s_axi_rdata[6]_i_3_n_0 ),
        .I4(\s_axi_rdata[5]_i_2_n_0 ),
        .I5(\s_axi_rdata[5]_i_3_n_0 ),
        .O(s_axi_rdata_1[5]));
  LUT6 #(
    .INIT(64'h0000000000A000C0)) 
    \s_axi_rdata[5]_i_2 
       (.I0(data5[5]),
        .I1(\irq_enabled_reg_n_0_[5] ),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[3]),
        .I4(s_axi_araddr[2]),
        .I5(s_axi_araddr[0]),
        .O(\s_axi_rdata[5]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h0000AC0000000000)) 
    \s_axi_rdata[5]_i_3 
       (.I0(spi_sr[5]),
        .I1(spi_sr_out_buf[5]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[5]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFF888)) 
    \s_axi_rdata[6]_i_1 
       (.I0(spi_io_mode[2]),
        .I1(\s_axi_rdata[6]_i_2_n_0 ),
        .I2(spi_sck_div[6]),
        .I3(\s_axi_rdata[6]_i_3_n_0 ),
        .I4(\s_axi_rdata[6]_i_4_n_0 ),
        .I5(\s_axi_rdata[6]_i_5_n_0 ),
        .O(s_axi_rdata_1[6]));
  (* SOFT_HLUTNM = "soft_lutpair15" *) 
  LUT4 #(
    .INIT(16'h0004)) 
    \s_axi_rdata[6]_i_2 
       (.I0(s_axi_araddr[1]),
        .I1(s_axi_araddr[0]),
        .I2(s_axi_araddr[2]),
        .I3(s_axi_araddr[3]),
        .O(\s_axi_rdata[6]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT4 #(
    .INIT(16'h0100)) 
    \s_axi_rdata[6]_i_3 
       (.I0(s_axi_araddr[0]),
        .I1(s_axi_araddr[1]),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_araddr[2]),
        .O(\s_axi_rdata[6]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000A000C0)) 
    \s_axi_rdata[6]_i_4 
       (.I0(data5[6]),
        .I1(\irq_enabled_reg_n_0_[6] ),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[3]),
        .I4(s_axi_araddr[2]),
        .I5(s_axi_araddr[0]),
        .O(\s_axi_rdata[6]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'h0000AC0000000000)) 
    \s_axi_rdata[6]_i_5 
       (.I0(\spi_sr_reg_n_0_[6] ),
        .I1(spi_sr_out_buf[6]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[6]_i_5_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFF888)) 
    \s_axi_rdata[7]_i_1 
       (.I0(\s_axi_rdata[7]_i_2_n_0 ),
        .I1(data5[7]),
        .I2(\s_axi_rdata[7]_i_3_n_0 ),
        .I3(p_1_in),
        .I4(\s_axi_rdata[7]_i_4_n_0 ),
        .O(s_axi_rdata_1[7]));
  (* SOFT_HLUTNM = "soft_lutpair13" *) 
  LUT4 #(
    .INIT(16'h0008)) 
    \s_axi_rdata[7]_i_2 
       (.I0(s_axi_araddr[1]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_araddr[0]),
        .O(\s_axi_rdata[7]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair12" *) 
  LUT4 #(
    .INIT(16'h0800)) 
    \s_axi_rdata[7]_i_3 
       (.I0(s_axi_araddr[1]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_araddr[0]),
        .O(\s_axi_rdata[7]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'h000000AC00000000)) 
    \s_axi_rdata[7]_i_4 
       (.I0(spi_sr_out_buf[7]),
        .I1(spi_sck_div[7]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(s_axi_araddr[3]),
        .I5(s_axi_araddr[2]),
        .O(\s_axi_rdata[7]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'h000000000A000C00)) 
    \s_axi_rdata[8]_i_1 
       (.I0(spi_sr_out_buf_valid),
        .I1(spi_lsb_first_reg_n_0),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .I4(s_axi_araddr[2]),
        .I5(s_axi_araddr[3]),
        .O(s_axi_rdata_1[8]));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT5 #(
    .INIT(32'h00200000)) 
    \s_axi_rdata[9]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(s_axi_araddr[3]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .I4(spi_rd_req),
        .O(s_axi_rdata_1[9]));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[0] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[0]),
        .Q(s_axi_rdata[0]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[16] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[16]),
        .Q(s_axi_rdata[10]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[17] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[17]),
        .Q(s_axi_rdata[11]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[1] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[1]),
        .Q(s_axi_rdata[1]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[20] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[20]),
        .Q(s_axi_rdata[12]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[21] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[21]),
        .Q(s_axi_rdata[13]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[22] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[22]),
        .Q(s_axi_rdata[14]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[23] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[23]),
        .Q(s_axi_rdata[15]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[28] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[28]),
        .Q(s_axi_rdata[16]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[29] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[29]),
        .Q(s_axi_rdata[17]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[2] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[2]),
        .Q(s_axi_rdata[2]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[30] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[30]),
        .Q(s_axi_rdata[18]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[31] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[31]),
        .Q(s_axi_rdata[19]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[3] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[3]),
        .Q(s_axi_rdata[3]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[4] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[4]),
        .Q(s_axi_rdata[4]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[5] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[5]),
        .Q(s_axi_rdata[5]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[6] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[6]),
        .Q(s_axi_rdata[6]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[7] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[7]),
        .Q(s_axi_rdata[7]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[8] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[8]),
        .Q(s_axi_rdata[8]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[9] 
       (.C(aclk),
        .CE(s_axi_rd_en),
        .D(s_axi_rdata_1[9]),
        .Q(s_axi_rdata[9]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  LUT3 #(
    .INIT(8'h5C)) 
    s_axi_rvalid_i_1
       (.I0(s_axi_rready),
        .I1(s_axi_arvalid),
        .I2(s_axi_rvalid_reg_0),
        .O(s_axi_rvalid_i_1_n_0));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RVALID" *) 
  FDRE s_axi_rvalid_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_rvalid_i_1_n_0),
        .Q(s_axi_rvalid_reg_0),
        .R(s_axi_awready_i_1_n_0));
  LUT3 #(
    .INIT(8'h04)) 
    spi_dir_i_1
       (.I0(spi_rd_req_ack_reg_n_0),
        .I1(spi_rd_req),
        .I2(spi_sr_out_buf_valid),
        .O(spi_dir_i_1_n_0));
  FDRE spi_dir_reg
       (.C(aclk),
        .CE(spi_io_mode_q),
        .D(spi_dir_i_1_n_0),
        .Q(spi_dir_reg_n_0),
        .R(\spi_io_o[0]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair17" *) 
  LUT5 #(
    .INIT(32'hABFFA800)) 
    spi_en_i_1
       (.I0(s_axi_wdata[1]),
        .I1(spi_sck_div_en_reg_n_0),
        .I2(s_axi_wdata[0]),
        .I3(\spi_mode[1]_i_2_n_0 ),
        .I4(spi_en),
        .O(spi_en_i_1_n_0));
  FDRE spi_en_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_en_i_1_n_0),
        .Q(spi_en),
        .R(s_axi_awready_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair36" *) 
  LUT2 #(
    .INIT(4'h1)) 
    \spi_fsm_state[0]_i_1 
       (.I0(\spi_fsm_state_reg_n_0_[0] ),
        .I1(\spi_fsm_state_reg_n_0_[1] ),
        .O(\spi_fsm_state[0]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair36" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \spi_fsm_state[1]_i_1 
       (.I0(\spi_fsm_state_reg_n_0_[0] ),
        .I1(\spi_fsm_state_reg_n_0_[1] ),
        .O(\spi_fsm_state[1]_i_1_n_0 ));
  FDRE \spi_fsm_state_reg[0] 
       (.C(aclk),
        .CE(spi_fsm_state),
        .D(\spi_fsm_state[0]_i_1_n_0 ),
        .Q(\spi_fsm_state_reg_n_0_[0] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_fsm_state_reg[1] 
       (.C(aclk),
        .CE(spi_fsm_state),
        .D(\spi_fsm_state[1]_i_1_n_0 ),
        .Q(\spi_fsm_state_reg_n_0_[1] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE spi_idle_q_reg
       (.C(aclk),
        .CE(1'b1),
        .D(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .Q(spi_idle_q),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_io_i_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[0]),
        .Q(\spi_io_i_reg_n_0_[0] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_io_i_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[1]),
        .Q(\spi_io_i_reg_n_0_[1] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hEFFF0400)) 
    \spi_io_mode[0]_i_1 
       (.I0(s_axi_wdata[6]),
        .I1(s_axi_wdata[4]),
        .I2(s_axi_wdata[5]),
        .I3(\spi_mode[1]_i_2_n_0 ),
        .I4(spi_io_mode[0]),
        .O(\spi_io_mode[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFBFF1000)) 
    \spi_io_mode[1]_i_1 
       (.I0(s_axi_wdata[6]),
        .I1(s_axi_wdata[4]),
        .I2(s_axi_wdata[5]),
        .I3(\spi_mode[1]_i_2_n_0 ),
        .I4(spi_io_mode[1]),
        .O(\spi_io_mode[1]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hEBFF0000)) 
    \spi_io_mode[2]_i_1 
       (.I0(s_axi_wdata[6]),
        .I1(s_axi_wdata[4]),
        .I2(s_axi_wdata[5]),
        .I3(\spi_mode[1]_i_2_n_0 ),
        .I4(spi_io_mode[2]),
        .O(\spi_io_mode[2]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hA8888888)) 
    \spi_io_mode_q[2]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state[2]_i_2_n_0 ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I3(\spi_io_mode_q[2]_i_2_n_0 ),
        .I4(spi_io_mode_q1__8),
        .O(spi_io_mode_q));
  LUT5 #(
    .INIT(32'h90090000)) 
    \spi_io_mode_q[2]_i_2 
       (.I0(\spi_io_mode_q_reg_n_0_[1] ),
        .I1(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .I2(\spi_io_mode_q_reg_n_0_[2] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .I4(\spi_io_mode_q[2]_i_3_n_0 ),
        .O(\spi_io_mode_q[2]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair19" *) 
  LUT4 #(
    .INIT(16'h0009)) 
    \spi_io_mode_q[2]_i_3 
       (.I0(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I1(\spi_io_mode_q_reg_n_0_[0] ),
        .I2(\spi_sr_bit_cnt_reg_n_0_[4] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[3] ),
        .O(\spi_io_mode_q[2]_i_3_n_0 ));
  FDRE \spi_io_mode_q_reg[0] 
       (.C(aclk),
        .CE(spi_io_mode_q),
        .D(spi_io_mode[0]),
        .Q(\spi_io_mode_q_reg_n_0_[0] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_io_mode_q_reg[1] 
       (.C(aclk),
        .CE(spi_io_mode_q),
        .D(spi_io_mode[1]),
        .Q(\spi_io_mode_q_reg_n_0_[1] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_io_mode_q_reg[2] 
       (.C(aclk),
        .CE(spi_io_mode_q),
        .D(spi_io_mode[2]),
        .Q(\spi_io_mode_q_reg_n_0_[2] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDSE \spi_io_mode_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_mode[0]_i_1_n_0 ),
        .Q(spi_io_mode[0]),
        .S(s_axi_awready_i_1_n_0));
  FDRE \spi_io_mode_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_mode[1]_i_1_n_0 ),
        .Q(spi_io_mode[1]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_io_mode_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_mode[2]_i_1_n_0 ),
        .Q(spi_io_mode[2]),
        .R(s_axi_awready_i_1_n_0));
  LUT1 #(
    .INIT(2'h1)) 
    \spi_io_o[0]_i_1 
       (.I0(spi_en),
        .O(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hEAAAFFFFEAAA0000)) 
    \spi_io_o[0]_i_2 
       (.I0(\spi_io_o[0]_i_3_n_0 ),
        .I1(\spi_io_o[0]_i_4_n_0 ),
        .I2(\spi_sr_reg_n_0_[0] ),
        .I3(spi_lsb_first_reg_n_0),
        .I4(\spi_io_o[0]_i_5_n_0 ),
        .I5(spi_io0_o),
        .O(\spi_io_o[0]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'h8F008800)) 
    \spi_io_o[0]_i_3 
       (.I0(\spi_io_o[0]_i_6_n_0 ),
        .I1(\spi_sr_reg_n_0_[6] ),
        .I2(\spi_io_o[1]_i_4_n_0 ),
        .I3(\spi_io_o[0]_i_4_n_0 ),
        .I4(p_1_in),
        .O(\spi_io_o[0]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFE00000000)) 
    \spi_io_o[0]_i_4 
       (.I0(\spi_sr_bit_cnt_reg_n_0_[3] ),
        .I1(\spi_sr_bit_cnt_reg_n_0_[4] ),
        .I2(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .I4(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I5(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(\spi_io_o[0]_i_4_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT5 #(
    .INIT(32'hFFFF2800)) 
    \spi_io_o[0]_i_5 
       (.I0(spi_sck_toggle),
        .I1(\spi_mode_reg_n_0_[0] ),
        .I2(spi_sck_int),
        .I3(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .O(\spi_io_o[0]_i_5_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair26" *) 
  LUT4 #(
    .INIT(16'h0010)) 
    \spi_io_o[0]_i_6 
       (.I0(\spi_io_mode_q_reg_n_0_[2] ),
        .I1(\spi_io_mode_q_reg_n_0_[0] ),
        .I2(\spi_io_mode_q_reg_n_0_[1] ),
        .I3(spi_lsb_first_reg_n_0),
        .O(\spi_io_o[0]_i_6_n_0 ));
  LUT6 #(
    .INIT(64'hAAAABFFFAAAA8000)) 
    \spi_io_o[1]_i_1 
       (.I0(spi_io_o),
        .I1(spi_sck_o1__3),
        .I2(\spi_io_o[1]_i_4_n_0 ),
        .I3(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I4(\spi_io_o[1]_i_5_n_0 ),
        .I5(spi_io1_o),
        .O(\spi_io_o[1]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hF8008800)) 
    \spi_io_o[1]_i_2 
       (.I0(\spi_sr_reg_n_0_[1] ),
        .I1(spi_lsb_first_reg_n_0),
        .I2(\spi_io_o[0]_i_6_n_0 ),
        .I3(\spi_io_o[0]_i_4_n_0 ),
        .I4(p_1_in),
        .O(spi_io_o));
  LUT6 #(
    .INIT(64'h0000000200020000)) 
    \spi_io_o[1]_i_3 
       (.I0(\spi_io_o[1]_i_6_n_0 ),
        .I1(\spi_io_o[1]_i_7_n_0 ),
        .I2(\spi_io_o[1]_i_8_n_0 ),
        .I3(\spi_io_o[1]_i_9_n_0 ),
        .I4(\spi_mode_reg_n_0_[0] ),
        .I5(spi_sck_int),
        .O(spi_sck_o1__3));
  (* SOFT_HLUTNM = "soft_lutpair24" *) 
  LUT4 #(
    .INIT(16'hFFEF)) 
    \spi_io_o[1]_i_4 
       (.I0(\spi_io_mode_q_reg_n_0_[2] ),
        .I1(\spi_io_mode_q_reg_n_0_[1] ),
        .I2(\spi_io_mode_q_reg_n_0_[0] ),
        .I3(spi_lsb_first_reg_n_0),
        .O(\spi_io_o[1]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF14000000)) 
    \spi_io_o[1]_i_5 
       (.I0(spi_sck_o_i_7_n_0),
        .I1(spi_sck_int),
        .I2(\spi_mode_reg_n_0_[0] ),
        .I3(spi_sck_toggle),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I5(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .O(\spi_io_o[1]_i_5_n_0 ));
  LUT5 #(
    .INIT(32'h82000082)) 
    \spi_io_o[1]_i_6 
       (.I0(spi_sck_div_en_reg_n_0),
        .I1(spi_sck_div[6]),
        .I2(spi_sck_div_cnt_reg[6]),
        .I3(spi_sck_div_cnt_reg[7]),
        .I4(spi_sck_div[7]),
        .O(\spi_io_o[1]_i_6_n_0 ));
  LUT4 #(
    .INIT(16'h6FF6)) 
    \spi_io_o[1]_i_7 
       (.I0(spi_sck_div_cnt_reg[5]),
        .I1(spi_sck_div[5]),
        .I2(spi_sck_div_cnt_reg[2]),
        .I3(spi_sck_div[2]),
        .O(\spi_io_o[1]_i_7_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair23" *) 
  LUT4 #(
    .INIT(16'h6FF6)) 
    \spi_io_o[1]_i_8 
       (.I0(spi_sck_div_cnt_reg[1]),
        .I1(spi_sck_div[1]),
        .I2(spi_sck_div_cnt_reg[0]),
        .I3(spi_sck_div[0]),
        .O(\spi_io_o[1]_i_8_n_0 ));
  LUT4 #(
    .INIT(16'h6FF6)) 
    \spi_io_o[1]_i_9 
       (.I0(spi_sck_div_cnt_reg[4]),
        .I1(spi_sck_div[4]),
        .I2(spi_sck_div_cnt_reg[3]),
        .I3(spi_sck_div[3]),
        .O(\spi_io_o[1]_i_9_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi_rtl:1.0 spi IO0_O" *) 
  FDRE \spi_io_o_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_o[0]_i_2_n_0 ),
        .Q(spi_io0_o),
        .R(\spi_io_o[0]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi_rtl:1.0 spi IO0_O" *) 
  FDRE \spi_io_o_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_o[1]_i_1_n_0 ),
        .Q(spi_io1_o),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hEAAAFFFFEAAA0000)) 
    \spi_io_t[0]_i_1 
       (.I0(\spi_io_t[0]_i_2_n_0 ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I2(spi_dir_reg_n_0),
        .I3(\spi_io_mode_q_reg_n_0_[1] ),
        .I4(\spi_io_o[0]_i_5_n_0 ),
        .I5(spi_io0_t),
        .O(\spi_io_t[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFEBFF00FFFFFF00)) 
    \spi_io_t[0]_i_2 
       (.I0(\spi_io_mode_q_reg_n_0_[2] ),
        .I1(\spi_io_mode_q_reg_n_0_[1] ),
        .I2(\spi_io_mode_q_reg_n_0_[0] ),
        .I3(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I5(spi_sck_o_i_7_n_0),
        .O(\spi_io_t[0]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFAEAFFFFFAEA0000)) 
    \spi_io_t[1]_i_1 
       (.I0(\spi_io_t[1]_i_2_n_0 ),
        .I1(spi_dir_reg_n_0),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I3(\spi_io_t[1]_i_3_n_0 ),
        .I4(\spi_io_o[0]_i_5_n_0 ),
        .I5(spi_io1_t),
        .O(\spi_io_t[1]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT3 #(
    .INIT(8'hF4)) 
    \spi_io_t[1]_i_2 
       (.I0(spi_sck_o_i_7_n_0),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .O(\spi_io_t[1]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair34" *) 
  LUT3 #(
    .INIT(8'hFD)) 
    \spi_io_t[1]_i_3 
       (.I0(\spi_io_mode_q_reg_n_0_[1] ),
        .I1(\spi_io_mode_q_reg_n_0_[0] ),
        .I2(\spi_io_mode_q_reg_n_0_[2] ),
        .O(\spi_io_t[1]_i_3_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi_rtl:1.0 spi IO0_T" *) 
  FDSE \spi_io_t_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_t[0]_i_1_n_0 ),
        .Q(spi_io0_t),
        .S(\spi_io_o[0]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi_rtl:1.0 spi IO0_T" *) 
  FDSE \spi_io_t_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_io_t[1]_i_1_n_0 ),
        .Q(spi_io1_t),
        .S(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hBFFFFFFF80000000)) 
    spi_lsb_first_i_1
       (.I0(s_axi_wdata[8]),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I2(s_axi_wstrb[1]),
        .I3(s_axi_wready),
        .I4(spi_lsb_first_i_2_n_0),
        .I5(spi_lsb_first_reg_n_0),
        .O(spi_lsb_first_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT4 #(
    .INIT(16'h0002)) 
    spi_lsb_first_i_2
       (.I0(s_axi_awaddr[0]),
        .I1(s_axi_awaddr[2]),
        .I2(s_axi_awaddr[3]),
        .I3(s_axi_awaddr[1]),
        .O(spi_lsb_first_i_2_n_0));
  FDRE spi_lsb_first_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_lsb_first_i_1_n_0),
        .Q(spi_lsb_first_reg_n_0),
        .R(s_axi_awready_i_1_n_0));
  LUT4 #(
    .INIT(16'hBF80)) 
    \spi_mode[0]_i_1 
       (.I0(s_axi_wdata[2]),
        .I1(\spi_mode[1]_i_2_n_0 ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I3(\spi_mode_reg_n_0_[0] ),
        .O(\spi_mode[0]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair20" *) 
  LUT4 #(
    .INIT(16'hBF80)) 
    \spi_mode[1]_i_1 
       (.I0(s_axi_wdata[3]),
        .I1(\spi_mode[1]_i_2_n_0 ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I3(spi_cpol),
        .O(\spi_mode[1]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0100000000000000)) 
    \spi_mode[1]_i_2 
       (.I0(s_axi_awaddr[1]),
        .I1(s_axi_awaddr[3]),
        .I2(s_axi_awaddr[2]),
        .I3(s_axi_awaddr[0]),
        .I4(s_axi_wready),
        .I5(s_axi_wstrb[0]),
        .O(\spi_mode[1]_i_2_n_0 ));
  FDRE \spi_mode_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_mode[0]_i_1_n_0 ),
        .Q(\spi_mode_reg_n_0_[0] ),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_mode_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_mode[1]_i_1_n_0 ),
        .Q(spi_cpol),
        .R(s_axi_awready_i_1_n_0));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF800000)) 
    spi_rd_req_ack_i_1
       (.I0(spi_io_mode_q1__8),
        .I1(\spi_io_mode_q[2]_i_2_n_0 ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I3(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I4(spi_dir_i_1_n_0),
        .I5(spi_rd_req_ack_i_2_n_0),
        .O(spi_rd_req_ack));
  (* SOFT_HLUTNM = "soft_lutpair10" *) 
  LUT5 #(
    .INIT(32'h88888880)) 
    spi_rd_req_ack_i_2
       (.I0(spi_rd_req_ack_reg_n_0),
        .I1(spi_rd_req),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I3(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(spi_rd_req_ack_i_2_n_0));
  FDRE spi_rd_req_ack_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_rd_req_ack),
        .Q(spi_rd_req_ack_reg_n_0),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0400F4F004000400)) 
    spi_rd_req_i_1
       (.I0(s_axi_wdata[8]),
        .I1(s_axi_wdata[9]),
        .I2(spi_rd_req),
        .I3(spi_rd_req_i_2_n_0),
        .I4(spi_rd_req_ack_reg_n_0),
        .I5(spi_en),
        .O(spi_rd_req_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair22" *) 
  LUT4 #(
    .INIT(16'h8000)) 
    spi_rd_req_i_2
       (.I0(s_axi_wready),
        .I1(s_axi_wstrb[0]),
        .I2(s_axi_wstrb[1]),
        .I3(\spi_sr_out_buf[7]_i_4_n_0 ),
        .O(spi_rd_req_i_2_n_0));
  FDRE spi_rd_req_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_rd_req_i_1_n_0),
        .Q(spi_rd_req),
        .R(s_axi_awready_i_1_n_0));
  LUT6 #(
    .INIT(64'h0000000000000002)) 
    \spi_sck_div[7]_i_1 
       (.I0(s_axi_awaddr[2]),
        .I1(s_axi_awaddr[3]),
        .I2(s_axi_awaddr[0]),
        .I3(s_axi_awaddr[1]),
        .I4(spi_sck_div_en_reg_n_0),
        .I5(\spi_ss_o[0]_i_2_n_0 ),
        .O(spi_sck_div_0));
  (* SOFT_HLUTNM = "soft_lutpair35" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \spi_sck_div_cnt[0]_i_1 
       (.I0(spi_sck_div_cnt_reg[0]),
        .O(p_0_in[0]));
  (* SOFT_HLUTNM = "soft_lutpair35" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \spi_sck_div_cnt[1]_i_1 
       (.I0(spi_sck_div_cnt_reg[0]),
        .I1(spi_sck_div_cnt_reg[1]),
        .O(p_0_in[1]));
  (* SOFT_HLUTNM = "soft_lutpair23" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \spi_sck_div_cnt[2]_i_1 
       (.I0(spi_sck_div_cnt_reg[0]),
        .I1(spi_sck_div_cnt_reg[1]),
        .I2(spi_sck_div_cnt_reg[2]),
        .O(p_0_in[2]));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \spi_sck_div_cnt[3]_i_1 
       (.I0(spi_sck_div_cnt_reg[2]),
        .I1(spi_sck_div_cnt_reg[1]),
        .I2(spi_sck_div_cnt_reg[0]),
        .I3(spi_sck_div_cnt_reg[3]),
        .O(p_0_in[3]));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \spi_sck_div_cnt[4]_i_1 
       (.I0(spi_sck_div_cnt_reg[3]),
        .I1(spi_sck_div_cnt_reg[0]),
        .I2(spi_sck_div_cnt_reg[1]),
        .I3(spi_sck_div_cnt_reg[2]),
        .I4(spi_sck_div_cnt_reg[4]),
        .O(p_0_in[4]));
  LUT6 #(
    .INIT(64'h7FFFFFFF80000000)) 
    \spi_sck_div_cnt[5]_i_1 
       (.I0(spi_sck_div_cnt_reg[2]),
        .I1(spi_sck_div_cnt_reg[1]),
        .I2(spi_sck_div_cnt_reg[0]),
        .I3(spi_sck_div_cnt_reg[3]),
        .I4(spi_sck_div_cnt_reg[4]),
        .I5(spi_sck_div_cnt_reg[5]),
        .O(p_0_in[5]));
  (* SOFT_HLUTNM = "soft_lutpair33" *) 
  LUT2 #(
    .INIT(4'h9)) 
    \spi_sck_div_cnt[6]_i_1 
       (.I0(\spi_sck_div_cnt[7]_i_4_n_0 ),
        .I1(spi_sck_div_cnt_reg[6]),
        .O(p_0_in[6]));
  LUT2 #(
    .INIT(4'hB)) 
    \spi_sck_div_cnt[7]_i_1 
       (.I0(spi_sck_toggle),
        .I1(spi_sck_div_en_reg_n_0),
        .O(\spi_sck_div_cnt[7]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair33" *) 
  LUT3 #(
    .INIT(8'hD2)) 
    \spi_sck_div_cnt[7]_i_2 
       (.I0(spi_sck_div_cnt_reg[6]),
        .I1(\spi_sck_div_cnt[7]_i_4_n_0 ),
        .I2(spi_sck_div_cnt_reg[7]),
        .O(p_0_in[7]));
  LUT4 #(
    .INIT(16'h0002)) 
    \spi_sck_div_cnt[7]_i_3 
       (.I0(\spi_io_o[1]_i_6_n_0 ),
        .I1(\spi_io_o[1]_i_7_n_0 ),
        .I2(\spi_io_o[1]_i_8_n_0 ),
        .I3(\spi_io_o[1]_i_9_n_0 ),
        .O(spi_sck_toggle));
  LUT6 #(
    .INIT(64'h7FFFFFFFFFFFFFFF)) 
    \spi_sck_div_cnt[7]_i_4 
       (.I0(spi_sck_div_cnt_reg[2]),
        .I1(spi_sck_div_cnt_reg[1]),
        .I2(spi_sck_div_cnt_reg[0]),
        .I3(spi_sck_div_cnt_reg[3]),
        .I4(spi_sck_div_cnt_reg[4]),
        .I5(spi_sck_div_cnt_reg[5]),
        .O(\spi_sck_div_cnt[7]_i_4_n_0 ));
  FDRE \spi_sck_div_cnt_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[0]),
        .Q(spi_sck_div_cnt_reg[0]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[1]),
        .Q(spi_sck_div_cnt_reg[1]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[2]),
        .Q(spi_sck_div_cnt_reg[2]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[3]),
        .Q(spi_sck_div_cnt_reg[3]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[4] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[4]),
        .Q(spi_sck_div_cnt_reg[4]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[5] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[5]),
        .Q(spi_sck_div_cnt_reg[5]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[6] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[6]),
        .Q(spi_sck_div_cnt_reg[6]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  FDRE \spi_sck_div_cnt_reg[7] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in[7]),
        .Q(spi_sck_div_cnt_reg[7]),
        .R(\spi_sck_div_cnt[7]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair17" *) 
  LUT5 #(
    .INIT(32'hEAFF2A00)) 
    spi_sck_div_en_i_1
       (.I0(s_axi_wdata[0]),
        .I1(spi_en),
        .I2(s_axi_wdata[1]),
        .I3(\spi_mode[1]_i_2_n_0 ),
        .I4(spi_sck_div_en_reg_n_0),
        .O(spi_sck_div_en_i_1_n_0));
  FDRE spi_sck_div_en_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sck_div_en_i_1_n_0),
        .Q(spi_sck_div_en_reg_n_0),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[0] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[0]),
        .Q(spi_sck_div[0]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[1] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[1]),
        .Q(spi_sck_div[1]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[2] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[2]),
        .Q(spi_sck_div[2]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[3] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[3]),
        .Q(spi_sck_div[3]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[4] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[4]),
        .Q(spi_sck_div[4]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[5] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[5]),
        .Q(spi_sck_div[5]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[6] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[6]),
        .Q(spi_sck_div[6]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sck_div_reg[7] 
       (.C(aclk),
        .CE(spi_sck_div_0),
        .D(s_axi_wdata[7]),
        .Q(spi_sck_div[7]),
        .R(s_axi_awready_i_1_n_0));
  LUT3 #(
    .INIT(8'h60)) 
    spi_sck_int_i_1
       (.I0(spi_sck_int),
        .I1(spi_sck_toggle),
        .I2(spi_sck_div_en_reg_n_0),
        .O(spi_sck_int_i_1_n_0));
  FDRE spi_sck_int_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sck_int_i_1_n_0),
        .Q(spi_sck_int),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hFEFEFCFFFEFEFC00)) 
    spi_sck_o_i_1
       (.I0(spi_cpol),
        .I1(spi_sck_o_i_2_n_0),
        .I2(spi_sck_o_i_3_n_0),
        .I3(spi_sck_o_i_4_n_0),
        .I4(spi_sck_o_i_5_n_0),
        .I5(spi_sck_o),
        .O(spi_sck_o_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair32" *) 
  LUT3 #(
    .INIT(8'hE0)) 
    spi_sck_o_i_2
       (.I0(spi_cpol),
        .I1(spi_en),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .O(spi_sck_o_i_2_n_0));
  LUT6 #(
    .INIT(64'hACAFFFFFA0A00000)) 
    spi_sck_o_i_3
       (.I0(spi_sck_o_i_6_n_0),
        .I1(spi_sck_o_i_7_n_0),
        .I2(spi_sck_int),
        .I3(\spi_mode_reg_n_0_[0] ),
        .I4(spi_sck_toggle),
        .I5(spi_sck_o_i_8_n_0),
        .O(spi_sck_o_i_3_n_0));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT2 #(
    .INIT(4'h8)) 
    spi_sck_o_i_4
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I1(spi_sck_toggle),
        .O(spi_sck_o_i_4_n_0));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT5 #(
    .INIT(32'hAAEAFFFF)) 
    spi_sck_o_i_5
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I2(spi_sck_o1__3),
        .I3(spi_sck_o_i_7_n_0),
        .I4(spi_en),
        .O(spi_sck_o_i_5_n_0));
  (* SOFT_HLUTNM = "soft_lutpair20" *) 
  LUT2 #(
    .INIT(4'h8)) 
    spi_sck_o_i_6
       (.I0(spi_cpol),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(spi_sck_o_i_6_n_0));
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    spi_sck_o_i_7
       (.I0(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I1(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .I2(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[4] ),
        .I4(\spi_sr_bit_cnt_reg_n_0_[3] ),
        .O(spi_sck_o_i_7_n_0));
  (* SOFT_HLUTNM = "soft_lutpair32" *) 
  LUT3 #(
    .INIT(8'h08)) 
    spi_sck_o_i_8
       (.I0(spi_en),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I2(spi_cpol),
        .O(spi_sck_o_i_8_n_0));
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi_rtl:1.0 spi SCK_O" *) 
  FDRE spi_sck_o_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sck_o_i_1_n_0),
        .Q(spi_sck_o),
        .R(1'b0));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[0]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[0]),
        .I3(spi_sr_in_buf0_in[0]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[1]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[1]),
        .I3(spi_sr_in_buf0_in[1]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[1]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[2]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[2]),
        .I3(spi_sr_in_buf0_in[2]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[2]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[3]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[3]),
        .I3(spi_sr_in_buf0_in[3]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[3]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[4]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[4]),
        .I3(spi_sr_in_buf0_in[4]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[4]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[5]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[5]),
        .I3(spi_sr_in_buf0_in[5]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[5]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[6]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[6]),
        .I3(spi_sr_in_buf0_in[6]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[6]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \spi_sr[7]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\spi_sr[7]_i_2_n_0 ),
        .I2(spi_sr_out_buf[7]),
        .I3(spi_sr_in_buf0_in[7]),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .O(\spi_sr[7]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair25" *) 
  LUT4 #(
    .INIT(16'hEA00)) 
    \spi_sr[7]_i_2 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I2(\spi_io_mode_q[2]_i_2_n_0 ),
        .I3(spi_sr_out_buf_valid),
        .O(\spi_sr[7]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair19" *) 
  LUT3 #(
    .INIT(8'h28)) 
    \spi_sr_bit_cnt[0]_i_1 
       (.I0(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .I1(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I2(\spi_io_mode_q_reg_n_0_[0] ),
        .O(spi_sr_bit_cnt[0]));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT5 #(
    .INIT(32'h28822828)) 
    \spi_sr_bit_cnt[1]_i_1 
       (.I0(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .I1(\spi_io_mode_q_reg_n_0_[1] ),
        .I2(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I4(\spi_io_mode_q_reg_n_0_[0] ),
        .O(spi_sr_bit_cnt[1]));
  LUT6 #(
    .INIT(64'h20A28A088A0820A2)) 
    \spi_sr_bit_cnt[2]_i_1 
       (.I0(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .I1(\spi_sr_bit_cnt[2]_i_2_n_0 ),
        .I2(\spi_io_mode_q_reg_n_0_[1] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .I4(\spi_io_mode_q_reg_n_0_[2] ),
        .I5(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .O(spi_sr_bit_cnt[2]));
  (* SOFT_HLUTNM = "soft_lutpair34" *) 
  LUT2 #(
    .INIT(4'hB)) 
    \spi_sr_bit_cnt[2]_i_2 
       (.I0(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I1(\spi_io_mode_q_reg_n_0_[0] ),
        .O(\spi_sr_bit_cnt[2]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFF8F0F0F8FFF0F0)) 
    \spi_sr_bit_cnt[3]_i_1 
       (.I0(\spi_io_mode_q[2]_i_2_n_0 ),
        .I1(\FSM_onehot_spi_fsm_state[2]_i_2_n_0 ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I3(\spi_sr_bit_cnt[3]_i_2_n_0 ),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I5(\spi_sr_bit_cnt_reg_n_0_[3] ),
        .O(spi_sr_bit_cnt[3]));
  LUT6 #(
    .INIT(64'hF7FF55F751550051)) 
    \spi_sr_bit_cnt[3]_i_2 
       (.I0(\spi_io_mode_q_reg_n_0_[2] ),
        .I1(\spi_io_mode_q_reg_n_0_[0] ),
        .I2(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I3(\spi_io_mode_q_reg_n_0_[1] ),
        .I4(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .I5(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .O(\spi_sr_bit_cnt[3]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFAA20AA20AA20)) 
    \spi_sr_bit_cnt[4]_i_1 
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .I1(spi_rd_req_ack_reg_n_0),
        .I2(spi_rd_req),
        .I3(spi_sr_out_buf_valid),
        .I4(spi_io_mode_q1__8),
        .I5(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(\spi_sr_bit_cnt[4]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFF8E000000710000)) 
    \spi_sr_bit_cnt[4]_i_2 
       (.I0(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .I1(\spi_sr_bit_cnt[4]_i_3_n_0 ),
        .I2(\spi_io_mode_q_reg_n_0_[2] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[3] ),
        .I4(\spi_sr_bit_cnt[4]_i_4_n_0 ),
        .I5(\spi_sr_bit_cnt_reg_n_0_[4] ),
        .O(spi_sr_bit_cnt[4]));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT4 #(
    .INIT(16'hDF0D)) 
    \spi_sr_bit_cnt[4]_i_3 
       (.I0(\spi_io_mode_q_reg_n_0_[0] ),
        .I1(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .I2(\spi_io_mode_q_reg_n_0_[1] ),
        .I3(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .O(\spi_sr_bit_cnt[4]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT5 #(
    .INIT(32'h55DF0000)) 
    \spi_sr_bit_cnt[4]_i_4 
       (.I0(\spi_io_mode_q[2]_i_2_n_0 ),
        .I1(spi_rd_req_ack_reg_n_0),
        .I2(spi_rd_req),
        .I3(spi_sr_out_buf_valid),
        .I4(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(\spi_sr_bit_cnt[4]_i_4_n_0 ));
  FDRE \spi_sr_bit_cnt_reg[0] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(spi_sr_bit_cnt[0]),
        .Q(\spi_sr_bit_cnt_reg_n_0_[0] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_bit_cnt_reg[1] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(spi_sr_bit_cnt[1]),
        .Q(\spi_sr_bit_cnt_reg_n_0_[1] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_bit_cnt_reg[2] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(spi_sr_bit_cnt[2]),
        .Q(\spi_sr_bit_cnt_reg_n_0_[2] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_bit_cnt_reg[3] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(spi_sr_bit_cnt[3]),
        .Q(\spi_sr_bit_cnt_reg_n_0_[3] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_bit_cnt_reg[4] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(spi_sr_bit_cnt[4]),
        .Q(\spi_sr_bit_cnt_reg_n_0_[4] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[0]_i_1 
       (.I0(\spi_sr_in_buf[0]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(\spi_sr_reg_n_0_[2] ),
        .I3(\spi_io_i_reg_n_0_[1] ),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[0]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[0]_i_2 
       (.I0(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I1(\spi_sr_reg_n_0_[0] ),
        .I2(\spi_io_o[0]_i_6_n_0 ),
        .I3(\spi_io_i_reg_n_0_[0] ),
        .I4(\spi_sr_reg_n_0_[1] ),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[0]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[1]_i_1 
       (.I0(\spi_sr_in_buf[1]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(\spi_sr_reg_n_0_[3] ),
        .I3(\spi_sr_reg_n_0_[0] ),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[1]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[1]_i_2 
       (.I0(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I1(\spi_sr_reg_n_0_[1] ),
        .I2(\spi_io_o[0]_i_6_n_0 ),
        .I3(\spi_io_i_reg_n_0_[1] ),
        .I4(\spi_sr_reg_n_0_[2] ),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[1]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[2]_i_1 
       (.I0(\spi_sr_in_buf[2]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(spi_sr[4]),
        .I3(\spi_sr_reg_n_0_[1] ),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[2]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[2]_i_2 
       (.I0(\spi_io_o[0]_i_6_n_0 ),
        .I1(\spi_sr_reg_n_0_[0] ),
        .I2(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I3(\spi_sr_reg_n_0_[2] ),
        .I4(\spi_sr_reg_n_0_[3] ),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[2]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[3]_i_1 
       (.I0(\spi_sr_in_buf[3]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(spi_sr[5]),
        .I3(\spi_sr_reg_n_0_[2] ),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[3]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[3]_i_2 
       (.I0(\spi_io_o[0]_i_6_n_0 ),
        .I1(\spi_sr_reg_n_0_[1] ),
        .I2(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I3(\spi_sr_reg_n_0_[3] ),
        .I4(spi_sr[4]),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[3]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[4]_i_1 
       (.I0(\spi_sr_in_buf[4]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(\spi_sr_reg_n_0_[6] ),
        .I3(\spi_sr_reg_n_0_[3] ),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[4]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[4]_i_2 
       (.I0(\spi_io_o[0]_i_6_n_0 ),
        .I1(\spi_sr_reg_n_0_[2] ),
        .I2(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I3(spi_sr[4]),
        .I4(spi_sr[5]),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[4]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[5]_i_1 
       (.I0(\spi_sr_in_buf[5]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(p_1_in),
        .I3(spi_sr[4]),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[5]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[5]_i_2 
       (.I0(\spi_io_o[0]_i_6_n_0 ),
        .I1(\spi_sr_reg_n_0_[3] ),
        .I2(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I3(spi_sr[5]),
        .I4(\spi_sr_reg_n_0_[6] ),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[5]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hEAEAFFEA)) 
    \spi_sr_in_buf[6]_i_1 
       (.I0(\spi_sr_in_buf[6]_i_2_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I2(\spi_io_i_reg_n_0_[0] ),
        .I3(spi_sr[5]),
        .I4(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[6]));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \spi_sr_in_buf[6]_i_2 
       (.I0(\spi_io_o[0]_i_6_n_0 ),
        .I1(spi_sr[4]),
        .I2(\spi_sr_in_buf[6]_i_3_n_0 ),
        .I3(\spi_sr_reg_n_0_[6] ),
        .I4(p_1_in),
        .I5(\spi_sr_in_buf[7]_i_4_n_0 ),
        .O(\spi_sr_in_buf[6]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair31" *) 
  LUT3 #(
    .INIT(8'hEB)) 
    \spi_sr_in_buf[6]_i_3 
       (.I0(\spi_io_mode_q_reg_n_0_[2] ),
        .I1(\spi_io_mode_q_reg_n_0_[1] ),
        .I2(\spi_io_mode_q_reg_n_0_[0] ),
        .O(\spi_sr_in_buf[6]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'h00B0000000000000)) 
    \spi_sr_in_buf[7]_i_1 
       (.I0(spi_dir_reg_n_0),
        .I1(\spi_sr_in_buf[7]_i_3_n_0 ),
        .I2(\spi_io_mode_q[2]_i_2_n_0 ),
        .I3(spi_sr_in_buf_valid_reg_n_0),
        .I4(spi_io_mode_q1__8),
        .I5(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(spi_sr_in_buf_valid));
  LUT6 #(
    .INIT(64'hFECCFECCFFFFFECC)) 
    \spi_sr_in_buf[7]_i_2 
       (.I0(\spi_sr_in_buf[7]_i_4_n_0 ),
        .I1(\spi_sr_in_buf[7]_i_5_n_0 ),
        .I2(\spi_sr_in_buf[7]_i_6_n_0 ),
        .I3(\spi_io_i_reg_n_0_[1] ),
        .I4(\spi_sr_reg_n_0_[6] ),
        .I5(\spi_io_o[1]_i_4_n_0 ),
        .O(spi_sr_in_buf0_in[7]));
  (* SOFT_HLUTNM = "soft_lutpair31" *) 
  LUT3 #(
    .INIT(8'hFD)) 
    \spi_sr_in_buf[7]_i_3 
       (.I0(\spi_io_mode_q_reg_n_0_[0] ),
        .I1(\spi_io_mode_q_reg_n_0_[1] ),
        .I2(\spi_io_mode_q_reg_n_0_[2] ),
        .O(\spi_sr_in_buf[7]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair24" *) 
  LUT4 #(
    .INIT(16'h0200)) 
    \spi_sr_in_buf[7]_i_4 
       (.I0(spi_lsb_first_reg_n_0),
        .I1(\spi_io_mode_q_reg_n_0_[2] ),
        .I2(\spi_io_mode_q_reg_n_0_[1] ),
        .I3(\spi_io_mode_q_reg_n_0_[0] ),
        .O(\spi_sr_in_buf[7]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hAAA0A0AAAAACA0AA)) 
    \spi_sr_in_buf[7]_i_5 
       (.I0(p_1_in),
        .I1(spi_sr[5]),
        .I2(\spi_io_mode_q_reg_n_0_[2] ),
        .I3(\spi_io_mode_q_reg_n_0_[0] ),
        .I4(\spi_io_mode_q_reg_n_0_[1] ),
        .I5(spi_lsb_first_reg_n_0),
        .O(\spi_sr_in_buf[7]_i_5_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair26" *) 
  LUT4 #(
    .INIT(16'h0200)) 
    \spi_sr_in_buf[7]_i_6 
       (.I0(spi_lsb_first_reg_n_0),
        .I1(\spi_io_mode_q_reg_n_0_[2] ),
        .I2(\spi_io_mode_q_reg_n_0_[0] ),
        .I3(\spi_io_mode_q_reg_n_0_[1] ),
        .O(\spi_sr_in_buf[7]_i_6_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair18" *) 
  LUT5 #(
    .INIT(32'h40BF4000)) 
    spi_sr_in_buf_ready_i_1
       (.I0(s_axi_rvalid_reg_0),
        .I1(s_axi_arvalid),
        .I2(\s_axi_rdata[7]_i_2_n_0 ),
        .I3(spi_sr_in_buf_valid_reg_n_0),
        .I4(spi_sr_in_buf_ready_reg_n_0),
        .O(spi_sr_in_buf_ready_i_1_n_0));
  FDRE spi_sr_in_buf_ready_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sr_in_buf_ready_i_1_n_0),
        .Q(spi_sr_in_buf_ready_reg_n_0),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_in_buf_reg[0] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[0]),
        .Q(data5[0]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[1] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[1]),
        .Q(data5[1]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[2] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[2]),
        .Q(data5[2]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[3] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[3]),
        .Q(data5[3]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[4] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[4]),
        .Q(data5[4]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[5] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[5]),
        .Q(data5[5]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[6] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[6]),
        .Q(data5[6]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_in_buf_reg[7] 
       (.C(aclk),
        .CE(spi_sr_in_buf_valid),
        .D(spi_sr_in_buf0_in[7]),
        .Q(data5[7]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFF4C4C4C4C4C4C4C)) 
    spi_sr_in_buf_valid_i_1
       (.I0(spi_sr_in_buf_valid_i_2_n_0),
        .I1(spi_sr_in_buf_valid_reg_n_0),
        .I2(spi_sr_in_buf_ready_reg_n_0),
        .I3(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I4(spi_io_mode_q1__8),
        .I5(spi_sr_in_buf_valid_i_3_n_0),
        .O(spi_sr_in_buf_valid_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair10" *) 
  LUT3 #(
    .INIT(8'hFE)) 
    spi_sr_in_buf_valid_i_2
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I2(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .O(spi_sr_in_buf_valid_i_2_n_0));
  LUT6 #(
    .INIT(64'h4444444400000040)) 
    spi_sr_in_buf_valid_i_3
       (.I0(spi_sr_in_buf_valid_reg_n_0),
        .I1(\spi_io_mode_q[2]_i_2_n_0 ),
        .I2(\spi_io_mode_q_reg_n_0_[0] ),
        .I3(\spi_io_mode_q_reg_n_0_[1] ),
        .I4(\spi_io_mode_q_reg_n_0_[2] ),
        .I5(spi_dir_reg_n_0),
        .O(spi_sr_in_buf_valid_i_3_n_0));
  FDRE spi_sr_in_buf_valid_q_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sr_in_buf_valid_reg_n_0),
        .Q(spi_sr_in_buf_valid_q),
        .R(s_axi_awready_i_1_n_0));
  FDRE spi_sr_in_buf_valid_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sr_in_buf_valid_i_1_n_0),
        .Q(spi_sr_in_buf_valid_reg_n_0),
        .R(\spi_io_o[0]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair27" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[0]_i_1 
       (.I0(s_axi_wdata[0]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[0]));
  (* SOFT_HLUTNM = "soft_lutpair27" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[1]_i_1 
       (.I0(s_axi_wdata[1]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[1]));
  (* SOFT_HLUTNM = "soft_lutpair28" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[2]_i_1 
       (.I0(s_axi_wdata[2]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[2]));
  (* SOFT_HLUTNM = "soft_lutpair28" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[3]_i_1 
       (.I0(s_axi_wdata[3]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[3]));
  (* SOFT_HLUTNM = "soft_lutpair29" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[4]_i_1 
       (.I0(s_axi_wdata[4]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[4]));
  (* SOFT_HLUTNM = "soft_lutpair29" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[5]_i_1 
       (.I0(s_axi_wdata[5]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[5]));
  (* SOFT_HLUTNM = "soft_lutpair30" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[6]_i_1 
       (.I0(s_axi_wdata[6]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[6]));
  LUT6 #(
    .INIT(64'h02000000FFFFFFFF)) 
    \spi_sr_out_buf[7]_i_1 
       (.I0(s_axi_awaddr[2]),
        .I1(s_axi_awaddr[3]),
        .I2(s_axi_awaddr[1]),
        .I3(s_axi_awaddr[0]),
        .I4(\spi_sr_out_buf[7]_i_3_n_0 ),
        .I5(spi_en),
        .O(\spi_sr_out_buf[7]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair21" *) 
  LUT4 #(
    .INIT(16'h0100)) 
    \spi_sr_out_buf[7]_i_10 
       (.I0(s_axi_awaddr[1]),
        .I1(s_axi_awaddr[0]),
        .I2(s_axi_awaddr[3]),
        .I3(s_axi_awaddr[2]),
        .O(\spi_sr_out_buf[7]_i_10_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair21" *) 
  LUT3 #(
    .INIT(8'h10)) 
    \spi_sr_out_buf[7]_i_11 
       (.I0(s_axi_awaddr[2]),
        .I1(s_axi_awaddr[3]),
        .I2(s_axi_awaddr[1]),
        .O(\spi_sr_out_buf[7]_i_11_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair30" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    \spi_sr_out_buf[7]_i_2 
       (.I0(s_axi_wdata[7]),
        .I1(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I2(\spi_sr_out_buf[7]_i_5_n_0 ),
        .O(p_2_in[7]));
  LUT6 #(
    .INIT(64'h0000008000000000)) 
    \spi_sr_out_buf[7]_i_3 
       (.I0(s_axi_wstrb[0]),
        .I1(s_axi_wready),
        .I2(s_axi_wstrb[1]),
        .I3(spi_sr_out_buf_valid),
        .I4(s_axi_wdata[9]),
        .I5(s_axi_wdata[8]),
        .O(\spi_sr_out_buf[7]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT5 #(
    .INIT(32'h00200000)) 
    \spi_sr_out_buf[7]_i_4 
       (.I0(s_axi_awaddr[2]),
        .I1(s_axi_awaddr[3]),
        .I2(s_axi_awaddr[0]),
        .I3(s_axi_awaddr[1]),
        .I4(spi_en),
        .O(\spi_sr_out_buf[7]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFBFAA)) 
    \spi_sr_out_buf[7]_i_5 
       (.I0(\spi_sr_out_buf[7]_i_6_n_0 ),
        .I1(s_axi_wstrb[0]),
        .I2(s_axi_wstrb[1]),
        .I3(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I4(\spi_sr_out_buf[7]_i_7_n_0 ),
        .I5(\spi_sr_out_buf[7]_i_8_n_0 ),
        .O(\spi_sr_out_buf[7]_i_5_n_0 ));
  LUT6 #(
    .INIT(64'hFFD0D0D0D0D0D0D0)) 
    \spi_sr_out_buf[7]_i_6 
       (.I0(s_axi_wdata[8]),
        .I1(s_axi_wdata[9]),
        .I2(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I3(s_axi_wready),
        .I4(\spi_sr_out_buf[7]_i_9_n_0 ),
        .I5(spi_en),
        .O(\spi_sr_out_buf[7]_i_6_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair16" *) 
  LUT5 #(
    .INIT(32'h00020000)) 
    \spi_sr_out_buf[7]_i_7 
       (.I0(spi_en),
        .I1(s_axi_awaddr[1]),
        .I2(s_axi_awaddr[3]),
        .I3(s_axi_awaddr[2]),
        .I4(s_axi_wready),
        .O(\spi_sr_out_buf[7]_i_7_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFC8CCC8CCC8CC)) 
    \spi_sr_out_buf[7]_i_8 
       (.I0(\spi_sr_out_buf[7]_i_10_n_0 ),
        .I1(spi_en),
        .I2(\spi_sr_out_buf[7]_i_11_n_0 ),
        .I3(s_axi_wready),
        .I4(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I5(spi_sr_out_buf_valid),
        .O(\spi_sr_out_buf[7]_i_8_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair16" *) 
  LUT3 #(
    .INIT(8'hEC)) 
    \spi_sr_out_buf[7]_i_9 
       (.I0(s_axi_awaddr[2]),
        .I1(s_axi_awaddr[3]),
        .I2(s_axi_awaddr[1]),
        .O(\spi_sr_out_buf[7]_i_9_n_0 ));
  LUT6 #(
    .INIT(64'hFCFCFCFCFC4C4C4C)) 
    spi_sr_out_buf_ready_i_1
       (.I0(spi_sr_out_buf_ready_i_2_n_0),
        .I1(spi_sr_out_buf_ready_reg_n_0),
        .I2(spi_sr_out_buf_valid),
        .I3(spi_io_mode_q1__8),
        .I4(spi_sr_out_buf_ready_i_3_n_0),
        .I5(\FSM_onehot_spi_fsm_state_reg_n_0_[0] ),
        .O(spi_sr_out_buf_ready_i_1_n_0));
  LUT2 #(
    .INIT(4'hE)) 
    spi_sr_out_buf_ready_i_2
       (.I0(\FSM_onehot_spi_fsm_state_reg_n_0_[1] ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(spi_sr_out_buf_ready_i_2_n_0));
  (* SOFT_HLUTNM = "soft_lutpair25" *) 
  LUT2 #(
    .INIT(4'h8)) 
    spi_sr_out_buf_ready_i_3
       (.I0(\spi_io_mode_q[2]_i_2_n_0 ),
        .I1(\FSM_onehot_spi_fsm_state_reg_n_0_[2] ),
        .O(spi_sr_out_buf_ready_i_3_n_0));
  FDRE spi_sr_out_buf_ready_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sr_out_buf_ready_i_1_n_0),
        .Q(spi_sr_out_buf_ready_reg_n_0),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_out_buf_reg[0] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[0]),
        .Q(spi_sr_out_buf[0]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[1] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[1]),
        .Q(spi_sr_out_buf[1]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[2] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[2]),
        .Q(spi_sr_out_buf[2]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[3] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[3]),
        .Q(spi_sr_out_buf[3]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[4] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[4]),
        .Q(spi_sr_out_buf[4]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[5] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[5]),
        .Q(spi_sr_out_buf[5]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[6] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[6]),
        .Q(spi_sr_out_buf[6]),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_out_buf_reg[7] 
       (.C(aclk),
        .CE(\spi_sr_out_buf[7]_i_1_n_0 ),
        .D(p_2_in[7]),
        .Q(spi_sr_out_buf[7]),
        .R(s_axi_awready_i_1_n_0));
  LUT4 #(
    .INIT(16'hAEAA)) 
    spi_sr_out_buf_valid_i_1
       (.I0(spi_sr_out_buf_valid_i_2_n_0),
        .I1(spi_sr_out_buf_valid),
        .I2(spi_sr_out_buf_ready_reg_n_0),
        .I3(spi_en),
        .O(spi_sr_out_buf_valid_i_1_n_0));
  LUT6 #(
    .INIT(64'h4000000000000000)) 
    spi_sr_out_buf_valid_i_2
       (.I0(spi_sr_out_buf_valid),
        .I1(spi_rd_req128_out__0),
        .I2(\spi_sr_out_buf[7]_i_4_n_0 ),
        .I3(s_axi_wstrb[1]),
        .I4(s_axi_wstrb[0]),
        .I5(s_axi_wready),
        .O(spi_sr_out_buf_valid_i_2_n_0));
  LUT2 #(
    .INIT(4'h2)) 
    spi_sr_out_buf_valid_i_3
       (.I0(s_axi_wdata[8]),
        .I1(s_axi_wdata[9]),
        .O(spi_rd_req128_out__0));
  FDRE spi_sr_out_buf_valid_reg
       (.C(aclk),
        .CE(1'b1),
        .D(spi_sr_out_buf_valid_i_1_n_0),
        .Q(spi_sr_out_buf_valid),
        .R(s_axi_awready_i_1_n_0));
  FDRE \spi_sr_reg[0] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[0]_i_1_n_0 ),
        .Q(\spi_sr_reg_n_0_[0] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[1] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[1]_i_1_n_0 ),
        .Q(\spi_sr_reg_n_0_[1] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[2] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[2]_i_1_n_0 ),
        .Q(\spi_sr_reg_n_0_[2] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[3] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[3]_i_1_n_0 ),
        .Q(\spi_sr_reg_n_0_[3] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[4] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[4]_i_1_n_0 ),
        .Q(spi_sr[4]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[5] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[5]_i_1_n_0 ),
        .Q(spi_sr[5]),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[6] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[6]_i_1_n_0 ),
        .Q(\spi_sr_reg_n_0_[6] ),
        .R(\spi_io_o[0]_i_1_n_0 ));
  FDRE \spi_sr_reg[7] 
       (.C(aclk),
        .CE(\spi_sr_bit_cnt[4]_i_1_n_0 ),
        .D(\spi_sr[7]_i_1_n_0 ),
        .Q(p_1_in),
        .R(\spi_io_o[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFEFFFFF00200000)) 
    \spi_ss_o[0]_i_1 
       (.I0(s_axi_wdata[0]),
        .I1(\spi_ss_o[0]_i_2_n_0 ),
        .I2(s_axi_awaddr[3]),
        .I3(s_axi_awaddr[2]),
        .I4(\spi_ss_o[0]_i_3_n_0 ),
        .I5(spi_ss_o),
        .O(\spi_ss_o[0]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair22" *) 
  LUT2 #(
    .INIT(4'h7)) 
    \spi_ss_o[0]_i_2 
       (.I0(s_axi_wstrb[0]),
        .I1(s_axi_wready),
        .O(\spi_ss_o[0]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h1)) 
    \spi_ss_o[0]_i_3 
       (.I0(s_axi_awaddr[0]),
        .I1(s_axi_awaddr[1]),
        .O(\spi_ss_o[0]_i_3_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:spi_rtl:1.0 spi SS_O" *) 
  FDSE \spi_ss_o_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\spi_ss_o[0]_i_1_n_0 ),
        .Q(spi_ss_o),
        .S(s_axi_awready_i_1_n_0));
endmodule
`ifndef GLBL
`define GLBL
`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;
    parameter GRES_WIDTH = 10000;
    parameter GRES_START = 10000;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    wire GRESTORE;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;
    wire FCSBO_GLBL;
    wire [3:0] DO_GLBL;
    wire [3:0] DI_GLBL;
   
    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;
    reg GRESTORE_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (strong1, weak0) GSR = GSR_int;
    assign (strong1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;
    assign (strong1, weak0) GRESTORE = GRESTORE_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

    initial begin 
	GRESTORE_int = 1'b0;
	#(GRES_START);
	GRESTORE_int = 1'b1;
	#(GRES_WIDTH);
	GRESTORE_int = 1'b0;
    end

endmodule
`endif
