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

#ifndef _ShapeFix_Face_HeaderFile
#define _ShapeFix_Face_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <ShapeFix_Root.hxx>
#include <Standard_Real.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <ShapeFix_DataMapOfShapeBox2d.hxx>
#include <ShapeExtend_Status.hxx>
class ShapeAnalysis_Surface;
class ShapeFix_Wire;
class Geom_Surface;
class ShapeExtend_BasicMsgRegistrator;
class TopoDS_Wire;
class ShapeExtend_WireData;
class TopoDS_Vertex;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeFix_Face;
DEFINE_STANDARD_HANDLE(ShapeFix_Face, ShapeFix_Root)

//! This operator allows to perform various fixes on face
//! and its wires: fixes provided by ShapeFix_Wire,
//! fixing orientation of wires, addition of natural bounds,
//! fixing of missing seam edge,
//! and detection and removal of null-area wires
class ShapeFix_Face : public ShapeFix_Root
{

public:

  
  //! Creates an empty tool
  Standard_EXPORT ShapeFix_Face();
  
  //! Creates a tool and loads a face
  Standard_EXPORT ShapeFix_Face(const TopoDS_Face& face);
  
  //! Sets all modes to default
  Standard_EXPORT virtual void ClearModes();
  
  //! Loads a whole face already created, with its wires, sense and
  //! location
  Standard_EXPORT void Init (const TopoDS_Face& face);
  
  //! Starts the creation of the face
  //! By default it will be FORWARD, or REVERSED if <fwd> is False
  Standard_EXPORT void Init (const Handle(Geom_Surface)& surf, const Standard_Real preci, const Standard_Boolean fwd = Standard_True);
  
  //! Starts the creation of the face
  //! By default it will be FORWARD, or REVERSED if <fwd> is False
  Standard_EXPORT void Init (const Handle(ShapeAnalysis_Surface)& surf, const Standard_Real preci, const Standard_Boolean fwd = Standard_True);
  
  //! Sets message registrator
  Standard_EXPORT virtual void SetMsgRegistrator (const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg) Standard_OVERRIDE;
  
  //! Sets basic precision value (also to FixWireTool)
  Standard_EXPORT virtual void SetPrecision (const Standard_Real preci) Standard_OVERRIDE;
  
  //! Sets minimal allowed tolerance (also to FixWireTool)
  Standard_EXPORT virtual void SetMinTolerance (const Standard_Real mintol) Standard_OVERRIDE;
  
  //! Sets maximal allowed tolerance (also to FixWireTool)
  Standard_EXPORT virtual void SetMaxTolerance (const Standard_Real maxtol) Standard_OVERRIDE;
  
  //! Returns (modifiable) the mode for applying fixes of
  //! ShapeFix_Wire, by default True.
    Standard_Integer& FixWireMode();
  
  //! Returns (modifiable) the fix orientation mode, by default
  //! True. If True, wires oriented to border limited square.
    Standard_Integer& FixOrientationMode();
  
  //! Returns (modifiable) the add natural bound mode.
  //! If true, natural boundary is added on faces that miss them.
  //! Default is False for faces with single wire (they are
  //! handled by FixOrientation in that case) and True for others.
    Standard_Integer& FixAddNaturalBoundMode();
  
  //! Returns (modifiable) the fix missing seam mode, by default
  //! True. If True, tries to insert seam is missed.
    Standard_Integer& FixMissingSeamMode();
  
  //! Returns (modifiable) the fix small area wire mode, by default
  //! False. If True, drops small wires.
    Standard_Integer& FixSmallAreaWireMode();
  
  //! Returns (modifiable) the remove face with small area, by default
  //! False. If True, drops faces with small outer wires.
    Standard_Integer& RemoveSmallAreaFaceMode();
  
  //! Returns (modifiable) the fix intersecting wires mode
  //! by default True.
    Standard_Integer& FixIntersectingWiresMode();
  
  //! Returns (modifiable) the fix loop wires mode
  //! by default True.
    Standard_Integer& FixLoopWiresMode();
  
  //! Returns (modifiable) the fix split face mode
  //! by default True.
    Standard_Integer& FixSplitFaceMode();
  
  //! Returns (modifiable) the auto-correct precision mode
  //! by default False.
    Standard_Integer& AutoCorrectPrecisionMode();
  
  //! Returns (modifiable) the activation flag for periodic
  //! degenerated fix. False by default.
    Standard_Integer& FixPeriodicDegeneratedMode();
  
  //! Returns a face which corresponds to the current state
  //! Warning: The finally produced face may be another one ... but with the
  //! same support
    TopoDS_Face Face() const;
  
  //! Returns resulting shape (Face or Shell if split)
  //! To be used instead of Face() if FixMissingSeam involved
    TopoDS_Shape Result() const;
  
  //! Add a wire to current face using BRep_Builder.
  //! Wire is added without taking into account orientation of face
  //! (as if face were FORWARD).
  Standard_EXPORT void Add (const TopoDS_Wire& wire);
  
  //! Performs all the fixes, depending on modes
  //! Function Status returns the status of last call to Perform()
  //! ShapeExtend_OK   : face was OK, nothing done
  //! ShapeExtend_DONE1: some wires are fixed
  //! ShapeExtend_DONE2: orientation of wires fixed
  //! ShapeExtend_DONE3: missing seam added
  //! ShapeExtend_DONE4: small area wire removed
  //! ShapeExtend_DONE5: natural bounds added
  //! ShapeExtend_FAIL1: some fails during fixing wires
  //! ShapeExtend_FAIL2: cannot fix orientation of wires
  //! ShapeExtend_FAIL3: cannot add missing seam
  //! ShapeExtend_FAIL4: cannot remove small area wire
  Standard_EXPORT Standard_Boolean Perform();
  
  //! Fixes orientation of wires on the face
  //! It tries to make all wires lie outside all others (according
  //! to orientation) by reversing orientation of some of them.
  //! If face lying on sphere or torus has single wire and
  //! AddNaturalBoundMode is True, that wire is not reversed in
  //! any case (supposing that natural bound will be added).
  //! Returns True if wires were reversed
  Standard_EXPORT Standard_Boolean FixOrientation();
  
  //! Fixes orientation of wires on the face
  //! It tries to make all wires lie outside all others (according
  //! to orientation) by reversing orientation of some of them.
  //! If face lying on sphere or torus has single wire and
  //! AddNaturalBoundMode is True, that wire is not reversed in
  //! any case (supposing that natural bound will be added).
  //! Returns True if wires were reversed
  //! OutWires return information about out wires + list of
  //! internal wires for each (for performing split face).
  Standard_EXPORT Standard_Boolean FixOrientation (TopTools_DataMapOfShapeListOfShape& MapWires);
  
  //! Adds natural boundary on face if it is missing.
  //! Two cases are supported:
  //! - face has no wires
  //! - face lies on geometrically double-closed surface
  //! (sphere or torus) and none of wires is left-oriented
  //! Returns True if natural boundary was added
  Standard_EXPORT Standard_Boolean FixAddNaturalBound();
  
  //! Detects and fixes the special case when face on a closed
  //! surface is given by two wires closed in 3d but with gap in 2d.
  //! In that case it creates a new wire from the two, and adds a
  //! missing seam edge
  //! Returns True if missing seam was added
  Standard_EXPORT Standard_Boolean FixMissingSeam();
  
  //! Detects wires with small area (that is less than
  //! 100*Precision::PConfusion(). Removes these wires if they are internal.
  //! Returns : True if at least one small wire removed,
  //! False if does nothing.
  Standard_EXPORT Standard_Boolean FixSmallAreaWire (const Standard_Boolean theIsRemoveSmallFace);
  
  //! Detects if wire has a loop and fixes this situation by splitting on the few parts.
  //! if wire has a loops and it was split Status was set to value ShapeExtend_DONE6.
  Standard_EXPORT Standard_Boolean FixLoopWire (TopTools_SequenceOfShape& aResWires);
  
  //! Detects and fixes the special case when face has more than one wire
  //! and this wires have intersection point
  Standard_EXPORT Standard_Boolean FixIntersectingWires();
  
  //! If wire contains two coincidence edges it must be removed
  //! Queries on status after Perform()
  Standard_EXPORT Standard_Boolean FixWiresTwoCoincEdges();
  
  //! Split face if there are more than one out wire
  //! using inrormation after FixOrientation()
  Standard_EXPORT Standard_Boolean FixSplitFace (const TopTools_DataMapOfShapeListOfShape& MapWires);
  
  //! Fixes topology for a specific case when face is composed
  //! by a single wire belting a periodic surface. In that case
  //! a degenerated edge is reconstructed in the degenerated pole
  //! of the surface. Initial wire gets consistent orientation.
  //! Must be used in couple and before FixMissingSeam routine
  Standard_EXPORT Standard_Boolean FixPeriodicDegenerated();
  
  //! Returns the status of last call to Perform()
  //! ShapeExtend_OK   : face was OK, nothing done
  //! ShapeExtend_DONE1: some wires are fixed
  //! ShapeExtend_DONE2: orientation of wires fixed
  //! ShapeExtend_DONE3: missing seam added
  //! ShapeExtend_DONE4: small area wire removed
  //! ShapeExtend_DONE5: natural bounds added
  //! ShapeExtend_DONE8: face may be splited
  //! ShapeExtend_FAIL1: some fails during fixing wires
  //! ShapeExtend_FAIL2: cannot fix orientation of wires
  //! ShapeExtend_FAIL3: cannot add missing seam
  //! ShapeExtend_FAIL4: cannot remove small area wire
    Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Returns tool for fixing wires.
    Handle(ShapeFix_Wire) FixWireTool();




  DEFINE_STANDARD_RTTIEXT(ShapeFix_Face,ShapeFix_Root)

protected:


  Handle(ShapeAnalysis_Surface) mySurf;
  TopoDS_Face myFace;
  TopoDS_Shape myResult;
  Handle(ShapeFix_Wire) myFixWire;
  Standard_Boolean myFwd;
  Standard_Integer myStatus;


private:

  
  Standard_EXPORT Standard_Boolean SplitEdge (const Handle(ShapeExtend_WireData)& sewd, const Standard_Integer num, const Standard_Real param, const TopoDS_Vertex& vert, const Standard_Real preci, ShapeFix_DataMapOfShapeBox2d& boxes);
  
  Standard_EXPORT Standard_Boolean SplitEdge (const Handle(ShapeExtend_WireData)& sewd, const Standard_Integer num, const Standard_Real param1, const Standard_Real param2, const TopoDS_Vertex& vert, const Standard_Real preci, ShapeFix_DataMapOfShapeBox2d& boxes);

  Standard_Integer myFixWireMode;
  Standard_Integer myFixOrientationMode;
  Standard_Integer myFixAddNaturalBoundMode;
  Standard_Integer myFixMissingSeamMode;
  Standard_Integer myFixSmallAreaWireMode;
  Standard_Integer myRemoveSmallAreaFaceMode;
  Standard_Integer myFixLoopWiresMode;
  Standard_Integer myFixIntersectingWiresMode;
  Standard_Integer myFixSplitFaceMode;
  Standard_Integer myAutoCorrectPrecisionMode;
  Standard_Integer myFixPeriodicDegenerated;


};


#include <ShapeFix_Face.lxx>





#endif // _ShapeFix_Face_HeaderFile
