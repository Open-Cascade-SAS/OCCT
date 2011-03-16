// File:	GccInt_BParab.cxx
// Created:	Mon Oct  7 15:33:07 1991
// Author:	Remi GILET
//		<reg@phobox>


#include <GccInt_BParab.ixx>
#include <Standard_DomainError.hxx>
#include <gp_Parab2d.hxx>

GccInt_BParab::
   GccInt_BParab(const gp_Parab2d& Parab) {
   par = gp_Parab2d(Parab);
 }

GccInt_IType GccInt_BParab::
   ArcType() const {
   return GccInt_Par;
 }

gp_Parab2d GccInt_BParab::
  Parabola() const { return par; }

