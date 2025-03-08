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

#include <DESTL_ConfigurationNode.hxx>

#include <DESTL_Provider.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <NCollection_Buffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DESTL_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DESTL_ConfigurationNode> THE_OCCT_STL_COMPONENT_PLUGIN;
} // namespace

//=================================================================================================

DESTL_ConfigurationNode::DESTL_ConfigurationNode()
    : DE_ConfigurationNode()
{
}

//=================================================================================================

DESTL_ConfigurationNode::DESTL_ConfigurationNode(const Handle(DESTL_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=================================================================================================

bool DESTL_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadMergeAngle =
    theResource->RealVal("read.merge.angle", InternalParameters.ReadMergeAngle, aScope);
  InternalParameters.ReadBRep =
    theResource->BooleanVal("read.brep", InternalParameters.ReadBRep, aScope);
  InternalParameters.WriteAscii =
    theResource->BooleanVal("write.ascii", InternalParameters.WriteAscii, aScope);
  return true;
}

//=================================================================================================

TCollection_AsciiString DESTL_ConfigurationNode::Save() const
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
  aResult += "!Input merge angle value\n";
  aResult += "!Default value (in degrees): 90.0. Angle should be within [0.0, 90.0] range\n";
  aResult += aScope + "read.merge.angle :\t " + InternalParameters.ReadMergeAngle + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up Boundary Representation flag\n";
  aResult += "!Default value: false. Available values: \"on\", \"off\"\n";
  aResult += aScope + "read.brep :\t " + InternalParameters.ReadBRep + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up writing mode (Ascii or Binary)\n";
  aResult += "!Default value: 1(Binary). Available values: 0(Ascii), 1(Binary)\n";
  aResult += aScope + "write.ascii :\t " + InternalParameters.WriteAscii + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=================================================================================================

Handle(DE_ConfigurationNode) DESTL_ConfigurationNode::Copy() const
{
  return new DESTL_ConfigurationNode(*this);
}

//=================================================================================================

Handle(DE_Provider) DESTL_ConfigurationNode::BuildProvider()
{
  return new DESTL_Provider(this);
}

//=================================================================================================

bool DESTL_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=================================================================================================

bool DESTL_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=================================================================================================

TCollection_AsciiString DESTL_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("STL");
}

//=================================================================================================

TCollection_AsciiString DESTL_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

TColStd_ListOfAsciiString DESTL_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("stl");
  return anExt;
}

//=================================================================================================

bool DESTL_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  if (theBuffer.IsNull() || theBuffer->Size() < 7)
  {
    return false;
  }
  const char* aBytes = (const char*)theBuffer->Data();
  if (!(::strncmp(aBytes, "solid", 5) || ::strncmp(aBytes, "SOLID", 5)) && isspace(aBytes[5]))
  {
    return true;
  }
  // binary STL has no header for identification - format can be detected only by file extension
  return false;
}
