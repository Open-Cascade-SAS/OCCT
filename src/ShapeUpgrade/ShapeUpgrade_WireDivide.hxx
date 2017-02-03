// Created on: 1999-04-15
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

#ifndef _ShapeUpgrade_WireDivide_HeaderFile
#define _ShapeUpgrade_WireDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <Standard_Integer.hxx>
#include <ShapeUpgrade_Tool.hxx>
#include <Standard_Boolean.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeUpgrade_SplitCurve3d;
class ShapeUpgrade_SplitCurve2d;
class ShapeUpgrade_EdgeDivide;
class ShapeAnalysis_TransferParameters;
class ShapeUpgrade_FixSmallCurves;
class Geom_Surface;
class TopoDS_Edge;
class TopLoc_Location;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeUpgrade_WireDivide;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_WireDivide, ShapeUpgrade_Tool)

//! Divides edges in the wire lying on the face or free wires or
//! free edges with a criterion.
//! Splits 3D curve and pcurve(s) of the edge on the face.
//! Other pcurves which may be associated with the edge are simply
//! copied.
//! If 3D curve is splitted then pcurve on the face is splitted as
//! well, and wice-versa.
//! Input shape is not modified.
//! The modifications made are recorded in external context
//! (ShapeBuild_ReShape). This tool is applied to all edges
//! before splitting them in order to keep sharing.
class ShapeUpgrade_WireDivide : public ShapeUpgrade_Tool
{

public:

  
  //! Empty constructor
  Standard_EXPORT ShapeUpgrade_WireDivide();
  
  //! Initializes by wire and face
  Standard_EXPORT void Init (const TopoDS_Wire& W, const TopoDS_Face& F);
  
  //! Initializes by wire and surface
  Standard_EXPORT void Init (const TopoDS_Wire& W, const Handle(Geom_Surface)& S);
  
  //! Loads working wire
  Standard_EXPORT void Load (const TopoDS_Wire& W);
  
  //! Creates wire of one edge and calls Load for wire
  Standard_EXPORT void Load (const TopoDS_Edge& E);
  
  //! Sets supporting surface by face
  Standard_EXPORT void SetFace (const TopoDS_Face& F);
  
  //! Sets supporting surface
  Standard_EXPORT void SetSurface (const Handle(Geom_Surface)& S);
  
  //! Sets supporting surface with location
  Standard_EXPORT void SetSurface (const Handle(Geom_Surface)& S, const TopLoc_Location& L);
  
  //! Computes the resulting wire by splitting all the edges
  //! according to splitting criteria.
  //! All the modifications made are recorded in context
  //! (ShapeBuild_ReShape). This tool is applied to all edges
  //! before splitting them in order to keep sharings.
  //! If no supporting face or surface is defined, only 3d
  //! splitting criteria are used.
  Standard_EXPORT virtual void Perform();
  
  //! Gives the resulting Wire (equal to initial one if not done
  //! or Null if not loaded)
  Standard_EXPORT const TopoDS_Wire& Wire() const;
  
  //! Queries status of last call to Perform()
  //! OK - no edges were splitted, wire left untouched
  //! DONE1 - some edges were splitted
  //! FAIL1 - some edges have no 3d curve (skipped)
  //! FAIL2 - some edges have no pcurve (skipped)
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Sets the tool for splitting 3D curves.
  Standard_EXPORT void SetSplitCurve3dTool (const Handle(ShapeUpgrade_SplitCurve3d)& splitCurve3dTool);
  
  //! Sets the tool for splitting pcurves.
  Standard_EXPORT void SetSplitCurve2dTool (const Handle(ShapeUpgrade_SplitCurve2d)& splitCurve2dTool);
  
  //! Sets the tool for Transfer parameters between curves and pcurves.
  Standard_EXPORT void SetTransferParamTool (const Handle(ShapeAnalysis_TransferParameters)& TransferParam);
  
  //! Sets tool for splitting edge
  Standard_EXPORT void SetEdgeDivideTool (const Handle(ShapeUpgrade_EdgeDivide)& edgeDivideTool);
  
  //! returns tool for splitting edges
  Standard_EXPORT virtual Handle(ShapeUpgrade_EdgeDivide) GetEdgeDivideTool() const;
  
  //! Returns the tool for Transfer of parameters.
  Standard_EXPORT virtual Handle(ShapeAnalysis_TransferParameters) GetTransferParamTool();
  
  //! Sets mode for splitting 3d curves from edges.
  //! 0 - only curve 3d from free edges.
  //! 1 - only curve 3d from shared edges.
  //! 2 -  all curve 3d.
  Standard_EXPORT void SetEdgeMode (const Standard_Integer EdgeMode);
  
  //! Sets tool for fixing small curves with specified min tolerance;
  Standard_EXPORT void SetFixSmallCurveTool (const Handle(ShapeUpgrade_FixSmallCurves)& FixSmallCurvesTool);
  
  //! Returns tool for fixing small curves
  Standard_EXPORT Handle(ShapeUpgrade_FixSmallCurves) GetFixSmallCurveTool() const;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_WireDivide,ShapeUpgrade_Tool)

protected:

  
  //! Returns the tool for splitting 3D curves.
  Standard_EXPORT virtual Handle(ShapeUpgrade_SplitCurve3d) GetSplitCurve3dTool() const;
  
  //! Returns the tool for splitting pcurves.
  Standard_EXPORT virtual Handle(ShapeUpgrade_SplitCurve2d) GetSplitCurve2dTool() const;

  TopoDS_Face myFace;
  TopoDS_Wire myWire;
  Standard_Integer myStatus;
  Standard_Integer myEdgeMode;


private:


  Handle(ShapeUpgrade_SplitCurve3d) mySplitCurve3dTool;
  Handle(ShapeUpgrade_SplitCurve2d) mySplitCurve2dTool;
  Handle(ShapeUpgrade_EdgeDivide) myEdgeDivide;
  Handle(ShapeAnalysis_TransferParameters) myTransferParamTool;
  Handle(ShapeUpgrade_FixSmallCurves) myFixSmallCurveTool;


};







#endif // _ShapeUpgrade_WireDivide_HeaderFile
