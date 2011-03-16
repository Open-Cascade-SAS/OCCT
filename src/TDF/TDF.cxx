// File:	TDF.cxx
//      	-------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
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

