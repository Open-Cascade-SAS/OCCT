// Created on: 1998-02-18
// Created by: Pierre BARRAS
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

#ifndef _ShapeUpgrade_FaceDivide_HeaderFile
#define _ShapeUpgrade_FaceDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <ShapeUpgrade_Tool.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeUpgrade_SplitSurface;
class ShapeUpgrade_WireDivide;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeUpgrade_FaceDivide;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_FaceDivide, ShapeUpgrade_Tool)

//! Divides a Face (both edges in the wires, by splitting
//! curves and pcurves, and the face itself, by splitting
//! supporting surface) according to splitting criteria.
//! * The domain of the face to divide is defined by the PCurves
//! of the wires on the Face.
//!
//! * all the PCurves are supposed to be defined (in the parametric
//! space of the supporting surface).
//!
//! The result is available after the call to the Build method.
//! It is a Shell containing all the resulting Faces.
//!
//! All the modifications made during splitting are recorded in the
//! external context (ShapeBuild_ReShape).
class ShapeUpgrade_FaceDivide : public ShapeUpgrade_Tool
{

public:

  
  //! Creates empty  constructor.
  Standard_EXPORT ShapeUpgrade_FaceDivide();
  
  //! Initialize by a Face.
  Standard_EXPORT ShapeUpgrade_FaceDivide(const TopoDS_Face& F);
  
  //! Initialize by a Face.
  Standard_EXPORT void Init (const TopoDS_Face& F);
  
  //! Purpose sets mode for trimming (segment) surface by
  //! wire UV bounds.
  Standard_EXPORT void SetSurfaceSegmentMode (const Standard_Boolean Segment);
  
  //! Performs splitting and computes the resulting shell
  //! The context is used to keep track of former splittings
  //! in order to keep sharings. It is updated according to
  //! modifications made.
  Standard_EXPORT virtual Standard_Boolean Perform();
  
  //! Performs splitting of surface and computes the shell
  //! from source face.
  Standard_EXPORT virtual Standard_Boolean SplitSurface();
  
  //! Performs splitting of curves of all the edges in the
  //! shape and divides these edges.
  Standard_EXPORT virtual Standard_Boolean SplitCurves();
  
  //! Gives the resulting Shell, or Face, or Null shape if not done.
  Standard_EXPORT TopoDS_Shape Result() const;
  
  //! Queries the status of last call to Perform
  //! OK   : no splitting was done (or no call to Perform)
  //! DONE1: some edges were splitted
  //! DONE2: surface was splitted
  //! DONE3: surface was modified without splitting
  //! FAIL1: some fails encountered during splitting wires
  //! FAIL2: face cannot be splitted
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Sets the tool for splitting surfaces.
  Standard_EXPORT void SetSplitSurfaceTool (const Handle(ShapeUpgrade_SplitSurface)& splitSurfaceTool);
  
  //! Sets the tool for dividing edges on Face.
  Standard_EXPORT void SetWireDivideTool (const Handle(ShapeUpgrade_WireDivide)& wireDivideTool);
  
  //! Returns the tool for splitting surfaces.
  //! This tool must be already initialized.
  Standard_EXPORT virtual Handle(ShapeUpgrade_SplitSurface) GetSplitSurfaceTool() const;
  
  //! Returns the tool for dividing edges on Face.
  //! This tool must be already initialized.
  Standard_EXPORT virtual Handle(ShapeUpgrade_WireDivide) GetWireDivideTool() const;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_FaceDivide,ShapeUpgrade_Tool)

protected:


  TopoDS_Face myFace;
  TopoDS_Shape myResult;
  Standard_Boolean mySegmentMode;
  Standard_Integer myStatus;


private:


  Handle(ShapeUpgrade_SplitSurface) mySplitSurfaceTool;
  Handle(ShapeUpgrade_WireDivide) myWireDivideTool;


};







#endif // _ShapeUpgrade_FaceDivide_HeaderFile
