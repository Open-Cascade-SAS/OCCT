Data Exchange Wrapper (DE_Wrapper)  {#occt_user_guides__de_wrapper}
============================

@tableofcontents

@section occt_de_wrapper_1 Introduction

This guide explains how to use the **Data Exchange Wrapper** (DE Wrapper).
It provides basic directions on setup, usage and file creation via DE_Wrapper.

The Data Exchange Wrapper (DE Wrapper) module allows reading and writing supported CAD formats to shape objects or special XDE documents, setting up the transfer process for all CAD files.

It is also possible to add support for new CAD formats by prototyping existing tools.

The DE Wrapper component requires @ref occt_user_guides__xde "XDE" toolkit for operation.

This guide mainly explains how to convert CAD files to Open CASCADE Technology (OCCT) shapes and vice versa.
This guide principally deals with the following OCCT classes:
  * The Provider class, which loads CAD files and translates their contents to OCCT shapes or XDE documents, or translates OCCT shapes or XDE documents to CAD entities and then writes those entities to a CAD file.
  * The Configuration class, which contains all information for the transfer process, such as the units, tolerance, and all internal information for the OCC readers or writers.
  * The wrapper class, which contains all loaded configuration objects with own CAD format, reads or writes CAD files in the format derived from file extension or contents and saves or loads configuration settings for loaded configuration objects.

@section occt_de_wrapper_2 Supported CAD formats

| CAD format | Extensions | RW support | Thread Safety | Presentation | Package |
| :--------- | :--------- | :--------- | :----------- | :----------- | :------ |
| STEP | .stp, .step .stepz | RW | No | BRep, Mesh | STEPCAFControl |
| XCAF | .xbf | RW | Yes | BRep, Mesh | DEXCAFCascade |
| BREP | .brep | RW | Yes | BRep, Mesh | DEBRepCascade |
| IGES | .igs, .iges | RW | No | BRep | IGESCAFControl |
| OBJ | .obj | RW | Yes | Mesh | RWObj |
| STL | .stl | RW | Yes | Mesh | RWStl |
| PLY | .ply | W | Yes | Mesh | RWPly |
| GLTF | .glTF .glb | RW | Yes | Mesh | RWGltf |
| VRML | .wrl .vrml | RW | Yes | Mesh | Vrml |

**Note** :
  * The format names in the first column match the FormatName values used for configuration nodes.
  * The VendorName for all listed CAD formats is "OCC".

@section occt_de_wrapper_3 DE Session Configuration

Any providers can have their own read/write parameters. The transfer process is set up using DE configuration nodes, which hold all relevant parameters. There are two ways to change the parameter values: directly from code or by an external resource file/string.
The session is a global or static DE_Wrapper object that stores registered DE configuration nodes and wraps DE commands to work with them. It has some configuration parameters of its own and also keeps track of loaded nodes and specilal global parameters.

@subsection occt_de_wrapper_3_1 Getting a DE session. Code sample

Working with a DE session requires a DE_Wrapper object to be loaded or created first.

Getting the global DE_Wrapping object:
~~~~{.cpp}
Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
~~~~
Creating a local DE_Wrapper:
~~~~{.cpp}
Handle(DE_Wrapper) aSession = new DE_Wrapper();
~~~~
It is recommended to create a local one-time copy to work with the session, if no global changes are intended.
~~~~{.cpp}
Handle(DE_Wrapper) aOneTimeSession = aSession->Copy();
~~~~
@subsection occt_de_wrapper_3_2 Configuration resource

Configuration resource is an external file or string of the following format:
~~~~{.cpp}
global.priority.STEP :   OCC DTK
global.general.length.unit :     1
provider.STEP.OCC.read.precision.val :   0.0001
~~~~

@subsubsection occt_de_wrapper_3_2_1 Configuration resource: graph of scopes
   * **global.** is a scope of global parameters
     *  **priority.** is a scope of priority to use vendors with their providers.
     *  **general.** is a scope of global configuration parameter values
     *  <strong>"..."</strong> is an internal configuration with any internal scopes
     *  <strong>". : "</strong> is a separator of key-value
     *  __...__ parameter value, can't contain new line symbols.
   * **provider.** is a scope of configuration providers
     *  **STEP.** is a scope of CAD format to configure
     *  **OCC.** is a scope of a vendor or provider
     *  <strong>"..."</strong> is an internal configuration with any internal scopes
     *  <strong>". : "</strong> is a separator of key-value
     *  <strong>"..."</strong> parameter value, can't contain new line symbols.

@subsubsection occt_de_wrapper_3_2_2 Loading configuration resources. Configuring DE Session

The resource should be loaded after the registration of all providers that should be configured. The resource only impacts registered parameters. To configure a new registered provider it is necessary to load the resource again. Parameters not present in the resource will remain unchanged.

There are two ways to check what parameters are available:
* C++: Open ConfigureNode file and check the InternalParameter field. Each parameter will be described with a comment. To check the global parameters, use the DE_Wrapper class public methods.
* Resource: Register all available Nodes to the session, then save the configuration and view all existing parameters.

There are two options for loading a resource: recursive and global parameters only. Recursive is the default option to configure all global parameters (units, priority, enable status) and all registered providers. Non-recursive configures only global parameters and ignores all provider settings. This option is the best for updating provider priority.

@subsubsection occt_de_wrapper_3_2_3 Loading configuration resources. Code sample

Configuring using a resource string:
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aString =
    "global.priority.STEP :   OCC DTK\n"
    "global.general.length.unit : 1\n"
    "provider.STEP.OCC.read.precision.val : 0.\n";
  Standard_Boolean aIsRecursive = Standard_True;
  if (!aSession->Load(aString, aIsRecursive))
  {
    Message::SendFail() << "Error: configuration is incorrect";
  }
~~~~
Configuring using a resource file:
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPathToFile = "";
  Standard_Boolean aIsRecursive = Standard_True;
  if (!aSession->Load(aPathToFile, aIsRecursive))
  {
    Message::SendFail() << "Error: configuration is incorrect";
  }
~~~~
@subsubsection occt_de_wrapper_3_2_4 Loading configuration resources. DRAW sample

Configuring using a resource string:
~~~~{.cpp}
set conf "
global.priority.STEP :   OCC
global.general.length.unit :     1
provider.STEP.OCC.read.iges.bspline.continuity :         1
provider.STEP.OCC.read.precision.mode :  0
provider.STEP.OCC.read.precision.val :   0.0001
"
LoadConfiguration ${conf} -recursive on
~~~~

Configuring using a resource file:
~~~~{.cpp}
set pathToFile ""
LoadConfiguration ${pathToFile} -recursive on
~~~~

@subsubsection occt_de_wrapper_3_2_5 Saving configuration resources. Dump of configuration DE Session

Saving the configuration of a DE Session involves dumping all parameters of registered providers.
If a parameter did not change during the session, its value remains as default.

There are two ways to save a resource: recursive and global parameters only. Recursive is the way to dump all registered provider information. Non-recursive dumps only global parameters, for example, save priority of vendors or the length unit.

It is possible to filter what vendors or providers to save by providing the correct name of the vendor or provider.

@subsubsection occt_de_wrapper_3_2_6 Saving configuration resources. Code sample

Dump to resource string. If the vendors list is empty, saves all vendors. If the providers list is empty, saves all providers of valid vendors.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TColStd_ListOfAsciiString aFormats;
  TColStd_ListOfAsciiString aVendors;
  aFormats.Appends("STEP");
  aVendors.Appends("OCC");
  Standard_Boolean aIsRecursive = Standard_True;
  TCollection_AsciiString aConf = aSession->aConf->Save(aIsRecursive, aFormats, aVendors);
~~~~
Configure using a resource file. If the vendors list is empty, saves all vendors. If the providers list is empty, saves all providers of valid vendors.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPathToFile = "";
  TColStd_ListOfAsciiString aFormats;
  TColStd_ListOfAsciiString aVendors;
  aFormats.Appends("STEP");
  aVendors.Appends("OCC");
  Standard_Boolean aIsRecursive = Standard_True;
  if (!aSession->Save(aPathToFile, aIsRecursive, aFormats,aVendors))
  {
    Message::SendFail() << "Error: configuration is not saved";
  }
~~~~
@subsubsection occt_de_wrapper_3_2_7 Saving configuration resources. DRAW sample

Dump configuration to string. If no list of vendors is passed or it is empty, all vendors are saved. If no providers list is passed or it is empty, all providers of valid vendors are saved.
~~~~{.cpp}
set vendors "OCC"
set format "STEP"
set dump_conf [DumpConfiguration -recursive on -format ${format} -vendor ${vendors}]
~~~~

Dump configuration to file. If no vendors list are set as an argument or it is empty, saves all vendors. If no providers list as argument or it is empty, saves all providers of valid vendors:
~~~~{.cpp}
set vendors "OCC"
set format "STEP"
set pathToFile ""
DumpConfiguration -path ${pathToFile} -recursive on -format ${format} -vendor ${vendors}
~~~~

@subsection occt_de_wrapper_3_3 Registering providers

To transfer a CAD file using DE Wrapper, it is necessary to register a CAD provider.
The provider contains internal and global parameters that have default values in the creation stage.
All registered providers are set to the map with information about its vendor and kept as smart handles. Therefore, it is possible to change the values via handle from external code.

@subsubsection occt_de_wrapper_3_3_1 Registering providers. Code sample

It is nesessary to register only one ConfigurationNode for all needed formats.
~~~~{.cpp}
Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
Handle(DE_ConfigurationNode) aNode = new DESTEP_ConfigurationNode();
aSession->Bind(aNode);
~~~~
@subsubsection occt_de_wrapper_3_3_2 Registering providers. DRAW Sample

Use DRAW with all providers registered by the following command:
~~~~{.cpp}
pload XDE
~~~~

@subsubsection occt_de_wrapper_3_3_3 Realtime initialization. Code sample

It is possible to change a parameter from code using a smart pointer.

~~~~{.cpp}
// global variable
static Handle(DESTEP_ConfigurationNode) THE_STEP_NODE;

static Handle(DE_ConfigurationNode) RegisterStepNode()
{
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  if (!THE_STEP_NODE.IsNull())
  {
    return THE_STEP_NODE;
  }

  THE_STEP_NODE = new DESTEP_ConfigurationNode();
  aSession->Bind(THE_STEP_NODE);
  return THE_STEP_NODE;
}

// Change parameter value
THE_STEP_NODE->InternalParameters.ReadRelationship = false;
THE_STEP_NODE->InternalParameters.ReadName = false;
THE_STEP_NODE->InternalParameters.ReadProps = false;
~~~~

@subsection occt_de_wrapper_3_4 Priority of Vendors

DE session is able to work with several vendors with the same supported CAD format. To choose the preferred vendor for each format, use a special priority list.

If the high priority vendor's provider is not supported, a transfer operation is needed (write/read), then the next vendor will be chosen.

@subsubsection occt_de_wrapper_3_4_1 Priority of Vendors. Code sample

~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aFormat = "STEP";
  TColStd_ListOfAsciiString aVendors;
  aVendors.Appends("OCC"); // high priority
  aVendors.Appends("DTK");
  // Flag to disable not chosen vendors, in this case configuration is possible
  // otherwise, lower their priority and continue to check ability to transfer
  Standard_Boolean aToDisable = Standard_True;
  aSession->ChangePriority(aFormat, aVendors, aToDisable);
~~~~

@subsubsection occt_de_wrapper_3_4_2 Priority of Vendors. DRAW Sample

It is recommended to disable recursion and update only global parameters.
~~~~{.cpp}
set conf "
global.priority.STEP :   OCC DTK
"
LoadConfiguration ${conf} -recursive off
~~~~

@section occt_de_wrapper_4 Transfer of CAD files

To transfer from a CAD file to OCC or from OCC to a CAD file, it is necessary to use a configured DE_Wrapper object. It can be local, one-time or global. Global configuration of DE_Wrapper propagates to all nodes via transfer. There are two options for transferring: using OCC shape or XCAF document. It is possible to work only with real path to/from the file. Streaming is not supported (yet).

The format of input/output file is automatically determined by its extension or contents.

@subsection occt_de_wrapper_4_1 Transfer of CAD files. Code samples

Reading STEP file to Shape.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPathToFile = "example.stp";
  TopoDS_Shape aShRes;
  if (!aSession->Read(aPathToFile, aShRes))
  {
    Message::SendFail() << "Error: Can't read file";
  }
~~~~

Writing Shape to STEP file.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPathToFile = "example.stp";
  TopoDS_Shape aShFrom = ...;
  if (!aSession->Write(aPathToFile, aShRes))
  {
    Message::SendFail() << "Error: Can't write file";
  }
~~~~

Reading STEP file into XCAF document.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPathToFile = "example.stp";
  Handle(TDocStd_Document) aDoc = ...;
  if (!aSession->Read(aPathToFile, aDoc))
  {
    Message::SendFail() << "Error: Can't read file";
  }
~~~~

Writing XCAF document into STEP.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPathToFile = "example.stp";
  Handle(TDocStd_Document) aDoc = ...;
  if (!aSession->Write(aPathToFile, aDoc))
  {
    Message::SendFail() << "Error: Can't write file";
  }
~~~~

@subsection occt_de_wrapper_4_2 Transfer of CAD files. DRAW samples

Reading a STEP file into a Shape.
~~~~{.cpp}
set fileName "sample.stp"
readfile shape ${fileName}
~~~~

Writing a Shape into STEP.
~~~~{.cpp}
set fileName "sample.stp"
writefile shape ${fileName}
~~~~

Reading STEP into XCAF document.
~~~~{.cpp}
set fileName "sample.stp"
ReadFile D ${fileName}
~~~~

Writing XCAF document into STEP.
~~~~{.cpp}
set fileName "sample.stp"
WriteFile D ${fileName}
~~~~

@subsection occt_de_wrapper_4_3 Transfer using DE Provider. Code sample

It is possible to read and write CAD files directly from a special provider.

~~~~{.cpp}
// Creating or getting node
Handle(DESTEP_ConfigurationNode) aNode = new DESTEP_ConfigurationNode();
// Creating an one-time provider
Handle(DE_Provider) aProvider = aNode->BuildProvider();
// Setting configuration with all parameters
aProvider->SetNode(aNode);
if (!aProvider->Read(...))
{
  Message::SendFail() << "Error: Can't read STEP file";
}
if (!aProvider->Write(...))
{
  Message::SendFail() << "Error: Can't write STEP file";
}
~~~~

@subsection occt_de_wrapper_4_4 Temporary configuration via transfer

It is possible to change the configuration of only one transfer operation. To avoid changing parameters in a session, one-time clone of the session can be created and used for transfer. This way is recommended for use in multithreaded mode.

@subsubsection occt_de_wrapper_4_4_1 Temporary configuration via transfer. Code sample

Code sample to configure via transfer.
~~~~{.cpp}
  Handle(DE_Wrapper) aSession = DE_Wrapper::GlobalWrapper()->Copy();
  TCollection_AsciiString aString =
    "global.priority.STEP :   OCC DTK\n"
    "global.general.length.unit : 1\n"
    "provider.STEP.OCC.read.precision.val : 0.\n";
  if (!aSession->Load(aString, aIsRecursive))
  {
    Message::SendFail() << "Error: configuration is incorrect";
  }
  TCollection_AsciiString aPathToFile = "example.stp";
  TopoDS_Shape aShRes;
  if (!aSession->Read(aPathToFile, aShRes))
  {
    Message::SendFail() << "Error: Can't read file";
  }
~~~~

@subsubsection occt_de_wrapper_4_4_2 Temporary configuration via transfer. DRAW sample

Code sample to configure via transfer within DRAW command.
~~~~{.cpp}
set fileName "sample.stp"
readfile S5 $filename -conf "global.general.length.unit : 1000 "
~~~~

Code sample to configure via transfer as variable.
~~~~{.cpp}
set fileName "sample.stp"
set conf "
global.priority.STEP :   OCC
global.general.length.unit :     1
provider.STEP.OCC.read.iges.bspline.continuity :         1
provider.STEP.OCC.read.precision.mode :  0
provider.STEP.OCC.read.precision.val :   0.0001
"
readfile S5 $filename -conf ${conf}
~~~~
