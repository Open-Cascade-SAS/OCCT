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

#include <XCAFDoc_Datum.ixx>

#include <TDF_RelocationTable.hxx>


//=======================================================================
//function : XCAFDoc_Datum
//purpose  : 
//=======================================================================

XCAFDoc_Datum::XCAFDoc_Datum()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Datum::GetID() 
{
  static Standard_GUID DID("58ed092e-44de-11d8-8776-001083004c77");
  //static Standard_GUID ID("efd212e2-6dfd-11d4-b9c8-0060b0ee281b");
  return DID;
  //return ID;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_Datum) XCAFDoc_Datum::Set(const TDF_Label& label,
                                         const Handle(TCollection_HAsciiString)& aName,
                                         const Handle(TCollection_HAsciiString)& aDescription,
                                         const Handle(TCollection_HAsciiString)& anIdentification) 
{
  Handle(XCAFDoc_Datum) A;
  if (!label.FindAttribute(XCAFDoc_Datum::GetID(), A)) {
    A = new XCAFDoc_Datum();
    label.AddAttribute(A);
  }
  A->Set(aName,aDescription,anIdentification); 
  return A;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::Set(const Handle(TCollection_HAsciiString)& aName,
                        const Handle(TCollection_HAsciiString)& aDescription,
                        const Handle(TCollection_HAsciiString)& anIdentification) 
{
  myName = aName;
  myDescription = aDescription;
  myIdentification = anIdentification;
}


//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Datum::GetName() const
{
  return myName;
}


//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Datum::GetDescription() const
{
  return myDescription;
}


//=======================================================================
//function : GetIdentification
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Datum::GetIdentification() const
{
  return myIdentification;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Datum::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::Restore(const Handle(TDF_Attribute)& With) 
{
  myName = Handle(XCAFDoc_Datum)::DownCast(With)->GetName();
  myDescription = Handle(XCAFDoc_Datum)::DownCast(With)->GetDescription();
  myIdentification = Handle(XCAFDoc_Datum)::DownCast(With)->GetIdentification();
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Datum::NewEmpty() const
{
  return new XCAFDoc_Datum();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Datum::Paste(const Handle(TDF_Attribute)& Into,
                          const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(XCAFDoc_Datum)::DownCast(Into)->Set(myName,myDescription,myIdentification);
}

