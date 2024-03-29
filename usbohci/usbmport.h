#ifndef USBMPORT_H__
#define USBMPORT_H__

#define USBPORT_HCI_MN 0x10000001

/* Tranfer types */
#define USBPORT_TRANSFER_TYPE_ISOCHRONOUS 0
#define USBPORT_TRANSFER_TYPE_CONTROL     1
#define USBPORT_TRANSFER_TYPE_BULK        2
#define USBPORT_TRANSFER_TYPE_INTERRUPT   3

/* Endpoint states */
#define USBPORT_ENDPOINT_UNKNOWN 0
#define USBPORT_ENDPOINT_PAUSED 2
#define USBPORT_ENDPOINT_ACTIVE 3
#define USBPORT_ENDPOINT_REMOVE 4
#define USBPORT_ENDPOINT_CLOSED 5

/* Endpoint status */
#define USBPORT_ENDPOINT_RUN     0
#define USBPORT_ENDPOINT_HALT    1
#define USBPORT_ENDPOINT_CONTROL 4

/* Interrupt Endpoint Poll Interval */
#define ENDPOINT_INTERRUPT_1ms   1
#define ENDPOINT_INTERRUPT_2ms   2
#define ENDPOINT_INTERRUPT_4ms   4
#define ENDPOINT_INTERRUPT_8ms   8
#define ENDPOINT_INTERRUPT_16ms  16
#define ENDPOINT_INTERRUPT_32ms  32

#define INTERRUPT_ENDPOINTs (ENDPOINT_INTERRUPT_32ms + \
                             ENDPOINT_INTERRUPT_16ms + \
                             ENDPOINT_INTERRUPT_8ms  + \
                             ENDPOINT_INTERRUPT_4ms  + \
                             ENDPOINT_INTERRUPT_2ms  + \
                             ENDPOINT_INTERRUPT_1ms)

/* Types of resources. For USBPORT_RESOURCES::ResourcesTypes */
#define USBPORT_RESOURCES_PORT      1
#define USBPORT_RESOURCES_INTERRUPT 2
#define USBPORT_RESOURCES_MEMORY    4

typedef struct _USBPORT_RESOURCES {
    ULONG ResourcesTypes;
    USB_CONTROLLER_FLAVOR HcFlavor;
    ULONG InterruptVector;
    KIRQL InterruptLevel;
    UCHAR Padded1[3];
    KAFFINITY InterruptAffinity;
    BOOLEAN ShareVector;
    UCHAR Padded2[3];
    KINTERRUPT_MODE InterruptMode;
    ULONG_PTR Reserved;
    PVOID ResourceBase;
    ULONG IoSpaceLength;
    ULONG_PTR StartVA;
    ULONG StartPA;
    UCHAR LegacySupport;
    BOOLEAN IsChirpHandled;
    UCHAR Reserved2;
    UCHAR Reserved3;
} USBPORT_RESOURCES, * PUSBPORT_RESOURCES;

C_ASSERT(sizeof(USBPORT_RESOURCES) == 32 + 5 * sizeof(PVOID));

typedef struct _USBPORT_ENDPOINT_PROPERTIES {
    USHORT DeviceAddress;
    USHORT EndpointAddress;
    USHORT TotalMaxPacketSize; // TransactionPerMicroframe * MaxPacketSize
    UCHAR Period;
    UCHAR Reserved1;
    USB_DEVICE_SPEED DeviceSpeed;
    ULONG UsbBandwidth;
    ULONG ScheduleOffset;
    ULONG TransferType;
    ULONG Direction;
    ULONG_PTR BufferVA;
    ULONG BufferPA;
    ULONG BufferLength;
    ULONG Reserved3;
    ULONG MaxTransferSize;
    USHORT HubAddr;
    USHORT PortNumber;
    UCHAR InterruptScheduleMask;
    UCHAR SplitCompletionMask;
    UCHAR TransactionPerMicroframe; // 1 + additional transactions. Total: from 1 to 3)
    UCHAR Reserved4;
    ULONG MaxPacketSize;
    ULONG Reserved6;
} USBPORT_ENDPOINT_PROPERTIES, * PUSBPORT_ENDPOINT_PROPERTIES;

C_ASSERT(sizeof(USBPORT_ENDPOINT_PROPERTIES) == 56 + 2 * sizeof(PVOID));

typedef struct _USBPORT_TRANSFER_PARAMETERS {
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    ULONG TransferCounter;
    BOOL IsTransferSplited;
    ULONG Reserved2;
    USB_DEFAULT_PIPE_SETUP_PACKET SetupPacket;
} USBPORT_TRANSFER_PARAMETERS, * PUSBPORT_TRANSFER_PARAMETERS;

C_ASSERT(sizeof(USBPORT_TRANSFER_PARAMETERS) == 28);

typedef struct _USBPORT_SCATTER_GATHER_ELEMENT {
    PHYSICAL_ADDRESS SgPhysicalAddress;
    ULONG Reserved1;
    ULONG SgTransferLength;
    ULONG SgOffset;
    ULONG Reserved2;
} USBPORT_SCATTER_GATHER_ELEMENT, * PUSBPORT_SCATTER_GATHER_ELEMENT;

C_ASSERT(sizeof(USBPORT_SCATTER_GATHER_ELEMENT) == 24);

typedef struct _USBPORT_SCATTER_GATHER_LIST {
    ULONG Flags;
    ULONG_PTR CurrentVa;
    PVOID MappedSystemVa;
    ULONG SgElementCount;
    USBPORT_SCATTER_GATHER_ELEMENT SgElement[2];
} USBPORT_SCATTER_GATHER_LIST, * PUSBPORT_SCATTER_GATHER_LIST;

C_ASSERT(sizeof(USBPORT_SCATTER_GATHER_LIST) == 48 + 4 * sizeof(PVOID));

typedef struct _USBPORT_ENDPOINT_REQUIREMENTS {
    ULONG HeaderBufferSize;
    ULONG MaxTransferSize;
} USBPORT_ENDPOINT_REQUIREMENTS, * PUSBPORT_ENDPOINT_REQUIREMENTS;

typedef ULONG MPSTATUS; // Miniport status
typedef ULONG RHSTATUS; // Roothub status

#define MP_STATUS_SUCCESS       0
#define MP_STATUS_FAILURE       1
#define MP_STATUS_NO_RESOURCES  2
#define MP_STATUS_NO_BANDWIDTH  3
#define MP_STATUS_ERROR         4
#define MP_STATUS_RESERVED1     5
#define MP_STATUS_NOT_SUPPORTED 6
#define MP_STATUS_HW_ERROR      7
#define MP_STATUS_UNSUCCESSFUL  8

#define RH_STATUS_SUCCESS       0
#define RH_STATUS_NO_CHANGES    1
#define RH_STATUS_UNSUCCESSFUL  2

/* Additional USB Class Codes from USB.org */
#define USBC_DEVICE_CLASS_TYPE_C_BRIDGE         0x12

/* Miniport functions */
typedef MPSTATUS
(NTAPI* PHCI_OPEN_ENDPOINT)(
    PVOID,
    PUSBPORT_ENDPOINT_PROPERTIES,
    PVOID);

typedef MPSTATUS
(NTAPI* PHCI_REOPEN_ENDPOINT)(
    PVOID,
    PUSBPORT_ENDPOINT_PROPERTIES,
    PVOID);

typedef VOID
(NTAPI* PHCI_QUERY_ENDPOINT_REQUIREMENTS)(
    PVOID,
    PUSBPORT_ENDPOINT_PROPERTIES,
    PUSBPORT_ENDPOINT_REQUIREMENTS);

typedef VOID
(NTAPI* PHCI_CLOSE_ENDPOINT)(
    PVOID,
    PVOID,
    BOOLEAN);

typedef MPSTATUS
(NTAPI* PHCI_START_CONTROLLER)(
    PVOID,
    PUSBPORT_RESOURCES);

typedef VOID
(NTAPI* PHCI_STOP_CONTROLLER)(
    PVOID,
    BOOLEAN);

typedef VOID
(NTAPI* PHCI_SUSPEND_CONTROLLER)(PVOID);

typedef MPSTATUS
(NTAPI* PHCI_RESUME_CONTROLLER)(PVOID);

typedef BOOLEAN
(NTAPI* PHCI_INTERRUPT_SERVICE)(PVOID);

typedef VOID
(NTAPI* PHCI_INTERRUPT_DPC)(
    PVOID,
    BOOLEAN);

typedef MPSTATUS
(NTAPI* PHCI_SUBMIT_TRANSFER)(
    PVOID,
    PVOID,
    PUSBPORT_TRANSFER_PARAMETERS,
    PVOID,
    PUSBPORT_SCATTER_GATHER_LIST);

typedef MPSTATUS
(NTAPI* PHCI_SUBMIT_ISO_TRANSFER)(
    PVOID,
    PVOID,
    PUSBPORT_TRANSFER_PARAMETERS,
    PVOID,
    PVOID);

typedef VOID
(NTAPI* PHCI_ABORT_TRANSFER)(
    PVOID,
    PVOID,
    PVOID,
    PULONG);

typedef ULONG
(NTAPI* PHCI_GET_ENDPOINT_STATE)(
    PVOID,
    PVOID);

typedef VOID
(NTAPI* PHCI_SET_ENDPOINT_STATE)(
    PVOID,
    PVOID,
    ULONG);

typedef VOID
(NTAPI* PHCI_POLL_ENDPOINT)(
    PVOID,
    PVOID);

typedef VOID
(NTAPI* PHCI_CHECK_CONTROLLER)(PVOID);

typedef ULONG
(NTAPI* PHCI_GET_32BIT_FRAME_NUMBER)(PVOID);

typedef VOID
(NTAPI* PHCI_INTERRUPT_NEXT_SOF)(PVOID);

typedef VOID
(NTAPI* PHCI_ENABLE_INTERRUPTS)(PVOID);

typedef VOID
(NTAPI* PHCI_DISABLE_INTERRUPTS)(PVOID);

typedef VOID
(NTAPI* PHCI_POLL_CONTROLLER)(PVOID);

typedef VOID
(NTAPI* PHCI_SET_ENDPOINT_DATA_TOGGLE)(
    PVOID,
    PVOID,
    ULONG);

typedef ULONG
(NTAPI* PHCI_GET_ENDPOINT_STATUS)(
    PVOID,
    PVOID);

typedef VOID
(NTAPI* PHCI_SET_ENDPOINT_STATUS)(
    PVOID,
    PVOID,
    ULONG);

typedef VOID
(NTAPI* PHCI_RESET_CONTROLLER)(PVOID);

/* Roothub functions */
typedef VOID
(NTAPI* PHCI_RH_GET_ROOT_HUB_DATA)(
    PVOID,
    PVOID);

typedef MPSTATUS
(NTAPI* PHCI_RH_GET_STATUS)(
    PVOID,
    PUSHORT);

#define USB20_PORT_STATUS_RESERVED1_OWNED_BY_COMPANION (1 << 2)

typedef MPSTATUS
(NTAPI* PHCI_RH_GET_PORT_STATUS)(
    PVOID,
    USHORT,
    PUSB_PORT_STATUS_AND_CHANGE);

typedef MPSTATUS
(NTAPI* PHCI_RH_GET_HUB_STATUS)(
    PVOID,
    PUSB_HUB_STATUS_AND_CHANGE);

typedef MPSTATUS
(NTAPI* PHCI_RH_SET_FEATURE_PORT_RESET)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_SET_FEATURE_PORT_POWER)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_SET_FEATURE_PORT_ENABLE)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_SET_FEATURE_PORT_SUSPEND)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_ENABLE)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_POWER)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_SUSPEND)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_ENABLE_CHANGE)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_CONNECT_CHANGE)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_RESET_CHANGE)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_SUSPEND_CHANGE)(
    PVOID,
    USHORT);

typedef MPSTATUS
(NTAPI* PHCI_RH_CLEAR_FEATURE_PORT_OVERCURRENT_CHANGE)(
    PVOID,
    USHORT);

typedef VOID
(NTAPI* PHCI_RH_DISABLE_IRQ)(PVOID);

typedef VOID
(NTAPI* PHCI_RH_ENABLE_IRQ)(PVOID);

/* Miniport ioctl functions */
typedef MPSTATUS
(NTAPI* PHCI_START_SEND_ONE_PACKET)(
    PVOID,
    PVOID,
    PVOID,
    PULONG,
    PVOID,
    PVOID,
    ULONG,
    USBD_STATUS*);

typedef MPSTATUS
(NTAPI* PHCI_END_SEND_ONE_PACKET)(
    PVOID,
    PVOID,
    PVOID,
    PULONG,
    PVOID,
    PVOID,
    ULONG,
    USBD_STATUS*);

typedef MPSTATUS
(NTAPI* PHCI_PASS_THRU)(
    PVOID,
    PVOID,
    ULONG,
    PVOID);

/* Port functions */
typedef ULONG
(*PUSBPORT_DBG_PRINT)(
    PVOID,
    ULONG,
    PCH,
    ...);

typedef ULONG
(NTAPI* PUSBPORT_TEST_DEBUG_BREAK)(PVOID);

typedef ULONG
(NTAPI* PUSBPORT_ASSERT_FAILURE)(
    PVOID,
    PVOID,
    PVOID,
    ULONG,
    PCHAR);

typedef MPSTATUS
(NTAPI* PUSBPORT_GET_MINIPORT_REGISTRY_KEY_VALUE)(
    PVOID,
    BOOL,
    PCWSTR,
    SIZE_T,
    PVOID,
    SIZE_T);

typedef ULONG
(NTAPI* PUSBPORT_INVALIDATE_ROOT_HUB)(PVOID);

typedef ULONG
(NTAPI* PUSBPORT_INVALIDATE_ENDPOINT)(
    PVOID,
    PVOID);

typedef VOID
(NTAPI* PUSBPORT_COMPLETE_TRANSFER)(
    PVOID,
    PVOID,
    PVOID,
    USBD_STATUS,
    ULONG);

typedef ULONG
(NTAPI* PUSBPORT_COMPLETE_ISO_TRANSFER)(
    PVOID,
    PVOID,
    PVOID,
    ULONG);

typedef ULONG
(NTAPI* PUSBPORT_LOG_ENTRY)(
    PVOID,
    ULONG,
    ULONG,
    ULONG,
    ULONG,
    ULONG);

typedef PVOID
(NTAPI* PUSBPORT_GET_MAPPED_VIRTUAL_ADDRESS)(
    ULONG,
    PVOID,
    PVOID);

typedef VOID
(NTAPI ASYNC_TIMER_CALLBACK)(
    IN PVOID MiniportExtension,
    IN PVOID CallBackContext);

typedef ULONG
(NTAPI* PUSBPORT_REQUEST_ASYNC_CALLBACK)(
    PVOID,
    ULONG,
    PVOID,
    SIZE_T,
    ASYNC_TIMER_CALLBACK*);

typedef MPSTATUS
(NTAPI* PUSBPORT_READ_WRITE_CONFIG_SPACE)(
    PVOID,
    BOOLEAN,
    PVOID,
    ULONG,
    ULONG);

typedef NTSTATUS
(NTAPI* PUSBPORT_WAIT)(
    PVOID,
    ULONG);

#define USBPORT_INVALIDATE_CONTROLLER_RESET            1
#define USBPORT_INVALIDATE_CONTROLLER_SURPRISE_REMOVE  2
#define USBPORT_INVALIDATE_CONTROLLER_SOFT_INTERRUPT   3

typedef ULONG
(NTAPI* PUSBPORT_INVALIDATE_CONTROLLER)(
    PVOID,
    ULONG);

typedef VOID
(NTAPI* PUSBPORT_BUG_CHECK)(PVOID);

typedef ULONG
(NTAPI* PUSBPORT_NOTIFY_DOUBLE_BUFFER)(
    PVOID,
    PVOID,
    PVOID,
    SIZE_T);

/* Miniport functions */
typedef VOID
(NTAPI* PHCI_REBALANCE_ENDPOINT)(
    PVOID,
    PUSBPORT_ENDPOINT_PROPERTIES,
    PVOID);

typedef VOID
(NTAPI* PHCI_FLUSH_INTERRUPTS)(PVOID);

typedef MPSTATUS
(NTAPI* PHCI_RH_CHIRP_ROOT_PORT)(
    PVOID,
    USHORT);

typedef VOID
(NTAPI* PHCI_TAKE_PORT_CONTROL)(PVOID);

#define USB_MINIPORT_VERSION_OHCI 0x01
#define USB_MINIPORT_VERSION_UHCI 0x02
#define USB_MINIPORT_VERSION_EHCI 0x03
#define USB_MINIPORT_VERSION_XHCI 0x04

#define USB_MINIPORT_FLAGS_INTERRUPT    0x0001
#define USB_MINIPORT_FLAGS_PORT_IO      0x0002
#define USB_MINIPORT_FLAGS_MEMORY_IO    0x0004
#define USB_MINIPORT_FLAGS_USB2         0x0010
#define USB_MINIPORT_FLAGS_DISABLE_SS   0x0020
#define USB_MINIPORT_FLAGS_NOT_LOCK_INT 0x0040
#define USB_MINIPORT_FLAGS_POLLING      0x0080
#define USB_MINIPORT_FLAGS_NO_DMA       0x0100
#define USB_MINIPORT_FLAGS_WAKE_SUPPORT 0x0200

#define TOTAL_USB11_BUS_BANDWIDTH  12000
#define TOTAL_USB20_BUS_BANDWIDTH  400000

typedef struct _USBPORT_REGISTRATION_PACKET {
    ULONG MiniPortVersion;
    ULONG MiniPortFlags;
    ULONG MiniPortBusBandwidth;
    ULONG Reserved1;
    SIZE_T MiniPortExtensionSize;
    SIZE_T MiniPortEndpointSize;
    SIZE_T MiniPortTransferSize;
    ULONG Reserved2;
    ULONG Reserved3;
    SIZE_T MiniPortResourcesSize;

    /* Miniport */
    PHCI_OPEN_ENDPOINT OpenEndpoint;
    PHCI_REOPEN_ENDPOINT ReopenEndpoint;
    PHCI_QUERY_ENDPOINT_REQUIREMENTS QueryEndpointRequirements;
    PHCI_CLOSE_ENDPOINT CloseEndpoint;
    PHCI_START_CONTROLLER StartController;
    PHCI_STOP_CONTROLLER StopController;
    PHCI_SUSPEND_CONTROLLER SuspendController;
    PHCI_RESUME_CONTROLLER ResumeController;
    PHCI_INTERRUPT_SERVICE InterruptService;
    PHCI_INTERRUPT_DPC InterruptDpc;
    PHCI_SUBMIT_TRANSFER SubmitTransfer;
    PHCI_SUBMIT_ISO_TRANSFER SubmitIsoTransfer;
    PHCI_ABORT_TRANSFER AbortTransfer;
    PHCI_GET_ENDPOINT_STATE GetEndpointState;
    PHCI_SET_ENDPOINT_STATE SetEndpointState;
    PHCI_POLL_ENDPOINT PollEndpoint;
    PHCI_CHECK_CONTROLLER CheckController;
    PHCI_GET_32BIT_FRAME_NUMBER Get32BitFrameNumber;
    PHCI_INTERRUPT_NEXT_SOF InterruptNextSOF;
    PHCI_ENABLE_INTERRUPTS EnableInterrupts;
    PHCI_DISABLE_INTERRUPTS DisableInterrupts;
    PHCI_POLL_CONTROLLER PollController;
    PHCI_SET_ENDPOINT_DATA_TOGGLE SetEndpointDataToggle;
    PHCI_GET_ENDPOINT_STATUS GetEndpointStatus;
    PHCI_SET_ENDPOINT_STATUS SetEndpointStatus;
    PHCI_RESET_CONTROLLER ResetController;

    /* Roothub */
    PHCI_RH_GET_ROOT_HUB_DATA RH_GetRootHubData;
    PHCI_RH_GET_STATUS RH_GetStatus;
    PHCI_RH_GET_PORT_STATUS RH_GetPortStatus;
    PHCI_RH_GET_HUB_STATUS RH_GetHubStatus;
    PHCI_RH_SET_FEATURE_PORT_RESET RH_SetFeaturePortReset;
    PHCI_RH_SET_FEATURE_PORT_POWER RH_SetFeaturePortPower;
    PHCI_RH_SET_FEATURE_PORT_ENABLE RH_SetFeaturePortEnable;
    PHCI_RH_SET_FEATURE_PORT_SUSPEND RH_SetFeaturePortSuspend;
    PHCI_RH_CLEAR_FEATURE_PORT_ENABLE RH_ClearFeaturePortEnable;
    PHCI_RH_CLEAR_FEATURE_PORT_POWER RH_ClearFeaturePortPower;
    PHCI_RH_CLEAR_FEATURE_PORT_SUSPEND RH_ClearFeaturePortSuspend;
    PHCI_RH_CLEAR_FEATURE_PORT_ENABLE_CHANGE RH_ClearFeaturePortEnableChange;
    PHCI_RH_CLEAR_FEATURE_PORT_CONNECT_CHANGE RH_ClearFeaturePortConnectChange;
    PHCI_RH_CLEAR_FEATURE_PORT_RESET_CHANGE RH_ClearFeaturePortResetChange;
    PHCI_RH_CLEAR_FEATURE_PORT_SUSPEND_CHANGE RH_ClearFeaturePortSuspendChange;
    PHCI_RH_CLEAR_FEATURE_PORT_OVERCURRENT_CHANGE RH_ClearFeaturePortOvercurrentChange;
    PHCI_RH_DISABLE_IRQ RH_DisableIrq;
    PHCI_RH_ENABLE_IRQ RH_EnableIrq;

    /* Miniport ioctl */
    PHCI_START_SEND_ONE_PACKET StartSendOnePacket;
    PHCI_END_SEND_ONE_PACKET EndSendOnePacket;
    PHCI_PASS_THRU PassThru;

    /* Port */
    PUSBPORT_DBG_PRINT UsbPortDbgPrint;
    PUSBPORT_TEST_DEBUG_BREAK UsbPortTestDebugBreak;
    PUSBPORT_ASSERT_FAILURE UsbPortAssertFailure;
    PUSBPORT_GET_MINIPORT_REGISTRY_KEY_VALUE UsbPortGetMiniportRegistryKeyValue;
    PUSBPORT_INVALIDATE_ROOT_HUB UsbPortInvalidateRootHub;
    PUSBPORT_INVALIDATE_ENDPOINT UsbPortInvalidateEndpoint;
    PUSBPORT_COMPLETE_TRANSFER UsbPortCompleteTransfer;
    PUSBPORT_COMPLETE_ISO_TRANSFER UsbPortCompleteIsoTransfer;
    PUSBPORT_LOG_ENTRY UsbPortLogEntry;
    PUSBPORT_GET_MAPPED_VIRTUAL_ADDRESS UsbPortGetMappedVirtualAddress;
    PUSBPORT_REQUEST_ASYNC_CALLBACK UsbPortRequestAsyncCallback;
    PUSBPORT_READ_WRITE_CONFIG_SPACE UsbPortReadWriteConfigSpace;
    PUSBPORT_WAIT UsbPortWait;
    PUSBPORT_INVALIDATE_CONTROLLER UsbPortInvalidateController;
    PUSBPORT_BUG_CHECK UsbPortBugCheck;
    PUSBPORT_NOTIFY_DOUBLE_BUFFER UsbPortNotifyDoubleBuffer;

    /* Miniport */
    PHCI_REBALANCE_ENDPOINT RebalanceEndpoint;
    PHCI_FLUSH_INTERRUPTS FlushInterrupts;
    PHCI_RH_CHIRP_ROOT_PORT RH_ChirpRootPort;
    PHCI_TAKE_PORT_CONTROL TakePortControl;
    ULONG Reserved4;
    ULONG Reserved5;
} USBPORT_REGISTRATION_PACKET, * PUSBPORT_REGISTRATION_PACKET;

#define USB10_MINIPORT_INTERFACE_VERSION  100
#define USB20_MINIPORT_INTERFACE_VERSION  200

typedef struct _USBPORT_MINIPORT_INTERFACE {
    PDRIVER_OBJECT DriverObject;
    LIST_ENTRY DriverLink;
    PDRIVER_UNLOAD DriverUnload;
    ULONG Version;
    USBPORT_REGISTRATION_PACKET Packet;
} USBPORT_MINIPORT_INTERFACE, * PUSBPORT_MINIPORT_INTERFACE;

C_ASSERT(sizeof(USBPORT_MINIPORT_INTERFACE) == 32 + 76 * sizeof(PVOID));

#define USBPORT_TRANSFER_DIRECTION_OUT  1 // From host to device
#define USBPORT_MAX_DEVICE_ADDRESS      127

/* For USB1.1 or USB3 Hub Descriptors */
typedef union _USBPORT_HUB_11_CHARACTERISTICS {
    struct {
        USHORT PowerControlMode : 1;
        USHORT NoPowerSwitching : 1; // Reserved. Used only on 1.0 compliant hubs that implement no power switching.
        USHORT PartOfCompoundDevice : 1;
        USHORT OverCurrentProtectionMode : 1;
        USHORT NoOverCurrentProtection : 1;
        USHORT Reserved1 : 11;
    };
    USHORT AsUSHORT;
} USBPORT_HUB_11_CHARACTERISTICS;

C_ASSERT(sizeof(USBPORT_HUB_11_CHARACTERISTICS) == sizeof(USHORT));

/* For USB2.0 Hub Descriptors */
typedef union _USBPORT_HUB_20_CHARACTERISTICS {
    struct {
        USHORT PowerControlMode : 1;
        USHORT NoPowerSwitching : 1; // Reserved. Used only on 1.0 compliant hubs that implement no power switching.
        USHORT PartOfCompoundDevice : 1;
        USHORT OverCurrentProtectionMode : 1;
        USHORT NoOverCurrentProtection : 1;
        USHORT TtThinkTime : 2;
        USHORT PortIndicatorsSupported : 1;
        USHORT Reserved1 : 8;
    };
    USHORT AsUSHORT;
} USBPORT_HUB_20_CHARACTERISTICS;

C_ASSERT(sizeof(USBPORT_HUB_20_CHARACTERISTICS) == sizeof(USHORT));

typedef USBPORT_HUB_11_CHARACTERISTICS USBPORT_HUB_30_CHARACTERISTICS;

typedef union _USBPORT_HUB_CHARACTERISTICS {
    USHORT AsUSHORT;
    USBPORT_HUB_11_CHARACTERISTICS Usb11HubCharacteristics;
    USBPORT_HUB_20_CHARACTERISTICS Usb20HubCharacteristics;
    USBPORT_HUB_30_CHARACTERISTICS Usb30HubCharacteristics;
} USBPORT_HUB_CHARACTERISTICS;

C_ASSERT(sizeof(USBPORT_HUB_CHARACTERISTICS) == sizeof(USHORT));

typedef struct _USBPORT_ROOT_HUB_DATA {
    ULONG NumberOfPorts;
    USBPORT_HUB_CHARACTERISTICS HubCharacteristics;
    USHORT Padded1;
    ULONG PowerOnToPowerGood;
    ULONG HubControlCurrent;
} USBPORT_ROOT_HUB_DATA, * PUSBPORT_ROOT_HUB_DATA;

C_ASSERT(sizeof(USBPORT_ROOT_HUB_DATA) == 16);

ULONG
NTAPI
USBPORT_GetHciMn(VOID);

NTSTATUS
NTAPI
USBPORT_RegisterUSBPortDriver(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG Version,
    IN PUSBPORT_REGISTRATION_PACKET RegistrationPacket);

#endif /* USBMPORT_H__ */