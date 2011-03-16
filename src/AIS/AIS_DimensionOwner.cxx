// File:	AIS_LengthDimension.cdl
// Created:	Tue Dec  5 17:04:35 1996
// Author:	Odile Olivier
//              <ODL>


#include <AIS_DimensionOwner.ixx>


//-----------------------------------------------------------------------
// function : create
// purpose  :
//-----------------------------------------------------------------------

AIS_DimensionOwner::AIS_DimensionOwner(const Handle(SelectMgr_SelectableObject)& aSO,
				       const Standard_Integer aPriority)
:SelectMgr_EntityOwner(aSO,aPriority)
{
}
