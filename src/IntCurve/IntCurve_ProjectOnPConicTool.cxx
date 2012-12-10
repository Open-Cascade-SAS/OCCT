// Created on: 1992-10-13
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


#include <ElCLib.hxx>
#include <IntCurve_ProjectOnPConicTool.ixx>

Standard_Real IntCurve_ProjectOnPConicTool::FindParameter
                                       (const IntCurve_PConic& ThePConic,
					const gp_Pnt2d& P,
					const Standard_Real LowParameter,
					const Standard_Real HighParameter,
					const Standard_Real)  {


  Standard_Real ParamSup,ParamInf,Param=0;
  if(LowParameter>HighParameter) {
    ParamSup=LowParameter;
    ParamInf=HighParameter;
  }
  else {
    ParamInf=LowParameter;
    ParamSup=HighParameter;
  }    

  switch(ThePConic.TypeCurve()) {

  case GeomAbs_Line: 
    Param=ElCLib::LineParameter(ThePConic.Axis2().XAxis(),P);
    break;

  case GeomAbs_Circle:
    Param=ElCLib::CircleParameter(ThePConic.Axis2(),P);
    if(Param<0.0) { Param+=M_PI+M_PI; }
    break;

  case GeomAbs_Ellipse: {
    Param=ElCLib::EllipseParameter(ThePConic.Axis2()
			   ,ThePConic.Param1()
			   ,ThePConic.Param2()
			   ,P);
    if (Param < 0.0) { Param+=M_PI+M_PI; }
    break;
  }

  case GeomAbs_Parabola: {
    Param=ElCLib::ParabolaParameter(ThePConic.Axis2(),P);
    break;
  }
  case GeomAbs_Hyperbola: {
    Param=ElCLib::HyperbolaParameter(ThePConic.Axis2()
			     ,ThePConic.Param1()
			     ,ThePConic.Param2(),P);
    break;
  }
  default:
    break;
  }
  if(ParamInf!=ParamSup) {
    if(Param<ParamInf) return(ParamInf);
    if(Param>ParamSup) return(ParamSup);
  }
  return(Param);
}

    
Standard_Real IntCurve_ProjectOnPConicTool::FindParameter
                                       (const IntCurve_PConic& ThePConic,
					const gp_Pnt2d& P,
					const Standard_Real)  {

  //cout<<"\n\n---- Dans ProjectOnPConicTool::FindParameter  Point : "<<P.X()<<","<<P.Y();

  Standard_Real Param=0;

  switch(ThePConic.TypeCurve()) {

  case GeomAbs_Line: 
    Param=ElCLib::LineParameter(ThePConic.Axis2().XAxis(),P);
    break;
    
  case GeomAbs_Circle:
    Param=ElCLib::CircleParameter(ThePConic.Axis2(),P);
    if(Param<0.0) { Param+=M_PI+M_PI; }
    break;

  case GeomAbs_Ellipse: {
    Param=ElCLib::EllipseParameter(ThePConic.Axis2()
			   ,ThePConic.Param1()
			   ,ThePConic.Param2()
			   ,P);
    if (Param < 0.0) { Param+=M_PI+M_PI; }
    break;
  }

  case GeomAbs_Parabola: {
    Param=ElCLib::ParabolaParameter(ThePConic.Axis2(),P);
    break;
  }
  case GeomAbs_Hyperbola: {
    Param=ElCLib::HyperbolaParameter(ThePConic.Axis2()
			     ,ThePConic.Param1()
			     ,ThePConic.Param2(),P);
    break;
  }
  default:
    break;
  }

  return(Param);
}
