// File:      HLRAppli_ReflectLines.cdl
// Created:   05.12.12 15:53:35
// Created by: Julia GERASIMOVA
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

#ifndef _HLRAppli_ReflectLines_HeaderFile
#define _HLRAppli_ReflectLines_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_TypeOfResultingEdge.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>

//! This class builds reflect lines on a shape
//! according to the axes of view defined by user.
//! Reflect lines are represented by edges in 3d.
class HLRAppli_ReflectLines
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor
  Standard_EXPORT HLRAppli_ReflectLines(const TopoDS_Shape& aShape);

  //! Sets the normal to the plane of visualisation,
  //! the coordinates of the view point and
  //! the coordinates of the vertical direction vector.
  Standard_EXPORT void SetAxes(const Standard_Real Nx,
                               const Standard_Real Ny,
                               const Standard_Real Nz,
                               const Standard_Real XAt,
                               const Standard_Real YAt,
                               const Standard_Real ZAt,
                               const Standard_Real XUp,
                               const Standard_Real YUp,
                               const Standard_Real ZUp);

  Standard_EXPORT void Perform();

  //! returns resulting compound of reflect lines
  //! represented by edges in 3d
  Standard_EXPORT TopoDS_Shape GetResult() const;

  //! returns resulting compound of lines
  //! of specified type and visibility
  //! represented by edges in 3d or 2d
  Standard_EXPORT TopoDS_Shape GetCompoundOf3dEdges(const HLRBRep_TypeOfResultingEdge type,
                                                    const Standard_Boolean            visible,
                                                    const Standard_Boolean            In3d) const;

protected:
private:
  HLRAlgo_Projector    myProjector;
  Handle(HLRBRep_Algo) myHLRAlgo;
  TopoDS_Shape         myShape;
  // TopoDS_Shape myCompound;
};

#endif // _HLRAppli_ReflectLines_HeaderFile
