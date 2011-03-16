// File:	TDF_ClosureMode.cxx
//      	-------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	May 26 1997	Creation



#include <TDF_ClosureMode.ixx>


//=======================================================================
//function : TDF_ClosureMode
//purpose  : 
//=======================================================================

TDF_ClosureMode::TDF_ClosureMode(const Standard_Boolean aMode) :
myFlags(aMode ? ~0 : 0)
{}
