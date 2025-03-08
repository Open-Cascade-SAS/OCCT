// Created on: 1998-08-12
// Created by: DATA EXCHANGE TEAM
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

#ifndef _ShapeFix_Shape_HeaderFile
#define _ShapeFix_Shape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
#include <ShapeExtend_Status.hxx>
#include <Message_ProgressRange.hxx>

class ShapeFix_Solid;
class ShapeFix_Shell;
class ShapeFix_Face;
class ShapeFix_Wire;
class ShapeFix_Edge;
class ShapeExtend_BasicMsgRegistrator;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeFix_Shape;
DEFINE_STANDARD_HANDLE(ShapeFix_Shape, ShapeFix_Root)

//! Fixing shape in general
class ShapeFix_Shape : public ShapeFix_Root
{

public:
  //! Empty Constructor
  Standard_EXPORT ShapeFix_Shape();

  //! Initislises by shape.
  Standard_EXPORT ShapeFix_Shape(const TopoDS_Shape& shape);

  //! Initislises by shape.
  Standard_EXPORT void Init(const TopoDS_Shape& shape);

  //! Iterates on sub- shape and performs fixes
  Standard_EXPORT Standard_Boolean
    Perform(const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Returns resulting shape
  Standard_EXPORT TopoDS_Shape Shape() const;

  //! Returns tool for fixing solids.
  Handle(ShapeFix_Solid) FixSolidTool() const;

  //! Returns tool for fixing shells.
  Handle(ShapeFix_Shell) FixShellTool() const;

  //! Returns tool for fixing faces.
  Handle(ShapeFix_Face) FixFaceTool() const;

  //! Returns tool for fixing wires.
  Handle(ShapeFix_Wire) FixWireTool() const;

  //! Returns tool for fixing edges.
  Handle(ShapeFix_Edge) FixEdgeTool() const;

  //! Returns the status of the last Fix.
  //! This can be a combination of the following flags:
  //! ShapeExtend_DONE1: some free edges were fixed
  //! ShapeExtend_DONE2: some free wires were fixed
  //! ShapeExtend_DONE3: some free faces were fixed
  //! ShapeExtend_DONE4: some free shells were fixed
  //! ShapeExtend_DONE5: some free solids were fixed
  //! ShapeExtend_DONE6: shapes in compound(s) were fixed
  Standard_EXPORT Standard_Boolean Status(const ShapeExtend_Status status) const;

  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator(
    const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg) Standard_OVERRIDE;

  //! Sets basic precision value (also to FixSolidTool)
  Standard_EXPORT virtual void SetPrecision(const Standard_Real preci) Standard_OVERRIDE;

  //! Sets minimal allowed tolerance (also to FixSolidTool)
  Standard_EXPORT virtual void SetMinTolerance(const Standard_Real mintol) Standard_OVERRIDE;

  //! Sets maximal allowed tolerance (also to FixSolidTool)
  Standard_EXPORT virtual void SetMaxTolerance(const Standard_Real maxtol) Standard_OVERRIDE;

  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Solid, by default True.
  Standard_Integer& FixSolidMode();

  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Shell, by default True.
  Standard_Integer& FixFreeShellMode();

  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Face, by default True.
  Standard_Integer& FixFreeFaceMode();

  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Wire, by default True.
  Standard_Integer& FixFreeWireMode();

  //! Returns (modifiable) the mode for applying
  //! ShapeFix::SameParameter after all fixes, by default True.
  Standard_Integer& FixSameParameterMode();

  //! Returns (modifiable) the mode for applying
  //! ShapeFix::FixVertexPosition before all fixes, by default False.
  Standard_Integer& FixVertexPositionMode();

  //! Returns (modifiable) the mode for fixing tolerances of vertices on whole shape
  //! after performing all fixes
  Standard_Integer& FixVertexTolMode();

  DEFINE_STANDARD_RTTIEXT(ShapeFix_Shape, ShapeFix_Root)

protected:
  //! Fixes same parameterization problem on the passed shape
  //! by updating tolerances of the corresponding topological
  //! entities.
  Standard_EXPORT void SameParameter(
    const TopoDS_Shape&          shape,
    const Standard_Boolean       enforce,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  TopoDS_Shape           myResult;
  Handle(ShapeFix_Solid) myFixSolid;
  TopTools_MapOfShape    myMapFixingShape;
  Standard_Integer       myFixSolidMode;
  Standard_Integer       myFixShellMode;
  Standard_Integer       myFixFaceMode;
  Standard_Integer       myFixWireMode;
  Standard_Integer       myFixSameParameterMode;
  Standard_Integer       myFixVertexPositionMode;
  Standard_Integer       myFixVertexTolMode;
  Standard_Integer       myStatus;

private:
};

#include <ShapeFix_Shape.lxx>

#endif // _ShapeFix_Shape_HeaderFile
