// File:	StdPrs_ToolPoint.cxx
// Created:	Tue Aug 24 12:45:41 1993
// Author:	Jean-Louis FRENKEL
//		<jlf@stylox>


#include <StdPrs_ToolPoint.ixx>


void StdPrs_ToolPoint::Coord (const Handle(Geom_Point)& aPoint,
			       Standard_Real& X,
			       Standard_Real& Y,
			       Standard_Real& Z) {
  aPoint->Coord(X,Y,Z);
}
