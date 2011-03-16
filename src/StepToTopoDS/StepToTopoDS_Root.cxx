// File:	StepToTopoDS_Root.cxx
// Created:	Thu Apr  8 17:45:20 1999
// Author:	data exchange team
//		<det@doomox.nnov.matra-dtv.fr>
//    gka 13.04.99 S4136: add myPrecision and myMaxTol fields


#include <StepToTopoDS_Root.ixx>
#include <Precision.hxx>

//=======================================================================
//function : StepToTopoDS_Root
//purpose  : 
//=======================================================================

StepToTopoDS_Root::StepToTopoDS_Root () : done(Standard_False) 
{
  myPrecision = myMaxTol = Precision::Confusion();
}

