// File:	TDF_DeltaOnAddition.cxx
//      	-----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Oct 10 1997	Creation



#include <TDF_DeltaOnAddition.ixx>

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>


//=======================================================================
//function : TDF_DeltaOnAddition
//purpose  : 
//=======================================================================

TDF_DeltaOnAddition::TDF_DeltaOnAddition
(const Handle(TDF_Attribute)& anAtt)
: TDF_AttributeDelta(anAtt)
{}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDF_DeltaOnAddition::Apply() 
{
  Handle(TDF_Attribute) currentAtt;
  if (Label().FindAttribute(ID(),currentAtt)) {
    Label().ForgetAttribute (currentAtt);
  }
}
