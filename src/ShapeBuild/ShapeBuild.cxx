// File:	ShapeBuild.cxx
// Created:	Wed Jun 10 16:03:56 1998
// Author:	data exchange team
//		<det@nicebox.nnov.matra-dtv.fr>


#include <ShapeBuild.ixx>

//=======================================================================
//function : PlaneXOY
//purpose  : 
//=======================================================================

Handle(Geom_Plane) ShapeBuild::PlaneXOY()
{
  static Handle(Geom_Plane) xoy;
  if (xoy.IsNull()) xoy = new Geom_Plane (0,0,1,0);
  return xoy;
}

