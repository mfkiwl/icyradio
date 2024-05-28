// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2023.2 (lin64) Build 4029153 Fri Oct 13 20:13:54 MDT 2023
// Date        : Sun May 26 20:15:13 2024
// Host        : node4-dev running 64-bit Ubuntu 22.04.4 LTS
// Command     : write_verilog -force -mode funcsim
//               /home/joao/icyradio/firmware/a200t/icyradio.gen/sources_1/bd/icyradio/ip/icyradio_axi_dna_0_0/icyradio_axi_dna_0_0_sim_netlist.v
// Design      : icyradio_axi_dna_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7a200tfbg484-2
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "icyradio_axi_dna_0_0,axi_dna,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "module_ref" *) 
(* X_CORE_INFO = "axi_dna,Vivado 2023.2" *) 
(* NotValidForBitStream *)
module icyradio_axi_dna_0_0
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
    dna,
    dna_ready,
    efuse_usr,
    usr_access);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 aclk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME aclk, ASSOCIATED_BUSIF s_axi, ASSOCIATED_RESET aresetn, FREQ_HZ 125000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN icyradio_axi_pcie_0_0_axi_aclk_out, INSERT_VIP 0" *) input aclk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 aresetn RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME aresetn, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input aresetn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWADDR" *) input [4:0]s_axi_awaddr;
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
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARADDR" *) input [4:0]s_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARPROT" *) input [2:0]s_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARVALID" *) input s_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi ARREADY" *) output s_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) output [31:0]s_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RRESP" *) output [1:0]s_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RVALID" *) output s_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s_axi, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 125000000, ID_WIDTH 0, ADDR_WIDTH 5, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN icyradio_axi_pcie_0_0_axi_aclk_out, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input s_axi_rready;
  output [56:0]dna;
  output dna_ready;
  output [31:0]efuse_usr;
  output [31:0]usr_access;

  wire \<const0> ;
  wire aclk;
  wire aresetn;
  wire [56:0]dna;
  wire dna_ready;
  wire [31:0]efuse_usr;
  wire [4:0]s_axi_araddr;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire s_axi_awready;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire [1:1]\^s_axi_bresp ;
  wire s_axi_bvalid;
  wire [31:0]s_axi_rdata;
  wire s_axi_rready;
  wire s_axi_rvalid;
  wire s_axi_wvalid;
  wire [31:0]usr_access;

  assign s_axi_bresp[1] = \^s_axi_bresp [1];
  assign s_axi_bresp[0] = \<const0> ;
  assign s_axi_rresp[1] = \<const0> ;
  assign s_axi_rresp[0] = \<const0> ;
  assign s_axi_wready = s_axi_awready;
  GND GND
       (.G(\<const0> ));
  icyradio_axi_dna_0_0_axi_dna inst
       (.aclk(aclk),
        .aresetn(aresetn),
        .dna(dna),
        .dna_ready_reg_0(dna_ready),
        .efuse_usr(efuse_usr),
        .s_axi_araddr(s_axi_araddr[4:2]),
        .s_axi_arready(s_axi_arready),
        .s_axi_arvalid(s_axi_arvalid),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_bready(s_axi_bready),
        .s_axi_bresp(\^s_axi_bresp ),
        .s_axi_bvalid(s_axi_bvalid),
        .s_axi_rdata(s_axi_rdata),
        .s_axi_rready(s_axi_rready),
        .s_axi_rvalid_reg_0(s_axi_rvalid),
        .s_axi_wready(s_axi_awready),
        .s_axi_wvalid(s_axi_wvalid),
        .usr_access(usr_access));
endmodule

(* ORIG_REF_NAME = "axi_dna" *) 
module icyradio_axi_dna_0_0_axi_dna
   (s_axi_rvalid_reg_0,
    efuse_usr,
    s_axi_wready,
    usr_access,
    dna,
    s_axi_rdata,
    s_axi_bvalid,
    dna_ready_reg_0,
    s_axi_arready,
    s_axi_bresp,
    s_axi_arvalid,
    s_axi_rready,
    aresetn,
    aclk,
    s_axi_bready,
    s_axi_wvalid,
    s_axi_awvalid,
    s_axi_araddr);
  output s_axi_rvalid_reg_0;
  output [31:0]efuse_usr;
  output s_axi_wready;
  output [31:0]usr_access;
  output [56:0]dna;
  output [31:0]s_axi_rdata;
  output s_axi_bvalid;
  output dna_ready_reg_0;
  output s_axi_arready;
  output [0:0]s_axi_bresp;
  input s_axi_arvalid;
  input s_axi_rready;
  input aresetn;
  input aclk;
  input s_axi_bready;
  input s_axi_wvalid;
  input s_axi_awvalid;
  input [2:0]s_axi_araddr;

  wire READ0;
  wire aclk;
  wire aresetn;
  wire [56:0]dna;
  wire dna0__4;
  wire \dna[56]_i_1_n_0 ;
  wire dna_clk;
  wire \dna_clk_div_cnt[2]_i_1_n_0 ;
  wire \dna_clk_div_cnt[3]_i_1_n_0 ;
  wire [3:0]dna_clk_div_cnt_reg;
  wire dna_clk_i_1_n_0;
  wire dna_clk_toggle;
  wire dna_dout;
  wire dna_ready_i_1_n_0;
  wire dna_ready_reg_0;
  wire [5:0]dna_shift_cnt_reg;
  wire [31:0]efuse_usr;
  wire [5:0]p_0_in;
  wire [3:0]p_0_in__0;
  wire [2:0]s_axi_araddr;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire s_axi_awready0__0;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire [0:0]s_axi_bresp;
  wire \s_axi_bresp[1]_i_1_n_0 ;
  wire s_axi_bvalid;
  wire s_axi_bvalid_i_1_n_0;
  wire [31:0]s_axi_rdata;
  wire \s_axi_rdata[0]_i_2_n_0 ;
  wire \s_axi_rdata[10]_i_2_n_0 ;
  wire \s_axi_rdata[11]_i_2_n_0 ;
  wire \s_axi_rdata[12]_i_2_n_0 ;
  wire \s_axi_rdata[13]_i_2_n_0 ;
  wire \s_axi_rdata[14]_i_2_n_0 ;
  wire \s_axi_rdata[15]_i_2_n_0 ;
  wire \s_axi_rdata[16]_i_2_n_0 ;
  wire \s_axi_rdata[17]_i_2_n_0 ;
  wire \s_axi_rdata[18]_i_2_n_0 ;
  wire \s_axi_rdata[19]_i_2_n_0 ;
  wire \s_axi_rdata[1]_i_2_n_0 ;
  wire \s_axi_rdata[20]_i_2_n_0 ;
  wire \s_axi_rdata[21]_i_2_n_0 ;
  wire \s_axi_rdata[22]_i_2_n_0 ;
  wire \s_axi_rdata[23]_i_2_n_0 ;
  wire \s_axi_rdata[24]_i_2_n_0 ;
  wire \s_axi_rdata[2]_i_2_n_0 ;
  wire \s_axi_rdata[31]_i_1_n_0 ;
  wire \s_axi_rdata[31]_i_2_n_0 ;
  wire \s_axi_rdata[31]_i_4_n_0 ;
  wire \s_axi_rdata[3]_i_2_n_0 ;
  wire \s_axi_rdata[4]_i_2_n_0 ;
  wire \s_axi_rdata[5]_i_2_n_0 ;
  wire \s_axi_rdata[6]_i_2_n_0 ;
  wire \s_axi_rdata[7]_i_2_n_0 ;
  wire \s_axi_rdata[8]_i_2_n_0 ;
  wire \s_axi_rdata[9]_i_2_n_0 ;
  wire [31:0]s_axi_rdata_0;
  wire s_axi_rready;
  wire s_axi_rvalid_i_1_n_0;
  wire s_axi_rvalid_reg_0;
  wire s_axi_wready;
  wire s_axi_wvalid;
  wire [31:0]usr_access;
  wire usr_access0;
  wire [31:0]usr_access_r;
  wire usr_access_ready;
  wire usr_access_valid;
  wire NLW_USR_ACCESSE2_inst_CFGCLK_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  DNA_PORT #(
    .SIM_DNA_VALUE(57'h000000000000000)) 
    DNA_PORT_inst
       (.CLK(dna_clk),
        .DIN(1'b0),
        .DOUT(dna_dout),
        .READ(READ0),
        .SHIFT(1'b1));
  (* BOX_TYPE = "PRIMITIVE" *) 
  EFUSE_USR #(
    .SIM_EFUSE_VALUE(32'h00000000)) 
    EFUSE_USR_inst
       (.EFUSEUSR(efuse_usr));
  (* BOX_TYPE = "PRIMITIVE" *) 
  USR_ACCESSE2 USR_ACCESSE2_inst
       (.CFGCLK(NLW_USR_ACCESSE2_inst_CFGCLK_UNCONNECTED),
        .DATA(usr_access_r),
        .DATAVALID(usr_access_valid));
  LUT4 #(
    .INIT(16'h0400)) 
    \dna[56]_i_1 
       (.I0(dna_ready_reg_0),
        .I1(dna_clk_toggle),
        .I2(dna_clk),
        .I3(dna0__4),
        .O(\dna[56]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT4 #(
    .INIT(16'h4000)) 
    \dna[56]_i_2 
       (.I0(dna_clk_div_cnt_reg[3]),
        .I1(dna_clk_div_cnt_reg[2]),
        .I2(dna_clk_div_cnt_reg[1]),
        .I3(dna_clk_div_cnt_reg[0]),
        .O(dna_clk_toggle));
  LUT6 #(
    .INIT(64'h01FFFFFFFFFFFFFF)) 
    \dna[56]_i_3 
       (.I0(dna_shift_cnt_reg[0]),
        .I1(dna_shift_cnt_reg[1]),
        .I2(dna_shift_cnt_reg[2]),
        .I3(dna_shift_cnt_reg[4]),
        .I4(dna_shift_cnt_reg[5]),
        .I5(dna_shift_cnt_reg[3]),
        .O(dna0__4));
  LUT1 #(
    .INIT(2'h1)) 
    \dna_clk_div_cnt[0]_i_1 
       (.I0(dna_clk_div_cnt_reg[0]),
        .O(p_0_in__0[0]));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \dna_clk_div_cnt[1]_i_1 
       (.I0(dna_clk_div_cnt_reg[0]),
        .I1(dna_clk_div_cnt_reg[1]),
        .O(p_0_in__0[1]));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \dna_clk_div_cnt[2]_i_1 
       (.I0(dna_clk_div_cnt_reg[1]),
        .I1(dna_clk_div_cnt_reg[0]),
        .I2(dna_clk_div_cnt_reg[2]),
        .O(\dna_clk_div_cnt[2]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hAAAAEAAA)) 
    \dna_clk_div_cnt[3]_i_1 
       (.I0(dna_ready_reg_0),
        .I1(dna_clk_div_cnt_reg[0]),
        .I2(dna_clk_div_cnt_reg[1]),
        .I3(dna_clk_div_cnt_reg[2]),
        .I4(dna_clk_div_cnt_reg[3]),
        .O(\dna_clk_div_cnt[3]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \dna_clk_div_cnt[3]_i_2 
       (.I0(dna_clk_div_cnt_reg[0]),
        .I1(dna_clk_div_cnt_reg[1]),
        .I2(dna_clk_div_cnt_reg[2]),
        .I3(dna_clk_div_cnt_reg[3]),
        .O(p_0_in__0[3]));
  FDRE \dna_clk_div_cnt_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in__0[0]),
        .Q(dna_clk_div_cnt_reg[0]),
        .R(\dna_clk_div_cnt[3]_i_1_n_0 ));
  FDRE \dna_clk_div_cnt_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in__0[1]),
        .Q(dna_clk_div_cnt_reg[1]),
        .R(\dna_clk_div_cnt[3]_i_1_n_0 ));
  FDRE \dna_clk_div_cnt_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(\dna_clk_div_cnt[2]_i_1_n_0 ),
        .Q(dna_clk_div_cnt_reg[2]),
        .R(\dna_clk_div_cnt[3]_i_1_n_0 ));
  FDRE \dna_clk_div_cnt_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(p_0_in__0[3]),
        .Q(dna_clk_div_cnt_reg[3]),
        .R(\dna_clk_div_cnt[3]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h000000009AAAAAAA)) 
    dna_clk_i_1
       (.I0(dna_clk),
        .I1(dna_clk_div_cnt_reg[3]),
        .I2(dna_clk_div_cnt_reg[2]),
        .I3(dna_clk_div_cnt_reg[1]),
        .I4(dna_clk_div_cnt_reg[0]),
        .I5(dna_ready_reg_0),
        .O(dna_clk_i_1_n_0));
  FDRE dna_clk_reg
       (.C(aclk),
        .CE(1'b1),
        .D(dna_clk_i_1_n_0),
        .Q(dna_clk),
        .R(1'b0));
  LUT5 #(
    .INIT(32'h888888A8)) 
    dna_ready_i_1
       (.I0(aresetn),
        .I1(dna_ready_reg_0),
        .I2(dna_clk_toggle),
        .I3(dna_clk),
        .I4(dna0__4),
        .O(dna_ready_i_1_n_0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE dna_ready_reg
       (.C(aclk),
        .CE(1'b1),
        .D(dna_ready_i_1_n_0),
        .Q(dna_ready_reg_0),
        .R(1'b0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[0] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna_dout),
        .Q(dna[0]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[10] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[9]),
        .Q(dna[10]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[11] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[10]),
        .Q(dna[11]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[12] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[11]),
        .Q(dna[12]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[13] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[12]),
        .Q(dna[13]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[14] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[13]),
        .Q(dna[14]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[15] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[14]),
        .Q(dna[15]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[16] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[15]),
        .Q(dna[16]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[17] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[16]),
        .Q(dna[17]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[18] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[17]),
        .Q(dna[18]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[19] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[18]),
        .Q(dna[19]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[1] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[0]),
        .Q(dna[1]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[20] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[19]),
        .Q(dna[20]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[21] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[20]),
        .Q(dna[21]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[22] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[21]),
        .Q(dna[22]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[23] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[22]),
        .Q(dna[23]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[24] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[23]),
        .Q(dna[24]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[25] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[24]),
        .Q(dna[25]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[26] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[25]),
        .Q(dna[26]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[27] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[26]),
        .Q(dna[27]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[28] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[27]),
        .Q(dna[28]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[29] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[28]),
        .Q(dna[29]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[2] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[1]),
        .Q(dna[2]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[30] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[29]),
        .Q(dna[30]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[31] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[30]),
        .Q(dna[31]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[32] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[31]),
        .Q(dna[32]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[33] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[32]),
        .Q(dna[33]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[34] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[33]),
        .Q(dna[34]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[35] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[34]),
        .Q(dna[35]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[36] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[35]),
        .Q(dna[36]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[37] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[36]),
        .Q(dna[37]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[38] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[37]),
        .Q(dna[38]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[39] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[38]),
        .Q(dna[39]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[3] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[2]),
        .Q(dna[3]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[40] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[39]),
        .Q(dna[40]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[41] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[40]),
        .Q(dna[41]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[42] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[41]),
        .Q(dna[42]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[43] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[42]),
        .Q(dna[43]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[44] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[43]),
        .Q(dna[44]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[45] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[44]),
        .Q(dna[45]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[46] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[45]),
        .Q(dna[46]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[47] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[46]),
        .Q(dna[47]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[48] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[47]),
        .Q(dna[48]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[49] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[48]),
        .Q(dna[49]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[4] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[3]),
        .Q(dna[4]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[50] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[49]),
        .Q(dna[50]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[51] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[50]),
        .Q(dna[51]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[52] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[51]),
        .Q(dna[52]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[53] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[52]),
        .Q(dna[53]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[54] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[53]),
        .Q(dna[54]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[55] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[54]),
        .Q(dna[55]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[56] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[55]),
        .Q(dna[56]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[5] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[4]),
        .Q(dna[5]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[6] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[5]),
        .Q(dna[6]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[7] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[6]),
        .Q(dna[7]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[8] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[7]),
        .Q(dna[8]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \dna_reg[9] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(dna[8]),
        .Q(dna[9]),
        .R(READ0));
  LUT1 #(
    .INIT(2'h1)) 
    \dna_shift_cnt[0]_i_1 
       (.I0(dna_shift_cnt_reg[0]),
        .O(p_0_in[0]));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \dna_shift_cnt[1]_i_1 
       (.I0(dna_shift_cnt_reg[0]),
        .I1(dna_shift_cnt_reg[1]),
        .O(p_0_in[1]));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \dna_shift_cnt[2]_i_1 
       (.I0(dna_shift_cnt_reg[0]),
        .I1(dna_shift_cnt_reg[1]),
        .I2(dna_shift_cnt_reg[2]),
        .O(p_0_in[2]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \dna_shift_cnt[3]_i_1 
       (.I0(dna_shift_cnt_reg[1]),
        .I1(dna_shift_cnt_reg[0]),
        .I2(dna_shift_cnt_reg[2]),
        .I3(dna_shift_cnt_reg[3]),
        .O(p_0_in[3]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \dna_shift_cnt[4]_i_1 
       (.I0(dna_shift_cnt_reg[2]),
        .I1(dna_shift_cnt_reg[0]),
        .I2(dna_shift_cnt_reg[1]),
        .I3(dna_shift_cnt_reg[3]),
        .I4(dna_shift_cnt_reg[4]),
        .O(p_0_in[4]));
  LUT6 #(
    .INIT(64'h7FFFFFFF80000000)) 
    \dna_shift_cnt[5]_i_1 
       (.I0(dna_shift_cnt_reg[3]),
        .I1(dna_shift_cnt_reg[1]),
        .I2(dna_shift_cnt_reg[0]),
        .I3(dna_shift_cnt_reg[2]),
        .I4(dna_shift_cnt_reg[4]),
        .I5(dna_shift_cnt_reg[5]),
        .O(p_0_in[5]));
  FDRE \dna_shift_cnt_reg[0] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(p_0_in[0]),
        .Q(dna_shift_cnt_reg[0]),
        .R(READ0));
  FDRE \dna_shift_cnt_reg[1] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(p_0_in[1]),
        .Q(dna_shift_cnt_reg[1]),
        .R(READ0));
  FDRE \dna_shift_cnt_reg[2] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(p_0_in[2]),
        .Q(dna_shift_cnt_reg[2]),
        .R(READ0));
  FDRE \dna_shift_cnt_reg[3] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(p_0_in[3]),
        .Q(dna_shift_cnt_reg[3]),
        .R(READ0));
  FDRE \dna_shift_cnt_reg[4] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(p_0_in[4]),
        .Q(dna_shift_cnt_reg[4]),
        .R(READ0));
  FDRE \dna_shift_cnt_reg[5] 
       (.C(aclk),
        .CE(\dna[56]_i_1_n_0 ),
        .D(p_0_in[5]),
        .Q(dna_shift_cnt_reg[5]),
        .R(READ0));
  (* SOFT_HLUTNM = "soft_lutpair12" *) 
  LUT1 #(
    .INIT(2'h1)) 
    s_axi_arready_INST_0
       (.I0(s_axi_rvalid_reg_0),
        .O(s_axi_arready));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT5 #(
    .INIT(32'h0B000000)) 
    s_axi_awready0
       (.I0(s_axi_bready),
        .I1(s_axi_bvalid),
        .I2(s_axi_wready),
        .I3(s_axi_wvalid),
        .I4(s_axi_awvalid),
        .O(s_axi_awready0__0));
  LUT1 #(
    .INIT(2'h1)) 
    s_axi_awready_i_1
       (.I0(aresetn),
        .O(READ0));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi AWREADY" *) 
  FDRE s_axi_awready_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_awready0__0),
        .Q(s_axi_wready),
        .R(READ0));
  LUT3 #(
    .INIT(8'hDC)) 
    \s_axi_bresp[1]_i_1 
       (.I0(s_axi_bready),
        .I1(s_axi_wready),
        .I2(s_axi_bresp),
        .O(\s_axi_bresp[1]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi BRESP" *) 
  FDRE \s_axi_bresp_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\s_axi_bresp[1]_i_1_n_0 ),
        .Q(s_axi_bresp),
        .R(READ0));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
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
        .R(READ0));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT3 #(
    .INIT(8'h8B)) 
    \s_axi_rdata[0]_i_1 
       (.I0(\s_axi_rdata[0]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .O(s_axi_rdata_0[0]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[0]_i_2 
       (.I0(dna[32]),
        .I1(dna[0]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[0]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[0]),
        .O(\s_axi_rdata[0]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair10" *) 
  LUT3 #(
    .INIT(8'hB8)) 
    \s_axi_rdata[10]_i_1 
       (.I0(\s_axi_rdata[10]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .O(s_axi_rdata_0[10]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[10]_i_2 
       (.I0(dna[42]),
        .I1(dna[10]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[10]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[10]),
        .O(\s_axi_rdata[10]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair15" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[11]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[11]_i_2_n_0 ),
        .O(s_axi_rdata_0[11]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[11]_i_2 
       (.I0(dna[43]),
        .I1(dna[11]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[11]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[11]),
        .O(\s_axi_rdata[11]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair15" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[12]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[12]_i_2_n_0 ),
        .O(s_axi_rdata_0[12]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[12]_i_2 
       (.I0(dna[44]),
        .I1(dna[12]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[12]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[12]),
        .O(\s_axi_rdata[12]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT4 #(
    .INIT(16'hB888)) 
    \s_axi_rdata[13]_i_1 
       (.I0(\s_axi_rdata[13]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .O(s_axi_rdata_0[13]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[13]_i_2 
       (.I0(dna[45]),
        .I1(dna[13]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[13]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[13]),
        .O(\s_axi_rdata[13]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT4 #(
    .INIT(16'hB8BB)) 
    \s_axi_rdata[14]_i_1 
       (.I0(\s_axi_rdata[14]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .O(s_axi_rdata_0[14]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[14]_i_2 
       (.I0(dna[46]),
        .I1(dna[14]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[14]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[14]),
        .O(\s_axi_rdata[14]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair16" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[15]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[15]_i_2_n_0 ),
        .O(s_axi_rdata_0[15]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[15]_i_2 
       (.I0(dna[47]),
        .I1(dna[15]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[15]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[15]),
        .O(\s_axi_rdata[15]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT4 #(
    .INIT(16'h8BBB)) 
    \s_axi_rdata[16]_i_1 
       (.I0(\s_axi_rdata[16]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .O(s_axi_rdata_0[16]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[16]_i_2 
       (.I0(dna[48]),
        .I1(dna[16]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[16]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[16]),
        .O(\s_axi_rdata[16]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair16" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[17]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[17]_i_2_n_0 ),
        .O(s_axi_rdata_0[17]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[17]_i_2 
       (.I0(dna[49]),
        .I1(dna[17]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[17]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[17]),
        .O(\s_axi_rdata[17]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair17" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[18]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[18]_i_2_n_0 ),
        .O(s_axi_rdata_0[18]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[18]_i_2 
       (.I0(dna[50]),
        .I1(dna[18]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[18]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[18]),
        .O(\s_axi_rdata[18]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair11" *) 
  LUT3 #(
    .INIT(8'h8B)) 
    \s_axi_rdata[19]_i_1 
       (.I0(\s_axi_rdata[19]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .O(s_axi_rdata_0[19]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[19]_i_2 
       (.I0(dna[51]),
        .I1(dna[19]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[19]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[19]),
        .O(\s_axi_rdata[19]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT4 #(
    .INIT(16'hB888)) 
    \s_axi_rdata[1]_i_1 
       (.I0(\s_axi_rdata[1]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .I3(s_axi_araddr[1]),
        .O(s_axi_rdata_0[1]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[1]_i_2 
       (.I0(dna[33]),
        .I1(dna[1]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[1]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[1]),
        .O(\s_axi_rdata[1]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT4 #(
    .INIT(16'h888B)) 
    \s_axi_rdata[20]_i_1 
       (.I0(\s_axi_rdata[20]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .O(s_axi_rdata_0[20]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[20]_i_2 
       (.I0(dna[52]),
        .I1(dna[20]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[20]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[20]),
        .O(\s_axi_rdata[20]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair17" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[21]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[21]_i_2_n_0 ),
        .O(s_axi_rdata_0[21]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[21]_i_2 
       (.I0(dna[53]),
        .I1(dna[21]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[21]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[21]),
        .O(\s_axi_rdata[21]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair11" *) 
  LUT3 #(
    .INIT(8'h8B)) 
    \s_axi_rdata[22]_i_1 
       (.I0(\s_axi_rdata[22]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .O(s_axi_rdata_0[22]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[22]_i_2 
       (.I0(dna[54]),
        .I1(dna[22]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[22]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[22]),
        .O(\s_axi_rdata[22]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair18" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[23]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[23]_i_2_n_0 ),
        .O(s_axi_rdata_0[23]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[23]_i_2 
       (.I0(dna[55]),
        .I1(dna[23]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[23]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[23]),
        .O(\s_axi_rdata[23]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair10" *) 
  LUT3 #(
    .INIT(8'hB8)) 
    \s_axi_rdata[24]_i_1 
       (.I0(\s_axi_rdata[24]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[0]),
        .O(s_axi_rdata_0[24]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[24]_i_2 
       (.I0(dna[56]),
        .I1(dna[24]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[24]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[24]),
        .O(\s_axi_rdata[24]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h0A0A0000CFC0FFFF)) 
    \s_axi_rdata[25]_i_1 
       (.I0(dna[25]),
        .I1(efuse_usr[25]),
        .I2(s_axi_araddr[0]),
        .I3(usr_access[25]),
        .I4(s_axi_araddr[2]),
        .I5(s_axi_araddr[1]),
        .O(s_axi_rdata_0[25]));
  LUT6 #(
    .INIT(64'h0000AA00CCFFF000)) 
    \s_axi_rdata[26]_i_1 
       (.I0(dna[26]),
        .I1(efuse_usr[26]),
        .I2(usr_access[26]),
        .I3(s_axi_araddr[2]),
        .I4(s_axi_araddr[0]),
        .I5(s_axi_araddr[1]),
        .O(s_axi_rdata_0[26]));
  LUT6 #(
    .INIT(64'h0000AA00CCFFF000)) 
    \s_axi_rdata[27]_i_1 
       (.I0(dna[27]),
        .I1(efuse_usr[27]),
        .I2(usr_access[27]),
        .I3(s_axi_araddr[2]),
        .I4(s_axi_araddr[0]),
        .I5(s_axi_araddr[1]),
        .O(s_axi_rdata_0[27]));
  LUT6 #(
    .INIT(64'h0000CC00AA00F0FF)) 
    \s_axi_rdata[28]_i_1 
       (.I0(dna[28]),
        .I1(efuse_usr[28]),
        .I2(usr_access[28]),
        .I3(s_axi_araddr[2]),
        .I4(s_axi_araddr[1]),
        .I5(s_axi_araddr[0]),
        .O(s_axi_rdata_0[28]));
  LUT6 #(
    .INIT(64'h0A0AA8080000A808)) 
    \s_axi_rdata[29]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(usr_access[29]),
        .I2(s_axi_araddr[0]),
        .I3(efuse_usr[29]),
        .I4(s_axi_araddr[1]),
        .I5(dna[29]),
        .O(s_axi_rdata_0[29]));
  (* SOFT_HLUTNM = "soft_lutpair13" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[2]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[2]_i_2_n_0 ),
        .O(s_axi_rdata_0[2]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[2]_i_2 
       (.I0(dna[34]),
        .I1(dna[2]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[2]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[2]),
        .O(\s_axi_rdata[2]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h00FFAA00CCFFF0FF)) 
    \s_axi_rdata[30]_i_1 
       (.I0(dna[30]),
        .I1(efuse_usr[30]),
        .I2(usr_access[30]),
        .I3(s_axi_araddr[2]),
        .I4(s_axi_araddr[0]),
        .I5(s_axi_araddr[1]),
        .O(s_axi_rdata_0[30]));
  LUT4 #(
    .INIT(16'hD0FF)) 
    \s_axi_rdata[31]_i_1 
       (.I0(s_axi_arvalid),
        .I1(s_axi_rvalid_reg_0),
        .I2(s_axi_rready),
        .I3(aresetn),
        .O(\s_axi_rdata[31]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h2)) 
    \s_axi_rdata[31]_i_2 
       (.I0(s_axi_arvalid),
        .I1(s_axi_rvalid_reg_0),
        .O(\s_axi_rdata[31]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair18" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[31]_i_3 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[31]_i_4_n_0 ),
        .O(s_axi_rdata_0[31]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[31]_i_4 
       (.I0(dna_ready_reg_0),
        .I1(dna[31]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[31]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[31]),
        .O(\s_axi_rdata[31]_i_4_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT4 #(
    .INIT(16'h888B)) 
    \s_axi_rdata[3]_i_1 
       (.I0(\s_axi_rdata[3]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .O(s_axi_rdata_0[3]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[3]_i_2 
       (.I0(dna[35]),
        .I1(dna[3]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[3]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[3]),
        .O(\s_axi_rdata[3]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair13" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[4]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[4]_i_2_n_0 ),
        .O(s_axi_rdata_0[4]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[4]_i_2 
       (.I0(dna[36]),
        .I1(dna[4]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[4]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[4]),
        .O(\s_axi_rdata[4]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair14" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[5]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[5]_i_2_n_0 ),
        .O(s_axi_rdata_0[5]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[5]_i_2 
       (.I0(dna[37]),
        .I1(dna[5]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[5]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[5]),
        .O(\s_axi_rdata[5]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT3 #(
    .INIT(8'h8B)) 
    \s_axi_rdata[6]_i_1 
       (.I0(\s_axi_rdata[6]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .O(s_axi_rdata_0[6]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[6]_i_2 
       (.I0(dna[38]),
        .I1(dna[6]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[6]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[6]),
        .O(\s_axi_rdata[6]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair14" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \s_axi_rdata[7]_i_1 
       (.I0(s_axi_araddr[2]),
        .I1(\s_axi_rdata[7]_i_2_n_0 ),
        .O(s_axi_rdata_0[7]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[7]_i_2 
       (.I0(dna[39]),
        .I1(dna[7]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[7]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[7]),
        .O(\s_axi_rdata[7]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT4 #(
    .INIT(16'h888B)) 
    \s_axi_rdata[8]_i_1 
       (.I0(\s_axi_rdata[8]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .O(s_axi_rdata_0[8]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[8]_i_2 
       (.I0(dna[40]),
        .I1(dna[8]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[8]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[8]),
        .O(\s_axi_rdata[8]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT4 #(
    .INIT(16'h888B)) 
    \s_axi_rdata[9]_i_1 
       (.I0(\s_axi_rdata[9]_i_2_n_0 ),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_araddr[1]),
        .I3(s_axi_araddr[0]),
        .O(s_axi_rdata_0[9]));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \s_axi_rdata[9]_i_2 
       (.I0(dna[41]),
        .I1(dna[9]),
        .I2(s_axi_araddr[1]),
        .I3(efuse_usr[9]),
        .I4(s_axi_araddr[0]),
        .I5(usr_access[9]),
        .O(\s_axi_rdata[9]_i_2_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[0] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[0]),
        .Q(s_axi_rdata[0]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[10] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[10]),
        .Q(s_axi_rdata[10]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[11] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[11]),
        .Q(s_axi_rdata[11]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[12] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[12]),
        .Q(s_axi_rdata[12]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[13] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[13]),
        .Q(s_axi_rdata[13]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[14] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[14]),
        .Q(s_axi_rdata[14]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[15] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[15]),
        .Q(s_axi_rdata[15]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[16] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[16]),
        .Q(s_axi_rdata[16]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[17] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[17]),
        .Q(s_axi_rdata[17]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[18] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[18]),
        .Q(s_axi_rdata[18]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[19] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[19]),
        .Q(s_axi_rdata[19]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[1] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[1]),
        .Q(s_axi_rdata[1]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[20] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[20]),
        .Q(s_axi_rdata[20]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[21] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[21]),
        .Q(s_axi_rdata[21]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[22] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[22]),
        .Q(s_axi_rdata[22]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[23] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[23]),
        .Q(s_axi_rdata[23]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[24] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[24]),
        .Q(s_axi_rdata[24]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[25] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[25]),
        .Q(s_axi_rdata[25]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[26] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[26]),
        .Q(s_axi_rdata[26]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[27] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[27]),
        .Q(s_axi_rdata[27]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[28] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[28]),
        .Q(s_axi_rdata[28]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[29] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[29]),
        .Q(s_axi_rdata[29]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[2] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[2]),
        .Q(s_axi_rdata[2]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[30] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[30]),
        .Q(s_axi_rdata[30]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[31] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[31]),
        .Q(s_axi_rdata[31]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[3] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[3]),
        .Q(s_axi_rdata[3]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[4] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[4]),
        .Q(s_axi_rdata[4]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[5] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[5]),
        .Q(s_axi_rdata[5]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[6] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[6]),
        .Q(s_axi_rdata[6]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[7] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[7]),
        .Q(s_axi_rdata[7]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[8] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[8]),
        .Q(s_axi_rdata[8]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RDATA" *) 
  FDRE \s_axi_rdata_reg[9] 
       (.C(aclk),
        .CE(\s_axi_rdata[31]_i_2_n_0 ),
        .D(s_axi_rdata_0[9]),
        .Q(s_axi_rdata[9]),
        .R(\s_axi_rdata[31]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair12" *) 
  LUT3 #(
    .INIT(8'h4E)) 
    s_axi_rvalid_i_1
       (.I0(s_axi_rvalid_reg_0),
        .I1(s_axi_arvalid),
        .I2(s_axi_rready),
        .O(s_axi_rvalid_i_1_n_0));
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s_axi RVALID" *) 
  FDRE s_axi_rvalid_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_rvalid_i_1_n_0),
        .Q(s_axi_rvalid_reg_0),
        .R(READ0));
  LUT2 #(
    .INIT(4'hB)) 
    \usr_access[31]_i_1 
       (.I0(usr_access_valid),
        .I1(usr_access_ready),
        .O(usr_access0));
  FDRE usr_access_ready_reg
       (.C(aclk),
        .CE(1'b1),
        .D(aresetn),
        .Q(usr_access_ready),
        .R(1'b0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[0] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[0]),
        .Q(usr_access[0]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[10] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[10]),
        .Q(usr_access[10]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[11] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[11]),
        .Q(usr_access[11]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[12] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[12]),
        .Q(usr_access[12]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[13] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[13]),
        .Q(usr_access[13]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[14] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[14]),
        .Q(usr_access[14]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[15] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[15]),
        .Q(usr_access[15]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[16] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[16]),
        .Q(usr_access[16]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[17] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[17]),
        .Q(usr_access[17]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[18] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[18]),
        .Q(usr_access[18]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[19] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[19]),
        .Q(usr_access[19]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[1] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[1]),
        .Q(usr_access[1]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[20] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[20]),
        .Q(usr_access[20]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[21] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[21]),
        .Q(usr_access[21]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[22] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[22]),
        .Q(usr_access[22]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[23] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[23]),
        .Q(usr_access[23]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[24] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[24]),
        .Q(usr_access[24]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[25] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[25]),
        .Q(usr_access[25]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[26] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[26]),
        .Q(usr_access[26]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[27] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[27]),
        .Q(usr_access[27]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[28] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[28]),
        .Q(usr_access[28]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[29] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[29]),
        .Q(usr_access[29]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[2] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[2]),
        .Q(usr_access[2]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[30] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[30]),
        .Q(usr_access[30]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[31] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[31]),
        .Q(usr_access[31]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[3] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[3]),
        .Q(usr_access[3]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[4] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[4]),
        .Q(usr_access[4]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[5] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[5]),
        .Q(usr_access[5]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[6] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[6]),
        .Q(usr_access[6]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[7] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[7]),
        .Q(usr_access[7]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[8] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[8]),
        .Q(usr_access[8]),
        .R(READ0));
  (* X_INTERFACE_IGNORE = "true" *) 
  FDRE \usr_access_reg[9] 
       (.C(aclk),
        .CE(usr_access0),
        .D(usr_access_r[9]),
        .Q(usr_access[9]),
        .R(READ0));
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
