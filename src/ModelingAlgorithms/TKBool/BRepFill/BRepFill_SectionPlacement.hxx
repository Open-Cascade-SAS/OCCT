// Created on: 1998-02-11
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_SectionPlacement_HeaderFile
#define _BRepFill_SectionPlacement_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <Standard_Integer.hxx>
class BRepFill_LocationLaw;

//! Place a shape in a local axis coordinate
class BRepFill_SectionPlacement
{
public:
  DEFINE_STANDARD_ALLOC

  //! Automatic placement
  Standard_EXPORT BRepFill_SectionPlacement(const occ::handle<BRepFill_LocationLaw>& Law,
                                            const TopoDS_Shape&                 Section,
                                            const bool WithContact    = false,
                                            const bool WithCorrection = false);

  //! Placement on vertex
  Standard_EXPORT BRepFill_SectionPlacement(const occ::handle<BRepFill_LocationLaw>& Law,
                                            const TopoDS_Shape&                 Section,
                                            const TopoDS_Shape&                 Vertex,
                                            const bool WithContact    = false,
                                            const bool WithCorrection = false);

  Standard_EXPORT const gp_Trsf& Transformation() const;

  Standard_EXPORT double AbscissaOnPath();

private:
  Standard_EXPORT void Perform(const bool WithContact,
                               const bool WithCorrection,
                               const TopoDS_Shape&    Vertex);

  occ::handle<BRepFill_LocationLaw> myLaw;
  TopoDS_Shape                 mySection;
  gp_Trsf                      myTrsf;
  double                myParam;
  int             myIndex;
};

#endif // _BRepFill_SectionPlacement_HeaderFile
