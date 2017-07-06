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

#ifndef _ShapeAnalysis_TransferParameters_HeaderFile
#define _ShapeAnalysis_TransferParameters_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Edge;
class TopoDS_Face;


class ShapeAnalysis_TransferParameters;
DEFINE_STANDARD_HANDLE(ShapeAnalysis_TransferParameters, Standard_Transient)

//! This tool is used for transferring parameters
//! from 3d curve of the edge to pcurve and vice versa.
//!
//! Default behaviour is to trsnafer parameters with help
//! of linear transformation:
//!
//! T2d = myShift + myScale * T3d
//! where
//! myScale = ( Last2d - First2d ) / ( Last3d - First3d )
//! myShift = First2d - First3d * myScale
//! [First3d, Last3d] and [First2d, Last2d] are ranges of
//! edge on curve and pcurve
//!
//! This behaviour can be redefined in derived classes, for example,
//! using projection.
class ShapeAnalysis_TransferParameters : public Standard_Transient
{

public:

  
  //! Creates empty tool with myShift = 0 and myScale = 1
  Standard_EXPORT ShapeAnalysis_TransferParameters();
  
  //! Creates a tool and initializes it with edge and face
  Standard_EXPORT ShapeAnalysis_TransferParameters(const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Initialize a tool with edge and face
  Standard_EXPORT virtual void Init (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Sets maximal tolerance to use linear recomputation of
  //! parameters.
  Standard_EXPORT void SetMaxTolerance (const Standard_Real maxtol);
  
  //! Transfers parameters given by sequence Params from 3d curve
  //! to pcurve (if To2d is True) or back (if To2d is False)
  Standard_EXPORT virtual Handle(TColStd_HSequenceOfReal) Perform (const Handle(TColStd_HSequenceOfReal)& Params, const Standard_Boolean To2d);
  
  //! Transfers parameter given by sequence Params from 3d curve
  //! to pcurve (if To2d is True) or back (if To2d is False)
  Standard_EXPORT virtual Standard_Real Perform (const Standard_Real Param, const Standard_Boolean To2d);
  
  //! Recomputes range of curves from NewEdge.
  //! If Is2d equals True parameters are recomputed by curve2d else by curve3d.
  Standard_EXPORT virtual void TransferRange (TopoDS_Edge& newEdge, const Standard_Real prevPar, const Standard_Real currPar, const Standard_Boolean To2d);
  
  //! Returns True if 3d curve of edge and pcurve are SameRange
  //! (in default implementation, if myScale == 1 and myShift == 0)
  Standard_EXPORT virtual Standard_Boolean IsSameRange() const;




  DEFINE_STANDARD_RTTIEXT(ShapeAnalysis_TransferParameters,Standard_Transient)

protected:


  Standard_Real myFirst;
  Standard_Real myLast;
  TopoDS_Edge myEdge;
  Standard_Real myMaxTolerance;


private:


  Standard_Real myShift;
  Standard_Real myScale;
  Standard_Real myFirst2d;
  Standard_Real myLast2d;
  TopoDS_Face myFace;


};







#endif // _ShapeAnalysis_TransferParameters_HeaderFile
