// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
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

#include <Geom2dInt_TheProjPCurOfGInter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#include <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#include <Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#include <gp_Pnt2d.hxx>
#include <Extrema_POnCurv2d.hxx>

Standard_Real Geom2dInt_TheProjPCurOfGInter::FindParameter(const Adaptor2d_Curve2d& C,
                                                           const gp_Pnt2d&          P,
                                                           const Standard_Real      LowParameter,
                                                           const Standard_Real      HighParameter,
                                                           const Standard_Real)
{
  Standard_Real     theparam, defaultparam;
  Standard_Integer  NbPts   = Geom2dInt_Geom2dCurveTool::NbSamples(C);
  Standard_Real     theEpsX = Geom2dInt_Geom2dCurveTool::EpsX(C);
  Extrema_POnCurv2d POnC;

  Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter::Locate(P,
                                                         C,
                                                         NbPts,
                                                         LowParameter,
                                                         HighParameter,
                                                         POnC);
  defaultparam = POnC.Parameter();
  Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter Loc(P, C, defaultparam, theEpsX);

  if (Loc.IsDone() == Standard_False)
  {
    //-- cout<<"\n Erreur dans LocateExtPC "<<endl;
    theparam = defaultparam;
  }
  else
  {
    if (Loc.IsMin() == Standard_False)
    {
      //-- cout<<"\n Erreur dans LocateExtPC (Maximum trouve) "<<endl;
      theparam = defaultparam;
    }
    else
    {
      theparam = Loc.Point().Parameter();
    }
  }
  return theparam;
}

Standard_Real Geom2dInt_TheProjPCurOfGInter::FindParameter(const Adaptor2d_Curve2d& C,
                                                           const gp_Pnt2d&          P,
                                                           const Standard_Real      Tol)
{

  Standard_Real theParam;
  theParam = FindParameter(C,
                           P,
                           Geom2dInt_Geom2dCurveTool::FirstParameter(C),
                           Geom2dInt_Geom2dCurveTool::LastParameter(C),
                           Tol);
  return theParam;
}
