// File:	TDF_DeltaOnModification.cxx
//      	---------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Oct 10 1997	Creation



#include <TDF_DeltaOnModification.ixx>


//=======================================================================
//function : TDF_DeltaOnModification
//purpose  : 
//=======================================================================

TDF_DeltaOnModification::TDF_DeltaOnModification
(const Handle(TDF_Attribute)& anAttribute)
: TDF_AttributeDelta(anAttribute)
{}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDF_DeltaOnModification::Apply() 
{ Attribute()->DeltaOnModification(this); }
