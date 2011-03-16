// File:	IntCurve_ProjectOnPConicTool.cxx
// Created:	Tue Oct 13 18:45:14 1992
// Author:	Laurent BUCHARD
//		<lbr@sdsun2>

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
    if(Param<0.0) { Param+=PI+PI; }
    break;

  case GeomAbs_Ellipse: {
    Param=ElCLib::EllipseParameter(ThePConic.Axis2()
			   ,ThePConic.Param1()
			   ,ThePConic.Param2()
			   ,P);
    if (Param < 0.0) { Param+=PI+PI; }
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
#ifndef DEB
  default:
    break;
#endif
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
    if(Param<0.0) { Param+=PI+PI; }
    break;

  case GeomAbs_Ellipse: {
    Param=ElCLib::EllipseParameter(ThePConic.Axis2()
			   ,ThePConic.Param1()
			   ,ThePConic.Param2()
			   ,P);
    if (Param < 0.0) { Param+=PI+PI; }
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
#ifndef DEB
  default:
    break;
#endif
  }

  return(Param);
}
