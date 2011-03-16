// File:	GccInt_BCirc.cxx
// Created:	Mon Oct  7 15:33:07 1991
// Author:	Remi GILET
//		<reg@phobox>


#include <GccInt_BCirc.ixx>
#include <Standard_DomainError.hxx>
#include <gp_Circ2d.hxx>

GccInt_BCirc::
   GccInt_BCirc(const gp_Circ2d& Circ) {
   cir = gp_Circ2d(Circ);
 }

GccInt_IType GccInt_BCirc::
   ArcType() const {
   return GccInt_Cir;
 }

gp_Circ2d GccInt_BCirc::
  Circle() const { return cir; }

