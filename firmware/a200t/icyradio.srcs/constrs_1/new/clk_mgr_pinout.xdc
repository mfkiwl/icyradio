## Control IOs
set_property IOSTANDARD LVCMOS33 [get_ports CLK_MNGR_IRQn]
set_property IOSTANDARD LVCMOS33 [get_ports {CLK_MNGR_OEn[0]}]
set_property PULLUP true [get_ports CLK_MNGR_IRQn]
set_property PACKAGE_PIN U21 [get_ports CLK_MNGR_IRQn]
set_property PACKAGE_PIN T20 [get_ports {CLK_MNGR_OEn[0]}]


# Clocks
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_CLK0]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_CLK1]
#set_property IOSTANDARD LVCMOS33 [get_ports FPGA_CLK2]
#set_property IOSTANDARD LVCMOS33 [get_ports FPGA_CLK3]
set_property PACKAGE_PIN W11 [get_ports FPGA_CLK0]
set_property PACKAGE_PIN V13 [get_ports FPGA_CLK1]
#set_property PACKAGE_PIN W19 [get_ports FPGA_CLK2]
#set_property PACKAGE_PIN Y18 [get_ports FPGA_CLK3]

create_clock -name FPGA_CLK0 -period 20.000 [get_ports FPGA_CLK0]
create_clock -name FPGA_CLK1 -period 20.345 [get_ports FPGA_CLK1]
#create_clock -name FPGA_CLK2 -period 20.000 [get_ports FPGA_CLK2]
#create_clock -name FPGA_CLK3 -period 20.000 [get_ports FPGA_CLK3]

set_input_jitter FPGA_CLK0 0.3422
set_input_jitter FPGA_CLK1 0.3422
#set_input_jitter FPGA_CLK2 0
#set_input_jitter FPGA_CLK3 0