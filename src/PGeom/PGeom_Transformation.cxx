// File:	PGeom_Transformation.cxx
// Created:	Thu Mar  4 10:58:59 1993
// Author:	Philippe DAUTRY
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993


#include <PGeom_Transformation.ixx>

//=======================================================================
//function : PGeom_Transformation
//purpose  : 
//=======================================================================

PGeom_Transformation::PGeom_Transformation()
{}


//=======================================================================
//function : PGeom_Transformation
//purpose  : 
//=======================================================================

PGeom_Transformation::PGeom_Transformation(const gp_Trsf& aTrsf) :
       trsf(aTrsf)
{}


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

void  PGeom_Transformation::Trsf(const gp_Trsf& aTrsf)
{ trsf = aTrsf; }


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

gp_Trsf  PGeom_Transformation::Trsf() const 
{ return trsf; }
