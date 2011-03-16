// File:	GccInt_BLine.cxx
// Created:	Mon Oct  7 15:33:07 1991
// Author:	Remi GILET
//		<reg@phobox>


#include <GccInt_BLine.ixx>
#include <Standard_DomainError.hxx>
#include <gp_Lin2d.hxx>

GccInt_BLine::
   GccInt_BLine(const gp_Lin2d& Line) {
   lin = gp_Lin2d(Line);
 }

GccInt_IType GccInt_BLine::
   ArcType() const { return GccInt_Lin; }

gp_Lin2d GccInt_BLine::
  Line() const { return lin; }
