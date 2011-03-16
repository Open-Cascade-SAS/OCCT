// File:	TNaming_DeltaOnRemoval.cxx
// Created:	Fri Dec  5 16:02:11 1997
// Author:	Yves FRICAUD
//		<yfr@claquox.paris1.matra-dtv.fr>


#include <TNaming_DeltaOnRemoval.ixx>
#include <TNaming_DeltaOnModification.hxx>
#include <TDF_DeltaOnRemoval.hxx>

//=======================================================================
//function : TNaming_DeltaOnModification
//purpose  : 
//=======================================================================

TNaming_DeltaOnRemoval::TNaming_DeltaOnRemoval(const Handle(TNaming_NamedShape)& NS)
:TDF_DeltaOnRemoval(NS)
{
  myDelta = new TNaming_DeltaOnModification(NS);
}

//=======================================================================
//function : Apply
//purpose  : 
//=======================================================================

void TNaming_DeltaOnRemoval::Apply()
{
  myDelta->Apply();
}
