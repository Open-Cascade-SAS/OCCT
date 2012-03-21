// Created on: 1992-03-30
// Created by: Laurent BUCHARD
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



#include <IntCurve_PConic.ixx>



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


