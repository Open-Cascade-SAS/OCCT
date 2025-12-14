// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <IntCurveSurface_ThePolygonOfHInter.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include "IntCurveSurface_PolygonUtils.pxx"
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <Standard_OutOfRange.hxx>

//==================================================================================================

IntCurveSurface_ThePolygonOfHInter::IntCurveSurface_ThePolygonOfHInter(
  const Handle(Adaptor3d_Curve)& Curve,
  const Standard_Integer         NbPnt)
    : ThePnts(1, (NbPnt < 5) ? 5 : NbPnt),
      ClosedPolygon(Standard_False)
{
  NbPntIn = (NbPnt < 5) ? 5 : NbPnt;
  Binf    = IntCurveSurface_TheHCurveTool::FirstParameter(Curve);
  Bsup    = IntCurveSurface_TheHCurveTool::LastParameter(Curve);
  Init(Curve);
}

//==================================================================================================

IntCurveSurface_ThePolygonOfHInter::IntCurveSurface_ThePolygonOfHInter(
  const Handle(Adaptor3d_Curve)& Curve,
  const Standard_Real            U1,
  const Standard_Real            U2,
  const Standard_Integer         NbPnt)
    : ThePnts(1, (NbPnt < 5) ? 5 : NbPnt),
      ClosedPolygon(Standard_False),
      Binf(U1),
      Bsup(U2)
{
  NbPntIn = (NbPnt < 5) ? 5 : NbPnt;
  Init(Curve);
}

//==================================================================================================

IntCurveSurface_ThePolygonOfHInter::IntCurveSurface_ThePolygonOfHInter(
  const Handle(Adaptor3d_Curve)& Curve,
  const TColStd_Array1OfReal&    Upars)
    : ThePnts(1, Upars.Length()),
      ClosedPolygon(Standard_False),
      Binf(Upars(Upars.Lower())),
      Bsup(Upars(Upars.Upper()))
{
  NbPntIn = Upars.Length();
  Init(Curve, Upars);
}

//==================================================================================================

void IntCurveSurface_ThePolygonOfHInter::Init(const Handle(Adaptor3d_Curve)& Curve)
{
  IntCurveSurface_PolygonUtils::InitUniform<Handle(Adaptor3d_Curve), IntCurveSurface_TheHCurveTool>(
    Curve,
    Binf,
    Bsup,
    NbPntIn,
    ThePnts,
    TheBnd,
    TheDeflection);
  ClosedPolygon = Standard_False;
}

//==================================================================================================

void IntCurveSurface_ThePolygonOfHInter::Init(const Handle(Adaptor3d_Curve)& Curve,
                                              const TColStd_Array1OfReal&    Upars)
{
  IntCurveSurface_PolygonUtils::
    InitWithParams<Handle(Adaptor3d_Curve), IntCurveSurface_TheHCurveTool>(Curve,
                                                                          Upars,
                                                                          NbPntIn,
                                                                          ThePnts,
                                                                          TheBnd,
                                                                          TheDeflection,
                                                                          myParams);
  ClosedPolygon = Standard_False;
}

//==================================================================================================

Standard_Real IntCurveSurface_ThePolygonOfHInter::ApproxParamOnCurve(
  const Standard_Integer Index,
  const Standard_Real    ParamOnLine) const
{
  return IntCurveSurface_PolygonUtils::ApproxParamOnCurve(Index,
                                                         ParamOnLine,
                                                         Binf,
                                                         Bsup,
                                                         NbPntIn,
                                                         myParams);
}

//==================================================================================================

void IntCurveSurface_ThePolygonOfHInter::Dump() const
{
#if 0
  static Standard_Integer Compteur=0;
  char tamp[100];
  Compteur++;
  Sprintf(tamp,"Poly%d",Compteur);
  std::cout<<"  @@@@@@@@@@@ F i c h i e r   :   "<<tamp<<" @@@@@@@@@@"<<std::endl;
  FILE *fp;
  fp=fopen(tamp,"w");
  if(fp==NULL) {
    std::cout<<"PolyGonGen::Erreur en Ouverture Fichier"<<tamp<<std::endl;
    return;
  }
  fprintf(fp,"\n#Discretisation de : %f ---> %f \n",Binf,Bsup);
  fprintf(fp,"\npol %d %d %f",Compteur,NbPntIn,TheDeflection);
  gp_Pnt p1,p2;
  for (Standard_Integer iObje=1; iObje<=NbSegments(); iObje++) {
    p1=BeginOfSeg(iObje);
    fprintf(fp,"\npnt %d %f %f",Compteur,p1.X(),p1.Y());
  }
  p1=EndOfSeg(NbSegments());
  fprintf(fp,"\npnt %d %f %f",Compteur,p1.X(),p1.Y());
  fprintf(fp,"\ndispol %d\n#\n",Compteur);
  fclose(fp);
#endif
}
