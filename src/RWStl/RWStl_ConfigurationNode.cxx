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

#include <RWStl_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <NCollection_Buffer.hxx>
#include <RWStl_Provider.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWStl_ConfigurationNode, DE_ConfigurationNode)

namespace
{
  static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
  {
    static const TCollection_AsciiString aScope = "provider";
    return aScope;
  }

  // Wrapper to auto-load DE component
  DE_PluginHolder<RWStl_ConfigurationNode> THE_OCCT_STL_COMPONENT_PLUGIN;
}

//=======================================================================
// function : STEPCAFControl_ConfigurationNode
// purpose  :
//=======================================================================
RWStl_ConfigurationNode::RWStl_ConfigurationNode() :
  DE_ConfigurationNode()
{}

//=======================================================================
// function : STEPCAFControl_ConfigurationNode
// purpose  :
//=======================================================================
RWStl_ConfigurationNode::RWStl_ConfigurationNode(const Handle(RWStl_ConfigurationNode)& theNode)
  :DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool RWStl_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadMergeAngle = 
    theResource->RealVal("read.merge.angle", InternalParameters.ReadMergeAngle, aScope);
  InternalParameters.ReadBRep = 
    theResource->BooleanVal("read.brep", InternalParameters.ReadBRep, aScope);
  InternalParameters.WriteAscii = 
    theResource->BooleanVal("write.ascii", InternalParameters.WriteAscii, aScope);
  return true;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString RWStl_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult = aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

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

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) RWStl_ConfigurationNode::Copy() const
{
  return new RWStl_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) RWStl_ConfigurationNode::BuildProvider()
{
  return new RWStl_Provider(this);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool RWStl_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool RWStl_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString RWStl_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("STL");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString RWStl_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString RWStl_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("stl");
  return anExt;
}

//=======================================================================
// function : CheckContent
// purpose  :
//=======================================================================
bool RWStl_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
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
