// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <XCAFDoc_DimTol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_DimTol,TDF_Attribute)

//=======================================================================
//function : XCAFDoc_DimTol
//purpose  : 
//=======================================================================
XCAFDoc_DimTol::XCAFDoc_DimTol()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DimTol::GetID() 
{
  static Standard_GUID DGTID ("58ed092d-44de-11d8-8776-001083004c77");
  //static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  return DGTID; 
  //return ID;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_DimTol) XCAFDoc_DimTol::Set(const TDF_Label& label,
                                           const Standard_Integer kind,
                                           const Handle(TColStd_HArray1OfReal)& aVal,
                                           const Handle(TCollection_HAsciiString)& aName,
                                           const Handle(TCollection_HAsciiString)& aDescription) 
{
  Handle(XCAFDoc_DimTol) A;
  if (!label.FindAttribute(XCAFDoc_DimTol::GetID(), A)) {
    A = new XCAFDoc_DimTol();
    label.AddAttribute(A);
  }
  A->Set(kind,aVal,aName,aDescription); 
  return A;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void XCAFDoc_DimTol::Set(const Standard_Integer kind,
                         const Handle(TColStd_HArray1OfReal)& aVal,
                         const Handle(TCollection_HAsciiString)& aName,
                         const Handle(TCollection_HAsciiString)& aDescription) 
{
  Backup();
  myKind = kind;
  myVal = aVal;
  myName = aName;
  myDescription = aDescription;
}


//=======================================================================
//function : GetKind
//purpose  : 
//=======================================================================

Standard_Integer XCAFDoc_DimTol::GetKind() const
{
  return myKind;
}


//=======================================================================
//function : GetVal
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) XCAFDoc_DimTol::GetVal() const
{
  return myVal;
}


//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_DimTol::GetName() const
{
  return myName;
}


//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_DimTol::GetDescription() const
{
  return myDescription;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_DimTol::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_DimTol::Restore(const Handle(TDF_Attribute)& With) 
{
  myKind = Handle(XCAFDoc_DimTol)::DownCast(With)->GetKind();
  myVal = Handle(XCAFDoc_DimTol)::DownCast(With)->GetVal();
  myName = Handle(XCAFDoc_DimTol)::DownCast(With)->GetName();
  myDescription = Handle(XCAFDoc_DimTol)::DownCast(With)->GetDescription();
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_DimTol::NewEmpty() const
{
  return new XCAFDoc_DimTol();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_DimTol::Paste(const Handle(TDF_Attribute)& Into,
                           const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(XCAFDoc_DimTol)::DownCast(Into)->Set(myKind,myVal,myName,myDescription);
}

