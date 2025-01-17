// Copyright (c) 2022 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <DEGLTF_ConfigurationNode.hxx>

#include <DEGLTF_Provider.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEGLTF_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEGLTF_ConfigurationNode> THE_OCCT_GLTF_COMPONENT_PLUGIN;
} // namespace

//=================================================================================================

DEGLTF_ConfigurationNode::DEGLTF_ConfigurationNode()
    : DE_ConfigurationNode()
{
}

//=================================================================================================

DEGLTF_ConfigurationNode::DEGLTF_ConfigurationNode(const Handle(DEGLTF_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=================================================================================================

bool DEGLTF_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.FileLengthUnit =
    theResource->RealVal("file.length.unit", InternalParameters.FileLengthUnit, aScope);
  InternalParameters.SystemCS =
    (RWMesh_CoordinateSystem)(theResource->IntegerVal("system.cs",
                                                      (int)InternalParameters.SystemCS,
                                                      aScope)
                              % 2);
  InternalParameters.FileCS =
    (RWMesh_CoordinateSystem)(theResource->IntegerVal("file.cs",
                                                      (int)InternalParameters.SystemCS,
                                                      aScope)
                              % 2);

  InternalParameters.ReadSinglePrecision =
    theResource->BooleanVal("read.single.precision",
                            InternalParameters.ReadSinglePrecision,
                            aScope);
  InternalParameters.ReadCreateShapes =
    theResource->BooleanVal("read.create.shapes", InternalParameters.ReadCreateShapes, aScope);
  InternalParameters.ReadRootPrefix =
    theResource->StringVal("read.root.prefix", InternalParameters.ReadRootPrefix, aScope);
  InternalParameters.ReadFillDoc =
    theResource->BooleanVal("read.fill.doc", InternalParameters.ReadFillDoc, aScope);
  InternalParameters.ReadFillIncomplete =
    theResource->BooleanVal("read.fill.incomplete", InternalParameters.ReadFillIncomplete, aScope);
  InternalParameters.ReadMemoryLimitMiB =
    theResource->IntegerVal("read.memory.limit.mib", InternalParameters.ReadMemoryLimitMiB, aScope);
  InternalParameters.ReadParallel =
    theResource->BooleanVal("read.parallel", InternalParameters.ReadParallel, aScope);
  InternalParameters.ReadSkipEmptyNodes =
    theResource->BooleanVal("read.skip.empty.nodes", InternalParameters.ReadSkipEmptyNodes, aScope);
  InternalParameters.ReadLoadAllScenes =
    theResource->BooleanVal("read.load.all.scenes", InternalParameters.ReadLoadAllScenes, aScope);
  InternalParameters.ReadUseMeshNameAsFallback =
    theResource->BooleanVal("read.use.mesh.name.as.fallback",
                            InternalParameters.ReadUseMeshNameAsFallback,
                            aScope);
  InternalParameters.ReadSkipLateDataLoading =
    theResource->BooleanVal("read.skip.late.data.loading",
                            InternalParameters.ReadSkipLateDataLoading,
                            aScope);
  InternalParameters.ReadKeepLateData =
    theResource->BooleanVal("read.keep.late.data", InternalParameters.ReadKeepLateData, aScope);
  InternalParameters.ReadPrintDebugMessages =
    theResource->BooleanVal("read.print.debug.message",
                            InternalParameters.ReadPrintDebugMessages,
                            aScope);

  InternalParameters.WriteComment =
    theResource->StringVal("write.comment", InternalParameters.WriteComment, aScope);
  InternalParameters.WriteAuthor =
    theResource->StringVal("write.author", InternalParameters.WriteAuthor, aScope);

  InternalParameters.WriteTrsfFormat =
    (RWGltf_WriterTrsfFormat)(theResource->IntegerVal("write.trsf.format",
                                                      InternalParameters.WriteTrsfFormat,
                                                      aScope)
                              % (RWGltf_WriterTrsfFormat_UPPER + 1));
  InternalParameters.WriteNodeNameFormat =
    (RWMesh_NameFormat)(theResource->IntegerVal("write.node.name.format",
                                                InternalParameters.WriteNodeNameFormat,
                                                aScope)
                        % (RWMesh_NameFormat_ProductAndInstanceAndOcaf + 1));
  InternalParameters.WriteMeshNameFormat =
    (RWMesh_NameFormat)(theResource->IntegerVal("write.mesh.name.format",
                                                InternalParameters.WriteMeshNameFormat,
                                                aScope)
                        % (RWMesh_NameFormat_ProductAndInstanceAndOcaf + 1));
  InternalParameters.WriteForcedUVExport =
    theResource->BooleanVal("write.forced.uv.export",
                            InternalParameters.WriteForcedUVExport,
                            aScope);
  InternalParameters.WriteEmbedTexturesInGlb =
    theResource->BooleanVal("write.embed.textures.in.glb",
                            InternalParameters.WriteEmbedTexturesInGlb,
                            aScope);
  InternalParameters.WriteMergeFaces =
    theResource->BooleanVal("write.merge.faces", InternalParameters.WriteMergeFaces, aScope);
  InternalParameters.WriteSplitIndices16 =
    theResource->BooleanVal("write.split.indices16",
                            InternalParameters.WriteSplitIndices16,
                            aScope);
  return true;
}

//=================================================================================================

TCollection_AsciiString DEGLTF_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult =
    aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Common parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!File length units to convert from while reading the file, defined as scale factor "
             "for m (meters)\n";
  aResult += "!Default value: 1.0(M)\n";
  aResult += aScope + "file.length.unit :\t " + InternalParameters.FileLengthUnit + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!System origin coordinate system to perform conversion into during read\n";
  aResult += "!Default value: 0(Zup). Available values: 0(Zup), 1(Yup)\n";
  aResult += aScope + "system.cs :\t " + InternalParameters.SystemCS + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!File origin coordinate system to perform conversion during read\n";
  aResult += "!Default value: 1(Yup). Available values: 0(Zup), 1(Yup)\n";
  aResult += aScope + "file.cs :\t " + InternalParameters.FileCS + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Read parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for reading vertex data with single or double floating point precision\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.single.precision :\t " + InternalParameters.ReadSinglePrecision + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for create a single triangulation\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.create.shapes :\t " + InternalParameters.ReadCreateShapes + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Root folder for generating root labels names\n";
  aResult += "!Default value: "
             "(empty). Available values: <path>\n";
  aResult += aScope + "read.root.prefix :\t " + InternalParameters.ReadRootPrefix + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for fill document from shape sequence\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.fill.doc :\t " + InternalParameters.ReadFillDoc + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for fill the document with partially retrieved data even if reader has fa-iled "
             "with er-ror\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.fill.incomplete :\t " + InternalParameters.ReadFillIncomplete + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Memory usage limit(MiB)\n";
  aResult += "!Default value: -1(no limit). Available values: -1(no limit), any positive value\n";
  aResult += aScope + "read.memory.limit.mib :\t " + InternalParameters.ReadMemoryLimitMiB + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to use multithreading\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.parallel :\t " + InternalParameters.ReadParallel + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to ignore nodes without Geometry\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.skip.empty.nodes :\t " + InternalParameters.ReadSkipEmptyNodes + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to load all scenes in the document\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.load.all.scenes :\t " + InternalParameters.ReadLoadAllScenes + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to use Mesh name in case if Node name is empty\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.use.mesh.name.as.fallback :\t "
             + InternalParameters.ReadUseMeshNameAsFallback + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to skip triangulation loading\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult +=
    aScope + "read.skip.late.data.loading :\t " + InternalParameters.ReadSkipLateDataLoading + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult +=
    "!Flag to keep information about deferred storage to load/unload triangulation later\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.keep.late.data :\t " + InternalParameters.ReadKeepLateData + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to print additional debug information\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult +=
    aScope + "read.print.debug.message :\t " + InternalParameters.ReadPrintDebugMessages + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Export special comment\n";
  aResult += "!Default value: "
             "(empty). Available values: <string>\n";
  aResult += aScope + "write.comment :\t " + InternalParameters.WriteComment + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Author of exported file name\n";
  aResult += "!Default value: "
             "(empty). Available values: <string>\n";
  aResult += aScope + "write.author :\t " + InternalParameters.WriteAuthor + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Transformation format to write into glTF file\n";
  aResult += "!Default value: 0(Compact). Available values: 0(Compact), 1(Mat4), 2(TRS)\n";
  aResult += aScope + "write.trsf.format :\t " + InternalParameters.WriteTrsfFormat + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "! Name format for exporting Nodes\n";
  aResult += "!Default value: 3(InstanceOrProduct). Available values: 0(Compact), 1(Mat4), 2(TRS), "
             "3(InstanceOrProduct), 4(ProductOrInstance), 5(ProductAndInstance), "
             "6(ProductAndInstanceAndOcaf)\n";
  aResult += aScope + "write.node.name.format :\t " + InternalParameters.WriteNodeNameFormat + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Name format for exporting Meshes\n";
  aResult += "!Default value: 1(Product). Available values: 0(Compact), 1(Mat4), 2(TRS), "
             "3(InstanceOrProduct), 4(ProductOrInstance), 5(ProductAndInstance), "
             "6(ProductAndInstanceAndOcaf)\n";
  aResult += aScope + "write.mesh.name.format :\t " + InternalParameters.WriteMeshNameFormat + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Export UV coordinates even if there are no mapped texture\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.forced.uv.export :\t " + InternalParameters.WriteForcedUVExport + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to write image textures into GLB file\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult +=
    aScope + "write.embed.textures.in.glb :\t " + InternalParameters.WriteEmbedTexturesInGlb + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to merge faces within a single part\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.merge.faces :\t " + InternalParameters.WriteMergeFaces + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag to prefer keeping 16-bit indexes while merging face\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.split.indices16 :\t " + InternalParameters.WriteSplitIndices16 + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=================================================================================================

Handle(DE_ConfigurationNode) DEGLTF_ConfigurationNode::Copy() const
{
  return new DEGLTF_ConfigurationNode(*this);
}

//=================================================================================================

Handle(DE_Provider) DEGLTF_ConfigurationNode::BuildProvider()
{
  return new DEGLTF_Provider(this);
}

//=================================================================================================

bool DEGLTF_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=================================================================================================

bool DEGLTF_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=================================================================================================

TCollection_AsciiString DEGLTF_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("GLTF");
}

//=================================================================================================

TCollection_AsciiString DEGLTF_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

TColStd_ListOfAsciiString DEGLTF_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("gltf");
  anExt.Append("glb");
  return anExt;
}
