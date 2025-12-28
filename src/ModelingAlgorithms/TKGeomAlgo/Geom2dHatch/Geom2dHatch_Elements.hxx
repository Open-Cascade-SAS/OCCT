// Created on: 1994-12-16
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Geom2dHatch_Elements_HeaderFile
#define _Geom2dHatch_Elements_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2dHatch_Element.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
class Geom2dHatch_Element;
class gp_Pnt2d;
class gp_Lin2d;
class Geom2dAdaptor_Curve;

class Geom2dHatch_Elements
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Geom2dHatch_Elements();

  Standard_EXPORT Geom2dHatch_Elements(const Geom2dHatch_Elements& Other);

  Standard_EXPORT void Clear();

  ~Geom2dHatch_Elements() { Clear(); }

  Standard_EXPORT bool Bind(const int K, const Geom2dHatch_Element& I);

  Standard_EXPORT bool IsBound(const int K) const;

  Standard_EXPORT bool UnBind(const int K);

  Standard_EXPORT const Geom2dHatch_Element& Find(const int K) const;

  const Geom2dHatch_Element& operator()(const int K) const { return Find(K); }

  Standard_EXPORT Geom2dHatch_Element& ChangeFind(const int K);

  Geom2dHatch_Element& operator()(const int K) { return ChangeFind(K); }

  Standard_EXPORT bool CheckPoint(gp_Pnt2d& P);

  Standard_EXPORT bool Reject(const gp_Pnt2d& P) const;

  Standard_EXPORT bool Segment(const gp_Pnt2d& P, gp_Lin2d& L, double& Par);

  Standard_EXPORT bool OtherSegment(const gp_Pnt2d& P, gp_Lin2d& L, double& Par);

  Standard_EXPORT void InitWires();

  Standard_EXPORT bool MoreWires() const;

  Standard_EXPORT void NextWire();

  Standard_EXPORT bool RejectWire(const gp_Lin2d& L, const double Par) const;

  Standard_EXPORT void InitEdges();

  Standard_EXPORT bool MoreEdges() const;

  Standard_EXPORT void NextEdge();

  Standard_EXPORT bool RejectEdge(const gp_Lin2d& L, const double Par) const;

  Standard_EXPORT void CurrentEdge(Geom2dAdaptor_Curve& E, TopAbs_Orientation& Or) const;

private:
  NCollection_DataMap<int, Geom2dHatch_Element>           myMap;
  NCollection_DataMap<int, Geom2dHatch_Element>::Iterator Iter;
  int                                                     NumWire;
  int                                                     NumEdge;
  int                                                     myCurEdge;
  double                                                  myCurEdgePar;
};

#endif // _Geom2dHatch_Elements_HeaderFile
