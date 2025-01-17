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

#include <DEPLY_ConfigurationNode.hxx>

#include <DEPLY_Provider.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <NCollection_Buffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEPLY_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEPLY_ConfigurationNode> THE_OCCT_PLY_COMPONENT_PLUGIN;
} // namespace

//=================================================================================================

DEPLY_ConfigurationNode::DEPLY_ConfigurationNode()
    : DE_ConfigurationNode()
{
}

//=================================================================================================

DEPLY_ConfigurationNode::DEPLY_ConfigurationNode(const Handle(DEPLY_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=================================================================================================

bool DEPLY_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
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

  InternalParameters.WriteNormals =
    theResource->BooleanVal("write.normals", InternalParameters.WriteNormals, aScope);
  InternalParameters.WriteColors =
    theResource->BooleanVal("write.colors", InternalParameters.WriteColors, aScope);
  InternalParameters.WriteTexCoords =
    theResource->BooleanVal("write.tex.coords", InternalParameters.WriteTexCoords, aScope);
  InternalParameters.WritePartId =
    theResource->BooleanVal("write.part.id", InternalParameters.WritePartId, aScope);
  InternalParameters.WriteFaceId =
    theResource->BooleanVal("write.face.id", InternalParameters.WriteFaceId, aScope);
  InternalParameters.WriteComment =
    theResource->StringVal("write.comment", InternalParameters.WriteComment, aScope);
  InternalParameters.WriteAuthor =
    theResource->StringVal("write.author", InternalParameters.WriteAuthor, aScope);
  return Standard_True;
}

//=================================================================================================

TCollection_AsciiString DEPLY_ConfigurationNode::Save() const
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
  aResult += "!Default value: 1.0(MM)\n";
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
  aResult += "!Write parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for write normals\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.normals :\t " + InternalParameters.WriteNormals + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for write colors\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.colors :\t " + InternalParameters.WriteColors + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for write UV / texture coordinates\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.tex.coords :\t " + InternalParameters.WriteTexCoords + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for write part Id as element attribute\n";
  aResult += "!Default value: 1(true). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.part.id :\t " + InternalParameters.WritePartId + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Flag for write face Id as element attribute. Cannot be combined with HasPartId\n";
  aResult += "!Default value: 0(false). Available values: 0(false), 1(true)\n";
  aResult += aScope + "write.face.id :\t " + InternalParameters.WriteFaceId + "\n";
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

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=================================================================================================

Handle(DE_ConfigurationNode) DEPLY_ConfigurationNode::Copy() const
{
  return new DEPLY_ConfigurationNode(*this);
}

//=================================================================================================

Handle(DE_Provider) DEPLY_ConfigurationNode::BuildProvider()
{
  return new DEPLY_Provider(this);
}

//=================================================================================================

bool DEPLY_ConfigurationNode::IsImportSupported() const
{
  return Standard_False;
}

//=================================================================================================

bool DEPLY_ConfigurationNode::IsExportSupported() const
{
  return Standard_True;
}

//=================================================================================================

TCollection_AsciiString DEPLY_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("PLY");
}

//=================================================================================================

TCollection_AsciiString DEPLY_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

TColStd_ListOfAsciiString DEPLY_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("ply");
  return anExt;
}

//=================================================================================================

bool DEPLY_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  if (theBuffer.IsNull() || theBuffer->Size() < 4)
  {
    return false;
  }
  const char* aBytes = (const char*)theBuffer->Data();
  if (!::strncmp(aBytes, "ply", 3) && ::isspace(aBytes[3]))
  {
    return true;
  }
  return false;
}
