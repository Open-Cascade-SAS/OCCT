// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <XCAFNoteObjects_NoteObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFNoteObjects_NoteObject, Standard_Transient)

//=================================================================================================

XCAFNoteObjects_NoteObject::XCAFNoteObjects_NoteObject()
    : myHasPlane(Standard_False),
      myHasPnt(Standard_False),
      myHasPntTxt(Standard_False)
{
}

//=================================================================================================

XCAFNoteObjects_NoteObject::XCAFNoteObjects_NoteObject(
  const Handle(XCAFNoteObjects_NoteObject)& theObj)
    : myPlane(theObj->myPlane),
      myPnt(theObj->myPnt),
      myPntTxt(theObj->myPntTxt),
      myPresentation(theObj->myPresentation),
      myHasPlane(theObj->myHasPlane),
      myHasPnt(theObj->myHasPnt),
      myHasPntTxt(theObj->myHasPntTxt)
{
}

//=================================================================================================

void XCAFNoteObjects_NoteObject::SetPlane(const gp_Ax2& thePlane)
{
  myPlane    = thePlane;
  myHasPlane = Standard_True;
}

//=================================================================================================

void XCAFNoteObjects_NoteObject::SetPoint(const gp_Pnt& thePnt)
{
  myPnt    = thePnt;
  myHasPnt = Standard_True;
}

//=================================================================================================

void XCAFNoteObjects_NoteObject::SetPointText(const gp_Pnt& thePnt)
{
  myPntTxt    = thePnt;
  myHasPntTxt = Standard_True;
}

//=================================================================================================

void XCAFNoteObjects_NoteObject::SetPresentation(const TopoDS_Shape& thePresentation)
{
  myPresentation = thePresentation;
}

//=================================================================================================

void XCAFNoteObjects_NoteObject::Reset()
{
  myHasPlane  = Standard_False;
  myHasPnt    = Standard_False;
  myHasPntTxt = Standard_False;
  myPresentation.Nullify();
}
