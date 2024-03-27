################################################################################

# This XDC is used only for OOC mode of synthesis, implementation
# This constraints file contains default clock frequencies to be used during
# out-of-context flows such as OOC Synthesis and Hierarchical Designs.
# This constraints file is not used in normal top-down synthesis (default flow
# of Vivado)
################################################################################
create_clock -name TRX_DATA_CLK -period 16.276 [get_ports TRX_DATA_CLK]
create_clock -name FPGA_CLK1 -period 20.345 [get_ports FPGA_CLK1]
create_clock -name TRX_CLK_OUT -period 10 [get_ports TRX_CLK_OUT]
create_clock -name FPGA_CLK0 -period 20 [get_ports FPGA_CLK0]
create_clock -name PCIe_REFCLK_clk_p -period 10 [get_ports PCIe_REFCLK_clk_p]

################################################################################