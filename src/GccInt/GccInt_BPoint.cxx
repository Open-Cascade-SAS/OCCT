// File:	GccInt_BPoint.cxx
// Created:	Mon Oct  7 15:33:07 1991
// Author:	Remi GILET
//		<reg@phobox>


#include <GccInt_BPoint.ixx>
#include <Standard_DomainError.hxx>
#include <gp_Pnt2d.hxx>

GccInt_BPoint::
   GccInt_BPoint(const gp_Pnt2d& Point) {
   pnt = gp_Pnt2d(Point);
 }

GccInt_IType GccInt_BPoint::
   ArcType() const {
   return GccInt_Pnt;
 }

gp_Pnt2d GccInt_BPoint::
   Point() const { return pnt; }


