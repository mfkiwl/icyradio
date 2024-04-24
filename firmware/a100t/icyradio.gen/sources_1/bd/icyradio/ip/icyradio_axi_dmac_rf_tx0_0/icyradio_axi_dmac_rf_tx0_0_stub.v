// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2023.2 (lin64) Build 4029153 Fri Oct 13 20:13:54 MDT 2023
// Date        : Wed Apr  3 12:19:01 2024
// Host        : node4-dev running 64-bit Ubuntu 22.04.4 LTS
// Command     : write_verilog -force -mode synth_stub
//               /home/joao/icyradio/firmware/a100t/icyradio.gen/sources_1/bd/icyradio/ip/icyradio_axi_dmac_rf_tx0_0/icyradio_axi_dmac_rf_tx0_0_stub.v
// Design      : icyradio_axi_dmac_rf_tx0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a100tfgg484-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "axi_dmac,Vivado 2023.2" *)
module icyradio_axi_dmac_rf_tx0_0(s_axi_aclk, s_axi_aresetn, s_axi_awvalid, 
  s_axi_awaddr, s_axi_awready, s_axi_awprot, s_axi_wvalid, s_axi_wdata, s_axi_wstrb, 
  s_axi_wready, s_axi_bvalid, s_axi_bresp, s_axi_bready, s_axi_arvalid, s_axi_araddr, 
  s_axi_arready, s_axi_arprot, s_axi_rvalid, s_axi_rready, s_axi_rresp, s_axi_rdata, irq, 
  m_src_axi_aclk, m_src_axi_aresetn, m_src_axi_arready, m_src_axi_arvalid, 
  m_src_axi_araddr, m_src_axi_arlen, m_src_axi_arsize, m_src_axi_arburst, m_src_axi_arprot, 
  m_src_axi_arcache, m_src_axi_rdata, m_src_axi_rready, m_src_axi_rvalid, m_src_axi_rresp, 
  m_src_axi_rlast, m_axis_aclk, m_axis_ready, m_axis_valid, m_axis_data, m_axis_strb, 
  m_axis_keep, m_axis_user, m_axis_id, m_axis_dest, m_axis_last, m_axis_xfer_req)
/* synthesis syn_black_box black_box_pad_pin="s_axi_aresetn,s_axi_awvalid,s_axi_awaddr[10:0],s_axi_awready,s_axi_awprot[2:0],s_axi_wvalid,s_axi_wdata[31:0],s_axi_wstrb[3:0],s_axi_wready,s_axi_bvalid,s_axi_bresp[1:0],s_axi_bready,s_axi_arvalid,s_axi_araddr[10:0],s_axi_arready,s_axi_arprot[2:0],s_axi_rvalid,s_axi_rready,s_axi_rresp[1:0],s_axi_rdata[31:0],irq,m_src_axi_aresetn,m_src_axi_arready,m_src_axi_arvalid,m_src_axi_araddr[31:0],m_src_axi_arlen[7:0],m_src_axi_arsize[2:0],m_src_axi_arburst[1:0],m_src_axi_arprot[2:0],m_src_axi_arcache[3:0],m_src_axi_rdata[63:0],m_src_axi_rready,m_src_axi_rvalid,m_src_axi_rresp[1:0],m_src_axi_rlast,m_axis_ready,m_axis_valid,m_axis_data[31:0],m_axis_strb[3:0],m_axis_keep[3:0],m_axis_user[0:0],m_axis_id[7:0],m_axis_dest[3:0],m_axis_last,m_axis_xfer_req" */
/* synthesis syn_force_seq_prim="s_axi_aclk" */
/* synthesis syn_force_seq_prim="m_src_axi_aclk" */
/* synthesis syn_force_seq_prim="m_axis_aclk" */;
  input s_axi_aclk /* synthesis syn_isclock = 1 */;
  input s_axi_aresetn;
  input s_axi_awvalid;
  input [10:0]s_axi_awaddr;
  output s_axi_awready;
  input [2:0]s_axi_awprot;
  input s_axi_wvalid;
  input [31:0]s_axi_wdata;
  input [3:0]s_axi_wstrb;
  output s_axi_wready;
  output s_axi_bvalid;
  output [1:0]s_axi_bresp;
  input s_axi_bready;
  input s_axi_arvalid;
  input [10:0]s_axi_araddr;
  output s_axi_arready;
  input [2:0]s_axi_arprot;
  output s_axi_rvalid;
  input s_axi_rready;
  output [1:0]s_axi_rresp;
  output [31:0]s_axi_rdata;
  output irq;
  input m_src_axi_aclk /* synthesis syn_isclock = 1 */;
  input m_src_axi_aresetn;
  input m_src_axi_arready;
  output m_src_axi_arvalid;
  output [31:0]m_src_axi_araddr;
  output [7:0]m_src_axi_arlen;
  output [2:0]m_src_axi_arsize;
  output [1:0]m_src_axi_arburst;
  output [2:0]m_src_axi_arprot;
  output [3:0]m_src_axi_arcache;
  input [63:0]m_src_axi_rdata;
  output m_src_axi_rready;
  input m_src_axi_rvalid;
  input [1:0]m_src_axi_rresp;
  input m_src_axi_rlast;
  input m_axis_aclk /* synthesis syn_isclock = 1 */;
  input m_axis_ready;
  output m_axis_valid;
  output [31:0]m_axis_data;
  output [3:0]m_axis_strb;
  output [3:0]m_axis_keep;
  output [0:0]m_axis_user;
  output [7:0]m_axis_id;
  output [3:0]m_axis_dest;
  output m_axis_last;
  output m_axis_xfer_req;
endmodule