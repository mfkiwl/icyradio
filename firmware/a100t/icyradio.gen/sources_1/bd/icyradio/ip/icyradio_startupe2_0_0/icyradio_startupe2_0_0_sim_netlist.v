// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2023.2 (lin64) Build 4029153 Fri Oct 13 20:13:54 MDT 2023
// Date        : Wed Mar 27 12:09:42 2024
// Host        : node4-dev running 64-bit Ubuntu 22.04.4 LTS
// Command     : write_verilog -force -mode funcsim -rename_top icyradio_startupe2_0_0 -prefix
//               icyradio_startupe2_0_0_ icyradio_startupe2_0_0_sim_netlist.v
// Design      : icyradio_startupe2_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7a100tfgg484-2
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "icyradio_startupe2_0_0,startupe2,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "module_ref" *) 
(* X_CORE_INFO = "startupe2,Vivado 2023.2" *) 
(* NotValidForBitStream *)
module icyradio_startupe2_0_0
   (cfg_clk,
    cfg_mclk,
    eos,
    preq,
    pack,
    usrcclk,
    usrcclk_oen,
    usrdone,
    usrdone_oen);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 cfg_clk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME cfg_clk, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN icyradio_startupe2_0_0_cfg_clk, INSERT_VIP 0" *) output cfg_clk;
  output cfg_mclk;
  output eos;
  output preq;
  input pack;
  input usrcclk;
  input usrcclk_oen;
  input usrdone;
  input usrdone_oen;

  wire cfg_clk;
  wire cfg_mclk;
  wire eos;
  wire pack;
  wire preq;
  wire usrcclk;
  wire usrcclk_oen;
  wire usrdone;
  wire usrdone_oen;

  icyradio_startupe2_0_0_startupe2 inst
       (.cfg_clk(cfg_clk),
        .cfg_mclk(cfg_mclk),
        .eos(eos),
        .pack(pack),
        .preq(preq),
        .usrcclk(usrcclk),
        .usrcclk_oen(usrcclk_oen),
        .usrdone(usrdone),
        .usrdone_oen(usrdone_oen));
endmodule

module icyradio_startupe2_0_0_startupe2
   (cfg_clk,
    cfg_mclk,
    eos,
    preq,
    pack,
    usrcclk,
    usrcclk_oen,
    usrdone,
    usrdone_oen);
  output cfg_clk;
  output cfg_mclk;
  output eos;
  output preq;
  input pack;
  input usrcclk;
  input usrcclk_oen;
  input usrdone;
  input usrdone_oen;

  wire cfg_clk;
  wire cfg_mclk;
  wire eos;
  wire pack;
  wire preq;
  wire usrcclk;
  wire usrcclk_oen;
  wire usrdone;
  wire usrdone_oen;

  (* BOX_TYPE = "PRIMITIVE" *) 
  STARTUPE2 #(
    .PROG_USR("FALSE"),
    .SIM_CCLK_FREQ(0.000000)) 
    STARTUPE2_inst
       (.CFGCLK(cfg_clk),
        .CFGMCLK(cfg_mclk),
        .CLK(1'b0),
        .EOS(eos),
        .GSR(1'b0),
        .GTS(1'b0),
        .KEYCLEARB(1'b0),
        .PACK(pack),
        .PREQ(preq),
        .USRCCLKO(usrcclk),
        .USRCCLKTS(usrcclk_oen),
        .USRDONEO(usrdone),
        .USRDONETS(usrdone_oen));
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