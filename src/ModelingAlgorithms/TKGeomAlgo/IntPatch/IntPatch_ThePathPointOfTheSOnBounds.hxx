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

#ifndef _IntPatch_ThePathPointOfTheSOnBounds_HeaderFile
#define _IntPatch_ThePathPointOfTheSOnBounds_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <gp_Pnt.hxx>

class Adaptor3d_HVertex;
class Standard_DomainError;
class gp_Pnt;

class IntPatch_ThePathPointOfTheSOnBounds
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_ThePathPointOfTheSOnBounds();

  Standard_EXPORT IntPatch_ThePathPointOfTheSOnBounds(const gp_Pnt&                    P,
                                                      const Standard_Real              Tol,
                                                      const Handle(Adaptor3d_HVertex)& V,
                                                      const Handle(Adaptor2d_Curve2d)& A,
                                                      const Standard_Real              Parameter);

  Standard_EXPORT IntPatch_ThePathPointOfTheSOnBounds(const gp_Pnt&                    P,
                                                      const Standard_Real              Tol,
                                                      const Handle(Adaptor2d_Curve2d)& A,
                                                      const Standard_Real              Parameter);

  void SetValue(const gp_Pnt&                    P,
                const Standard_Real              Tol,
                const Handle(Adaptor3d_HVertex)& V,
                const Handle(Adaptor2d_Curve2d)& A,
                const Standard_Real              Parameter);

  void SetValue(const gp_Pnt&                    P,
                const Standard_Real              Tol,
                const Handle(Adaptor2d_Curve2d)& A,
                const Standard_Real              Parameter);

  const gp_Pnt& Value() const;

  Standard_Real Tolerance() const;

  Standard_Boolean IsNew() const;

  const Handle(Adaptor3d_HVertex)& Vertex() const;

  const Handle(Adaptor2d_Curve2d)& Arc() const;

  Standard_Real Parameter() const;

protected:
private:
  gp_Pnt                    point;
  Standard_Real             tol;
  Standard_Boolean          isnew;
  Handle(Adaptor3d_HVertex) vtx;
  Handle(Adaptor2d_Curve2d) arc;
  Standard_Real             param;
};

//=================================================================================================
// Inline implementations
//=================================================================================================

inline void IntPatch_ThePathPointOfTheSOnBounds::SetValue(const gp_Pnt&                    P,
                                                          const Standard_Real              Tol,
                                                          const Handle(Adaptor3d_HVertex)& V,
                                                          const Handle(Adaptor2d_Curve2d)& A,
                                                          const Standard_Real Parameter)
{
  isnew = Standard_False;
  point = P;
  tol   = Tol;
  vtx   = V;
  arc   = A;
  param = Parameter;
}

inline void IntPatch_ThePathPointOfTheSOnBounds::SetValue(const gp_Pnt&                    P,
                                                          const Standard_Real              Tol,
                                                          const Handle(Adaptor2d_Curve2d)& A,
                                                          const Standard_Real Parameter)
{
  isnew = Standard_True;
  point = P;
  tol   = Tol;
  arc   = A;
  param = Parameter;
}

inline const gp_Pnt& IntPatch_ThePathPointOfTheSOnBounds::Value() const
{
  return point;
}

inline Standard_Real IntPatch_ThePathPointOfTheSOnBounds::Tolerance() const
{
  return tol;
}

inline Standard_Boolean IntPatch_ThePathPointOfTheSOnBounds::IsNew() const
{
  return isnew;
}

inline const Handle(Adaptor3d_HVertex)& IntPatch_ThePathPointOfTheSOnBounds::Vertex() const
{
  if (isnew)
  {
    throw Standard_DomainError();
  }
  return vtx;
}

inline const Handle(Adaptor2d_Curve2d)& IntPatch_ThePathPointOfTheSOnBounds::Arc() const
{
  return arc;
}

inline Standard_Real IntPatch_ThePathPointOfTheSOnBounds::Parameter() const
{
  return param;
}

#endif // _IntPatch_ThePathPointOfTheSOnBounds_HeaderFile
