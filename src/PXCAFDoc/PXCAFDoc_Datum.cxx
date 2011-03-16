// File:      PXCAFDoc_Datum.cxx
// Created:   10.12.08 11:46:02
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <PXCAFDoc_Datum.ixx>

//=======================================================================
//function : PXCAFDoc_Datum
//purpose  : 
//=======================================================================

PXCAFDoc_Datum::PXCAFDoc_Datum ()
{}

//=======================================================================
//function : PXCAFDoc_Datum
//purpose  : 
//=======================================================================

PXCAFDoc_Datum::PXCAFDoc_Datum 
  (const Handle(PCollection_HAsciiString)& theName,
   const Handle(PCollection_HAsciiString)& theDescr,
   const Handle(PCollection_HAsciiString)& theId)
: myName(theName),
  myDescr(theDescr),
  myId(theId)
{}
    
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void PXCAFDoc_Datum::Set (const Handle(PCollection_HAsciiString)& theName,
                          const Handle(PCollection_HAsciiString)& theDescr,
                          const Handle(PCollection_HAsciiString)& theId)
{
  myName = theName;
  myDescr = theDescr;
  myId = theId;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Datum::GetName () const
{
  return myName;
}

//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Datum::GetDescription () const
{
  return myDescr;
}

//=======================================================================
//function : GetIdentification
//purpose  : 
//=======================================================================

Handle(PCollection_HAsciiString) PXCAFDoc_Datum::GetIdentification () const
{
  return myId;
}
