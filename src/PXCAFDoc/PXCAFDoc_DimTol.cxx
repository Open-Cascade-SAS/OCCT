// File:      PXCAFDoc_DimTol.cxx
// Created:   10.12.08 11:46:02
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

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
