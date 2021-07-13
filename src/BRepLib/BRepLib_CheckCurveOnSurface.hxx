// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BRepLib_CheckCurveOnSurface_HeaderFile
#define _BRepLib_CheckCurveOnSurface_HeaderFile

#include <GeomLib_CheckCurveOnSurface.hxx>

//! Computes the max distance between edge and its 2d representation on the face.
//! This class is not intended to process non-sameparameter edges.

class BRepLib_CheckCurveOnSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor
  BRepLib_CheckCurveOnSurface() {}
  
  //! Constructor
  Standard_EXPORT BRepLib_CheckCurveOnSurface(const TopoDS_Edge& theEdge,
                                              const TopoDS_Face& theFace);
  
  //! Sets the data for the algorithm
  Standard_EXPORT void Init (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);

  //! Performs the calculation
  //! If isMultiThread == Standard_True then computation will be performed in parallel.
  Standard_EXPORT void Perform(const Standard_Boolean isMultiThread = Standard_False);
  
  //! Returns true if the max distance has been found
  Standard_Boolean IsDone() const
  {
    return myCOnSurfGeom.ErrorStatus() == 0;
  }
  
  //! Returns error status
  //! The possible values are:
  //! 0 - OK;
  //! 1 - null curve or surface or 2d curve;
  //! 2 - invalid parametric range;
  //! 3 - error in calculations.
  Standard_Integer ErrorStatus() const
  {
    return myCOnSurfGeom.ErrorStatus();
  }
  
  //! Returns max distance
  Standard_Real MaxDistance() const
  {
    return myCOnSurfGeom.MaxDistance();
  }
  
  //! Returns parameter in which the distance is maximal
  Standard_Real MaxParameter() const
  {
    return myCOnSurfGeom.MaxParameter();
  }

protected:

  //! Computes the max distance for the 3d curve of <myCOnSurfGeom>
  //! and 2d curve <theCurveOnSurface>
  //! If isMultiThread == Standard_True then computation will be performed in parallel.
  Standard_EXPORT void Compute (const Handle(Adaptor3d_CurveOnSurface)& theCurveOnSurface,
                                const Standard_Boolean isMultiThread);

private:

  GeomLib_CheckCurveOnSurface myCOnSurfGeom;
  Handle(Adaptor3d_CurveOnSurface) myAdaptorCurveOnSurface;
  Handle(Adaptor3d_CurveOnSurface) myAdaptorCurveOnSurface2;
};

#endif // _BRepLib_CheckCurveOnSurface_HeaderFile
