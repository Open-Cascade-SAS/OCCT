// Created on: 1993-03-10
// Created by: JCV
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

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomEval_RepSurfaceDesc.hxx>
#include <GeomEval_RepUtils.pxx>
#include "Geom_ExtrusionUtils.pxx"
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_SurfaceOfLinearExtrusion, Geom_SweptSurface)

#define POLES (poles->Array2())
#define WEIGHTS (weights->Array2())
#define UKNOTS (uknots->Array1())
#define VKNOTS (vknots->Array1())
#define UFKNOTS (ufknots->Array1())
#define VFKNOTS (vfknots->Array1())
#define FMULTS (BSplCLib::NoMults())

typedef Geom_SurfaceOfLinearExtrusion SurfaceOfLinearExtrusion;
typedef Geom_Curve                    Curve;
typedef gp_Dir                        Dir;
typedef gp_Pnt                        Pnt;
typedef gp_Trsf                       Trsf;
typedef gp_Vec                        Vec;
typedef gp_XYZ                        XYZ;

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::SetEvalRepresentation(
  const occ::handle<GeomEval_RepSurfaceDesc::Base>& theDesc)
{
  GeomEval_RepUtils::ValidateSurfaceDesc(theDesc, this);
  myEvalRep = theDesc;
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_SurfaceOfLinearExtrusion::Copy() const
{
  occ::handle<Geom_SurfaceOfLinearExtrusion> aCopy =
    new SurfaceOfLinearExtrusion(basisCurve, direction);
  aCopy->myEvalRep = GeomEval_RepUtils::CloneSurfaceDesc(myEvalRep);
  return aCopy;
}

//=================================================================================================

Geom_SurfaceOfLinearExtrusion::Geom_SurfaceOfLinearExtrusion(const occ::handle<Geom_Curve>& C,
                                                             const Dir&                     V)
{
  basisCurve = occ::down_cast<Geom_Curve>(C->Copy());
  direction  = V;
  smooth     = C->Continuity();
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::UReverse()
{
  ClearEvalRepresentation();
  basisCurve->Reverse();
}

//=================================================================================================

double Geom_SurfaceOfLinearExtrusion::UReversedParameter(const double U) const
{

  return basisCurve->ReversedParameter(U);
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::VReverse()
{
  ClearEvalRepresentation();
  direction.Reverse();
}

//=================================================================================================

double Geom_SurfaceOfLinearExtrusion::VReversedParameter(const double V) const
{

  return (-V);
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::SetDirection(const Dir& V)
{
  ClearEvalRepresentation();
  direction = V;
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::SetBasisCurve(const occ::handle<Geom_Curve>& C)
{
  ClearEvalRepresentation();
  smooth     = C->Continuity();
  basisCurve = occ::down_cast<Geom_Curve>(C->Copy());
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  V1 = -Precision::Infinite();
  V2 = Precision::Infinite();
  U1 = basisCurve->FirstParameter();
  U2 = basisCurve->LastParameter();
}

//=================================================================================================

gp_Pnt Geom_SurfaceOfLinearExtrusion::EvalD0(const double U, const double V) const
{
  gp_Pnt aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalSurfaceD0(myEvalRep, U, V, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const gp_Pnt aBasisD0 = basisCurve->EvalD0(U);
  gp_Pnt       aP;
  Geom_ExtrusionUtils::CalculateD0(aBasisD0, V, direction.XYZ(), aP);
  return aP;
}

//=================================================================================================

Geom_Surface::ResD1 Geom_SurfaceOfLinearExtrusion::EvalD1(const double U, const double V) const
{
  Geom_Surface::ResD1 aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalSurfaceD1(myEvalRep, U, V, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom_Curve::ResD1 aBasisD1 = basisCurve->EvalD1(U);
  Geom_Surface::ResD1     aResult;
  Geom_ExtrusionUtils::CalculateD1(aBasisD1.Point,
                                   aBasisD1.D1,
                                   V,
                                   direction.XYZ(),
                                   aResult.Point,
                                   aResult.D1U,
                                   aResult.D1V);
  return aResult;
}

//=================================================================================================

Geom_Surface::ResD2 Geom_SurfaceOfLinearExtrusion::EvalD2(const double U, const double V) const
{
  Geom_Surface::ResD2 aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalSurfaceD2(myEvalRep, U, V, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom_Curve::ResD2 aBasisD2 = basisCurve->EvalD2(U);
  Geom_Surface::ResD2     aResult;
  Geom_ExtrusionUtils::CalculateD2(aBasisD2.Point,
                                   aBasisD2.D1,
                                   aBasisD2.D2,
                                   V,
                                   direction.XYZ(),
                                   aResult.Point,
                                   aResult.D1U,
                                   aResult.D1V,
                                   aResult.D2U,
                                   aResult.D2V,
                                   aResult.D2UV);
  return aResult;
}

//=================================================================================================

Geom_Surface::ResD3 Geom_SurfaceOfLinearExtrusion::EvalD3(const double U, const double V) const
{
  Geom_Surface::ResD3 aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalSurfaceD3(myEvalRep, U, V, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom_Curve::ResD3 aBasisD3 = basisCurve->EvalD3(U);
  Geom_Surface::ResD3     aResult;
  Geom_ExtrusionUtils::CalculateD3(aBasisD3.Point,
                                   aBasisD3.D1,
                                   aBasisD3.D2,
                                   aBasisD3.D3,
                                   V,
                                   direction.XYZ(),
                                   aResult.Point,
                                   aResult.D1U,
                                   aResult.D1V,
                                   aResult.D2U,
                                   aResult.D2V,
                                   aResult.D2UV,
                                   aResult.D3U,
                                   aResult.D3V,
                                   aResult.D3UUV,
                                   aResult.D3UVV);
  return aResult;
}

//=================================================================================================

gp_Vec Geom_SurfaceOfLinearExtrusion::EvalDN(const double U,
                                             const double V,
                                             const int    Nu,
                                             const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    throw Geom_UndefinedDerivative();

  gp_Vec aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalSurfaceDN(myEvalRep, U, V, Nu, Nv, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  if (Nv == 0)
  {
    const gp_Vec aDN = basisCurve->EvalDN(U, Nu);
    return Geom_ExtrusionUtils::CalculateDN(aDN, direction.XYZ(), Nu, Nv);
  }
  else if (Nu == 0 && Nv == 1)
  {
    return gp_Vec(direction.XYZ());
  }
  return gp_Vec(0.0, 0.0, 0.0);
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_SurfaceOfLinearExtrusion::UIso(const double U) const
{

  occ::handle<Geom_Line> L;
  L = new Geom_Line(basisCurve->Value(U), direction);
  return L;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_SurfaceOfLinearExtrusion::VIso(const double V) const
{

  Vec Vdir(direction);
  Vdir.Multiply(V);
  occ::handle<Geom_Curve> C;
  C = occ::down_cast<Geom_Curve>(basisCurve->Translated(Vdir));
  return C;
}

//=================================================================================================

bool Geom_SurfaceOfLinearExtrusion::IsCNu(const int N) const
{

  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N);
}

//=================================================================================================

bool Geom_SurfaceOfLinearExtrusion::IsCNv(const int) const
{

  return true;
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::Transform(const Trsf& T)
{
  ClearEvalRepresentation();
  direction.Transform(T);
  basisCurve->Transform(T);
}

//=================================================================================================

bool Geom_SurfaceOfLinearExtrusion::IsUClosed() const
{

  return basisCurve->IsClosed();
}

//=================================================================================================

bool Geom_SurfaceOfLinearExtrusion::IsUPeriodic() const
{

  return basisCurve->IsPeriodic();
}

//=================================================================================================

bool Geom_SurfaceOfLinearExtrusion::IsVClosed() const
{

  return false;
}

//=================================================================================================

bool Geom_SurfaceOfLinearExtrusion::IsVPeriodic() const
{

  return false;
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::TransformParameters(double&        U,
                                                        double&        V,
                                                        const gp_Trsf& T) const
{
  U = basisCurve->TransformedParameter(U, T);
  if (!Precision::IsInfinite(V))
    V *= std::abs(T.ScaleFactor());
}

//=================================================================================================

gp_GTrsf2d Geom_SurfaceOfLinearExtrusion::ParametricTransformation(const gp_Trsf& T) const
{
  // transformation in the V Direction
  gp_GTrsf2d TV;
  gp_Ax2d    Axis(gp::Origin2d(), gp::DX2d());
  TV.SetAffinity(Axis, std::abs(T.ScaleFactor()));
  // transformation in the U Direction
  gp_GTrsf2d TU;
  Axis = gp_Ax2d(gp::Origin2d(), gp::DY2d());
  TU.SetAffinity(Axis, basisCurve->ParametricTransformation(T));

  return TU * TV;
}

//=================================================================================================

void Geom_SurfaceOfLinearExtrusion::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_SweptSurface)
}
