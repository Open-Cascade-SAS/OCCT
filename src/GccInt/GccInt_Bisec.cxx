// File:	GccInt_Bisec.cxx
// Created:	Mon Jan 27 09:44:43 1992
// Author:	Remi GILET
//		<reg@phobox>

#include <GccInt_Bisec.ixx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_NotImplemented.hxx>

gp_Circ2d GccInt_Bisec::
  Circle() const { 
    Standard_NotImplemented::Raise();
    return gp_Circ2d();
    }

gp_Elips2d GccInt_Bisec::
  Ellipse() const { 
    Standard_NotImplemented::Raise();
    return gp_Elips2d();
    }

gp_Hypr2d GccInt_Bisec::
  Hyperbola() const { 
    Standard_NotImplemented::Raise();
    return gp_Hypr2d();
    }

gp_Lin2d GccInt_Bisec::
  Line() const {
    Standard_NotImplemented::Raise();
    return gp_Lin2d();
    }

gp_Parab2d GccInt_Bisec::
  Parabola() const { 
    Standard_NotImplemented::Raise();
    return gp_Parab2d();
    }

gp_Pnt2d GccInt_Bisec::
  Point() const { 
    Standard_NotImplemented::Raise();
    return gp_Pnt2d();
    }

