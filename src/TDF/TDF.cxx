// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	-------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Nov 14 1997	Creation



#include <TDF.ixx>
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

