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

#include <XCAFDoc_DimTol.ixx>

#include <TDF_RelocationTable.hxx>


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

