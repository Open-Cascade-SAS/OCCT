// Created on: 1992-05-06
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

inline IntPatch_Point::IntPatch_Point()
    : para(0.0),
      tol(0.0),
      tgt(false),
      mult(false),
      onS1(false),
      vtxonS1(false),
      prm1(0.0),
      onS2(false),
      vtxonS2(false),
      prm2(0.0)
{
}

inline void IntPatch_Point::SetValue(const gp_Pnt& Pt)
{
  pt.SetValue(Pt);
}

//=======================================================================
////function : SetValue
// purpose  :
//=======================================================================
inline void IntPatch_Point::SetValue(const IntSurf_PntOn2S& thePOn2S)
{
  pt = thePOn2S;
}

inline void IntPatch_Point::SetTolerance(const double Tol)
{
  tol = Tol;
}

inline void IntPatch_Point::SetParameters(const double U1,
                                          const double V1,
                                          const double U2,
                                          const double V2)
{
  pt.SetValue(U1, V1, U2, V2);
}

inline void IntPatch_Point::SetParameter(const double Para)
{
  para = Para;
}

inline void IntPatch_Point::SetMultiple(const bool IsMult)
{
  mult = IsMult;
}

inline const gp_Pnt& IntPatch_Point::Value() const
{
  return pt.Value();
}

inline double IntPatch_Point::Tolerance() const
{
  return tol;
}

inline double IntPatch_Point::ParameterOnLine() const
{
  return para;
}

inline bool IntPatch_Point::IsTangencyPoint() const
{
  return tgt;
}

inline void IntPatch_Point::ParametersOnS1(double& U1, double& V1) const
{
  pt.ParametersOnS1(U1, V1);
}

inline void IntPatch_Point::ParametersOnS2(double& U2, double& V2) const
{
  pt.ParametersOnS2(U2, V2);
}

inline bool IntPatch_Point::IsMultiple() const
{
  return mult;
}

inline bool IntPatch_Point::IsOnDomS1() const
{
  return onS1;
}

inline bool IntPatch_Point::IsVertexOnS1() const
{
  return vtxonS1;
}

inline const occ::handle<Adaptor3d_HVertex>& IntPatch_Point::VertexOnS1() const
{
  if (!vtxonS1)
  {
    throw Standard_DomainError();
  }
  return vS1;
}

inline const occ::handle<Adaptor2d_Curve2d>& IntPatch_Point::ArcOnS1() const
{
  if (!onS1)
  {
    throw Standard_DomainError();
  }
  return arcS1;
}

inline const IntSurf_Transition& IntPatch_Point::TransitionLineArc1() const
{
  if (!onS1)
  {
    throw Standard_DomainError();
  }
  return traline1;
}

inline const IntSurf_Transition& IntPatch_Point::TransitionOnS1() const
{
  if (!onS1)
  {
    throw Standard_DomainError();
  }
  return tra1;
}

inline double IntPatch_Point::ParameterOnArc1() const
{
  if (!onS1)
  {
    throw Standard_DomainError();
  }
  return prm1;
}

inline bool IntPatch_Point::IsOnDomS2() const
{
  return onS2;
}

inline bool IntPatch_Point::IsVertexOnS2() const
{
  return vtxonS2;
}

inline const occ::handle<Adaptor3d_HVertex>& IntPatch_Point::VertexOnS2() const
{
  if (!vtxonS2)
  {
    throw Standard_DomainError();
  }
  return vS2;
}

inline const occ::handle<Adaptor2d_Curve2d>& IntPatch_Point::ArcOnS2() const
{
  if (!onS2)
  {
    throw Standard_DomainError();
  }
  return arcS2;
}

inline const IntSurf_Transition& IntPatch_Point::TransitionLineArc2() const
{
  if (!onS2)
  {
    throw Standard_DomainError();
  }
  return traline2;
}

inline const IntSurf_Transition& IntPatch_Point::TransitionOnS2() const
{
  if (!onS2)
  {
    throw Standard_DomainError();
  }
  return tra2;
}

inline double IntPatch_Point::ParameterOnArc2() const
{
  if (!onS2)
  {
    throw Standard_DomainError();
  }
  return prm2;
}

inline const IntSurf_PntOn2S& IntPatch_Point::PntOn2S() const
{
  return pt;
}

inline void IntPatch_Point::Parameters(double& U1, double& V1, double& U2, double& V2) const
{
  pt.Parameters(U1, V1, U2, V2);
}
