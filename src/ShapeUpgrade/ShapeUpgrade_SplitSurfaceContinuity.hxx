// Created on: 1999-04-14
// Created by: Roman LYGIN
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

#ifndef _ShapeUpgrade_SplitSurfaceContinuity_HeaderFile
#define _ShapeUpgrade_SplitSurfaceContinuity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeUpgrade_SplitSurface.hxx>


class ShapeUpgrade_SplitSurfaceContinuity;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_SplitSurfaceContinuity, ShapeUpgrade_SplitSurface)

//! Splits a Surface with a continuity criterion.
//! At the present moment C1 criterion is used only.
//! This tool works with tolerance. If C0 surface can be corrected
//! at a knot with given tolerance then the surface is corrected,
//! otherwise it is spltted at that knot.
class ShapeUpgrade_SplitSurfaceContinuity : public ShapeUpgrade_SplitSurface
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitSurfaceContinuity();
  
  //! Sets criterion for splitting.
  Standard_EXPORT void SetCriterion (const GeomAbs_Shape Criterion);
  
  //! Sets tolerance.
  Standard_EXPORT void SetTolerance (const Standard_Real Tol);
  
  Standard_EXPORT virtual void Compute (const Standard_Boolean Segment) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceContinuity,ShapeUpgrade_SplitSurface)

protected:




private:


  GeomAbs_Shape myCriterion;
  Standard_Real myTolerance;
  Standard_Integer myCont;


};







#endif // _ShapeUpgrade_SplitSurfaceContinuity_HeaderFile
