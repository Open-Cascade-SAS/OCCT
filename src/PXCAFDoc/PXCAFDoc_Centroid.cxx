// File:	PXCAFDoc_Centroid.cxx
// Created:	Fri Sep  8 18:08:26 2000
// Author:	data exchange team
//		<det@nordox.nnov.matra-dtv.fr>


#include <PXCAFDoc_Centroid.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Centroid::PXCAFDoc_Centroid()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Centroid::PXCAFDoc_Centroid(const gp_Pnt& pnt):
       myPCentroid(pnt)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void PXCAFDoc_Centroid::Set(const gp_Pnt& pnt) 
{
  myPCentroid = pnt;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

 gp_Pnt PXCAFDoc_Centroid::Get() const
{
  return myPCentroid;
}
