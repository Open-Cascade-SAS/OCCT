// Created on: 1998-06-03
// Created by: data exchange team
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

#ifndef _ShapeAnalysis_ShapeTolerance_HeaderFile
#define _ShapeAnalysis_ShapeTolerance_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_HSequenceOfShape.hxx>
class TopoDS_Shape;


//! Tool for computing shape tolerances (minimal, maximal, average),
//! finding shape with tolerance matching given criteria,
//! setting or limitating tolerances.
class ShapeAnalysis_ShapeTolerance 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_ShapeTolerance();
  
  //! Determines a tolerance from the ones stored in a shape
  //! Remark : calls InitTolerance and AddTolerance,
  //! hence, can be used to start a series for cumulating tolerance
  //! <mode> = 0 : returns the average value between sub-shapes,
  //! <mode> > 0 : returns the maximal found,
  //! <mode> < 0 : returns the minimal found.
  //! <type> defines what kinds of sub-shapes to consider:
  //! SHAPE (default) : all : VERTEX, EDGE, FACE,
  //! VERTEX : only vertices,
  //! EDGE   : only edges,
  //! FACE   : only faces,
  //! SHELL  : combined SHELL + FACE, for each face (and containing
  //! shell), also checks EDGE and VERTEX
  Standard_EXPORT Standard_Real Tolerance (const TopoDS_Shape& shape, const Standard_Integer mode, const TopAbs_ShapeEnum type = TopAbs_SHAPE);
  
  //! Determines which shapes have a tolerance over the given value
  //! <type> is interpreted as in the method Tolerance
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) OverTolerance (const TopoDS_Shape& shape, const Standard_Real value, const TopAbs_ShapeEnum type = TopAbs_SHAPE) const;
  
  //! Determines which shapes have a tolerance within a given interval
  //! <type> is interpreted as in the method Tolerance
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) InTolerance (const TopoDS_Shape& shape, const Standard_Real valmin, const Standard_Real valmax, const TopAbs_ShapeEnum type = TopAbs_SHAPE) const;
  
  //! Initializes computation of cumulated tolerance
  Standard_EXPORT void InitTolerance();
  
  //! Adds data on new Shape to compute Cumulated Tolerance
  //! (prepares three computations : maximal, average, minimal)
  Standard_EXPORT void AddTolerance (const TopoDS_Shape& shape, const TopAbs_ShapeEnum type = TopAbs_SHAPE);
  
  //! Returns the computed tolerance according to the <mode>
  //! <mode> = 0 : average
  //! <mode> > 0 : maximal
  //! <mode> < 0 : minimal
  Standard_EXPORT Standard_Real GlobalTolerance (const Standard_Integer mode) const;




protected:





private:



  Standard_Real myTols[3];
  Standard_Integer myNbTol;


};







#endif // _ShapeAnalysis_ShapeTolerance_HeaderFile
