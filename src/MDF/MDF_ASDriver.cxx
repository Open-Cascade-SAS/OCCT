// File:	MDF_ASDriver.cxx
//      	----------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Apr 28 1997	Creation



#include <MDF_ASDriver.ixx>
#include <TCollection_ExtendedString.hxx>
#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : MDF_ASDriver
//purpose  : 
//=======================================================================

MDF_ASDriver::MDF_ASDriver (const Handle(CDM_MessageDriver)& theMsgDriver)
     : myMessageDriver (theMsgDriver) {}

//=======================================================================
//function : WriteMessage
//purpose  : 
//=======================================================================

void MDF_ASDriver::WriteMessage(const TCollection_ExtendedString& theMessage) const {
  myMessageDriver->Write (theMessage.ToExtString());
}
