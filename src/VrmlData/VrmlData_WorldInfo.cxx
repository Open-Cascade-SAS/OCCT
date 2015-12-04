// Created on: 2007-08-01
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <VrmlData_WorldInfo.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_InBuffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlData_WorldInfo,VrmlData_Node)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)
#endif


//=======================================================================
//function : VrmlData_WorldInfo::VrmlData_WorldInfo
//purpose  : Constructor
//=======================================================================

VrmlData_WorldInfo::VrmlData_WorldInfo (const VrmlData_Scene&  theScene,
                                        const char             * theName,
                                        const char             * theTitle)
  : VrmlData_Node (theScene, theName),
    myInfo        (theScene.Allocator())
{
  SetTitle (theTitle);
}

//=======================================================================
//function : SetTitle
//purpose  : Set or modify the title.
//=======================================================================

void VrmlData_WorldInfo::SetTitle (const char * theString)
{
  if (theString == 0L)
    myTitle = 0L;
  else {
    const size_t len = strlen (theString) + 1;
    if (len == 1)
      myTitle = 0L;
    else {
      myTitle = static_cast <const char *>(Scene().Allocator()->Allocate(len));
      memcpy (const_cast<char *> (myTitle), theString, len);
    }
  }
}

//=======================================================================
//function : AddInfo
//purpose  : Add a string to the list of info strings.
//=======================================================================

void VrmlData_WorldInfo::AddInfo (const char * theString)
{
  if (theString != 0L)
    if (* theString != '\0') {
      const size_t len = strlen (theString) + 1;
      char * aStr = static_cast <char *>(Scene().Allocator()->Allocate(len));
      memcpy (aStr, theString, len);
      myInfo.Append (aStr);
    }
}

//=======================================================================
//function : Clone
//purpose  : Create a copy of this node
//=======================================================================

Handle(VrmlData_Node) VrmlData_WorldInfo::Clone
                                (const Handle(VrmlData_Node)& theOther) const
{
  Handle(VrmlData_WorldInfo) aResult =
    Handle(VrmlData_WorldInfo)::DownCast (VrmlData_Node::Clone(theOther));
  if (aResult.IsNull())
    aResult =
      new VrmlData_WorldInfo (theOther.IsNull() ? Scene() : theOther->Scene(),
                             Name());

  if (&aResult->Scene() == &Scene()) {
    aResult->myTitle = myTitle;
    aResult->myInfo  = myInfo;
  } else {
    aResult->SetTitle (myTitle);
    NCollection_List <const char *>::Iterator anIter (myInfo);
    for (; anIter.More(); anIter.Next())
      aResult->AddInfo (anIter.Value());
  }
  return aResult;
}

//=======================================================================
//function : Read
//purpose  : Read the Node from input stream.
//=======================================================================

VrmlData_ErrorStatus VrmlData_WorldInfo::Read (VrmlData_InBuffer& theBuffer)
{
  VrmlData_ErrorStatus aStatus;
  while (OK(aStatus, VrmlData_Scene::ReadLine(theBuffer))) {

    if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "title")) {
      TCollection_AsciiString aTitleString;
      if (OK (aStatus, ReadString (theBuffer, aTitleString)))
        SetTitle (aTitleString.ToCString());

    } else if (VRMLDATA_LCOMPARE (theBuffer.LinePtr, "info")) {
      NCollection_List<TCollection_AsciiString> lstInfo;
      if (OK (aStatus, ReadMultiString (theBuffer, lstInfo))) {
        NCollection_List<TCollection_AsciiString>::Iterator anIter (lstInfo);
        for (; anIter.More(); anIter.Next())
          AddInfo (anIter.Value().ToCString());
      }
    } else
      break;
  }

  // Read the terminating (closing) brace
  if (OK(aStatus))
    aStatus = readBrace (theBuffer);
  return aStatus;
}

//=======================================================================
//function : Write
//purpose  : Write the Node to the Scene output.
//=======================================================================

VrmlData_ErrorStatus VrmlData_WorldInfo::Write (const char * thePrefix) const
{
  VrmlData_ErrorStatus aStatus (VrmlData_StatusOK);
  const VrmlData_Scene& aScene = Scene();
  static char header[] = "WorldInfo {";
  if (aScene.IsDummyWrite() == Standard_False &&
      OK (aStatus, aScene.WriteLine (thePrefix, header, GlobalIndent())))
  {
    char buf[4096];
    if (myTitle) {
      Sprintf (buf, "title \"%s\"", myTitle);
      aStatus = aScene.WriteLine (buf);
    }

    if (myInfo.IsEmpty() == Standard_False && OK(aStatus)) {
      if (OK (aStatus, aScene.WriteLine ("info [", 0L, GlobalIndent()))) {
        NCollection_List<const char *>::Iterator anIter (myInfo);
        while (anIter.More()) {
          Sprintf (buf, "\"%s\"", anIter.Value());
          anIter.Next();
          if (anIter.More())
            aStatus = aScene.WriteLine (buf, ",");
          else
            aStatus = aScene.WriteLine (buf);
        }
      }
      aStatus = aScene.WriteLine ("]", 0L, -GlobalIndent());
    }

    aStatus = WriteClosing();
  }
  return aStatus;
}

//=======================================================================
//function : IsDefault
//purpose  : 
//=======================================================================

Standard_Boolean VrmlData_WorldInfo::IsDefault() const
{
  return (myTitle == 0L && myInfo.IsEmpty());
}
