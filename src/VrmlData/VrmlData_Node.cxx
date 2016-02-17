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
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <VrmlData_Geometry.hxx>
#include <VrmlData_TextureTransform.hxx>
#include <VrmlData_Texture.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_Node,Standard_Transient)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif

static VrmlData_Scene MyDefaultScene;

//=======================================================================
//function : IsEqual
//purpose  : Global method
//=======================================================================

Standard_Boolean IsEqual (const Handle(VrmlData_Node)& theOne,
                          const Handle(VrmlData_Node)& theTwo)
{
  Standard_Boolean aResult (Standard_False);
  if (theOne->Name() != 0L && theTwo->Name() != 0L)
    aResult = (strcmp (theOne->Name(), theTwo->Name()) == 0);
  return aResult;
}

//=======================================================================
//function : HashCode
//purpose  : Global method
//=======================================================================

Standard_Integer HashCode(const Handle(VrmlData_Node)& theNode,
                          const Standard_Integer       theUpper)
{
  return (theNode->Name() == 0L ? 0
          : HashCode((Standard_CString)theNode->Name(), theUpper));
}

//=======================================================================
//function : VrmlData_Node
//purpose  : 
//=======================================================================

VrmlData_Node::VrmlData_Node ()
  : myScene (&MyDefaultScene),
    myName  (0L) {}

//=======================================================================
//function : VrmlData_Node
//purpose  : Constructor
//=======================================================================

VrmlData_Node::VrmlData_Node (const VrmlData_Scene& theScene,
                              const char            * theName)
  : myScene   (&theScene)
{
  if (theName == 0L)
    theName = "";
  setName (theName);
}

//=======================================================================
//function : Clone
//purpose  : Create a copy of this node.
//=======================================================================

Handle(VrmlData_Node) VrmlData_Node::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  if (theOther.IsNull() == Standard_False) {
    if (theOther->IsKind (DynamicType()) == Standard_False)
      return NULL;
    if (&theOther->Scene() == myScene)
      theOther->myName = myName;
    else
      theOther->setName (myName);
  }
  return theOther;
}

//=======================================================================
//function : setName
//purpose  : 
//=======================================================================

void VrmlData_Node::setName (const char * theName, const char * theSuffix)
{
  size_t len[2] = {
    strlen(theName) + 1,
    0
  };
  if (theSuffix)
    len[1] = strlen (theSuffix);
  char * aName = (char *)Scene().Allocator()->Allocate(len[0]+len[1]);
  myName = aName;
  memcpy (aName, theName, len[0]);
  if (len[1])
    memcpy (&aName[len[0] - 1], theSuffix, len[1]+1);
}

//=======================================================================
//function : IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_Node::IsDefault () const
{
  return Standard_False;
}


//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::Write (const char *) const
{
  return VrmlData_NotImplemented;
}

//=======================================================================
//function : WriteClosing
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::WriteClosing () const
{
  VrmlData_ErrorStatus aResult = Scene().Status();
  if (aResult == VrmlData_StatusOK || aResult == VrmlData_NotImplemented)
    aResult = Scene().WriteLine ("}", 0L, -GlobalIndent());
  return aResult;
}

//=======================================================================
//function : readBrace
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::readBrace (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (theBuffer.LinePtr[0] == '}')
      theBuffer.LinePtr++;
    else
      aStatus = VrmlData_VrmlFormatError;
  }
  return aStatus;
}

//=======================================================================
//function : ReadBoolean
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadBoolean (VrmlData_InBuffer& theBuffer,
                                                 Standard_Boolean&  theResult)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "TRUE"))
      theResult = Standard_True;
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "FALSE"))
      theResult = Standard_False;
    else
      aStatus = VrmlData_BooleanInputError;
  }
  return aStatus;
}

//=======================================================================
//function : ReadInteger
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadInteger (VrmlData_InBuffer& theBuffer,
                                                 long&              theResult)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    char * endptr;
    long aResult;
    aResult = strtol (theBuffer.LinePtr, &endptr, 10);
    if (endptr == theBuffer.LinePtr)
      aStatus = VrmlData_NumericInputError;
    else {
      theResult = aResult;
      theBuffer.LinePtr = endptr;
    }
  }
  return aStatus;
}

//=======================================================================
//function : ReadString
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadString
                                (VrmlData_InBuffer& theBuffer,
                                 TCollection_AsciiString&  theResult)
{
  VrmlData_ErrorStatus aStatus;
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (theBuffer.LinePtr[0] != '\"')
      aStatus = VrmlData_StringInputError;
    else {
      char * ptr = &theBuffer.LinePtr[1];
      while (*ptr != '\0' && *ptr != '\"')
        ptr++;
      if (*ptr == '\0')
        aStatus = VrmlData_StringInputError;
      else {
        *ptr = '\0';
        theResult = (Standard_CString) &theBuffer.LinePtr[1];
        theBuffer.LinePtr = ptr+1;
      }
    }
  }
  return aStatus;
}

//=======================================================================
//function : ReadMultiString
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadMultiString
                        (VrmlData_InBuffer&                         theBuffer,
                         NCollection_List<TCollection_AsciiString>& theResult)
{
  VrmlData_ErrorStatus aStatus;
  Standard_Boolean isBracketed (Standard_False);
  // Read the list of URL
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (theBuffer.LinePtr[0] == '[') {
      theBuffer.LinePtr++;
      isBracketed = Standard_True;
    }
    while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
      if (isBracketed && theBuffer.LinePtr[0] == ']') { // closing bracket
        theBuffer.LinePtr++;
        break;
      }
      TCollection_AsciiString aString;
      if (!OK(aStatus, ReadString(theBuffer, aString)))
        break;
      theResult.Append(aString);
      if (isBracketed == Standard_False ||
          !OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
        break;
      if (theBuffer.LinePtr[0] == ',') {
        theBuffer.LinePtr++;
        continue;
      } else if (theBuffer.LinePtr[0] == ']') // closing bracket
        theBuffer.LinePtr++;
      else
        aStatus = VrmlData_VrmlFormatError;
      break;
    }
  }
  return aStatus;
}

//=======================================================================
//function : ReadNode
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Node::ReadNode
                                (VrmlData_InBuffer&             theBuffer,
                                 Handle(VrmlData_Node)&         theNode,
                                 const Handle(Standard_Type)&   theType)
{
  Handle(VrmlData_Node) aNode;
  VrmlData_ErrorStatus  aStatus;
  // First line of a new node should identify this node type
  if (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (VRMLDATA_LCOMPARE(theBuffer.LinePtr, "USE")) {
      TCollection_AsciiString aName;
      aStatus = VrmlData_Scene::ReadWord (theBuffer, aName);
      if (aStatus == VrmlData_StatusOK) {
        aNode = myScene->FindNode (aName.ToCString(), theType);
        if (aNode.IsNull())
          aStatus = VrmlData_NodeNameUnknown;
//         else
//           aNode = aNode->Clone(0L);
      }
    }

    // We create a relevant node using the line with the type ID
    else if (OK(aStatus,
                const_cast<VrmlData_Scene *>(myScene)->createNode (theBuffer,
                                                                   aNode,
                                                                   theType)))
      if (aNode.IsNull() == Standard_False)
        // The node data are read here, including the final closing brace
        aStatus = aNode->Read(theBuffer);

    if (aStatus == VrmlData_StatusOK)
      theNode = aNode;
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_ShapeNode::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_ShapeNode::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_ShapeNode) aResult =
    Handle(VrmlData_ShapeNode)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult= new VrmlData_ShapeNode(theOther.IsNull()?Scene():theOther->Scene(),
                                    Name());
  if (&aResult->Scene() == &Scene()) {
    aResult->SetAppearance (myAppearance);
    aResult->SetGeometry   (myGeometry);
  } else {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const Handle(VrmlData_UnknownNode) aDummyNode =
      new VrmlData_UnknownNode (aResult->Scene());
    if (myAppearance.IsNull() == Standard_False)
      aResult->SetAppearance(Handle(VrmlData_Appearance)::DownCast
                             (myAppearance->Clone (aDummyNode)));
    if (myGeometry.IsNull() == Standard_False)
      aResult->SetGeometry (Handle(VrmlData_Geometry)::DownCast
                            (myGeometry->Clone (aDummyNode)));
  }
  return aResult;
}

//=======================================================================
//function : VrmlData_ShapeNode::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_ShapeNode::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "appearance"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Appearance));
      myAppearance = Handle(VrmlData_Appearance)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "geometry"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode);
      myGeometry = Handle(VrmlData_Geometry)::DownCast (aNode);
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
    aStatus = readBrace (theBuffer);
  return aStatus;
}

//=======================================================================
//function : VrmlData_ShapeNode::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_ShapeNode::Write (const char * thePrefix) const
{
  VrmlData_ErrorStatus aStatus (VrmlData_StatusOK);
  const VrmlData_Scene& aScene = Scene();
  static char header[] = "Shape {";
  if (OK (aStatus, aScene.WriteLine (thePrefix, header, GlobalIndent())))
  {
    if (myAppearance.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("appearance", myAppearance);
    if (myGeometry.IsNull() == Standard_False && OK(aStatus))
      aStatus = aScene.WriteNode ("geometry", myGeometry);

    aStatus = WriteClosing();
  }
  return aStatus;
}  

//=======================================================================
//function : VrmlData_ShapeNode::IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_ShapeNode::IsDefault () const
{
  Standard_Boolean aResult (Standard_True);
  if (myGeometry.IsNull() == Standard_False)
    aResult = myGeometry->IsDefault();
  return aResult;
}

//=======================================================================
//function : VrmlData_UnknownNode::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_UnknownNode::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus = VrmlData_StatusOK;
  Standard_Integer aLevelCounter (0);
  // This loop searches for any opening brace.
  // Such brace increments the level counter. A closing brace decrements
  // the counter. The loop terminates when the counter becomes negative.
  while (aLevelCounter >= 0 &&
         OK(aStatus, VrmlData_Scene::ReadLine(theBuffer)))
  {
    int aChar;
    while ((aChar = theBuffer.LinePtr[0]) != '\0') {
      theBuffer.LinePtr++;
      if        (aChar == '{') {
        aLevelCounter++;
        break;
      } else if (aChar == '}') {
        aLevelCounter--;
        break;
      }
    }
  }
  return aStatus;
}

//=======================================================================
//function : VrmlData_UnknownNode::IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_UnknownNode::IsDefault () const
{
  return Standard_True;
}

//=======================================================================
//function : VrmlData_Appearance::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_Appearance::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_Appearance) aResult =
    Handle(VrmlData_Appearance)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult = new VrmlData_Appearance
      (theOther.IsNull() ? Scene() : theOther->Scene(), Name());
  if (&aResult->Scene() == &Scene()) {
    aResult->SetMaterial          (myMaterial);
    aResult->SetTexture           (myTexture);
    aResult->SetTextureTransform  (myTTransform);
  } else {
    // Create a dummy node to pass the different Scene instance to methods Clone
    const Handle(VrmlData_UnknownNode) aDummyNode =
      new VrmlData_UnknownNode (aResult->Scene());
    if (myMaterial.IsNull()   == Standard_False)
      aResult->SetMaterial (Handle(VrmlData_Material)::DownCast
                            (myMaterial->Clone (aDummyNode)));
    if (myTexture.IsNull()    == Standard_False)
      aResult->SetTexture(Handle(VrmlData_Texture)::DownCast
                          (myTexture->Clone (aDummyNode)));
    if (myTTransform.IsNull() == Standard_False)
      aResult->SetTextureTransform(Handle(VrmlData_TextureTransform)::DownCast
                                   (myTTransform->Clone (aDummyNode)));
  }
  return aResult;
}

//=======================================================================
//function : VrmlData_Appearance::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Appearance::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "material"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Material));
      myMaterial = Handle(VrmlData_Material)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "textureTransform"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode
                          /*,STANDARD_TYPE(VrmlData_TextureTransform)*/);
      myTTransform = Handle(VrmlData_TextureTransform)::DownCast (aNode);
    }
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "texture"))
    {
      Handle(VrmlData_Node) aNode;
      aStatus = ReadNode (theBuffer, aNode,
                          STANDARD_TYPE(VrmlData_Texture));
      myTexture = Handle(VrmlData_Texture)::DownCast (aNode);
    }
    else
      break;

    if (!OK(aStatus))
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace (theBuffer);
  return aStatus;
}

//=======================================================================
//function : VrmlData_Appearance::Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_Appearance::Write (const char * thePrefix) const
{
  static char header[] = "Appearance {";
  VrmlData_ErrorStatus aStatus;
  const VrmlData_Scene& aScene = Scene();
  if (OK (aStatus, aScene.WriteLine (thePrefix, header, GlobalIndent())))
  {
    if (myMaterial.IsNull() == Standard_False)
      aStatus = aScene.WriteNode ("material", myMaterial);
    if (myTexture.IsNull() == Standard_False && OK(aStatus))
      aStatus = aScene.WriteNode ("texture", myTexture);
    if (myTTransform.IsNull() == Standard_False && OK(aStatus))
      aStatus = aScene.WriteNode ("textureTransform", myTTransform);

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_Appearance::IsDefault () const
{
  Standard_Boolean aResult (Standard_True);
  if (myMaterial.IsNull() == Standard_False)
    aResult = myMaterial->IsDefault();
  if (aResult && myTexture.IsNull() == Standard_False)
    aResult = myTexture->IsDefault();
  if (aResult && myTTransform.IsNull() == Standard_False)
    aResult = myTTransform->IsDefault();
  return aResult;
}

//=======================================================================
//function : VrmlData_ImageTexture
//purpose  : Constructor
//=======================================================================

VrmlData_ImageTexture::VrmlData_ImageTexture (const VrmlData_Scene&  theScene,
                                              const char             * theName,
                                              const char             * theURL,
                                              const Standard_Boolean theRepS,
                                              const Standard_Boolean theRepT)
  : VrmlData_Texture (theScene, theName, theRepS, theRepT),
    myURL            (theScene.Allocator())
{
  myURL.Append (theURL ? (Standard_CString)theURL : "");
}

//=======================================================================
//function : VrmlData_ImageTexture::Clone
//purpose  : 
//=======================================================================

Handle(VrmlData_Node) VrmlData_ImageTexture::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_ImageTexture) aResult =
    Handle(VrmlData_ImageTexture)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult =
      new VrmlData_ImageTexture(theOther.IsNull() ? Scene() : theOther->Scene(),
                                Name());
  aResult->myURL = myURL;
  return aResult;
}

//=======================================================================
//function : VrmlData_ImageTexture::Read
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_ImageTexture::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  Standard_Boolean aRepeatS (Standard_True), aRepeatT (Standard_True);
  myURL.Clear();
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {
    if      (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "url"))
      aStatus = ReadMultiString (theBuffer, myURL);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "repeatS"))
      aStatus = ReadBoolean (theBuffer, aRepeatS);
    else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "repeatT"))
      aStatus = ReadBoolean (theBuffer, aRepeatT);
    else
      break;

    if (!OK(aStatus))
      break;
  }
  if (OK(aStatus) && OK(aStatus, readBrace (theBuffer))) {
    SetRepeatS (aRepeatS);
    SetRepeatT (aRepeatT);
  }
  return aStatus;
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

VrmlData_ErrorStatus VrmlData_ImageTexture::Write(const char *thePrefix)  const
{
  VrmlData_ErrorStatus aStatus = VrmlData_StatusOK;
  const VrmlData_Scene& aScene = Scene();
  static char header[] = "ImageTexture {";
  if (aScene.IsDummyWrite() == Standard_False &&
      OK(aStatus, aScene.WriteLine(thePrefix, header, GlobalIndent())))
  {
    TCollection_AsciiString url = "\"";
    url += URL().First();
    url += "\"";

    try {
      aStatus = aScene.WriteLine("url ", url.ToCString());
    }
    catch (...)
    {
      
    }
    aStatus = WriteClosing();
  }
  return aStatus;
}
