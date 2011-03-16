// File:	TDF_DeltaOnRemoval.cxx
//      	----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Oct 10 1997	Creation



#include <TDF_DeltaOnRemoval.ixx>


//=======================================================================
//function : TDF_DeltaOnRemoval
//purpose  : 
//=======================================================================

TDF_DeltaOnRemoval::TDF_DeltaOnRemoval(const Handle(TDF_Attribute)& anAtt)
: TDF_AttributeDelta(anAtt)
{}
