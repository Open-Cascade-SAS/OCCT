// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	-------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Nov 14 1997	Creation

#include <Standard_GUID.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF.hxx>
#include <TDF_GUIDProgIDMap.hxx>

static TDF_GUIDProgIDMap guidprogidmap;

//=======================================================================
//function : LowerID
//purpose  : 
//=======================================================================

const Standard_GUID& TDF::LowestID()
{
  static Standard_GUID lowestID("00000000-0000-0000-0000-000000000000");
  return lowestID;
}


//=======================================================================
//function : UpperID
//purpose  : 
//=======================================================================

const Standard_GUID& TDF::UppestID()
{
  static Standard_GUID uppestID("ffffffff-ffff-ffff-ffff-ffffffffffff");
  return uppestID;
}

//=======================================================================
//function : AddLinkGUIDToProgID
//purpose  : 
//=======================================================================
void TDF::AddLinkGUIDToProgID(const Standard_GUID& ID,const TCollection_ExtendedString& ProgID)
{
  guidprogidmap.UnBind1( ID ); 
  guidprogidmap.UnBind2( ProgID );
  
  guidprogidmap.Bind(ID, ProgID);  
}

//=======================================================================
//function : GUIDFromProgID
//purpose  : 
//=======================================================================
Standard_Boolean TDF::GUIDFromProgID(const TCollection_ExtendedString& ProgID,Standard_GUID& ID)
{
  if( guidprogidmap.IsBound2(ProgID) ) {
    ID = guidprogidmap.Find2( ProgID );
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ProgIDFromGUID
//purpose  : 
//=======================================================================
Standard_Boolean TDF::ProgIDFromGUID(const Standard_GUID& ID,TCollection_ExtendedString& ProgID) 
{
  if( guidprogidmap.IsBound1(ID) ) {
    ProgID = guidprogidmap.Find1( ID );
    return Standard_True;
  }
  return Standard_False;
}

