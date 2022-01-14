// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_UniformRectGrid_HeaderFile
#define _IGESGraph_UniformRectGrid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt2d;
class gp_Vec2d;


class IGESGraph_UniformRectGrid;
DEFINE_STANDARD_HANDLE(IGESGraph_UniformRectGrid, IGESData_IGESEntity)

//! defines IGESUniformRectGrid, Type <406> Form <22>
//! in package IGESGraph
//!
//! Stores sufficient information for the creation of
//! a uniform rectangular grid within a drawing
class IGESGraph_UniformRectGrid : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGraph_UniformRectGrid();
  
  //! This method is used to set the fields of the class
  //! UniformRectGrid
  //! - nbProps      : Number of property values (NP = 9)
  //! - finite       : Finite/Infinite grid flag
  //! - line         : Line/Point grid flag
  //! - weighted     : Weighted/Unweighted grid flag
  //! - aGridPoint   : Point on the grid
  //! - aGridSpacing : Grid spacing
  //! - pointsX      : No. of points/lines in X Direction
  //! - pointsY      : No. of points/lines in Y Direction
  Standard_EXPORT void Init (const Standard_Integer nbProps, const Standard_Integer finite, const Standard_Integer line, const Standard_Integer weighted, const gp_XY& aGridPoint, const gp_XY& aGridSpacing, const Standard_Integer pointsX, const Standard_Integer pointsY);
  
  //! returns the number of property values in <me>.
  Standard_EXPORT Standard_Integer NbPropertyValues() const;
  
  //! returns False if <me> is an infinite grid,
  //! True  if <me> is a finite grid.
  Standard_EXPORT Standard_Boolean IsFinite() const;
  
  //! returns False if <me> is a Point grid,
  //! True  if <me> is a Line grid.
  Standard_EXPORT Standard_Boolean IsLine() const;
  
  //! returns False if <me> is a Weighted grid,
  //! True  if <me> is not a Weighted grid.
  Standard_EXPORT Standard_Boolean IsWeighted() const;
  
  //! returns coordinates of lower left corner,
  //! if <me> is a finite grid,
  //! coordinates of an arbitrary point,
  //! if <me> is an infinite grid.
  Standard_EXPORT gp_Pnt2d GridPoint() const;
  
  //! returns the grid-spacing in drawing coordinates.
  Standard_EXPORT gp_Vec2d GridSpacing() const;
  
  //! returns the no. of points/lines in X direction
  //! (only applicable if IsFinite() = 1, i.e: a finite grid).
  Standard_EXPORT Standard_Integer NbPointsX() const;
  
  //! returns the no. of points/lines in Y direction
  //! (only applicable if IsFinite() = 1, i.e: a finite grid).
  Standard_EXPORT Standard_Integer NbPointsY() const;




  DEFINE_STANDARD_RTTIEXT(IGESGraph_UniformRectGrid,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbPropertyValues;
  Standard_Integer isItFinite;
  Standard_Integer isItLine;
  Standard_Integer isItWeighted;
  gp_XY theGridPoint;
  gp_XY theGridSpacing;
  Standard_Integer theNbPointsX;
  Standard_Integer theNbPointsY;


};







#endif // _IGESGraph_UniformRectGrid_HeaderFile
