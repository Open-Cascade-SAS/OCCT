// Created by: Peter KURNEV
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

#ifndef _IntTools_ShrunkRange_HeaderFile
#define _IntTools_ShrunkRange_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <Bnd_Box.hxx>
#include <Standard_Integer.hxx>
class IntTools_Context;

//! The class provides the computation of
//! a working (shrunk) range [t1, t2] for
//! the 3D-curve of the edge.
class IntTools_ShrunkRange
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntTools_ShrunkRange();
  Standard_EXPORT virtual ~IntTools_ShrunkRange();

  Standard_EXPORT void SetData(const TopoDS_Edge&   aE,
                               const double         aT1,
                               const double         aT2,
                               const TopoDS_Vertex& aV1,
                               const TopoDS_Vertex& aV2);

  Standard_EXPORT void SetContext(const occ::handle<IntTools_Context>& aCtx);

  Standard_EXPORT const occ::handle<IntTools_Context>& Context() const;

  Standard_EXPORT void SetShrunkRange(const double aT1, const double aT2);

  Standard_EXPORT void ShrunkRange(double& aT1, double& aT2) const;

  Standard_EXPORT const Bnd_Box& BndBox() const;

  Standard_EXPORT const TopoDS_Edge& Edge() const;

  Standard_EXPORT void Perform();

  //! Returns TRUE in case the shrunk range is computed
  bool IsDone() const { return myIsDone; }

  //! Returns FALSE in case the shrunk range is
  //! too short and the edge cannot be split,
  //! otherwise returns TRUE
  bool IsSplittable() const { return myIsSplittable; }

  //! Returns the length of the edge if computed.
  double Length() const { return myLength; }

protected:
  TopoDS_Edge                   myEdge;
  TopoDS_Vertex                 myV1;
  TopoDS_Vertex                 myV2;
  double                        myT1;
  double                        myT2;
  double                        myTS1;
  double                        myTS2;
  Bnd_Box                       myBndBox;
  occ::handle<IntTools_Context> myCtx;
  bool                          myIsDone;
  bool                          myIsSplittable;
  double                        myLength;
};

#endif // _IntTools_ShrunkRange_HeaderFile
