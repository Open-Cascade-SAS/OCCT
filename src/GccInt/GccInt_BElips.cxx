// File:	GccInt_BElips.cxx
// Created:	Mon Oct  7 15:33:07 1991
// Author:	Remi GILET
//		<reg@phobox>


#include <GccInt_BElips.ixx>
#include <Standard_DomainError.hxx>
#include <gp_Elips2d.hxx>

GccInt_BElips::
   GccInt_BElips(const gp_Elips2d& Ellipse) {
   eli = gp_Elips2d(Ellipse);
 }

GccInt_IType GccInt_BElips::
   ArcType() const {
   return GccInt_Ell;
 }

gp_Elips2d GccInt_BElips::
  Ellipse() const { return eli; }
