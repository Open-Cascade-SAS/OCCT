// File:	GccInt_BHyper.cxx
// Created:	Mon Oct  7 15:33:07 1991
// Author:	Remi GILET
//		<reg@phobox>


#include <GccInt_BHyper.ixx>
#include <Standard_NotImplemented.hxx>
#include <gp_Hypr2d.hxx>

GccInt_BHyper::
   GccInt_BHyper(const gp_Hypr2d& Hyper) {
   hyp = gp_Hypr2d(Hyper);
 }

GccInt_IType GccInt_BHyper::
   ArcType() const {
   return GccInt_Hpr;
 }

gp_Hypr2d GccInt_BHyper::
  Hyperbola() const { return hyp; }


