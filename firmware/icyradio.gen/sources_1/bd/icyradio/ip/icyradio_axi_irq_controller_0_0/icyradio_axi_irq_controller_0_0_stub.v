// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2021.1 (lin64) Build 3247384 Thu Jun 10 19:36:07 MDT 2021
// Date        : Fri Oct 27 00:54:26 2023
// Host        : node4-dev running 64-bit Ubuntu 22.04.3 LTS
// Command     : write_verilog -force -mode synth_stub -rename_top icyradio_axi_irq_controller_0_0 -prefix
//               icyradio_axi_irq_controller_0_0_ icyradio_axi_irq_controller_0_0_stub.v
// Design      : icyradio_axi_irq_controller_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a100tfgg484-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "axi_irq_controller,Vivado 2021.1" *)
module icyradio_axi_irq_controller_0_0(aclk, aresetn, s_axi_awaddr, s_axi_awprot, 
  s_axi_awvalid, s_axi_awready, s_axi_wdata, s_axi_wstrb, s_axi_wvalid, s_axi_wready, 
  s_axi_bresp, s_axi_bvalid, s_axi_bready, s_axi_araddr, s_axi_arprot, s_axi_arvalid, 
  s_axi_arready, s_axi_rdata, s_axi_rresp, s_axi_rvalid, s_axi_rready, irq_in, cpu_irq_out, 
  cpu_eoi_in, pcie_msi_enabled, pcie_msi_granted, pcie_msi_vector_width, pcie_msi_request, 
  pcie_msi_vector)
/* synthesis syn_black_box black_box_pad_pin="aclk,aresetn,s_axi_awaddr[7:0],s_axi_awprot[2:0],s_axi_awvalid,s_axi_awready,s_axi_wdata[31:0],s_axi_wstrb[3:0],s_axi_wvalid,s_axi_wready,s_axi_bresp[1:0],s_axi_bvalid,s_axi_bready,s_axi_araddr[7:0],s_axi_arprot[2:0],s_axi_arvalid,s_axi_arready,s_axi_rdata[31:0],s_axi_rresp[1:0],s_axi_rvalid,s_axi_rready,irq_in[15:0],cpu_irq_out[31:0],cpu_eoi_in[31:0],pcie_msi_enabled,pcie_msi_granted,pcie_msi_vector_width[2:0],pcie_msi_request,pcie_msi_vector[4:0]" */;
  input aclk;
  input aresetn;
  input [7:0]s_axi_awaddr;
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
  input [7:0]s_axi_araddr;
  input [2:0]s_axi_arprot;
  input s_axi_arvalid;
  output s_axi_arready;
  output [31:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  output s_axi_rvalid;
  input s_axi_rready;
  input [15:0]irq_in;
  output [31:0]cpu_irq_out;
  input [31:0]cpu_eoi_in;
  input pcie_msi_enabled;
  input pcie_msi_granted;
  input [2:0]pcie_msi_vector_width;
  output pcie_msi_request;
  output [4:0]pcie_msi_vector;
endmodule
