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

#include <Vrml_ConfigurationNode.hxx>

#include <DE_ConfigurationContext.hxx>
#include <Vrml_Provider.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_ConfigurationNode, DE_ConfigurationNode)

namespace
{
  static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
  {
    static const TCollection_AsciiString aScope = "provider";
    return aScope;
  }
}

//=======================================================================
// function : STEPCAFControl_ConfigurationNode
// purpose  :
//=======================================================================
Vrml_ConfigurationNode::Vrml_ConfigurationNode() :
  DE_ConfigurationNode()
{
  UpdateLoad();
}

//=======================================================================
// function : STEPCAFControl_ConfigurationNode
// purpose  :
//=======================================================================
Vrml_ConfigurationNode::Vrml_ConfigurationNode(const Handle(Vrml_ConfigurationNode)& theNode)
  :DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
  UpdateLoad();
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool Vrml_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.WriterVersion = (WriteMode_WriterVersion)
    theResource->IntegerVal("writer.version", InternalParameters.WriterVersion, aScope);
  InternalParameters.WriteRepresentationType = (WriteMode_RepresentationType)
    theResource->IntegerVal("write.representation.type", InternalParameters.WriteRepresentationType, aScope);

  return true;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString Vrml_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult = aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope = THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up writer version\n";
  aResult += "!Default value: 2. Available values: 1, 2\n";
  aResult += aScope + "writer.version :\t " + InternalParameters.WriterVersion + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Setting up representation\n";
  aResult += "!Default value: 1. Available values: 0(shaded), 1(wireframe), 2(both)\n";
  aResult += aScope + "write.representation.type :\t " + InternalParameters.WriteRepresentationType + "\n";
  aResult += "!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_ConfigurationNode) Vrml_ConfigurationNode::Copy() const
{
  return new Vrml_ConfigurationNode(*this);
}

//=======================================================================
// function : BuildProvider
// purpose  :
//=======================================================================
Handle(DE_Provider) Vrml_ConfigurationNode::BuildProvider()
{
  return new Vrml_Provider(this);
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool Vrml_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool Vrml_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString Vrml_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("VRML");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString Vrml_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : GetExtensions
// purpose  :
//=======================================================================
TColStd_ListOfAsciiString Vrml_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("vrml");
  anExt.Append("wrl");
  return anExt;
}
