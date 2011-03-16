// File:	IntCurve_PConic.cxx
// Created:	Mon Mar 30 17:35:40 1992
// Author:	Laurent BUCHARD
//		<lbr@topsn3>


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


