// Created on: 1993-02-05
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

#ifndef _Contap_ThePathPointOfTheSearch_HeaderFile
#define _Contap_ThePathPointOfTheSearch_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <gp_Pnt.hxx>

class Adaptor3d_HVertex;
class Standard_DomainError;
class gp_Pnt;

class Contap_ThePathPointOfTheSearch
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Contap_ThePathPointOfTheSearch();

  Standard_EXPORT Contap_ThePathPointOfTheSearch(const gp_Pnt&                    P,
                                                 const Standard_Real              Tol,
                                                 const Handle(Adaptor3d_HVertex)& V,
                                                 const Handle(Adaptor2d_Curve2d)& A,
                                                 const Standard_Real              Parameter);

  Standard_EXPORT Contap_ThePathPointOfTheSearch(const gp_Pnt&                    P,
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

inline void Contap_ThePathPointOfTheSearch::SetValue(const gp_Pnt&                    P,
                                                     const Standard_Real              Tol,
                                                     const Handle(Adaptor3d_HVertex)& V,
                                                     const Handle(Adaptor2d_Curve2d)& A,
                                                     const Standard_Real              Parameter)
{
  isnew = Standard_False;
  point = P;
  tol   = Tol;
  vtx   = V;
  arc   = A;
  param = Parameter;
}

inline void Contap_ThePathPointOfTheSearch::SetValue(const gp_Pnt&                    P,
                                                     const Standard_Real              Tol,
                                                     const Handle(Adaptor2d_Curve2d)& A,
                                                     const Standard_Real              Parameter)
{
  isnew = Standard_True;
  point = P;
  tol   = Tol;
  arc   = A;
  param = Parameter;
}

inline const gp_Pnt& Contap_ThePathPointOfTheSearch::Value() const
{
  return point;
}

inline Standard_Real Contap_ThePathPointOfTheSearch::Tolerance() const
{
  return tol;
}

inline Standard_Boolean Contap_ThePathPointOfTheSearch::IsNew() const
{
  return isnew;
}

inline const Handle(Adaptor3d_HVertex)& Contap_ThePathPointOfTheSearch::Vertex() const
{
  if (isnew)
  {
    throw Standard_DomainError();
  }
  return vtx;
}

inline const Handle(Adaptor2d_Curve2d)& Contap_ThePathPointOfTheSearch::Arc() const
{
  return arc;
}

inline Standard_Real Contap_ThePathPointOfTheSearch::Parameter() const
{
  return param;
}

#endif // _Contap_ThePathPointOfTheSearch_HeaderFile
