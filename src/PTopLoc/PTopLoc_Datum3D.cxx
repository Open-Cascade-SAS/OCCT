// File:	PTopLoc_Datum3D.cxx
// Created:	Wed Mar  3 18:19:54 1993
// Author:	Remi LEQUETTE
//		<rle@phobox>


#include <PTopLoc_Datum3D.ixx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : PTopLoc_Datum3D
//purpose  : 
//=======================================================================

PTopLoc_Datum3D::PTopLoc_Datum3D(const gp_Trsf& T) :
       myTrsf(T)
{
  // Update 7-05-96 FMA : validity control is not performed during 
  //                      Persistant instance construction
  // check the validity of the transformation
  //if (Abs(1. - myTrsf.VectorialPart().Determinant()) > 1.e-7)
  //Standard_ConstructionError::Raise
  //("PTopLoc_Datum3D::Non rigid transformation");
}


//=======================================================================
//function : Transformation
//purpose  : 
//=======================================================================

gp_Trsf  PTopLoc_Datum3D::Transformation()const 
{
  return myTrsf;
}


