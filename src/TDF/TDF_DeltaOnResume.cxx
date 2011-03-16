// File:	TDF_DeltaOnResume.cxx
//      	---------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1998

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Jul  6 1998	Creation



#include <TDF_DeltaOnResume.ixx>
#include <TDF_Label.hxx>


//=======================================================================
//function : TDF_DeltaOnResume
//purpose  : 
//=======================================================================

TDF_DeltaOnResume::TDF_DeltaOnResume
(const Handle(TDF_Attribute)& anAtt)
: TDF_AttributeDelta(anAtt)
{}


//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TDF_DeltaOnResume::Apply() 
{
  // Undo = Forget.
  Label().ForgetAttribute (Attribute());
#ifdef DEB
  cout<<"Forget attribute"<<endl;
#endif
}
