// File:	PGeom2d_Transformation.cxx
// Created:	Thu Apr 15 11:09:21 1993
// Author:	Philippe DAUTRY
//		<fid@mastox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_Transformation.ixx>

//=======================================================================
//function : PGeom2d_Transformation
//purpose  : 
//=======================================================================

PGeom2d_Transformation::PGeom2d_Transformation()
{}


//=======================================================================
//function : PGeom2d_Transformation
//purpose  : 
//=======================================================================

PGeom2d_Transformation::PGeom2d_Transformation(const gp_Trsf2d& aTrsf) :
       trsf(aTrsf)
{}


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

void  PGeom2d_Transformation::Trsf(const gp_Trsf2d& aTrsf)
{ trsf = aTrsf; }


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

gp_Trsf2d PGeom2d_Transformation::Trsf() const 
{ return trsf; }
