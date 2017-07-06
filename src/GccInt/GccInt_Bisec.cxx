// Created on: 1992-01-27
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <GccInt_Bisec.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GccInt_Bisec,Standard_Transient)

gp_Circ2d GccInt_Bisec::
  Circle() const { 
    throw Standard_NotImplemented();
    }

gp_Elips2d GccInt_Bisec::
  Ellipse() const { 
    throw Standard_NotImplemented();
    }

gp_Hypr2d GccInt_Bisec::
  Hyperbola() const { 
    throw Standard_NotImplemented();
    }

gp_Lin2d GccInt_Bisec::
  Line() const {
    throw Standard_NotImplemented();
    }

gp_Parab2d GccInt_Bisec::
  Parabola() const { 
    throw Standard_NotImplemented();
    }

gp_Pnt2d GccInt_Bisec::
  Point() const { 
    throw Standard_NotImplemented();
    }

