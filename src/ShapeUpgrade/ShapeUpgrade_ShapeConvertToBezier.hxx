// Created on: 1999-05-14
// Created by: Pavel DURANDIN
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

#ifndef _ShapeUpgrade_ShapeConvertToBezier_HeaderFile
#define _ShapeUpgrade_ShapeConvertToBezier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <ShapeUpgrade_ShapeDivide.hxx>
class TopoDS_Shape;
class ShapeUpgrade_FaceDivide;
class Message_Msg;


//! API class for performing conversion of 3D, 2D curves to bezier curves
//! and surfaces to bezier based surfaces (
//! bezier surface,
//! surface of revolution based on bezier curve,
//! offset surface based on any previous type).
class ShapeUpgrade_ShapeConvertToBezier  : public ShapeUpgrade_ShapeDivide
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_ShapeConvertToBezier();
  
  //! Initialize by a Shape.
  Standard_EXPORT ShapeUpgrade_ShapeConvertToBezier(const TopoDS_Shape& S);
  
  //! Sets mode for conversion 2D curves to bezier.
    void Set2dConversion (const Standard_Boolean mode);
  
  //! Returns the 2D conversion mode.
    Standard_Boolean Get2dConversion() const;
  
  //! Sets mode for conversion 3d curves to bezier.
    void Set3dConversion (const Standard_Boolean mode);
  
  //! Returns the 3D conversion mode.
    Standard_Boolean Get3dConversion() const;
  
  //! Sets mode for conversion surfaces curves to
  //! bezier basis.
    void SetSurfaceConversion (const Standard_Boolean mode);
  
  //! Returns the surface conversion mode.
    Standard_Boolean GetSurfaceConversion() const;
  
  //! Sets mode for conversion Geom_Line to bezier.
    void Set3dLineConversion (const Standard_Boolean mode);
  
  //! Returns the Geom_Line conversion mode.
    Standard_Boolean Get3dLineConversion() const;
  
  //! Sets mode for conversion Geom_Circle to bezier.
    void Set3dCircleConversion (const Standard_Boolean mode);
  
  //! Returns the Geom_Circle conversion mode.
    Standard_Boolean Get3dCircleConversion() const;
  
  //! Sets mode for conversion Geom_Conic to bezier.
    void Set3dConicConversion (const Standard_Boolean mode);
  
  //! Returns the Geom_Conic conversion mode.
    Standard_Boolean Get3dConicConversion() const;
  
  //! Sets mode for conversion Geom_Plane to Bezier
    void SetPlaneMode (const Standard_Boolean mode);
  
  //! Returns the Geom_Pline conversion mode.
    Standard_Boolean GetPlaneMode() const;
  
  //! Sets mode for conversion Geom_SurfaceOfRevolution to Bezier
    void SetRevolutionMode (const Standard_Boolean mode);
  
  //! Returns the Geom_SurfaceOfRevolution conversion mode.
    Standard_Boolean GetRevolutionMode() const;
  
  //! Sets mode for conversion Geom_SurfaceOfLinearExtrusion to Bezier
    void SetExtrusionMode (const Standard_Boolean mode);
  
  //! Returns the Geom_SurfaceOfLinearExtrusion conversion mode.
    Standard_Boolean GetExtrusionMode() const;
  
  //! Sets mode for conversion Geom_BSplineSurface to Bezier
    void SetBSplineMode (const Standard_Boolean mode);
  
  //! Returns the Geom_BSplineSurface conversion mode.
    Standard_Boolean GetBSplineMode() const;
  
  //! Performs converting and computes the resulting shape
  Standard_EXPORT virtual Standard_Boolean Perform (const Standard_Boolean newContext = Standard_True) Standard_OVERRIDE;




protected:

  
  //! Returns the tool for dividing faces.
  Standard_EXPORT virtual Handle(ShapeUpgrade_FaceDivide) GetSplitFaceTool() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Message_Msg GetFaceMsg() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Message_Msg GetWireMsg() const Standard_OVERRIDE;
  
  //! Returns a message decsribing modification of a shape.
  Standard_EXPORT virtual Message_Msg GetEdgeMsg() const Standard_OVERRIDE;




private:



  Standard_Boolean my2dMode;
  Standard_Boolean my3dMode;
  Standard_Boolean mySurfaceMode;
  Standard_Boolean my3dLineMode;
  Standard_Boolean my3dCircleMode;
  Standard_Boolean my3dConicMode;
  Standard_Boolean myPlaneMode;
  Standard_Boolean myRevolutionMode;
  Standard_Boolean myExtrusionMode;
  Standard_Boolean myBSplineMode;
  Standard_Integer myLevel;


};


#include <ShapeUpgrade_ShapeConvertToBezier.lxx>





#endif // _ShapeUpgrade_ShapeConvertToBezier_HeaderFile
