// File:	TDF_DeltaOnForget.cxx
//      	---------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Nov  3 1997	Creation



#include <TDF_DeltaOnForget.ixx>
#include <TDF_Label.hxx>


//=======================================================================
//function : TDF_DeltaOnForget
//purpose  : 
//=======================================================================

TDF_DeltaOnForget::TDF_DeltaOnForget
(const Handle(TDF_Attribute)& anAtt)
: TDF_AttributeDelta(anAtt)
{}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDF_DeltaOnForget::Apply() 
{
  // Undo = Resume.
  Label().ResumeAttribute(Attribute());
  Attribute()->mySavedTransaction = 0;
#ifdef DEB
  cout<<"Resume attribute"<<endl;
#endif
}
