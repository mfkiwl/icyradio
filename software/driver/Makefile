# Module Config
KPATH ?= /usr/src/linux-headers-`uname -r`
MOD_NAME = icyradio

# Directories
M ?= `pwd`
SOURCEDIR = src
INCLUDEDIR = include

# Version
MOD_VERSION=$(shell cat $(SOURCEDIR)/$(MOD_NAME).c | grep MODULE_VERSION | sed "s@MODULE_VERSION(\"@@g" | sed "s@\");@@g")

# Extra flags
EXTRA_CFLAGS := -I$(M)/$(INCLUDEDIR) -std=gnu99

# Sources & objects
SOURCES := $(notdir $(wildcard $(M)/$(SOURCEDIR)/*.c))
OBJECTS := $(SOURCES:%.c=$(SOURCEDIR)/%.o)

obj-m := $(MOD_NAME).o
$(MOD_NAME)-objs := $(OBJECTS)

.PHONY: all clean

all:
	@make -C $(KPATH) M=$(M) modules

dkms:
	@mkdir -p /usr/src/$(MOD_NAME)-$(MOD_VERSION)

	@cp -r Makefile /usr/src/$(MOD_NAME)-$(MOD_VERSION)
	@cp -r $(SOURCEDIR) /usr/src/$(MOD_NAME)-$(MOD_VERSION)
	@cp -r $(INCLUDEDIR) /usr/src/$(MOD_NAME)-$(MOD_VERSION)

	@echo "PACKAGE_NAME=\"$(MOD_NAME)\"" >> /usr/src/$(MOD_NAME)-${MOD_VERSION}/dkms.conf
	@echo "PACKAGE_VERSION=\"${MOD_VERSION}\"" >> /usr/src/$(MOD_NAME)-${MOD_VERSION}/dkms.conf
	@echo "BUILT_MODULE_NAME[0]=\"$(MOD_NAME)\"" >> /usr/src/$(MOD_NAME)-${MOD_VERSION}/dkms.conf
	@echo "DEST_MODULE_LOCATION[0]=\"/extra\"" >> /usr/src/$(MOD_NAME)-${MOD_VERSION}/dkms.conf
	@echo "AUTOINSTALL=\"yes\"" >> /usr/src/$(MOD_NAME)-${MOD_VERSION}/dkms.conf
	@echo "" >> /usr/src/$(MOD_NAME)-${MOD_VERSION}/dkms.conf

	@dkms add -m $(MOD_NAME) -v $(MOD_VERSION)
	@dkms build -m $(MOD_NAME) -v $(MOD_VERSION)
	@dkms install -m $(MOD_NAME) -v $(MOD_VERSION)

dkms-remove:
	@dkms remove -m $(MOD_NAME) -v $(MOD_VERSION) --all

clean:
	@make -C $(KPATH) M=$(M) clean
