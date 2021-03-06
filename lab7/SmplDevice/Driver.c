/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, SmplDeviceEvtDeviceAdd)
#pragma alloc_text (PAGE, SmplDeviceEvtDriverContextCleanup)
#endif


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING( DriverObject, RegistryPath );

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");



    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "      _       _                   _                     ______ \n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "   __| | __ _| |_ _ __ ___  _ __ (_) ___    _________  / __/ /_\n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "  / _` |/ _` | __| '__/ _ \\| '_ \\| |/ __|  / ___/ __ \\/ /_/ __/\n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, " | (_| | (_| | |_| | | (_) | | | | | (__  (__  ) /_/ / __/ /_  \n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "  \\__,_|\\__,_|\\__|_|  \\___/|_| |_|_|\\___|/____/\\____/_/  \\__/  \n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "\n");

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, " --------------------------------------------------------------\n");
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "| SMPL Simple Sample DriverEntry\n");
#if DBG
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, "| Built %s %s\n", __DATE__, __TIME__);
#endif
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, 1234, " --------------------------------------------------------------\n");


    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = SmplDeviceEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
                           SmplDeviceEvtDeviceAdd
                           );

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
        return status;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

NTSTATUS
SmplDeviceEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = SmplDeviceCreateDevice(DeviceInit);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
SmplDeviceEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++
Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE ();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Stop WPP Tracing
    //
    WPP_CLEANUP( WdfDriverWdmGetDriverObject(DriverObject) );

}
