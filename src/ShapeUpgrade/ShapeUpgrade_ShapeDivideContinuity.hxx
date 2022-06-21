// Created on: 1999-04-30
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeUpgrade_ShapeDivideContinuity_HeaderFile
#define _ShapeUpgrade_ShapeDivideContinuity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
#include <ShapeUpgrade_ShapeDivide.hxx>
class TopoDS_Shape;
class ShapeUpgrade_FaceDivide;


//! API Tool for converting shapes with C0 geometry into C1 ones
class ShapeUpgrade_ShapeDivideContinuity  : public ShapeUpgrade_ShapeDivide
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeUpgrade_ShapeDivideContinuity();
  
  //! Initialize by a Shape.
  Standard_EXPORT ShapeUpgrade_ShapeDivideContinuity(const TopoDS_Shape& S);
  
  //! Sets tolerance.
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);
  
  //! Sets tolerance.
  Standard_EXPORT void SetTolerance2d (const Standard_Real Tol);
  

  //! Defines a criterion of continuity for the boundary (all the
  //! Wires)
  //!
  //! The possible values are C0, G1, C1, G2, C2, C3, CN The
  //! default is C1 to respect the Cas.Cade Shape Validity.  G1
  //! and G2 are not authorized.
  Standard_EXPORT void SetBoundaryCriterion (const GeomAbs_Shape Criterion = GeomAbs_C1);
  

  //! Defines a criterion of continuity for the boundary (all the
  //! pcurves of Wires)
  //!
  //! The possible values are C0, G1, C1, G2, C2, C3, CN The
  //! default is C1 to respect the Cas.Cade Shape Validity.  G1
  //! and G2 are not authorized.
  Standard_EXPORT void SetPCurveCriterion (const GeomAbs_Shape Criterion = GeomAbs_C1);
  

  //! Defines a criterion of continuity for the boundary (all the
  //! Wires)
  //!
  //! The possible values are C0, G1, C1, G2, C2, C3, CN The
  //! default is C1 to respect the Cas.Cade Shape Validity.  G1
  //! and G2 are not authorized.
  Standard_EXPORT void SetSurfaceCriterion (const GeomAbs_Shape Criterion = GeomAbs_C1);




protected:

  
  //! Returns the tool for dividing faces.
  Standard_EXPORT virtual Handle(ShapeUpgrade_FaceDivide) GetSplitFaceTool() const Standard_OVERRIDE;




private:



  GeomAbs_Shape myCurve3dCriterion;
  GeomAbs_Shape myCurve2dCriterion;
  GeomAbs_Shape mySurfaceCriterion;
  Standard_Real myTolerance3d;
  Standard_Real myTolerance2d;


};







#endif // _ShapeUpgrade_ShapeDivideContinuity_HeaderFile
