/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"
#include "Hw8250.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, SmplDeviceCreateDevice)
#endif


NTSTATUS
SmplDeviceCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;
	WDF_PNPPOWER_EVENT_CALLBACKS PnpPowerCallbacks;

    PAGED_CODE();

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

	

	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&PnpPowerCallbacks);
	PnpPowerCallbacks.EvtDevicePrepareHardware = SmplDeviceEvtPrepareHardware;
	PnpPowerCallbacks.EvtDeviceReleaseHardware = SmplDeviceEvtReleaseHardware;
	PnpPowerCallbacks.EvtDeviceD0Entry = SmplDeviceEvtD0Entry;
	PnpPowerCallbacks.EvtDeviceD0Exit = SmplDeviceEvtD0Exit;
	WdfDeviceInitSetPnpPowerEventCallbacks( DeviceInit, &PnpPowerCallbacks);


    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
		WDF_INTERRUPT_CONFIG InterruptConfig;
        WDFINTERRUPT Interrupt;
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //
        deviceContext = DeviceGetContext(device);

        //
        // Initialize the context.
        //
        WDF_INTERRUPT_CONFIG_INIT(&InterruptConfig, SmplInterruptEvtIsr, NULL);
        
        InterruptConfig.EvtInterruptEnable = SmplInterruptEvtEnable;
        InterruptConfig.EvtInterruptDisable = SmplInterruptEvtDisable;

        status = WdfInterruptCreate( device, 
                                    &InterruptConfig,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &Interrupt);
        if(!NT_SUCCESS(status))
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234,"WdfInterruptCreate failed! 0x%x\n", status);
            return status;
        }


        //
        // Create a device interface so that applications can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_SMPLDEVICE,
            NULL // ReferenceString
            );

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = SmplDeviceQueueInitialize(device);
        }
    }

    return status;
}


NTSTATUS SmplDeviceEvtD0Entry(
  _In_  WDFDEVICE Device,
  _In_  WDF_POWER_DEVICE_STATE PreviousState
)
{
	PDEVICE_CONTEXT pSmplDeviceContext = DeviceGetContext(Device);
	Hw8250Init(pSmplDeviceContext->pPortAddress, 1200, 7, 1, 0);
	DbgPrintEx( DPFLTR_IHVDRIVER_ID,1234,"SmplDeviceEvtD0Entry . \n");
	return STATUS_SUCCESS;
}

NTSTATUS SmplDeviceEvtD0Exit(
  _In_  WDFDEVICE Device,
  _In_  WDF_POWER_DEVICE_STATE TargetState
)
{ 
	DbgPrintEx( DPFLTR_IHVDRIVER_ID,1234,"SmplDeviceEvtD0Exit . \n");
	return STATUS_SUCCESS;
}

