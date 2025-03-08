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

#include <DEVRML_ConfigurationNode.hxx>

#include <DEVRML_Provider.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEVRML_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEVRML_ConfigurationNode> THE_OCCT_VRML_COMPONENT_PLUGIN;
} // namespace

//=================================================================================================

DEVRML_ConfigurationNode::DEVRML_ConfigurationNode()
    : DE_ConfigurationNode()
{
}

//=================================================================================================

DEVRML_ConfigurationNode::DEVRML_ConfigurationNode(const Handle(DEVRML_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=================================================================================================

bool DEVRML_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadFileUnit =
    theResource->RealVal("read.file.unit", InternalParameters.ReadFileUnit, aScope);
  InternalParameters.ReadFileCoordinateSys =
    (RWMesh_CoordinateSystem)theResource->IntegerVal("read.file.coordinate.system",
                                                     InternalParameters.ReadFileCoordinateSys,
                                                     aScope);
  InternalParameters.ReadSystemCoordinateSys =
    (RWMesh_CoordinateSystem)theResource->IntegerVal("read.system.coordinate.system",
                                                     InternalParameters.ReadSystemCoordinateSys,
                                                     aScope);
  InternalParameters.ReadFillIncomplete =
    theResource->BooleanVal("read.fill.incomplete", InternalParameters.ReadFillIncomplete, aScope);

  InternalParameters.WriterVersion =
    (WriteMode_WriterVersion)theResource->IntegerVal("writer.version",
                                                     InternalParameters.WriterVersion,
                                                     aScope);
  InternalParameters.WriteRepresentationType =
    (WriteMode_RepresentationType)theResource->IntegerVal(
      "write.representation.type",
      InternalParameters.WriteRepresentationType,
      aScope);

  return true;
}

//=================================================================================================

TCollection_AsciiString DEVRML_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult =
    aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Read parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set (override) file length units to convert from while reading the file, defined as "
             "scale factor for m (meters).\n";
  aResult += "!Default value: 1. Available values: positive double\n";
  aResult += aScope + "read.file.unit :\t " + InternalParameters.ReadFileUnit + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set (override) file origin coordinate system to perform conversion during read.\n";
  aResult += "!Default value: Yup (1). { Zup (0) | Yup (1) }\n";
  aResult +=
    aScope + "read.file.coordinate.system :\t " + InternalParameters.ReadFileCoordinateSys + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set system origin coordinate system to perform conversion into during read.\n";
  aResult += "!Default value: Zup (0). Available values: { Zup (0) | Yup (1) }\n";
  aResult += aScope + "read.system.coordinate.system :\t "
             + InternalParameters.ReadSystemCoordinateSys + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Set flag allowing partially read file content to be put into the XDE document.\n";
  aResult += "!Default value: 1(\"ON\"). Available values: 0(\"OFF\"), 1(\"ON\")\n";
  aResult += aScope + "read.fill.incomplete :\t " + InternalParameters.ReadFillIncomplete + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up writer version.\n";
  aResult += "!Default value: 2. Available values: 1, 2\n";
  aResult += aScope + "writer.version :\t " + InternalParameters.WriterVersion + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up representation\n";
  aResult += "!Default value: 1. Available values: 0(shaded), 1(wireframe), 2(both).\n";
  aResult +=
    aScope + "write.representation.type :\t " + InternalParameters.WriteRepresentationType + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=================================================================================================

Handle(DE_ConfigurationNode) DEVRML_ConfigurationNode::Copy() const
{
  return new DEVRML_ConfigurationNode(*this);
}

//=================================================================================================

Handle(DE_Provider) DEVRML_ConfigurationNode::BuildProvider()
{
  return new DEVRML_Provider(this);
}

//=================================================================================================

bool DEVRML_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=================================================================================================

bool DEVRML_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=================================================================================================

TCollection_AsciiString DEVRML_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("VRML");
}

//=================================================================================================

TCollection_AsciiString DEVRML_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

TColStd_ListOfAsciiString DEVRML_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("vrml");
  anExt.Append("wrl");
  return anExt;
}
