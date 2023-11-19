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

#include <RWObj_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <RWObj_Provider.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWObj_ConfigurationNode, DE_ConfigurationNode)

namespace
{
  static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
  {
    static const TCollection_AsciiString aScope = "provider";
    return aScope;
  }

  // Wrapper to auto-load DE component
  DE_PluginHolder<RWObj_ConfigurationNode> THE_OCCT_OBJ_COMPONENT_PLUGIN;
}

//=======================================================================
// function : RWObj_ConfigurationNode
// purpose  :
//=======================================================================
RWObj_ConfigurationNode::RWObj_ConfigurationNode() :
  DE_ConfigurationNode()
{}

//=======================================================================
// function : RWObj_ConfigurationNode
// purpose  :
//=======================================================================
RWObj_ConfigurationNode::RWObj_ConfigurationNode(const Handle(RWObj_ConfigurationNode)& theNode)
  :DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool RWObj_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();
  InternalParameters.FileLengthUnit = 
    theResource->RealVal("file.length.unit", InternalParameters.FileLengthUnit, aScope);
  InternalParameters.SystemCS = (RWMesh_CoordinateSystem)
    (theResource->IntegerVal("system.cs", (int)InternalParameters.SystemCS, aScope) % 2);
  InternalParameters.FileCS = (RWMesh_CoordinateSystem)
    (theResource->IntegerVal("file.cs", (int)InternalParameters.SystemCS, aScope) % 2);

  InternalParameters.ReadSinglePrecision = 
    theResource->BooleanVal("read.single.precision", InternalParameters.ReadSinglePrecision, aScope);
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

  InternalParameters.WriteComment = 
    theResource->StringVal("write.comment", InternalParameters.WriteComment, aScope);
  InternalParameters.WriteAuthor = 
    theResource->StringVal("write.author", InternalParameters.WriteAuthor, aScope);
  return true;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString RWObj_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult = aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Common parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!File length units to convert from while reading the file, defined as scale factor for m (meters)\n";
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
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.single.precision :\t " + InternalParameters.ReadSinglePrecision + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for create a single triangulation\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.create.shapes :\t " + InternalParameters.ReadCreateShapes + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Root folder for generating root labels names\n";
  aResult += "!Default value: ""(empty). Available values: <path>\n";
  aResult += aScope + "read.root.prefix :\t " + InternalParameters.ReadRootPrefix + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for fill document from shape sequence\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.fill.doc :\t " + InternalParameters.ReadFillDoc + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for fill the document with partially retrieved data even if reader has fa-iled with er-ror\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "read.fill.incomplete :\t " + InternalParameters.ReadFillIncomplete + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Memory usage limit(MiB)\n";
  aResult += "!Default value: -1(no limit). Available values: -1(no limit), any positive value\n";
  aResult += aScope + "read.memory.limit.mib :\t " + InternalParameters.ReadMemoryLimitMiB + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Export special comment\n";
  aResult += "!Default value: ""(empty). Available values: <string>\n";
  aResult += aScope + "write.comment :\t " + InternalParameters.WriteComment + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Author of exported file name\n";
  aResult += "!Default value: ""(empty). Available values: <string>\n";
  aResult += aScope + "write.author :\t " + InternalParameters.WriteAuthor + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) RWObj_ConfigurationNode::Copy() const
{
  return new RWObj_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) RWObj_ConfigurationNode::BuildProvider()
{
  return new RWObj_Provider(this);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool RWObj_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool RWObj_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString RWObj_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("OBJ");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString RWObj_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString RWObj_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("obj");
  return anExt;
}
