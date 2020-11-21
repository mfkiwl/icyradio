#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb.h>

#define ICYRADIO_USB_VENDOR_ID  0x10C4
#define ICYRADIO_USB_PRODUCT_ID 0x8C67

#define ICYRADIO_USB_CTRL_INTERFACE     0
#define ICYRADIO_USB_CTRL_IN_ENDPOINT   (LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR)
#define ICYRADIO_USB_CTRL_OUT_ENDPOINT  (LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR)
#define ICYRADIO_USB_RX_INTERFACE       1
#define ICYRADIO_USB_RX_ENDPOINT        (LIBUSB_ENDPOINT_IN | (1 & 0xF))
#define ICYRADIO_USB_TX_INTERFACE       2
#define ICYRADIO_USB_TX_ENDPOINT        (LIBUSB_ENDPOINT_OUT | (2 & 0xF))

static volatile int shdn = 0;

void sig_handler(int sig)
{
	shdn = 1;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);

    int e = 0;
	libusb_context *pLibUSBContext = NULL;
	libusb_device **ppLibUSBDeviceList = NULL;
	libusb_device *pLibUSBDevice = NULL;
    struct libusb_device_handle *pLibUSBDeviceHandle = NULL;
    uint32_t ulDeviceIndex = 0;

	e = libusb_init(&pLibUSBContext);

	if(e)
		return e;

	int iDeviceCount = libusb_get_device_list(pLibUSBContext, &ppLibUSBDeviceList);
    uint32_t ulFoundDeviceCount = 0;

	for(int i = 0; i < iDeviceCount; i++)
    {
		pLibUSBDevice = ppLibUSBDeviceList[i];

	    struct libusb_device_descriptor sDeviceDescriptor;

		libusb_get_device_descriptor(pLibUSBDevice, &sDeviceDescriptor);

        if(sDeviceDescriptor.idVendor == ICYRADIO_USB_VENDOR_ID && sDeviceDescriptor.idProduct == ICYRADIO_USB_PRODUCT_ID)
        {
            ulFoundDeviceCount++;

            if(ulDeviceIndex == ulFoundDeviceCount - 1)
                break;
        }

		pLibUSBDevice = NULL;
	}

    if(!pLibUSBDevice)
        return -1;

	e = libusb_open(pLibUSBDevice, &pLibUSBDeviceHandle);

    libusb_free_device_list(ppLibUSBDeviceList, 1);

	if(e)
    {
		if(e == LIBUSB_ERROR_ACCESS)
			fprintf(stderr, "LIBUSB_ERROR_ACCESS - Are udev rules installed?\r\n");

		return e;
    }

	e = libusb_claim_interface(pLibUSBDeviceHandle, ICYRADIO_USB_TX_INTERFACE);

	if(e)
    {
		if(e == LIBUSB_ERROR_BUSY)
			fprintf(stderr, "LIBUSB_ERROR_BUSY - Is this the only instance?\r\n");

		return e;
    }

    uint8_t ubBuf[1024];
    int act_len = 0;

    while(!shdn)
    {
        fread(ubBuf, sizeof(ubBuf), 1, stdin);

        e = libusb_bulk_transfer(pLibUSBDeviceHandle, ICYRADIO_USB_TX_ENDPOINT, ubBuf, sizeof(ubBuf), &act_len, 0);

        if(e < 0)
            fprintf(stderr, "libusb_bulk_transfer failed with %d\r\n", e);
    }

	libusb_release_interface(pLibUSBDeviceHandle, ICYRADIO_USB_TX_INTERFACE);
	libusb_close(pLibUSBDeviceHandle);
	libusb_exit(pLibUSBContext);

    return 0;
}