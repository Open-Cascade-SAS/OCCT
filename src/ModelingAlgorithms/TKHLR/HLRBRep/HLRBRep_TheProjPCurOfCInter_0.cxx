// Created on: 1992-10-14
// Created by: Christophe MARION
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

#include <HLRBRep_TheProjPCurOfCInter.hxx>

#include <HLRBRep_CurveTool.hxx>
#include <HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter.hxx>
#include <HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter.hxx>
#include <HLRBRep_PCLocFOfTheLocateExtPCOfTheProjPCurOfCInter.hxx>
#include <gp_Pnt2d.hxx>
#include <Extrema_POnCurv2d.hxx>

Standard_Real HLRBRep_TheProjPCurOfCInter::FindParameter(const Standard_Address& C,
                                                         const gp_Pnt2d&         P,
                                                         const Standard_Real     LowParameter,
                                                         const Standard_Real     HighParameter,
                                                         const Standard_Real)
{
  Standard_Real     theparam, defaultparam;
  Standard_Integer  NbPts   = HLRBRep_CurveTool::NbSamples(C);
  Standard_Real     theEpsX = HLRBRep_CurveTool::EpsX(C);
  Extrema_POnCurv2d POnC;

  HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter::Locate(P,
                                                       C,
                                                       NbPts,
                                                       LowParameter,
                                                       HighParameter,
                                                       POnC);
  defaultparam = POnC.Parameter();
  HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter Loc(P, C, defaultparam, theEpsX);

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

Standard_Real HLRBRep_TheProjPCurOfCInter::FindParameter(const Standard_Address& C,
                                                         const gp_Pnt2d&         P,
                                                         const Standard_Real     Tol)
{

  Standard_Real theParam;
  theParam = FindParameter(C,
                           P,
                           HLRBRep_CurveTool::FirstParameter(C),
                           HLRBRep_CurveTool::LastParameter(C),
                           Tol);
  return theParam;
}
