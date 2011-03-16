// File:	HLRTopoBRep_VData.cxx
// Created:	Tue Feb  7 18:13:05 1995
// Author:	Christophe MARION
//		<cma@ecolox>

#include <HLRTopoBRep_VData.ixx>

//=======================================================================
//function : HLRTopoBRep_VData
//purpose  : 
//=======================================================================

HLRTopoBRep_VData::HLRTopoBRep_VData(const Standard_Real P, 
				     const TopoDS_Shape& V) :
				     myParameter(P),myVertex(V)
{}

