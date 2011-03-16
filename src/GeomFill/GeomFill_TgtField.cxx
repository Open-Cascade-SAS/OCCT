// File:	GeomFill_TgtField.cxx
// Created:	Mon Dec  4 14:45:37 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <GeomFill_TgtField.ixx>

//=======================================================================
//function : IsScalable
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_TgtField::IsScalable() const 
{
  return 0;
}

//=======================================================================
//function : Scale
//purpose  : 
//=======================================================================

void GeomFill_TgtField::Scale(const Handle(Law_BSpline)& )
{
}


