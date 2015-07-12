// Created on: 1993-06-03
// Created by: Jacques GOUSSARD
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Contap_ArcFunction.hxx>
#include <Contap_HContTool.hxx>
#include <Contap_HCurve2dTool.hxx>
#include <Contap_SurfProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <IntSurf_Quadric.hxx>

Contap_ArcFunction::Contap_ArcFunction ():
myMean(1.),
myType(Contap_ContourStd),
myDir(0.,0.,1.)
{}


void Contap_ArcFunction::Set(const Handle(Adaptor3d_HSurface)& S)
{
  mySurf = S;
  Standard_Integer i;
  Standard_Integer nbs = Contap_HContTool::NbSamplePoints(S);
  Standard_Real U,V;
  //  gp_Vec d1u,d1v;
  gp_Vec norm;
  if (nbs > 0) {
    myMean = 0.;
    for (i = 1; i <= nbs; i++) {
      Contap_HContTool::SamplePoint(S,i,U,V);
      //      Adaptor3d_HSurfaceTool::D1(S,U,V,solpt,d1u,d1v);
      //      myMean = myMean + d1u.Crossed(d1v).Magnitude();
      Contap_SurfProps::Normale(S,U,V,solpt,norm);
      myMean = myMean + norm.Magnitude();
    }
    myMean = myMean / ((Standard_Real)nbs);
  }
}


Standard_Boolean Contap_ArcFunction::Value (const Standard_Real U,
                                            Standard_Real& F)
{
  //gp_Vec d1u,d1v;
  gp_Pnt2d pt2d(Contap_HCurve2dTool::Value(myArc,U));
  //  Adaptor3d_HSurfaceTool::D1(mySurf,pt2d.X(),pt2d.Y(),solpt,d1u,d1v);
  //  gp_Vec norm(d1u.Crossed(d1v));
  gp_Vec norm;
  Contap_SurfProps::Normale(mySurf,pt2d.X(),pt2d.Y(),solpt,norm);

  switch (myType) {
  case Contap_ContourStd:
    {
      F = (norm.Dot(myDir))/myMean;
    }
    break;
  case Contap_ContourPrs:
    {
      F = (norm.Dot(gp_Vec(myEye,solpt)))/myMean;
    }
    break;
  case Contap_DraftStd:
    {
      F = (norm.Dot(myDir)-myCosAng*norm.Magnitude())/myMean;
    }
    break;
  case Contap_DraftPrs:
  default:
    {
    }
  }
  return Standard_True;
}


Standard_Boolean Contap_ArcFunction::Derivative (const Standard_Real U,
                                                 Standard_Real& D)
{
  gp_Pnt2d pt2d;
  gp_Vec2d d2d;
  Standard_Real dfu =0.,dfv =0.;
  //  gp_Vec d1u,d1v,d2u,d2v,d2uv;
  Contap_HCurve2dTool::D1(myArc,U,pt2d,d2d);
  //  Adaptor3d_HSurfaceTool::D2(mySurf,pt2d.X(),pt2d.Y(),solpt,d1u,d1v,d2u,d2v,d2uv);
  gp_Vec norm,dnu,dnv;
  Contap_SurfProps::NormAndDn(mySurf,pt2d.X(),pt2d.Y(),solpt,norm,dnu,dnv);

  switch (myType) {
  case Contap_ContourStd:
    {
      //      dfu = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(myDir))/myMean;
      //      dfv = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(myDir))/myMean;
      dfu = (dnu.Dot(myDir))/myMean;
      dfv = (dnv.Dot(myDir))/myMean;
    }
    break;
  case Contap_ContourPrs:
    {
      gp_Vec Ep(myEye,solpt);
      //      dfu = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(Ep))/myMean;
      //      dfv = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(Ep))/myMean;
      dfu = (dnu.Dot(Ep))/myMean;
      dfv = (dnv.Dot(Ep))/myMean;
    }
    break;
  case Contap_DraftStd:
    {
      /*
      gp_Vec norm(d1u.Crossed(d1v).Normalized());
      gp_Vec dnorm(d2u.Crossed(d1v) + d1u.Crossed(d2uv));
      dfu = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      dnorm = d2uv.Crossed(d1v) + d1u.Crossed(d2v);
      dfv = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      */
      norm.Normalized();
      dfu = (dnu.Dot(myDir)-myCosAng*dnu.Dot(norm))/myMean;
      dfv = (dnv.Dot(myDir)-myCosAng*dnv.Dot(norm))/myMean;
    }
    break;
  case Contap_DraftPrs:
  default:
    {
    }
  }
  D = d2d.X()*dfu + d2d.Y()*dfv;
  return Standard_True;
}

Standard_Boolean Contap_ArcFunction::Values (const Standard_Real U,
                                             Standard_Real& F,
                                             Standard_Real& D)
{
  gp_Pnt2d pt2d;
  gp_Vec2d d2d;
  Standard_Real dfu =0.,dfv =0.;
  // gp_Vec d1u,d1v,d2u,d2v,d2uv;
  Contap_HCurve2dTool::D1(myArc,U,pt2d,d2d);
  //  Adaptor3d_HSurfaceTool::D2(mySurf,pt2d.X(),pt2d.Y(),solpt,d1u,d1v,d2u,d2v,d2uv);
  //  gp_Vec norm(d1u.Crossed(d1v));
  gp_Vec norm,dnu,dnv;
  Contap_SurfProps::NormAndDn(mySurf,pt2d.X(),pt2d.Y(),solpt,norm,dnu,dnv);

  switch (myType) {
  case Contap_ContourStd:
    {
      F   = (norm.Dot(myDir))/myMean;
      //      dfu = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(myDir))/myMean;
      //      dfv = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(myDir))/myMean;
      dfu = (dnu.Dot(myDir))/myMean;
      dfv = (dnv.Dot(myDir))/myMean;
    }
    break;
  case Contap_ContourPrs:
    {
      gp_Vec Ep(myEye,solpt);
      F   = (norm.Dot(Ep))/myMean;
      //      dfu = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(Ep))/myMean;
      //      dfv = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(Ep))/myMean;
      dfu = (dnu.Dot(Ep))/myMean;
      dfv = (dnv.Dot(Ep))/myMean;
    }
    break;
  case Contap_DraftStd:
    {
      F = (norm.Dot(myDir)-myCosAng*norm.Magnitude())/myMean;
      norm.Normalize();
      /*
      gp_Vec dnorm(d2u.Crossed(d1v) + d1u.Crossed(d2uv));
      dfu = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      dnorm = d2uv.Crossed(d1v) + d1u.Crossed(d2v);
      dfv = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      */
      dfu = (dnu.Dot(myDir)-myCosAng*dnu.Dot(norm))/myMean;
      dfv = (dnv.Dot(myDir)-myCosAng*dnv.Dot(norm))/myMean;
    }
    break;
  case Contap_DraftPrs:
  default:
    {
    }
  }

  D = d2d.X()*dfu + d2d.Y()*dfv;
  return Standard_True;
}

Standard_Integer Contap_ArcFunction::GetStateNumber ()
{
  seqpt.Append(solpt);
  return seqpt.Length();
}

Standard_Integer Contap_ArcFunction::NbSamples () const
{
  return Max(Max(Contap_HContTool::NbSamplesU(mySurf,0.,0.),
    Contap_HContTool::NbSamplesV(mySurf,0.,0.)),
    Contap_HContTool::NbSamplesOnArc(myArc));
}

//modified by NIZNHY-PKV Thu Mar 29 16:53:07 2001f
//=======================================================================
//function : Quadric
//purpose  : returns empty Quadric
//=======================================================================
const IntSurf_Quadric& Contap_ArcFunction::Quadric() const 
{ 
  return(myQuad);
}
//modified by NIZNHY-PKV Thu Mar 29 16:53:09 2001t
