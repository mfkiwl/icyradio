set_property IOB TRUE [get_ports FLASH_QSPI_io0_io]
set_property IOB TRUE [get_ports FLASH_QSPI_io1_io]
set_property IOB TRUE [get_ports FLASH_QSPI_io2_io]
set_property IOB TRUE [get_ports FLASH_QSPI_io3_io]
set_property IOB TRUE [get_ports FLASH_QSPI_sck_io]
set_property IOB TRUE [get_ports {FLASH_QSPI_ss_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports FLASH_QSPI_io0_io]
set_property IOSTANDARD LVCMOS33 [get_ports FLASH_QSPI_io1_io]
set_property IOSTANDARD LVCMOS33 [get_ports FLASH_QSPI_io2_io]
set_property IOSTANDARD LVCMOS33 [get_ports FLASH_QSPI_io3_io]
set_property IOSTANDARD LVCMOS33 [get_ports FLASH_QSPI_sck_io]
set_property IOSTANDARD LVCMOS33 [get_ports {FLASH_QSPI_ss_io[0]}]
set_property PULLDOWN true [get_ports FLASH_QSPI_io0_io]
set_property PULLDOWN true [get_ports FLASH_QSPI_io1_io]
set_property PULLDOWN true [get_ports FLASH_QSPI_io2_io]
set_property PULLDOWN true [get_ports FLASH_QSPI_io3_io]
set_property PULLDOWN true [get_ports FLASH_QSPI_sck_io]
set_property PULLUP true [get_ports {FLASH_QSPI_ss_io[0]}]
set_property PACKAGE_PIN P22 [get_ports FLASH_QSPI_io0_io]
set_property PACKAGE_PIN R22 [get_ports FLASH_QSPI_io1_io]
set_property PACKAGE_PIN P21 [get_ports FLASH_QSPI_io2_io]
set_property PACKAGE_PIN R21 [get_ports FLASH_QSPI_io3_io]
set_property PACKAGE_PIN N13 [get_ports FLASH_QSPI_sck_io]
set_property PACKAGE_PIN T19 [get_ports {FLASH_QSPI_ss_io[0]}]

create_generated_clock -name FLASH_QSPI_SCK -source [get_pins -hier -filter {NAME =~ "*axi_qspi_mm_0/aclk"}] -divide_by 4