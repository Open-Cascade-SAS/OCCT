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

#include <HLRBRep_ThePolygonOfInterCSurf.hxx>

#include <Bnd_Box.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <HLRBRep_LineTool.hxx>
#include "IntCurveSurface_PolygonUtils.pxx"
#include <Standard_OutOfRange.hxx>

//==================================================================================================

HLRBRep_ThePolygonOfInterCSurf::HLRBRep_ThePolygonOfInterCSurf(const gp_Lin&          Curve,
                                                               const Standard_Integer NbPnt)
    : ThePnts(1, (NbPnt < 5) ? 5 : NbPnt),
      ClosedPolygon(Standard_False)
{
  NbPntIn = (NbPnt < 5) ? 5 : NbPnt;
  Binf    = HLRBRep_LineTool::FirstParameter(Curve);
  Bsup    = HLRBRep_LineTool::LastParameter(Curve);
  Init(Curve);
}

//==================================================================================================

HLRBRep_ThePolygonOfInterCSurf::HLRBRep_ThePolygonOfInterCSurf(const gp_Lin&          Curve,
                                                               const Standard_Real    U1,
                                                               const Standard_Real    U2,
                                                               const Standard_Integer NbPnt)
    : ThePnts(1, (NbPnt < 5) ? 5 : NbPnt),
      ClosedPolygon(Standard_False),
      Binf(U1),
      Bsup(U2)
{
  NbPntIn = (NbPnt < 5) ? 5 : NbPnt;
  Init(Curve);
}

//==================================================================================================

HLRBRep_ThePolygonOfInterCSurf::HLRBRep_ThePolygonOfInterCSurf(const gp_Lin&               Curve,
                                                               const TColStd_Array1OfReal& Upars)
    : ThePnts(1, Upars.Length()),
      ClosedPolygon(Standard_False),
      Binf(Upars(Upars.Lower())),
      Bsup(Upars(Upars.Upper()))
{
  NbPntIn = Upars.Length();
  Init(Curve, Upars);
}

//==================================================================================================

void HLRBRep_ThePolygonOfInterCSurf::Init(const gp_Lin& Curve)
{
  IntCurveSurface_PolygonUtils::InitUniform<gp_Lin, HLRBRep_LineTool>(Curve,
                                                                     Binf,
                                                                     Bsup,
                                                                     NbPntIn,
                                                                     ThePnts,
                                                                     TheBnd,
                                                                     TheDeflection);
  ClosedPolygon = Standard_False;
}

//==================================================================================================

void HLRBRep_ThePolygonOfInterCSurf::Init(const gp_Lin& Curve, const TColStd_Array1OfReal& Upars)
{
  IntCurveSurface_PolygonUtils::InitWithParams<gp_Lin, HLRBRep_LineTool>(Curve,
                                                                        Upars,
                                                                        NbPntIn,
                                                                        ThePnts,
                                                                        TheBnd,
                                                                        TheDeflection,
                                                                        myParams);
  ClosedPolygon = Standard_False;
}

//==================================================================================================

Standard_Real HLRBRep_ThePolygonOfInterCSurf::ApproxParamOnCurve(const Standard_Integer Index,
                                                                 const Standard_Real ParamOnLine) const
{
  return IntCurveSurface_PolygonUtils::ApproxParamOnCurve(Index,
                                                         ParamOnLine,
                                                         Binf,
                                                         Bsup,
                                                         NbPntIn,
                                                         myParams);
}

//==================================================================================================

void HLRBRep_ThePolygonOfInterCSurf::Dump() const
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
