#ifndef VB_HEADERS_H
#define VB_HEADERS_H

// References:
// http://sandsprite.com/vb-reversing/files/Alex_Ionescu_vb_structures.pdf

// Thread flags.
#define VB_APARTMENT_MODEL	0x1
#define VB_REQUIRE_LICENSE	0x2
#define VB_UNATTENDED		0x4
#define VB_SINGLE_THREADED	0x8
#define VB_RETAINED		0x10

// Represents the initial Visual Basic application header.
struct vb_header
{
	char szVbMagic[4];
	uint16_t wRuntimeBuild;
	char szLangDll[14];
	char szSecLangDll[14];
	uint16_t wRuntimeRevision;
	uint32_t dwLCID;
	uint32_t dwSecLCID;
	uint32_t lpSubMain;
	uint32_t lpProjectData;
	uint32_t fMdlIntCtls;
	uint32_t fMdlIntCtls2;
	uint32_t dwThreadFlags;
	uint32_t dwThreadCount;
	uint16_t wFormCount;
	uint16_t wExternalCount;
	uint32_t dwThunkCount;
	uint32_t lpGuiTable;
	uint32_t lpExternalTable;
	uint32_t lpComRegisterData;
	uint32_t bSZProjectDescription;
	uint32_t bSZProjectExeName;
	uint32_t bSZProjectHelpFile;
	uint32_t bSZProjectName;
};

// Represents Visual Basic COM registration data.
struct vb_com_registration_data
{
	uint32_t bRegInfo;
	uint32_t bSZProjectName;
	uint32_t bSZHelpDirectory;
	uint32_t bSZProjectDescription;
	uint8_t uuidProjectClsId[16];
	uint32_t dwTlbLcid;
	uint16_t wUnknown;
	uint16_t wTlbVerMajor;
	uint16_t wTlbVerMinor;
};

// Object types for the registration data.
#define VB_OBJECT_DESIGNER	0x2
#define VB_OBJECT_CLASS_MODULE	0x10
#define VB_OBJECT_USER_CONTROL	0x20
#define VB_OBJECT_USER_DOCUMENT	0x80

// Represents Visual Basic COM registration info.
struct vb_com_registration_info
{
	uint32_t bNextObject;
	uint32_t bObjectName;
	uint32_t bObjectDescription;
	uint32_t dwInstancing;
	uint32_t dwObjectId;
	uint8_t uuidObject[16];
	uint32_t flsInterface;
	uint32_t bUuidObjectIFace;
	uint32_t bUuidEventsIFace;
	uint32_t fHasEvents;
	uint32_t dwMiscStatus;
	uint8_t fClassType;
	uint8_t fObjectType;
	uint16_t wToolboxBitmap32;
	uint16_t wDefaultIcon;
	uint16_t fIsDesigner;
	uint32_t bDesignerData;
};

// Represents Visual Basic COM designer info.
struct vb_com_designer_info
{
	uint8_t uuidDesigner[16];
	uint32_t cbStructSize;
	uint32_t bstrAddinRegKey;
	// TODO: variable struct length!
};

// Represents Visual Basic project information.
struct vb_project_information
{
	uint32_t dwVersion;
	uint32_t lpObjectTable;
	uint32_t dwNull;
	uint32_t lpCodeStart;
	uint32_t lpCodeEnd;
	uint32_t dwDataSize;
	uint32_t lpThreadSpace;
	uint32_t lpVbaSeh;
	uint32_t lpNativeCode;
	char szPathInformation[528];
	uint32_t dwExternalCount;
};

// Represents Visual Basic secondary project information.
struct vb_sec_project_information
{
	uint32_t lpHeapLink;
	uint32_t lpObjectTable;
	uint32_t dwReserved;
	uint32_t dwUnused;
	uint32_t lpObjectList;
	uint32_t dwUnused2;
	uint32_t szProjectDescription;
	uint32_t szProjectHelpFile;
	uint32_t Reserved2;
	uint32_t dwHelpContextId;
};

// Represents Visual Basic object table.
struct vb_object_table
{
	uint32_t lpHeapLink;
	uint32_t lpExecProj;
	uint32_t lpProjectInfo2;
	uint32_t dwReserved;
	uint32_t dwNull;
	uint32_t lpProjectObject;
	uint8_t uuidObject[16];
	uint16_t fCompileState;
	uint16_t wTotalObjects;
	uint16_t wCompiledObjects;
	uint16_t wObjectsInUse;
	uint32_t lpObjectArray;
	uint32_t fIdeFlag;
	uint32_t lpIdeData;
	uint32_t lpIdeData2;
	uint32_t lpszProjectName;
	uint32_t dwLcid;
	uint32_t dwLcid2;
	uint32_t lpIdeData3;
	uint32_t dwIdentifier;
};

// Represents a Visual Basic private object descriptor.
struct vb_private_object_descriptor
{
	uint32_t lpHeapLink;
	uint32_t lpObjectInfo;
	uint32_t dwReserved;
	uint32_t dwIdeData[3];
	uint32_t lpObjectList;
	uint32_t dwIdeData2;
	uint32_t lpObjectList2;
	uint32_t dwIdeData3;
	uint32_t dwObjectType;
	uint32_t dwIdentifier;
};

// Represents a Visual Basic public object descriptor.
struct vb_public_object_descriptor
{
	uint32_t lpObjectInfo;
	uint32_t dwReserved;
	uint32_t lpPublicBytes;
	uint32_t lpStaticBytes;
	uint32_t lpModulePublic;
	uint32_t lpModuleStatic;
	uint32_t lpszObjectName;
	uint32_t dwMethodCount;
	uint32_t lpMethodNames;
	uint32_t bStaticVars;
	uint32_t fObjectType;
	uint32_t dwNull;
};

// Represents Visual Basic object info.
struct vb_object_info
{
	uint16_t wRefCount;
	uint16_t wObjectIndex;
	uint32_t lpObjectTable;
	uint32_t lpIdeData;
	uint32_t lpPrivateObject;
	uint32_t dwReserved;
	uint32_t dwNull;
	uint32_t lpObject;
	uint32_t lpProjectData;
	uint16_t wMethodCount;
	uint16_t wMethodCount2;
	uint32_t lpMethods;
	uint16_t wConstants;
	uint16_t wMaxConstants;
	uint32_t lpIdeData2;
	uint32_t lpIdeData3;
	uint32_t lpConstants;
};

// Represents Visual Basic optional object info.
struct vb_object_optional_info
{
	uint32_t dwObjectGuids;
	uint32_t lpObjectGuid;
	uint32_t dwNull;
	uint32_t lpuuidObjectTypes;
	uint32_t dwObjectTypeGuids;
	uint32_t lpControls2;
	uint32_t dwNull2;
	uint32_t lpObjectGuid2;
	uint32_t dwControlCount;
	uint32_t lpControls;
	uint16_t wEventCount;
	uint16_t wPCodeCount;
	uint16_t bWInitializeEvent;
	uint16_t bWTerminateEvent;
	uint32_t lpEvents;
	uint32_t lpBasicClassObject;
	uint32_t dwNull3;
	uint32_t lpIdeData;
};

// Represents Visual Basic control info.
struct vb_control_info
{
	uint32_t fControlType;
	uint16_t wEventCount;
	uint16_t bWEventsOffset;
	uint32_t lpGuid;
	uint32_t dwIndex;
	uint32_t dwNull;
	uint32_t dwNull2;
	uint32_t lpEventTable;
	uint32_t lpIdeData;
	uint32_t lpszName;
	uint32_t dwIndexCopy;
};

// Represents a Visual Basic structure for Win32 API call.
struct vb_call_api
{
	uint32_t pszLibraryName;
	uint32_t pszFunctionName;
};

#endif
