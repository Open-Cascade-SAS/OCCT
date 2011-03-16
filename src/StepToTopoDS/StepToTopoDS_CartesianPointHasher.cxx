// File:	StepToTopoDS_CartesianPointHasher.cxx
// Created:	Mon Aug 30 12:26:11 1993
// Author:	Martine LANGLOIS
//		<mla@nonox>


#include <StepToTopoDS_CartesianPointHasher.ixx>
#include <StepGeom_CartesianPoint.hxx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer StepToTopoDS_CartesianPointHasher::HashCode
  (const Handle(StepGeom_CartesianPoint)& K, const Standard_Integer Upper)
{
  return ::HashCode(K,Upper);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_CartesianPointHasher::IsEqual
  (const Handle(StepGeom_CartesianPoint)& K1,
   const Handle(StepGeom_CartesianPoint)& K2)
{
  return (K1 == K2);
}
