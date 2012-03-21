// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <PXCAFDoc_DimTol.ixx>

//=======================================================================
//function : PXCAFDoc_DimTol
//purpose  : 
//=======================================================================

PXCAFDoc_DimTol::PXCAFDoc_DimTol ()
{}

//=======================================================================
//function : PXCAFDoc_DimTol
//purpose  : 
//=======================================================================

PXCAFDoc_DimTol::PXCAFDoc_DimTol 
  (const Standard_Integer theKind,
   const Handle(PColStd_HArray1OfReal)& theVal,
   const Handle(PCollection_HAsciiString)& theName,
   const Handle(PCollection_HAsciiString)& theDescr)
: myKind(theKind),
  myVal(theVal),
  myName(theName),
  myDescr(theDescr)
{}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_DimTol::Set (const Standard_Integer theKind,
                           const Handle(PColStd_HArray1OfReal)& theVal,
                           const Handle(PCollection_HAsciiString)& theName,
                           const Handle(PCollection_HAsciiString)& theDescr)
{
  myKind = theKind;
  myVal = theVal;
  myName = theName;
  myDescr = theDescr;
}

//=======================================================================
//function : GetKind
//purpose  : 
//=======================================================================

Standard_Integer PXCAFDoc_DimTol::GetKind () const
{
  return myKind;
}

//=======================================================================
//function : GetVal
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal) PXCAFDoc_DimTol::GetVal () const
{
  return myVal;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_DimTol::GetName () const
{
  return myName;
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_DimTol::GetDescription () const
{
  return myDescr;
}
