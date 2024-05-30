// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2023.2 (lin64) Build 4029153 Fri Oct 13 20:13:54 MDT 2023
// Date        : Sun May 26 20:15:24 2024
// Host        : node4-dev running 64-bit Ubuntu 22.04.4 LTS
// Command     : write_verilog -force -mode synth_stub
//               /home/joao/icyradio/firmware/a200t/icyradio.gen/sources_1/bd/icyradio/ip/icyradio_axi_i2s_0/icyradio_axi_i2s_0_stub.v
// Design      : icyradio_axi_i2s_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a200tfbg484-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "axi_i2s,Vivado 2023.2" *)
module icyradio_axi_i2s_0(aclk, aresetn, s_axi_awaddr, s_axi_awprot, 
  s_axi_awvalid, s_axi_awready, s_axi_wdata, s_axi_wstrb, s_axi_wvalid, s_axi_wready, 
  s_axi_bresp, s_axi_bvalid, s_axi_bready, s_axi_araddr, s_axi_arprot, s_axi_arvalid, 
  s_axi_arready, s_axi_rdata, s_axi_rresp, s_axi_rvalid, s_axi_rready, irq, i2s_src_clk, 
  i2s_src_resetn, m_axis_tready, m_axis_tvalid, m_axis_tdata, s_axis_tready, s_axis_tvalid, 
  s_axis_tdata, i2s_mclk, i2s_bclk, i2s_lrclk, i2s_sdata_out, i2s_sdata_in)
/* synthesis syn_black_box black_box_pad_pin="aresetn,s_axi_awaddr[4:0],s_axi_awprot[2:0],s_axi_awvalid,s_axi_awready,s_axi_wdata[31:0],s_axi_wstrb[3:0],s_axi_wvalid,s_axi_wready,s_axi_bresp[1:0],s_axi_bvalid,s_axi_bready,s_axi_araddr[4:0],s_axi_arprot[2:0],s_axi_arvalid,s_axi_arready,s_axi_rdata[31:0],s_axi_rresp[1:0],s_axi_rvalid,s_axi_rready,irq,i2s_src_resetn,m_axis_tready,m_axis_tvalid,m_axis_tdata[31:0],s_axis_tready,s_axis_tvalid,s_axis_tdata[31:0],i2s_mclk,i2s_bclk,i2s_lrclk,i2s_sdata_out,i2s_sdata_in" */
/* synthesis syn_force_seq_prim="aclk" */
/* synthesis syn_force_seq_prim="i2s_src_clk" */;
  input aclk /* synthesis syn_isclock = 1 */;
  input aresetn;
  input [4:0]s_axi_awaddr;
  input [2:0]s_axi_awprot;
  input s_axi_awvalid;
  output s_axi_awready;
  input [31:0]s_axi_wdata;
  input [3:0]s_axi_wstrb;
  input s_axi_wvalid;
  output s_axi_wready;
  output [1:0]s_axi_bresp;
  output s_axi_bvalid;
  input s_axi_bready;
  input [4:0]s_axi_araddr;
  input [2:0]s_axi_arprot;
  input s_axi_arvalid;
  output s_axi_arready;
  output [31:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  output s_axi_rvalid;
  input s_axi_rready;
  output irq;
  input i2s_src_clk /* synthesis syn_isclock = 1 */;
  input i2s_src_resetn;
  input m_axis_tready;
  output m_axis_tvalid;
  output [31:0]m_axis_tdata;
  output s_axis_tready;
  input s_axis_tvalid;
  input [31:0]s_axis_tdata;
  output i2s_mclk;
  output i2s_bclk;
  output i2s_lrclk;
  output i2s_sdata_out;
  input i2s_sdata_in;
endmodule