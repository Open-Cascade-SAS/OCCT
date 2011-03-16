// File:	TDF_DefaultDeltaOnModification.cxx
//      	----------------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Oct 10 1997	Creation



#include <TDF_DefaultDeltaOnModification.ixx>

#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>


//=======================================================================
//function : TDF_DefaultDeltaOnModification
//purpose  : 
//=======================================================================

TDF_DefaultDeltaOnModification::TDF_DefaultDeltaOnModification
(const Handle(TDF_Attribute)& anAttribute)
: TDF_DeltaOnModification(anAttribute)
{}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDF_DefaultDeltaOnModification::Apply() 
{
  const Handle(TDF_Attribute)& savAtt = Attribute();
  Handle(TDF_Attribute) refAtt;
  if (Label().FindAttribute(savAtt->ID(),refAtt))
    refAtt->DeltaOnModification(this);
}

