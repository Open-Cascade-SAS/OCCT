// Created on: 1992-03-30
// Created by: Laurent BUCHARD
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


#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <IntCurve_PConic.hxx>

IntCurve_PConic::IntCurve_PConic(const IntCurve_PConic& PC) : 
       axe(PC.axe) , prm1(PC.prm1) , 
       prm2(PC.prm2) , TheEpsX(PC.TheEpsX) , TheAccuracy(PC.TheAccuracy) ,
       type(PC.type) { 
}
  

IntCurve_PConic::IntCurve_PConic(const gp_Elips2d& E) :
       axe(E.Axis()) ,
       prm1(E.MajorRadius()) , prm2(E.MinorRadius()) ,  
       TheEpsX(0.00000001) , TheAccuracy(20)       , type(GeomAbs_Ellipse) { 
}

IntCurve_PConic::IntCurve_PConic(const gp_Hypr2d& H) :
       axe(H.Axis()) , 
       prm1(H.MajorRadius()) , prm2(H.MinorRadius()) , 
       TheEpsX(0.00000001) , TheAccuracy(50)       , type(GeomAbs_Hyperbola) {
}

IntCurve_PConic::IntCurve_PConic(const gp_Circ2d& C) :
       axe(C.Axis()) , 
       prm1(C.Radius())      , TheEpsX(0.00000001) , TheAccuracy(20) , 
       type(GeomAbs_Circle) {
}
     
IntCurve_PConic::IntCurve_PConic(const gp_Parab2d& P) :
       axe(P.Axis()) ,
       prm1(P.Focal())       , TheEpsX(0.00000001) , TheAccuracy(20) , 
       type(GeomAbs_Parabola) { 
}
     

IntCurve_PConic::IntCurve_PConic(const gp_Lin2d& L) : 
       axe(gp_Ax22d(L.Position())) , TheEpsX(0.00000001) , 
       TheAccuracy(20)       , type(GeomAbs_Line) { 
}

void IntCurve_PConic::SetEpsX(const Standard_Real epsx) {
  TheEpsX = epsx;
}


void IntCurve_PConic::SetAccuracy(const Standard_Integer n) {
  TheAccuracy = n;
}


