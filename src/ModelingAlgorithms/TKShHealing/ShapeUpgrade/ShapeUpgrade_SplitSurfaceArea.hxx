// Created on: 2006-08-07
// Created by: Galina KULIKOVA
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_SplitSurfaceArea_HeaderFile
#define _ShapeUpgrade_SplitSurfaceArea_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <ShapeUpgrade_SplitSurface.hxx>

class ShapeUpgrade_SplitSurfaceArea;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_SplitSurfaceArea, ShapeUpgrade_SplitSurface)

//! Split surface in the parametric space
//! in according specified number of splits on the
class ShapeUpgrade_SplitSurfaceArea : public ShapeUpgrade_SplitSurface
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitSurfaceArea();

  //! Set number of split for surfaces
  Standard_Integer& NbParts();

  //! Set splitting mode
  //! If the mode is "splitting into squares",
  //! the face is splitted approximately into <myNbParts> parts,
  //! the parts are similar to squares in 2D.
  void SetSplittingIntoSquares(const Standard_Boolean theIsSplittingIntoSquares);

  //! Set fixed numbers of splits in U and V directions.
  //! Only for "Splitting Into Squares" mode
  void SetNumbersUVSplits(const Standard_Integer theNbUsplits, const Standard_Integer theNbVsplits);

  Standard_EXPORT virtual void Compute(const Standard_Boolean Segment = Standard_True)
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceArea, ShapeUpgrade_SplitSurface)

protected:
private:
  Standard_Integer myNbParts;
  Standard_Integer myUnbSplit;
  Standard_Integer myVnbSplit;
  Standard_Boolean myIsSplittingIntoSquares;
};

#include <ShapeUpgrade_SplitSurfaceArea.lxx>

#endif // _ShapeUpgrade_SplitSurfaceArea_HeaderFile
