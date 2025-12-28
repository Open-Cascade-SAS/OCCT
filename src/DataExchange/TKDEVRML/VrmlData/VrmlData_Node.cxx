// Created on: 2006-05-25
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#include <Precision.hxx>
#include <VrmlData_Appearance.hxx>
#include <VrmlData_ImageTexture.hxx>
#include <VrmlData_Material.hxx>
#include <VrmlData_ShapeNode.hxx>
#include <VrmlData_UnknownNode.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_InBuffer.hxx>
#include <VrmlData_Geometry.hxx>
#include <VrmlData_TextureTransform.hxx>
#include <VrmlData_Texture.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_Node, Standard_Transient)

#ifdef _MSC_VER
  #define _CRT_SECURE_NO_DEPRECATE
  #pragma warning(disable : 4996)
#endif

static VrmlData_Scene MyDefaultScene;

//=================================================================================================

bool IsEqual(const occ::handle<VrmlData_Node>& theOne, const occ::handle<VrmlData_Node>& theTwo)
{
  bool aResult(false);
  if (theOne->Name() != 0L && theTwo->Name() != 0L)
    aResult = (strcmp(theOne->Name(), theTwo->Name()) == 0);
  return aResult;
}

//=================================================================================================

VrmlData_Node::VrmlData_Node()
    : myScene(&MyDefaultScene),
      myName(0L)
{
}

//=================================================================================================

VrmlData_Node::VrmlData_Node(const VrmlData_Scene& theScene, const char* theName)
    : myScene(&theScene)
{
  if (theName == 0L)
    theName = "";
  setName(theName);
}

//=======================================================================
// function : Clone
// purpose  : Create a copy of this node.
//=======================================================================

occ::handle<VrmlData_Node> VrmlData_Node::Clone(const occ::handle<VrmlData_Node>& theOther) const
{
  if (theOther.IsNull() == false)
  {
    if (theOther->IsKind(DynamicType()) == false)
      return NULL;
    if (&theOther->Scene() == myScene)
      theOther->myName = myName;
    else
      theOther->setName(myName);
  }
  return theOther;
}

//=================================================================================================

void VrmlData_Node::setName(const char* theName, const char* theSuffix)
{
  size_t len[2] = {strlen(theName) + 1, 0};
  if (theSuffix)
    len[1] = strlen(theSuffix);
  char* aName = (char*)Scene().Allocator()->Allocate(len[0] + len[1]);
  myName      = aName;
  memcpy(aName, theName, len[0]);
  if (len[1])
    memcpy(&aName[len[0] - 1], theSuffix, len[1] + 1);
}

//=================================================================================================

bool VrmlData_Node::IsDefault() const
{
  return false;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::Write(const char*) const
{
  return VrmlData_NotImplemented;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::WriteClosing() const
{
  VrmlData_ErrorStatus aResult = Scene().Status();
  if (aResult == VrmlData_StatusOK || aResult == VrmlData_NotImplemented)
    aResult = Scene().WriteLine("}", 0L, -GlobalIndent());
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::readBrace(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (theBuffer.LinePtr[0] == '}')
      theBuffer.LinePtr++;
    else
      aStatus = VrmlData_VrmlFormatError;
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadBoolean(VrmlData_InBuffer& theBuffer, bool& theResult)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "TRUE"))
      theResult = true;
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "FALSE"))
      theResult = false;
    else
      aStatus = VrmlData_BooleanInputError;
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadInteger(VrmlData_InBuffer& theBuffer, long& theResult)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    char* endptr;
    long  aResult;
    aResult = strtol(theBuffer.LinePtr, &endptr, 10);
    if (endptr == theBuffer.LinePtr)
      aStatus = VrmlData_NumericInputError;
    else
    {
      theResult         = aResult;
      theBuffer.LinePtr = endptr;
    }
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadString(VrmlData_InBuffer&       theBuffer,
                                               TCollection_AsciiString& theResult)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (theBuffer.LinePtr[0] != '\"')
      aStatus = VrmlData_StringInputError;
    else
    {
      char* ptr = &theBuffer.LinePtr[1];
      while (*ptr != '\0' && *ptr != '\"')
        ptr++;
      if (*ptr == '\0')
        aStatus = VrmlData_StringInputError;
      else
      {
        *ptr              = '\0';
        theResult         = (const char*)&theBuffer.LinePtr[1];
        theBuffer.LinePtr = ptr + 1;
      }
    }
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadMultiString(
  VrmlData_InBuffer&                         theBuffer,
  NCollection_List<TCollection_AsciiString>& theResult)
{
  VrmlData_ErrorStatus aStatus;
  bool                 isBracketed(false);
  // Read the list of URL
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (theBuffer.LinePtr[0] == '[')
    {
      theBuffer.LinePtr++;
      isBracketed = true;
    }
    while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
    {
      if (isBracketed && theBuffer.LinePtr[0] == ']')
      { // closing bracket
        theBuffer.LinePtr++;
        break;
      }
      TCollection_AsciiString aString;
      if (!OK(aStatus, ReadString(theBuffer, aString)))
        break;
      theResult.Append(aString);
      if (isBracketed == false || !OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        break;
      if (theBuffer.LinePtr[0] == ',')
      {
        theBuffer.LinePtr++;
        continue;
      }
      else if (theBuffer.LinePtr[0] == ']') // closing bracket
        theBuffer.LinePtr++;
      else
        aStatus = VrmlData_VrmlFormatError;
      break;
    }
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadNode(VrmlData_InBuffer&                theBuffer,
                                             occ::handle<VrmlData_Node>&       theNode,
                                             const occ::handle<Standard_Type>& theType)
{
  occ::handle<VrmlData_Node> aNode;
  VrmlData_ErrorStatus       aStatus;
  // First line of a new node should identify this node type
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "USE"))
    {
      TCollection_AsciiString aName;
      aStatus = VrmlData_Scene::ReadWord(theBuffer, aName);
      if (aStatus == VrmlData_StatusOK)
      {
        aNode = myScene->FindNode(aName.ToCString(), theType);
        if (aNode.IsNull())
          aStatus = VrmlData_NodeNameUnknown;
        //         else
        //           aNode = aNode->Clone(0L);
      }
    }

    // We create a relevant node using the line with the type ID
    else if (OK(aStatus,
                const_cast<VrmlData_Scene*>(myScene)->createNode(theBuffer, aNode, theType)))
      if (aNode.IsNull() == false)
        // The node data are read here, including the final closing brace
        aStatus = aNode->Read(theBuffer);

    if (aStatus == VrmlData_StatusOK)
      theNode = aNode;
  }
  return aStatus;
}

//=================================================================================================

occ::handle<VrmlData_Node> VrmlData_ShapeNode::Clone(
  const occ::handle<VrmlData_Node>& theOther) const
{
  occ::handle<VrmlData_ShapeNode> aResult =
    occ::down_cast<VrmlData_ShapeNode>(VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_ShapeNode(theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  if (&aResult->Scene() == &Scene())
  {
    aResult->SetAppearance(myAppearance);
    aResult->SetGeometry(myGeometry);
  }
  else
  {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const occ::handle<VrmlData_UnknownNode> aDummyNode = new VrmlData_UnknownNode(aResult->Scene());
    if (myAppearance.IsNull() == false)
      aResult->SetAppearance(occ::down_cast<VrmlData_Appearance>(myAppearance->Clone(aDummyNode)));
    if (myGeometry.IsNull() == false)
      aResult->SetGeometry(occ::down_cast<VrmlData_Geometry>(myGeometry->Clone(aDummyNode)));
  }
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_ShapeNode::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "appearance"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus      = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_Appearance));
      myAppearance = occ::down_cast<VrmlData_Appearance>(aNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "geometry"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus    = ReadNode(theBuffer, aNode);
      myGeometry = occ::down_cast<VrmlData_Geometry>(aNode);
      // here we do not check for the Geometry type because unknown node types can
      // occur (IndexedLineSet, etc.)
      //                          STANDARD_TYPE(VrmlData_Geometry));
    }
    else
      break;

    if (!OK(aStatus))
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace(theBuffer);
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_ShapeNode::Write(const char* thePrefix) const
{
  VrmlData_ErrorStatus  aStatus(VrmlData_StatusOK);
  const VrmlData_Scene& aScene   = Scene();
  static char           header[] = "Shape {";
  if (OK(aStatus, aScene.WriteLine(thePrefix, header, GlobalIndent())))
  {
    if (myAppearance.IsNull() == false)
      aStatus = aScene.WriteNode("appearance", myAppearance);
    if (myGeometry.IsNull() == false && OK(aStatus))
      aStatus = aScene.WriteNode("geometry", myGeometry);

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=================================================================================================

bool VrmlData_ShapeNode::IsDefault() const
{
  bool aResult(true);
  if (myGeometry.IsNull() == false)
    aResult = myGeometry->IsDefault();
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_UnknownNode::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus = VrmlData_StatusOK;
  int                  aLevelCounter(0);
  // This loop searches for any opening brace.
  // Such brace increments the level counter. A closing brace decrements
  // the counter. The loop terminates when the counter becomes negative.
  while (aLevelCounter >= 0 && OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    int aChar;
    while ((aChar = theBuffer.LinePtr[0]) != '\0')
    {
      theBuffer.LinePtr++;
      if (aChar == '{')
      {
        aLevelCounter++;
        break;
      }
      else if (aChar == '}')
      {
        aLevelCounter--;
        break;
      }
    }
  }
  return aStatus;
}

//=================================================================================================

bool VrmlData_UnknownNode::IsDefault() const
{
  return true;
}

//=================================================================================================

occ::handle<VrmlData_Node> VrmlData_Appearance::Clone(
  const occ::handle<VrmlData_Node>& theOther) const
{
  occ::handle<VrmlData_Appearance> aResult =
    occ::down_cast<VrmlData_Appearance>(VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Appearance(theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  if (&aResult->Scene() == &Scene())
  {
    aResult->SetMaterial(myMaterial);
    aResult->SetTexture(myTexture);
    aResult->SetTextureTransform(myTTransform);
  }
  else
  {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const occ::handle<VrmlData_UnknownNode> aDummyNode = new VrmlData_UnknownNode(aResult->Scene());
    if (myMaterial.IsNull() == false)
      aResult->SetMaterial(occ::down_cast<VrmlData_Material>(myMaterial->Clone(aDummyNode)));
    if (myTexture.IsNull() == false)
      aResult->SetTexture(occ::down_cast<VrmlData_Texture>(myTexture->Clone(aDummyNode)));
    if (myTTransform.IsNull() == false)
      aResult->SetTextureTransform(
        occ::down_cast<VrmlData_TextureTransform>(myTTransform->Clone(aDummyNode)));
  }
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Appearance::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "material"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus    = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_Material));
      myMaterial = occ::down_cast<VrmlData_Material>(aNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "textureTransform"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus      = ReadNode(theBuffer, aNode
                         /*,STANDARD_TYPE(VrmlData_TextureTransform)*/);
      myTTransform = occ::down_cast<VrmlData_TextureTransform>(aNode);
    }
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "texture"))
    {
      occ::handle<VrmlData_Node> aNode;
      aStatus   = ReadNode(theBuffer, aNode, STANDARD_TYPE(VrmlData_Texture));
      myTexture = occ::down_cast<VrmlData_Texture>(aNode);
    }
    else
      break;

    if (!OK(aStatus))
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace(theBuffer);
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_Appearance::Write(const char* thePrefix) const
{
  static char           header[] = "Appearance {";
  VrmlData_ErrorStatus  aStatus;
  const VrmlData_Scene& aScene = Scene();
  if (OK(aStatus, aScene.WriteLine(thePrefix, header, GlobalIndent())))
  {
    if (myMaterial.IsNull() == false)
      aStatus = aScene.WriteNode("material", myMaterial);
    if (myTexture.IsNull() == false && OK(aStatus))
      aStatus = aScene.WriteNode("texture", myTexture);
    if (myTTransform.IsNull() == false && OK(aStatus))
      aStatus = aScene.WriteNode("textureTransform", myTTransform);

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=================================================================================================

bool VrmlData_Appearance::IsDefault() const
{
  bool aResult(true);
  if (myMaterial.IsNull() == false)
    aResult = myMaterial->IsDefault();
  if (aResult && myTexture.IsNull() == false)
    aResult = myTexture->IsDefault();
  if (aResult && myTTransform.IsNull() == false)
    aResult = myTTransform->IsDefault();
  return aResult;
}

//=================================================================================================

VrmlData_ImageTexture::VrmlData_ImageTexture(const VrmlData_Scene& theScene,
                                             const char*           theName,
                                             const char*           theURL,
                                             const bool            theRepS,
                                             const bool            theRepT)
    : VrmlData_Texture(theScene, theName, theRepS, theRepT),
      myURL(theScene.Allocator())
{
  myURL.Append(theURL ? (const char*)theURL : "");
}

//=================================================================================================

occ::handle<VrmlData_Node> VrmlData_ImageTexture::Clone(
  const occ::handle<VrmlData_Node>& theOther) const
{
  occ::handle<VrmlData_ImageTexture> aResult =
    occ::down_cast<VrmlData_ImageTexture>(VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_ImageTexture(theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  aResult->myURL = myURL;
  return aResult;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_ImageTexture::Read(VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  bool                 aRepeatS(true), aRepeatT(true);
  myURL.Clear();
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "url"))
      aStatus = ReadMultiString(theBuffer, myURL);
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "repeatS"))
      aStatus = ReadBoolean(theBuffer, aRepeatS);
    else if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "repeatT"))
      aStatus = ReadBoolean(theBuffer, aRepeatT);
    else
      break;

    if (!OK(aStatus))
      break;
  }
  if (OK(aStatus) && OK(aStatus, readBrace(theBuffer)))
  {
    SetRepeatS(aRepeatS);
    SetRepeatT(aRepeatT);
  }
  return aStatus;
}

//=================================================================================================

VrmlData_ErrorStatus VrmlData_ImageTexture::Write(const char* thePrefix) const
{
  VrmlData_ErrorStatus  aStatus  = VrmlData_StatusOK;
  const VrmlData_Scene& aScene   = Scene();
  static char           header[] = "ImageTexture {";
  if (aScene.IsDummyWrite() == false
      && OK(aStatus, aScene.WriteLine(thePrefix, header, GlobalIndent())))
  {
    TCollection_AsciiString url = "\"";
    url += URL().First();
    url += "\"";

    try
    {
      aStatus = aScene.WriteLine("url ", url.ToCString());
    }
    catch (...)
    {
    }
    aStatus = WriteClosing();
  }
  return aStatus;
}
