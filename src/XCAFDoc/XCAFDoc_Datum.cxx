// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

