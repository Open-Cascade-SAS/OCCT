// Created on: 1993-10-12
// Created by: Remi LEQUETTE
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

#ifndef _BRepPrimAPI_MakeRevol_HeaderFile
#define _BRepPrimAPI_MakeRevol_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepSweep_Revol.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepPrimAPI_MakeSweep.hxx>
#include <BRepTools_History.hxx>
class TopoDS_Shape;
class gp_Ax1;

//! Class to make revolved sweep topologies.
//!
//! a revolved sweep is defined by :
//!
//! * A basis topology which is swept.
//!
//! The   basis topology  must   not  contain solids
//! (neither composite solids.).
//!
//! The basis topology  may be copied  or  shared in
//! the result.
//!
//! * A rotation axis and angle :
//!
//! - The axis is an Ax1 from gp.
//!
//! - The angle is in [0, 2*Pi].
//!
//! - The angle default value is 2*Pi.
//!
//! The result is a topology with a higher dimension :
//!
//! - Vertex -> Edge.
//! - Edge   -> Face.
//! - Wire   -> Shell.
//! - Face   -> Solid.
//! - Shell  -> CompSolid.
//!
//! Sweeping a Compound sweeps  the elements  of the
//! compound  and creates    a  compound with    the
//! results.
class BRepPrimAPI_MakeRevol : public BRepPrimAPI_MakeSweep
{
public:
  DEFINE_STANDARD_ALLOC

  //! Builds the Revol of base S, axis  A and angle  D. If C
  //! is true, S is copied.
  Standard_EXPORT BRepPrimAPI_MakeRevol(const TopoDS_Shape&    S,
                                        const gp_Ax1&          A,
                                        const Standard_Real    D,
                                        const Standard_Boolean Copy = Standard_False);

  //! Builds the Revol of base S, axis  A and angle 2*Pi. If
  //! C is true, S is copied.
  Standard_EXPORT BRepPrimAPI_MakeRevol(const TopoDS_Shape&    S,
                                        const gp_Ax1&          A,
                                        const Standard_Boolean Copy = Standard_False);

  //! Returns the internal sweeping algorithm.
  Standard_EXPORT const BRepSweep_Revol& Revol() const;

  //! Builds the resulting shape (redefined from MakeShape).
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Returns the first shape of the revol  (coinciding with
  //! the generating shape).
  Standard_EXPORT TopoDS_Shape FirstShape() Standard_OVERRIDE;

  //! Returns the TopoDS Shape of the end of the revol.
  Standard_EXPORT TopoDS_Shape LastShape() Standard_OVERRIDE;

  //! Returns list of shape generated from shape S
  //! Warning: shape S must be shape of type VERTEX, EDGE, FACE, SOLID.
  //! For shapes of other types method always returns empty list
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated(const TopoDS_Shape& S)
    Standard_OVERRIDE;

  //! Returns true if the shape S has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted(const TopoDS_Shape& S) Standard_OVERRIDE;

  //! Returns the TopoDS Shape of the beginning of the revolution,
  //! generated with theShape  (subShape of the generating shape).
  Standard_EXPORT TopoDS_Shape FirstShape(const TopoDS_Shape& theShape);

  //! Returns the TopoDS Shape of the end of the revolution,
  //! generated with  theShape (subShape of the  generating shape).
  Standard_EXPORT TopoDS_Shape LastShape(const TopoDS_Shape& theShape);

  //! Check if there are degenerated edges in the result.
  Standard_EXPORT Standard_Boolean HasDegenerated() const;

  //! Returns the list of degenerated edges
  Standard_EXPORT const TopTools_ListOfShape& Degenerated() const;

protected:
  //! Checks possibilities of producing self-intersection surface
  //! returns true if all surfaces are valid
  Standard_EXPORT Standard_Boolean CheckValidity(const TopoDS_Shape& theShape, const gp_Ax1& theA);

private:
  BRepSweep_Revol           myRevol;
  TopTools_ListOfShape      myDegenerated;
  Handle(BRepTools_History) myHist;
  Standard_Boolean          myIsBuild;
};

#endif // _BRepPrimAPI_MakeRevol_HeaderFile
