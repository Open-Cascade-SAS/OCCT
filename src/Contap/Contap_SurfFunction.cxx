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

// jag 940616 Tolpetit = 1.e-16

#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Contap_HContTool.hxx>
#include <Contap_SurfFunction.hxx>
#include <Contap_SurfProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <StdFail_UndefinedDerivative.hxx>

Contap_SurfFunction::Contap_SurfFunction ():
  myMean(1.),
  myType(Contap_ContourStd),
  myDir(0.,0.,1.),
  myCosAng(0.), // PI/2 - Angle de depouille
  tol(1.e-6),
  computed(Standard_False),
  derived(Standard_False)
{}

void Contap_SurfFunction::Set(const Handle(Adaptor3d_HSurface)& S)
{
  mySurf = S;
  Standard_Integer i;
  Standard_Integer nbs = Contap_HContTool::NbSamplePoints(S);
  Standard_Real U,V;
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
  computed = Standard_False;
  derived = Standard_False;
}


Standard_Integer Contap_SurfFunction::NbVariables () const
{
  return 2;
}

Standard_Integer Contap_SurfFunction::NbEquations () const
{
  return 1;
}


Standard_Boolean Contap_SurfFunction::Value(const math_Vector& X,
                                            math_Vector& F)
{
  Usol = X(1); Vsol = X(2);
  //  Adaptor3d_HSurfaceTool::D1(mySurf,Usol,Vsol,solpt,d1u,d1v);
  //  gp_Vec norm(d1u.Crossed(d1v));
  gp_Vec norm;
  Contap_SurfProps::Normale(mySurf,Usol,Vsol,solpt,norm);
  switch (myType) {
  case Contap_ContourStd:
    {
      F(1) = valf = (norm.Dot(myDir))/myMean;
    }
    break;
  case Contap_ContourPrs:
    {
      F(1) = valf = (norm.Dot(gp_Vec(myEye,solpt)))/myMean;
    }
    break;
  case Contap_DraftStd:
    {
      F(1) = valf = (norm.Dot(myDir)-myCosAng*norm.Magnitude())/myMean;
    }
    break;
  default:
    {
    }
  }
  computed = Standard_False;
  derived = Standard_False;
  return Standard_True;
}


Standard_Boolean Contap_SurfFunction::Derivatives(const math_Vector& X,
                                                  math_Matrix& Grad)
{
  //  gp_Vec d2u,d2v,d2uv;
  Usol = X(1); Vsol = X(2);
  //  Adaptor3d_HSurfaceTool::D2(mySurf,Usol,Vsol,solpt,d1u,d1v,d2u,d2v,d2uv);

  gp_Vec norm,dnu,dnv;
  Contap_SurfProps::NormAndDn(mySurf,Usol,Vsol,solpt,norm,dnu,dnv);

  switch (myType) {
  case Contap_ContourStd:
    {
      //      Grad(1,1) = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(myDir))/myMean;
      //      Grad(1,2) = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(myDir))/myMean;
      Grad(1,1) = (dnu.Dot(myDir))/myMean;
      Grad(1,2) = (dnv.Dot(myDir))/myMean;
    }
    break;
  case Contap_ContourPrs:
    {
      gp_Vec Ep(myEye,solpt);
      Grad(1,1) = (dnu.Dot(Ep))/myMean;
      Grad(1,2) = (dnv.Dot(Ep))/myMean;
    }
    break;
  case Contap_DraftStd:
    {
      //      gp_Vec norm(d1u.Crossed(d1v).Normalized());
      //      gp_Vec dnorm(d2u.Crossed(d1v) + d1u.Crossed(d2uv));
      //      Grad(1,1) = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      //      dnorm = d2uv.Crossed(d1v) + d1u.Crossed(d2v);
      //      Grad(1,2) = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      norm.Normalize();
      Grad(1,1) = (dnu.Dot(myDir)-myCosAng*dnu.Dot(norm))/myMean;
      Grad(1,2) = (dnv.Dot(myDir)-myCosAng*dnv.Dot(norm))/myMean;
    }
    break;
  case Contap_DraftPrs:
  default:
    {
    }
  }
  Fpu = Grad(1,1); Fpv = Grad(1,2);
  computed = Standard_False;
  derived = Standard_True;
  return Standard_True;
}


Standard_Boolean Contap_SurfFunction::Values (const math_Vector& X,
                                              math_Vector& F,
                                              math_Matrix& Grad)
{
  //  gp_Vec d2u,d2v,d2uv;

  Usol = X(1); Vsol = X(2);
  //  Adaptor3d_HSurfaceTool::D2(mySurf,Usol,Vsol,solpt,d1u,d1v,d2u,d2v,d2uv);
  //  gp_Vec norm(d1u.Crossed(d1v));
  gp_Vec norm,dnu,dnv;
  Contap_SurfProps::NormAndDn(mySurf,Usol,Vsol,solpt,norm,dnu,dnv);

  switch (myType) {

  case Contap_ContourStd:
    {
      F(1)      = (norm.Dot(myDir))/myMean;
      //      Grad(1,1) = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(myDir))/myMean;
      //      Grad(1,2) = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(myDir))/myMean;
      Grad(1,1) = (dnu.Dot(myDir))/myMean;
      Grad(1,2) = (dnv.Dot(myDir))/myMean;
    }
    break;
  case Contap_ContourPrs:
    {
      gp_Vec Ep(myEye,solpt);
      F(1)      = (norm.Dot(Ep))/myMean;
      //      Grad(1,1) = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(Ep))/myMean;
      //      Grad(1,2) = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(Ep))/myMean;
      Grad(1,1) = (dnu.Dot(Ep))/myMean;
      Grad(1,2) = (dnv.Dot(Ep))/myMean;
    }
    break;
  case Contap_DraftStd:
    {
      F(1) = (norm.Dot(myDir)-myCosAng*norm.Magnitude())/myMean;
      norm.Normalize();
      /*
      gp_Vec dnorm(d2u.Crossed(d1v) + d1u.Crossed(d2uv));
      Grad(1,1) = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      dnorm = d2uv.Crossed(d1v) + d1u.Crossed(d2v);
      Grad(1,2) = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
      */
      Grad(1,1) = (dnu.Dot(myDir)-myCosAng*dnu.Dot(norm))/myMean;
      Grad(1,2) = (dnv.Dot(myDir)-myCosAng*dnv.Dot(norm))/myMean;
    }
    break;
  case Contap_DraftPrs:
  default:
    {
    }
  }
  valf = F(1);
  Fpu = Grad(1,1); Fpv = Grad(1,2);
  computed = Standard_False;
  derived = Standard_True;
  return Standard_True;
}


Standard_Boolean Contap_SurfFunction::IsTangent ()
{
  if (!computed) {
    computed = Standard_True;
    if(!derived) {
      //      gp_Vec d2u,d2v,d2uv;
      //      Adaptor3d_HSurfaceTool::D2(mySurf, Usol, Vsol, solpt, d1u, d1v, d2u, d2v, d2uv);
      gp_Vec norm,dnu,dnv;
      Contap_SurfProps::NormAndDn(mySurf,Usol,Vsol,solpt,norm,dnu,dnv);

      switch (myType) {
      case Contap_ContourStd:
        {
          //	  Fpu = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(myDir))/myMean;
          //	  Fpv = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(myDir))/myMean;
          Fpu = (dnu.Dot(myDir))/myMean;
          Fpv = (dnv.Dot(myDir))/myMean;
        }
        break;
      case Contap_ContourPrs:
        {
          gp_Vec Ep(myEye,solpt);
          //	  Fpu = ((d2u.Crossed(d1v) + d1u.Crossed(d2uv)).Dot(Ep))/myMean;
          //	  Fpv = ((d2uv.Crossed(d1v) + d1u.Crossed(d2v)).Dot(Ep))/myMean;
          Fpu = (dnu.Dot(Ep))/myMean;
          Fpv = (dnv.Dot(Ep))/myMean;
        }
        break;
      case Contap_DraftStd:
        {
          /*
          gp_Vec norm(d1u.Crossed(d1v).Normalized());
          gp_Vec dnorm(d2u.Crossed(d1v) + d1u.Crossed(d2uv));
          Fpu = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
          dnorm = d2uv.Crossed(d1v) + d1u.Crossed(d2v);
          Fpv = (dnorm.Dot(myDir)-myCosAng*dnorm.Dot(norm))/myMean;
          */
          norm.Normalize();
          Fpu = (dnu.Dot(myDir)-myCosAng*dnu.Dot(norm))/myMean;
          Fpv = (dnv.Dot(myDir)-myCosAng*dnv.Dot(norm))/myMean;
        }
        break;
      case Contap_DraftPrs:
      default:
        {
        }
      }
      derived = Standard_True;
    }
    tangent = Standard_False;
    Standard_Real D = Sqrt (Fpu * Fpu + Fpv * Fpv);

    if (D <= gp::Resolution()) {
      tangent = Standard_True;
    }
    else {
      d2d = gp_Dir2d(-Fpv,Fpu);
      gp_Vec d1u,d1v;
      Adaptor3d_HSurfaceTool::D1(mySurf, Usol, Vsol, solpt, d1u, d1v); // ajout jag 02.95

      gp_XYZ d3dxyz(-Fpv*d1u.XYZ());
      d3dxyz.Add(Fpu*d1v.XYZ());
      d3d.SetXYZ(d3dxyz);

      //jag 940616    if (d3d.Magnitude() <= Tolpetit) {
      if (d3d.Magnitude() <= tol) {
        tangent = Standard_True;
      }
    }
  }
  return tangent;
}
