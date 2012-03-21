// Created on: 1992-01-27
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

