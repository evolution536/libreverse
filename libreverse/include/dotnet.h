#include <cstddef>
#include <string>

// Maximum number of tables is 64, but most are not defined yet.
#define MAX_NUMBER_OF_METADATA_TABLES 64

// Represents a 16-byte GUID.
struct GUID
{
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];
};

// Represents the fixed fields in the metadata tables structure.
struct METADATA_TABLE
{
	uint32_t Reserved1;
	uint8_t MajorVersion;
	uint8_t MinorVersion;
	uint8_t HeapOffsetSizes;
	uint8_t Reserved2;
	uint64_t Valid;
	uint64_t Sorted;
};

// Metadata table definitions.
#define METADATA_TABLE_MODULE		0
#define METADATA_TABLE_TYPEREF		1
#define METADATA_TABLE_TYPEDEF		2
#define METADATA_TABLE_FIELD		4
#define METADATA_TABLE_METHODDEF	6
#define METADATA_TABLE_PARAM		8
#define METADATA_TABLE_INTERFACE_IMPL	9
#define METADATA_TABLE_MEMBERREF	10
#define METADATA_TABLE_CONSTANT		11
#define METADATA_TABLE_CUSTOM_ATTR	12
#define METADATA_TABLE_FIELD_MARSHAL	13
#define METADATA_TABLE_DECL_SECURITY	14
#define METADATA_TABLE_CLASS_LAYOUT	15
#define METADATA_TABLE_FIELD_LAYOUT	16
#define METADATA_TABLE_STANDALONE_SIG	17
#define METADATA_TABLE_EVENT_MAP	18
#define METADATA_TABLE_EVENT		20
#define METADATA_TABLE_PROPERTY_MAP	21
#define METADATA_TABLE_PROPERTY		23
#define METADATA_TABLE_METHOD_SEMANTICS	24
#define METADATA_TABLE_METHOD_IMPL	25
#define METADATA_TABLE_MODULE_REF	26
#define METADATA_TABLE_TYPE_SPEC	27
#define METADATA_TABLE_IMPL_MAP		28
#define METADATA_TABLE_FIELD_RVA	29
#define METADATA_TABLE_ASSEMBLY		32
#define METADATA_TABLE_ASSEMBLY_PROC	33
#define METADATA_TABLE_ASSEMBLY_OS	34
#define METADATA_TABLE_ASSEMBLY_REF	35
#define METADATA_TABLE_ASSEMBLY_REFPROC	36
#define METADATA_TABLE_ASSEMBLY_REF_OS	37
#define METADATA_TABLE_FILE		38
#define METADATA_TABLE_EXPORTED_TYPE	39
#define METADATA_TABLE_MANIFEST_RSRC	40
#define METADATA_TABLE_NESTED_CLASS	41
#define METADATA_TABLE_GENERIC_PARAM	42
#define METADATA_TABLE_GENERIC_PRM_CNST	44

// Represents a .NET stream object header.
struct dotnet_stream
{
	uint32_t Offset;
	uint32_t Size;
	std::string Name;
};

// Contains information about addressing a metadata table.
struct metadata_table_info
{
	// The number of rows in the metadata table.
	uint32_t RowCount;

	// The size of each row in the metadata table.
	uint32_t RowSize;

	// Indicates whether the table is sorted or not.
	bool Sorted;
};

// Flags constants for the typedef table.
enum CorTypeAttr
{
	// Use this mask to retrieve the type visibility information.
	tdVisibilityMask        =   0x00000007,
	tdNotPublic             =   0x00000000,     // Class is not public scope.
	tdPublic                =   0x00000001,     // Class is public scope.
	tdNestedPublic          =   0x00000002,     // Class is nested with public visibility.
	tdNestedPrivate         =   0x00000003,     // Class is nested with private visibility.
	tdNestedFamily          =   0x00000004,     // Class is nested with family visibility.
	tdNestedAssembly        =   0x00000005,     // Class is nested with assembly visibility.
	tdNestedFamANDAssem     =   0x00000006,     // Class is nested with family and assembly visibility.
	tdNestedFamORAssem      =   0x00000007,     // Class is nested with family or assembly visibility.

	// Use this mask to retrieve class layout information
	tdLayoutMask            =   0x00000018,
	tdAutoLayout            =   0x00000000,     // Class fields are auto-laid out
	tdSequentialLayout      =   0x00000008,     // Class fields are laid out sequentially
	tdExplicitLayout        =   0x00000010,     // Layout is supplied explicitly
	// end layout mask

	// Use this mask to retrieve class semantics information.
	tdClassSemanticsMask    =   0x00000060,
	tdClass                 =   0x00000000,     // Type is a class.
	tdInterface             =   0x00000020,     // Type is an interface.
	// end semantics mask

	// Special semantics in addition to class semantics.
	tdAbstract              =   0x00000080,     // Class is abstract
	tdSealed                =   0x00000100,     // Class is concrete and may not be extended
	tdSpecialName           =   0x00000400,     // Class name is special. Name describes how.

	// Implementation attributes.
	tdImport                =   0x00001000,     // Class / interface is imported
	tdSerializable          =   0x00002000,     // The class is Serializable.

	// Use tdStringFormatMask to retrieve string information for native interop
	tdStringFormatMask      =   0x00030000,
	tdAnsiClass             =   0x00000000,     // LPTSTR is interpreted as ANSI in this class
	tdUnicodeClass          =   0x00010000,     // LPTSTR is interpreted as UNICODE
	tdAutoClass             =   0x00020000,     // LPTSTR is interpreted automatically
	tdCustomFormatClass     =   0x00030000,     // A non-standard encoding specified by CustomFormatMask
	tdCustomFormatMask      =   0x00C00000,     // Use this mask to retrieve non-standard encoding information for native interop. The meaning of the values of these 2 bits is unspecified.

	// end string format mask

	tdBeforeFieldInit       =   0x00100000,     // Initialize the class any time before first static field access.
	tdForwarder             =   0x00200000,     // This ExportedType is a type forwarder.

	// Flags reserved for runtime use.
	tdReservedMask          =   0x00040800,
	tdRTSpecialName         =   0x00000800,     // Runtime should check name encoding.
	tdHasSecurity           =   0x00040000,     // Class has security associate with it.
};

// Flag constants for the field table.
enum CorFieldAttr
{
	// member access mask - Use this mask to retrieve accessibility information.
	fdFieldAccessMask           =   0x0007,
	fdPrivateScope              =   0x0000,     // Member not referenceable.
	fdPrivate                   =   0x0001,     // Accessible only by the parent type.
	fdFamANDAssem               =   0x0002,     // Accessible by sub-types only in this Assembly.
	fdAssembly                  =   0x0003,     // Accessibly by anyone in the Assembly.
	fdFamily                    =   0x0004,     // Accessible only by type and sub-types.
	fdFamORAssem                =   0x0005,     // Accessibly by sub-types anywhere, plus anyone in assembly.
	fdPublic                    =   0x0006,     // Accessibly by anyone who has visibility to this scope.
	// end member access mask

	// field contract attributes.
	fdStatic                    =   0x0010,     // Defined on type, else per instance.
	fdInitOnly                  =   0x0020,     // Field may only be initialized, not written to after init.
	fdLiteral                   =   0x0040,     // Value is compile time constant.
	fdNotSerialized             =   0x0080,     // Field does not have to be serialized when type is remoted.

	fdSpecialName               =   0x0200,     // field is special. Name describes how.

	// interop attributes
	fdPinvokeImpl               =   0x2000,     // Implementation is forwarded through pinvoke.

	// Reserved flags for runtime use only.
	fdReservedMask              =   0x9500,
	fdRTSpecialName             =   0x0400,     // Runtime(metadata internal APIs) should check name encoding.
	fdHasFieldMarshal           =   0x1000,     // Field has marshalling information.
	fdHasDefault                =   0x8000,     // Field has default.
	fdHasFieldRVA               =   0x0100,     // Field has RVA.
};

// Flag constants for the method table.
enum CorMethodAttr
{
	// member access mask - Use this mask to retrieve accessibility information.
	mdMemberAccessMask          =   0x0007,
	mdPrivateScope              =   0x0000,     // Member not referenceable.
	mdPrivate                   =   0x0001,     // Accessible only by the parent type.
	mdFamANDAssem               =   0x0002,     // Accessible by sub-types only in this Assembly.
	mdAssem                     =   0x0003,     // Accessibly by anyone in the Assembly.
	mdFamily                    =   0x0004,     // Accessible only by type and sub-types.
	mdFamORAssem                =   0x0005,     // Accessibly by sub-types anywhere, plus anyone in assembly.
	mdPublic                    =   0x0006,     // Accessibly by anyone who has visibility to this scope.
	// end member access mask

	// method contract attributes.
	mdStatic                    =   0x0010,     // Defined on type, else per instance.
	mdFinal                     =   0x0020,     // Method may not be overridden.
	mdVirtual                   =   0x0040,     // Method virtual.
	mdHideBySig                 =   0x0080,     // Method hides by name+sig, else just by name.

	// vtable layout mask - Use this mask to retrieve vtable attributes.
	mdVtableLayoutMask          =   0x0100,
	mdReuseSlot                 =   0x0000,     // The default.
	mdNewSlot                   =   0x0100,     // Method always gets a new slot in the vtable.
	// end vtable layout mask

	// method implementation attributes.
	mdCheckAccessOnOverride     =   0x0200,     // Overridability is the same as the visibility.
	mdAbstract                  =   0x0400,     // Method does not provide an implementation.
	mdSpecialName               =   0x0800,     // Method is special. Name describes how.

	// interop attributes
	mdPinvokeImpl               =   0x2000,     // Implementation is forwarded through pinvoke.
	mdUnmanagedExport           =   0x0008,     // Managed method exported via thunk to unmanaged code.

	// Reserved flags for runtime use only.
	mdReservedMask              =   0xd000,
	mdRTSpecialName             =   0x1000,     // Runtime should check name encoding.
	mdHasSecurity               =   0x4000,     // Method has security associate with it.
	mdRequireSecObject          =   0x8000,     // Method calls another method containing security code.
};

// Flag constants for the method implementations.
enum CorMethodImpl
{
	// code impl mask
	miCodeTypeMask      =   0x0003,   // Flags about code type.
	miIL                =   0x0000,   // Method impl is IL.
	miNative            =   0x0001,   // Method impl is native.
	miOPTIL             =   0x0002,   // Method impl is OPTIL
	miRuntime           =   0x0003,   // Method impl is provided by the runtime.
	// end code impl mask

	// managed mask
	miManagedMask       =   0x0004,   // Flags specifying whether the code is managed or unmanaged.
	miUnmanaged         =   0x0004,   // Method impl is unmanaged, otherwise managed.
	miManaged           =   0x0000,   // Method impl is managed.
	// end managed mask

	// implementation info and interop
	miForwardRef        =   0x0010,   // Indicates method is defined; used primarily in merge scenarios.
	miPreserveSig       =   0x0080,   // Indicates method sig is not to be mangled to do HRESULT conversion.

	miInternalCall      =   0x1000,   // Reserved for internal use.

	miSynchronized      =   0x0020,   // Method is single threaded through the body.
	miNoInlining        =   0x0008,   // Method may not be inlined.
	miMaxMethodImplVal  =   0xffff,   // Range check value
};

// Flag constants for the event table.
enum CorEventAttr
{
	evSpecialName           =   0x0200,     // event is special. Name describes how.

	// Reserved flags for Runtime use only.
	evReservedMask          =   0x0400,
	evRTSpecialName         =   0x0400,     // Runtime(metadata internal APIs) should check name encoding.
};

// Flag constants for the property table.
enum CorPropertyAttr
{
	prSpecialName           =   0x0200,     // property is special. Name describes how.

	// Reserved flags for Runtime use only.
	prReservedMask          =   0xf400,
	prRTSpecialName         =   0x0400,     // Runtime(metadata internal APIs) should check name encoding.
	prHasDefault            =   0x1000,     // Property has default

	prUnused                =   0xe9ff,
};

// Flag constants for the method semantics table.
enum CorMethodSemanticsAttr
{
	msSetter    =   0x0001,     // Setter for property
	msGetter    =   0x0002,     // Getter for property
	msOther     =   0x0004,     // other method for property or event
	msAddOn     =   0x0008,     // AddOn method for event
	msRemoveOn  =   0x0010,     // RemoveOn method for event
	msFire      =   0x0020,     // Fire method for event
};

// Flag constants for the Param metadata table rows.
enum CorParamAttr
{
	pdIn                        =   0x0001,     // Param is [In]
	pdOut                       =   0x0002,     // Param is [out]
	pdOptional                  =   0x0010,     // Param is optional

	// Reserved flags for Runtime use only.
	pdReservedMask              =   0xf000,
	pdHasDefault                =   0x1000,     // Param has default value.
	pdHasFieldMarshal           =   0x2000,     // Param has FieldMarshal.

	pdUnused                    =   0xcfe0,
};

// Flag constants for the implmap table.
enum  CorPinvokeMap
{
	pmNoMangle          = 0x0001,   // Pinvoke is to use the member name as specified.

	// Use this mask to retrieve the CharSet information.
	pmCharSetMask       = 0x0006,
	pmCharSetNotSpec    = 0x0000,
	pmCharSetAnsi       = 0x0002,
	pmCharSetUnicode    = 0x0004,
	pmCharSetAuto       = 0x0006,


	pmBestFitUseAssem   = 0x0000,
	pmBestFitEnabled    = 0x0010,
	pmBestFitDisabled   = 0x0020,
	pmBestFitMask       = 0x0030,

	pmThrowOnUnmappableCharUseAssem   = 0x0000,
	pmThrowOnUnmappableCharEnabled    = 0x1000,
	pmThrowOnUnmappableCharDisabled   = 0x2000,
	pmThrowOnUnmappableCharMask       = 0x3000,

	pmSupportsLastError = 0x0040,   // Information about target function. Not relevant for fields.

	// None of the calling convention flags is relevant for fields.
	pmCallConvMask      = 0x0700,
	pmCallConvWinapi    = 0x0100,   // Pinvoke will use native callconv appropriate to target windows platform.
	pmCallConvCdecl     = 0x0200,
	pmCallConvStdcall   = 0x0300,
	pmCallConvThiscall  = 0x0400,   // In M9, pinvoke will raise exception.
	pmCallConvFastcall  = 0x0500,

	pmMaxValue          = 0xFFFF,
};

// Flag constants for the assembly table.
enum CorAssemblyFlags
{
	afPublicKey             =   0x0001,     // The assembly ref holds the full (unhashed) public key.

	afPA_None               =   0x0000,     // Processor Architecture unspecified
	afPA_MSIL               =   0x0010,     // Processor Architecture: neutral (PE32)
	afPA_x86                =   0x0020,     // Processor Architecture: x86 (PE32)
	afPA_IA64               =   0x0030,     // Processor Architecture: Itanium (PE32+)
	afPA_AMD64              =   0x0040,     // Processor Architecture: AMD X64 (PE32+)
	afPA_Specified          =   0x0080,     // Propagate PA flags to AssemblyRef record
	afPA_Mask               =   0x0070,     // Bits describing the processor architecture
	afPA_FullMask           =   0x00F0,     // Bits describing the PA incl. Specified
	afPA_Shift              =   0x0004,     // NOT A FLAG, shift count in PA flags <--> index conversion

	afEnableJITcompileTracking  =   0x8000, // From "DebuggableAttribute".
	afDisableJITcompileOptimizer=   0x4000, // From "DebuggableAttribute".

	afRetargetable          =   0x0100,     // The assembly can be retargeted (at runtime) to an
	                                    // assembly from a different publisher.
};

// Flag constants for the manifest resource table.
enum CorManifestResourceFlags
{
	mrVisibilityMask        =   0x0007,
	mrPublic                =   0x0001,     // The Resource is exported from the Assembly.
	mrPrivate               =   0x0002,     // The Resource is private to the Assembly.
};

// Flag constants for the generaric params table.
enum CorGenericParamAttr
{
	// Variance of type parameters, only applicable to generic parameters
	// for generic interfaces and delegates
	gpVarianceMask          =   0x0003,
	gpNonVariant            =   0x0000,
	gpCovariant             =   0x0001,
	gpContravariant         =   0x0002,

	// Special constraints, applicable to any type parameters
	gpSpecialConstraintMask =  0x001C,
	gpNoSpecialConstraint   =   0x0000,
	gpReferenceTypeConstraint = 0x0004,      // type argument must be a reference type
	gpNotNullableValueTypeConstraint   =   0x0008, // type argument must be a value type but not Nullable
	gpDefaultConstructorConstraint = 0x0010, // type argument must have a public default constructor
};

// Defines a .NET method.
struct dotnet_method_data
{
	// First 12 bits indicate the flags.
	// Last 4 bits indicate the size of the header.
	uint16_t FlagsAndHeaderSize;

	// The maximum number of items on the operand stack.
	uint16_t MaxStack;

	// Size in bytes of the method body.
	uint32_t CodeSize;

	// Metadata token for local variables signature.
	uint32_t LocalVarSigTok;
};

// Represents the method flag constants.
enum MethodFlagConstants
{
	CorILMethod_TinyFormat = 0x2,
	CorILMethod_FatFormat = 0x3,
	CorILMethod_MoreSects = 0x8,
	CorILMethod_InitLocals = 0x10,
};
