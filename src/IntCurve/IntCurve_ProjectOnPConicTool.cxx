// Created on: 1992-10-13
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


#include <ElCLib.hxx>
#include <gp_Pnt2d.hxx>
#include <IntCurve_PConic.hxx>
#include <IntCurve_ProjectOnPConicTool.hxx>

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
