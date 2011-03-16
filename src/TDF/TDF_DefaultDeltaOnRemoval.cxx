// File:	TDF_DefaultDeltaOnRemoval.cxx
//      	-----------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Oct 10 1997	Creation



#include <TDF_DefaultDeltaOnRemoval.ixx>
#include <TDF_Label.hxx>


//=======================================================================
//function : TDF_DefaultDeltaOnRemoval
//purpose  : 
//=======================================================================

TDF_DefaultDeltaOnRemoval::TDF_DefaultDeltaOnRemoval
(const Handle(TDF_Attribute)& anAttribute)
: TDF_DeltaOnRemoval(anAttribute)
{}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDF_DefaultDeltaOnRemoval::Apply() 
{
  Label().AddAttribute(Attribute());
}
