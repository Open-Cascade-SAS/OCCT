// File:	GeomFill_Filling.cxx
// Created:	Tue Sep 28 17:42:54 1993
// Author:	Bruno DUMORTIER
//		<dub@sdsun1>

#include <GeomFill_Filling.ixx>

#include <gp_Pnt.hxx>

//=======================================================================
//function : GeomFill_Filling
//purpose  : 
//=======================================================================

GeomFill_Filling::GeomFill_Filling()
{
}


//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomFill_Filling::NbUPoles() const
{
  return myPoles->ColLength();
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer GeomFill_Filling::NbVPoles() const
{
  return myPoles->RowLength();
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  GeomFill_Filling::Poles(TColgp_Array2OfPnt& Poles)const 
{
  Poles = myPoles->ChangeArray2();
}

//=======================================================================
//function : isRational
//purpose  : 
//=======================================================================

Standard_Boolean  GeomFill_Filling::isRational()const 
{
  return IsRational;
}


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  GeomFill_Filling::Weights(TColStd_Array2OfReal& Weights)const 
{
  Weights = myWeights->ChangeArray2();
}


