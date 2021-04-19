// Created on: 1999-06-21
// Created by: Galina KULIKOVA
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

#ifndef _ShapeAnalysis_TransferParametersProj_HeaderFile
#define _ShapeAnalysis_TransferParametersProj_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Standard_Real.hxx>
#include <TopLoc_Location.hxx>
#include <Standard_Boolean.hxx>
#include <ShapeAnalysis_TransferParameters.hxx>
#include <TColStd_HSequenceOfReal.hxx>
class Geom_Curve;
class Geom2d_Curve;
class TopoDS_Edge;
class TopoDS_Face;
class TopoDS_Vertex;


class ShapeAnalysis_TransferParametersProj;
DEFINE_STANDARD_HANDLE(ShapeAnalysis_TransferParametersProj, ShapeAnalysis_TransferParameters)

//! This tool is used for transferring parameters
//! from 3d curve of the edge to pcurve and vice versa.
//! This tool transfers parameters with help of
//! projection points from curve 3d on curve 2d and
//! vice versa
class ShapeAnalysis_TransferParametersProj : public ShapeAnalysis_TransferParameters
{

public:

  
  //! Creates empty constructor.
  Standard_EXPORT ShapeAnalysis_TransferParametersProj();
  
  Standard_EXPORT ShapeAnalysis_TransferParametersProj(const TopoDS_Edge& E, const TopoDS_Face& F);
  
  Standard_EXPORT virtual void Init (const TopoDS_Edge& E, const TopoDS_Face& F) Standard_OVERRIDE;
  
  //! Transfers parameters given by sequence Params from 3d curve
  //! to pcurve (if To2d is True) or back (if To2d is False)
  Standard_EXPORT virtual Handle(TColStd_HSequenceOfReal) Perform (const Handle(TColStd_HSequenceOfReal)& Papams, const Standard_Boolean To2d) Standard_OVERRIDE;
  
  //! Transfers parameter given by  Param from 3d curve
  //! to pcurve (if To2d is True) or back (if To2d is False)
  Standard_EXPORT virtual Standard_Real Perform (const Standard_Real Param, const Standard_Boolean To2d) Standard_OVERRIDE;
  
  //! Returns modifiable flag forcing projection
  //! If it is False (default), projection is done only
  //! if edge is not SameParameter or if tolerance of edge
  //! is greater than MaxTolerance()
  Standard_EXPORT Standard_Boolean& ForceProjection();
  
  //! Recomputes range of curves from NewEdge.
  //! If Is2d equals True parameters are recomputed by curve2d else by curve3d.
  Standard_EXPORT virtual void TransferRange (TopoDS_Edge& newEdge, const Standard_Real prevPar, const Standard_Real currPar, const Standard_Boolean Is2d) Standard_OVERRIDE;
  
  //! Returns False;
  Standard_EXPORT virtual Standard_Boolean IsSameRange() const Standard_OVERRIDE;
  
  //! Make a copy of non-manifold vertex theVert
  //! (i.e. create new  TVertex and replace PointRepresentations for this vertex
  //! from fromedge to toedge. Other representations were copied)
  Standard_EXPORT static TopoDS_Vertex CopyNMVertex (const TopoDS_Vertex& theVert, const TopoDS_Edge& toedge, const TopoDS_Edge& fromedge);
  
  //! Make a copy of non-manifold vertex theVert
  //! (i.e. create new  TVertex and replace PointRepresentations for this vertex
  //! from fromFace to toFace. Other representations were copied)
  Standard_EXPORT static TopoDS_Vertex CopyNMVertex (const TopoDS_Vertex& theVert, const TopoDS_Face& toFace, const TopoDS_Face& fromFace);




  DEFINE_STANDARD_RTTIEXT(ShapeAnalysis_TransferParametersProj,ShapeAnalysis_TransferParameters)

protected:




private:

  
  Standard_EXPORT Standard_Real PreformSegment (const Standard_Real Param, const Standard_Boolean To2d, const Standard_Real First, const Standard_Real Last);

  Handle(Geom_Curve) myCurve;
  Handle(Geom2d_Curve) myCurve2d;
  Adaptor3d_CurveOnSurface myAC3d;
  Standard_Real myPrecision;
  TopLoc_Location myLocation;
  Standard_Boolean myForceProj;
  Standard_Boolean myInitOK;


};







#endif // _ShapeAnalysis_TransferParametersProj_HeaderFile
