// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Prs3d_Drawer_HeaderFile
#define _Prs3d_Drawer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <Handle_Prs3d_IsoAspect.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_Prs3d_LineAspect.hxx>
#include <Handle_Prs3d_TextAspect.hxx>
#include <Handle_Prs3d_ShadingAspect.hxx>
#include <Quantity_Length.hxx>
#include <Aspect_TypeOfDeflection.hxx>
#include <Standard_Real.hxx>
#include <Handle_Prs3d_PointAspect.hxx>
#include <Prs3d_VertexDrawMode.hxx>
#include <Handle_Prs3d_PlaneAspect.hxx>
#include <Handle_Prs3d_ArrowAspect.hxx>
#include <Handle_Prs3d_DatumAspect.hxx>
#include <Handle_Prs3d_DimensionAspect.hxx>
#include <Prs3d_DimensionUnits.hxx>
#include <Prs3d_TypeOfHLR.hxx>
#include <MMgt_TShared.hxx>

class Prs3d_IsoAspect;
class Prs3d_LineAspect;
class Prs3d_TextAspect;
class Prs3d_ShadingAspect;
class Prs3d_PointAspect;
class Prs3d_PlaneAspect;
class Prs3d_ArrowAspect;
class Prs3d_DatumAspect;
class Prs3d_DimensionAspect;
class TCollection_AsciiString;

DEFINE_STANDARD_HANDLE(Prs3d_Drawer, MMgt_TShared)

//! A graphic attribute manager which governs how
//! objects such as color, width, line thickness and deflection are displayed.
//! A drawer includes an instance of the Aspect classes with particular default values.
class Prs3d_Drawer : public MMgt_TShared
{

public:

  //! Default constructor.
  Standard_EXPORT Prs3d_Drawer();

  //! Sets the type of chordal deflection.
  //! This indicates whether the deflection value is absolute or relative to the size of the object.
  Standard_EXPORT void SetTypeOfDeflection (const Aspect_TypeOfDeflection theTypeOfDeflection);

  //! Returns the type of chordal deflection.
  //! This indicates whether the deflection value is absolute or relative to the size of the object.
  Aspect_TypeOfDeflection TypeOfDeflection() const
  {
    return HasOwnTypeOfDeflection() || myLink.IsNull()
         ? myTypeOfDeflection
         : myLink->TypeOfDeflection();
  }

  //! Returns true if the drawer has a type of deflection setting active.
  Standard_Boolean HasOwnTypeOfDeflection() const { return myHasOwnTypeOfDeflection; }

  //! Defines the maximal chordial deviation when drawing any curve.
  //! Even if the type of deviation is set to TOD_Relative, this value is used by: 
  //!   Prs3d_DeflectionCurve
  //!   Prs3d_WFDeflectionSurface
  //!   Prs3d_WFDeflectionRestrictedFace
  Standard_EXPORT void SetMaximalChordialDeviation (const Quantity_Length theChordialDeviation);

  //! Returns the maximal chordal deviation. The default value is 0.1.
  //! Drawings of curves or patches are made with respect to an absolute maximal chordal deviation.
  Quantity_Length MaximalChordialDeviation() const
  {
    return HasOwnMaximalChordialDeviation() || myLink.IsNull()
         ? myChordialDeviation
         : myLink->MaximalChordialDeviation();
  }

  //! Returns true if the drawer has a maximal chordial deviation setting active.
  Standard_Boolean HasOwnMaximalChordialDeviation() const { return myHasOwnChordialDeviation; }

  //! Sets the type of HLR algorithm used by drawer's interactive objects
  Standard_EXPORT void SetTypeOfHLR (const Prs3d_TypeOfHLR theTypeOfHLR);

  //! Returns the type of HLR algorithm currently in use.
  Standard_EXPORT Prs3d_TypeOfHLR TypeOfHLR();

  //! Returns true if the type of HLR is not equal to Prs3d_TOH_NotSet.
  Standard_Boolean HasOwnTypeOfHLR() const { return (myTypeOfHLR != Prs3d_TOH_NotSet); }

  //! Defines the maximum value allowed for the first and last
  //! parameters of an infinite curve.
  Standard_EXPORT void SetMaximalParameterValue (const Standard_Real theValue);

  //! Sets the maximum value allowed for the first and last
  //! parameters of an infinite curve. By default, this value is 500000.
  Standard_Real MaximalParameterValue() const
  {
    return HasOwnMaximalParameterValue() || myLink.IsNull()
         ? myMaximalParameterValue
         : myLink->MaximalParameterValue();
  }

  //! Returns true if the drawer has a maximum value allowed for the first and last
  //! parameters of an infinite curve setting active.
  Standard_Boolean HasOwnMaximalParameterValue() const { return myHasOwnMaximalParameterValue; }

  //! Sets IsoOnPlane on or off by setting the parameter theIsEnabled to true or false.
  Standard_EXPORT void SetIsoOnPlane (const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of isos on planes is enabled.
  Standard_Boolean IsoOnPlane() const
  {
    return HasOwnIsoOnPlane() || myLink.IsNull()
         ? myIsoOnPlane
         : myLink->IsoOnPlane();
  }

  //! Returns true if the drawer has IsoOnPlane setting active.
  Standard_Boolean HasOwnIsoOnPlane() const { return myHasOwnIsoOnPlane; }

  //! Sets the discretisation parameter theValue.
  Standard_EXPORT void SetDiscretisation (const Standard_Integer theValue);

  //! Returns the discretisation setting. 
  Standard_Integer Discretisation() const
  {
    return HasOwnDiscretisation() || myLink.IsNull()
         ? myNbPoints
         : myLink->Discretisation();
  }

  //! Returns true if the drawer has discretisation setting active.
  Standard_Boolean HasOwnDiscretisation() const { return myHasOwnNbPoints; }

  //! Sets the deviation coefficient theCoefficient.
  //! Also sets the hasOwnDeviationCoefficient flag to Standard_True and myPreviousDeviationCoefficient
  Standard_EXPORT void SetDeviationCoefficient (const Standard_Real theCoefficient);

  //! Returns the deviation coefficient.
  //! Drawings of curves or patches are made with respect
  //! to a maximal chordal deviation. A Deviation coefficient
  //! is used in the shading display mode. The shape is
  //! seen decomposed into triangles. These are used to
  //! calculate reflection of light from the surface of the
  //! object. The triangles are formed from chords of the
  //! curves in the shape. The deviation coefficient gives
  //! the highest value of the angle with which a chord can
  //! deviate from a tangent to a   curve. If this limit is
  //! reached, a new triangle is begun.
  //! This deviation is absolute and is set through the
  //! method: SetMaximalChordialDeviation. The default value is 0.001.
  //! In drawing shapes, however, you are allowed to ask
  //! for a relative deviation. This deviation will be:
  //! SizeOfObject * DeviationCoefficient.
  Standard_Real DeviationCoefficient() const
  {
    return HasOwnDeviationCoefficient() || myLink.IsNull()
         ? myDeviationCoefficient
         : myLink->DeviationCoefficient();
  }

  //! Sets the hasOwnDeviationCoefficient flag to Standard_False
  void SetDeviationCoefficient()
  {
    myHasOwnDeviationCoefficient = Standard_False;
  }

  //! Returns true if there is a local setting for deviation
  //! coefficient in this framework for a specific interactive object.
  Standard_Boolean HasOwnDeviationCoefficient() const { return myHasOwnDeviationCoefficient; }

  //! Saves the previous value used for the chordal
  //! deviation coefficient. 
  Standard_Real PreviousDeviationCoefficient() const
  {
    return myHasOwnDeviationCoefficient
         ? myPreviousDeviationCoefficient
         : 0.0;
  }

  //! Sets the deviation coefficient aCoefficient for removal
  //! of hidden lines created by different viewpoints in
  //! different presentations. The Default value is 0.02.
  //! Also sets the hasOwnHLRDeviationCoefficient flag to Standard_True and myPreviousHLRDeviationCoefficient
  Standard_EXPORT void SetHLRDeviationCoefficient (const Standard_Real theCoefficient);

  //! Returns the real number value of the hidden line
  //! removal deviation coefficient in this framework, if the flag
  //! hasOwnHLRDeviationCoefficient is true or there is no Link.
  //! Else the shape's HLR deviation coefficient is used.
  //! A Deviation coefficient is used in the shading display
  //! mode. The shape is seen decomposed into triangles.
  //! These are used to calculate reflection of light from the
  //! surface of the object.
  //! The triangles are formed from chords of the curves in
  //! the shape. The deviation coefficient give the highest
  //! value of the angle with which a chord can deviate
  //! from a tangent to a curve. If this limit is reached, a new triangle is begun.
  //! To find the hidden lines, hidden line display mode
  //! entails recalculation of the view at each different projector perspective.
  //! Since hidden lines entail calculations of more than
  //! usual complexity to decompose them into these
  //! triangles, a deviation coefficient allowing greater
  //! tolerance is used. This increases efficiency in calculation.
  //! The Default value is 0.02.
  Standard_Real HLRDeviationCoefficient() const
  {
    return HasOwnHLRDeviationCoefficient() || myLink.IsNull()
         ? myHLRDeviationCoefficient
         : myLink->HLRDeviationCoefficient();
  }

  //! Sets the hasOwnHLRDeviationCoefficient flag to Standard_False 
  void SetHLRDeviationCoefficient()
  {
    myHasOwnHLRDeviationCoefficient = Standard_False;
  }

  //! Returns true if the there is a setting for HLR deviation
  //! coefficient in this framework for a specific interactive object.
  Standard_Boolean HasOwnHLRDeviationCoefficient() const { return myHasOwnHLRDeviationCoefficient; }

  //! Returns the previous value of the hidden line removal deviation coefficient.
  Standard_Real PreviousHLRDeviationCoefficient() const
  {
    return myHasOwnHLRDeviationCoefficient
         ? myPreviousHLRDeviationCoefficient
         : 0.0;
  }

  //! Sets the deviation angle theAngle.
  //! Also sets the hasOwnDeviationAngle flag to Standard_True, and myPreviousDeviationAngle.
  Standard_EXPORT void SetDeviationAngle (const Standard_Real theAngle);

  //! Returns the value for deviation angle.
  Standard_Real DeviationAngle() const
  {
    return HasOwnDeviationAngle() || myLink.IsNull()
         ? myDeviationAngle
         : myLink->DeviationAngle();
  }

  //! Sets the hasOwnDeviationAngle flag to Standard_False
  void SetDeviationAngle()
  {
    myHasOwnDeviationAngle = Standard_False;
  }

  //! Returns true if the there is a local setting for deviation
  //! angle in this framework for a specific interactive object.
  Standard_Boolean HasOwnDeviationAngle() const { return myHasOwnDeviationAngle; }

  //! Returns the previous deviation angle
  Standard_Real PreviousDeviationAngle() const
  {
    return myHasOwnDeviationAngle
         ? myPreviousDeviationAngle
         : 0.0;
  }

  //! Sets anAngle, the angle of maximum chordal deviation for removal of hidden lines created by
  //! different viewpoints in different presentations.
  //! The default value is 20 * M_PI / 180.
  //! Also sets the hasOwnHLRDeviationAngle flag to Standard_True and myPreviousHLRDeviationAngle.
  Standard_EXPORT void SetHLRAngle (const Standard_Real theAngle);

  //! Returns the real number value of the deviation angle
  //! in hidden line removal views. The default value is 20 * M_PI / 180.
  Standard_Real HLRAngle() const
  {
    return HasOwnHLRDeviationAngle() || myLink.IsNull()
         ? myHLRAngle
         : myLink->HLRAngle();
  }

  //! Sets the hasOwnHLRDeviationAngle flag to Standard_False
  void SetHLRAngle()
  {
    myHasOwnHLRDeviationAngle = Standard_False;
  }

  //! Returns true if the there is a setting for HLR deviation
  //! angle in this framework for a specific interactive object.
  Standard_Boolean HasOwnHLRDeviationAngle() const { return myHasOwnHLRDeviationAngle; }

  //! Returns the previous value of the HLR deviation angle.
  Standard_Real PreviousHLRDeviationAngle() const
  {
    return myHasOwnHLRDeviationAngle
         ? myPreviousHLRDeviationAngle
         : 0.0;
  }

  //! Sets IsAutoTriangulated on or off by setting the parameter theIsEnabled to true or false.
  //! If this flag is True automatic re-triangulation with deflection-check logic will be applied.
  //! Else this feature will be disable and triangulation is expected to be computed by application itself
  //! and no shading presentation at all if unavailable.
  Standard_EXPORT void SetAutoTriangulation (const Standard_Boolean theIsEnabled);

  //! Returns True if automatic triangulation is enabled.
  Standard_Boolean IsAutoTriangulation() const
  {
    return HasOwnIsAutoTriangulation() || myLink.IsNull()
         ? myIsAutoTriangulated
         : myLink->IsAutoTriangulation();
  }

  //! Returns true if the drawer has IsoOnPlane setting active.
  Standard_Boolean HasOwnIsAutoTriangulation() const { return myHasOwnIsAutoTriangulated; }

  //! Defines the attributes which are used when drawing an
  //! U isoparametric curve of a face. Defines the number
  //! of U isoparametric curves to be drawn for a single face.
  //! The LineAspect for U isoparametric lines can be edited
  //! (methods SetColor, SetTypeOfLine, SetWidth, SetNumber)
  //! The default values are:
  //!   COLOR       : Quantity_NOC_GRAY75
  //!   TYPE OF LINE: Aspect_TOL_SOLID
  //!   WIDTH       : 0.5
  //!
  //! These attributes are used by the following algorithms:
  //!   Prs3d_WFDeflectionSurface
  //!   Prs3d_WFDeflectionRestrictedFace
  Standard_EXPORT const Handle(Prs3d_IsoAspect)& UIsoAspect();

  Standard_EXPORT void SetUIsoAspect (const Handle(Prs3d_IsoAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! UIso aspect that overrides the one in the link.
  Standard_Boolean HasOwnUIsoAspect() const { return myHasOwnUIsoAspect; }

  //! Defines the attributes which are used when drawing an
  //! V isoparametric curve of a face. Defines the number
  //! of V isoparametric curves to be drawn for a single face.
  //! The LineAspect for V isoparametric lines can be edited
  //! (methods SetColor, SetTypeOfLine, SetWidth, SetNumber)
  //! The default values are:
  //!   COLOR       : Quantity_NOC_GRAY82
  //!   TYPE OF LINE: Aspect_TOL_SOLID
  //!   WIDTH       : 0.5
  //!
  //! These attributes are used by the following algorithms:
  //!   Prs3d_WFDeflectionSurface
  //!   Prs3d_WFDeflectionRestrictedFace
  Standard_EXPORT const Handle(Prs3d_IsoAspect)& VIsoAspect();

  //! Sets the appearance of V isoparameters - theAspect.
  Standard_EXPORT void SetVIsoAspect (const Handle(Prs3d_IsoAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! VIso aspect that overrides the one in the link.
  Standard_Boolean HasOwnVIsoAspect() const { return myHasOwnVIsoAspect; }

  //! Returns wire aspect settings.
  //! The LineAspect for the free boundaries can be edited.
  //! The default values are:
  //!   Color: Quantity_NOC_GREEN
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  //! These attributes are used by the algorithm Prs3d_WFShape.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& WireAspect();

  //! Sets the parameter theAspect for display of wires.
  Standard_EXPORT void SetWireAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! wire aspect that overrides the one in the link.
  Standard_Boolean HasOwnWireAspect() const { return myHasOwnWireAspect; }

  //! Sets WireDraw on or off by setting the parameter theIsEnabled to true or false.
  Standard_EXPORT void SetWireDraw(const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of the wire is enabled.
  Standard_Boolean WireDraw() const
  {
    return HasOwnWireDraw() || myLink.IsNull()
         ? myWireDraw
         : myLink->WireDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw wires" flag that overrides the one in the link.
  Standard_Boolean HasOwnWireDraw() const { return myHasOwnWireDraw; }

  //! Returns the point aspect setting. The default values are
  //!   Color: Quantity_NOC_YELLOW
  //!   Type of marker: Aspect_TOM_PLUS
  //!   Scale: 1.0
  //! These attributes are used by the algorithms Prs3d_Point.
  Standard_EXPORT const Handle(Prs3d_PointAspect)& PointAspect();

  //! Sets the parameter theAspect for display attributes of points
  Standard_EXPORT void SetPointAspect (const Handle(Prs3d_PointAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! point aspect that overrides the one in the link.
  Standard_Boolean HasOwnPointAspect() const { return myHasOwnPointAspect; }

  //! Returns settings for line aspects.
  //! These settings can be edited. The default values are:
  //!   Color: Quantity_NOC_YELLOW
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  //! These attributes are used by the following algorithms:
  //!   Prs3d_Curve
  //!   Prs3d_Line
  //!   Prs3d_HLRShape
  Standard_EXPORT const Handle(Prs3d_LineAspect)& LineAspect();

  //! Sets the parameter theAspect for display attributes of lines.
  Standard_EXPORT void SetLineAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! line aspect that overrides the one in the link.
  Standard_Boolean HasOwnLineAspect() const { return myHasOwnLineAspect; }

  //! Returns settings for text aspect.
  //! These settings can be edited. The default value is:
  //! - Color: Quantity_NOC_YELLOW
  Standard_EXPORT const Handle(Prs3d_TextAspect)& TextAspect();

  //! Sets the parameter theAspect for display attributes of text.
  Standard_EXPORT void SetTextAspect (const Handle(Prs3d_TextAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! text aspect that overrides the one in the link.
  Standard_Boolean HasOwnTextAspect() const { return myHasOwnTextAspect; }

  //! Returns settings for shading aspects.
  //! These settings can be edited. The default values are:
  //! -   Color: Quantity_NOC_YELLOW
  //! -   Material: Graphic3d_NOM_BRASS
  //! Shading aspect is obtained through decomposition of
  //! 3d faces into triangles, each side of each triangle
  //! being a chord of the corresponding curved edge in the face.
  //! Reflection of light in each projector perspective is then calculated for each of the
  //! resultant triangular planes.
  Standard_EXPORT const Handle(Prs3d_ShadingAspect)& ShadingAspect();

  //! Sets the parameter theAspect for display attributes of shading.
  Standard_EXPORT  void SetShadingAspect (const Handle(Prs3d_ShadingAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! shading aspect that overrides the one in the link.
  Standard_Boolean HasOwnShadingAspect() const { return myHasOwnShadingAspect; }

  //! Returns True if the ShadingAspect is applied
  //! to the whole presentation.
  Standard_Boolean ShadingAspectGlobal() const
  {
    return HasOwnShadingAspectGlobal() || myLink.IsNull()
         ? myShadingAspectGlobal
         : myLink->ShadingAspectGlobal();
  }

  //! Indicates that the ShadingAspect will be apply
  //! to the whole presentation. This allows to modify
  //! the aspect without recomputing the content of the presentation.
  Standard_EXPORT void SetShadingAspectGlobal (const Standard_Boolean theValue);

  //! Returns true if the drawer has its own attribute for
  //! ShadingAspectGlobal flag that overrides the one in the link.
  Standard_Boolean HasOwnShadingAspectGlobal() const { return myHasOwnShadingAspectGlobal; }

  //! Returns settings for seen line aspects.
  //! These settings can be edited. The default values are:
  //!   Color: Quantity_NOC_YELLOW
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  Standard_EXPORT const Handle(Prs3d_LineAspect)& SeenLineAspect();

  //! Sets the parameter theAspect for the display of seen lines in hidden line removal mode.
  Standard_EXPORT void SetSeenLineAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! seen line aspect that overrides the one in the link.
  Standard_Boolean HasOwnSeenLineAspect() const { return myHasOwnSeenLineAspect; }

  //! Returns settings for the appearance of planes.
  Standard_EXPORT const Handle(Prs3d_PlaneAspect)& PlaneAspect();

  //! Sets the parameter theAspect for the display of planes. 
  Standard_EXPORT void SetPlaneAspect (const Handle(Prs3d_PlaneAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! plane aspect that overrides the one in the link.
  Standard_Boolean HasOwnPlaneAspect() const { return myHasOwnPlaneAspect; }

  //! Returns the attributes for display of arrows.
  Standard_EXPORT const Handle(Prs3d_ArrowAspect)& ArrowAspect();

  //! Sets the parameter theAspect for display attributes of arrows.
  Standard_EXPORT void SetArrowAspect (const Handle(Prs3d_ArrowAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! arrow aspect that overrides the one in the link.
  Standard_Boolean HasOwnArrowAspect() const { return myHasOwnArrowAspect; }

  //! Enables the drawing of an arrow at the end of each line.
  //! By default the arrows are not drawn.
  Standard_EXPORT void SetLineArrowDraw (const Standard_Boolean theIsEnabled);

  //! Returns True if drawing an arrow at the end of each edge is enabled
  //! and False otherwise (the default).
  Standard_Boolean LineArrowDraw() const
  {
    return HasOwnLineArrowDraw() || myLink.IsNull()
         ? myLineArrowDraw
         : myLink->LineArrowDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw arrow" flag that overrides the one in the link.
  Standard_Boolean HasOwnLineArrowDraw() const
  {
    return myHasOwnLineArrowDraw;
  }

  //! Returns settings for hidden line aspects.
  //! These settings can be edited. The default values are:
  //!   Color: Quantity_NOC_YELLOW
  //!   Type of line: Aspect_TOL_DASH
  //!   Width: 1.0
  Standard_EXPORT const Handle(Prs3d_LineAspect)& HiddenLineAspect();

  //! Sets the parameter theAspect for the display of hidden lines in hidden line removal mode.
  Standard_EXPORT void SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! hidden lines aspect that overrides the one in the link.
  Standard_Boolean HasOwnHiddenLineAspect() const { return myHasOwnHiddenLineAspect; }

  //! Returns Standard_True if the hidden lines are to be drawn.
  //! By default the hidden lines are not drawn.
  Standard_Boolean DrawHiddenLine() const
  {
    return HasOwnDrawHiddenLine() || myLink.IsNull()
         ? myDrawHiddenLine
         : myLink->DrawHiddenLine();
  }

  //! Enables the DrawHiddenLine function.
  Standard_EXPORT void EnableDrawHiddenLine();

  //! Disables the DrawHiddenLine function.
  Standard_EXPORT void DisableDrawHiddenLine();

  //! Returns true if the drawer has its own attribute for
  //! "draw hidden lines" flag that overrides the one in the link.
  Standard_Boolean HasOwnDrawHiddenLine() const { return myHasOwnDrawHiddenLine; }

  //! Returns settings for the appearance of vectors.
  //! These settings can be edited. The default values are:
  //!   Color: Quantity_NOC_SKYBLUE
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  Standard_EXPORT const Handle(Prs3d_LineAspect)& VectorAspect();

  //! Sets the modality theAspect for the display of vectors.
  Standard_EXPORT void SetVectorAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! vector aspect that overrides the one in the link.
  Standard_Boolean HasOwnVectorAspect() const { return myHasOwnVectorAspect; }

  //! Sets the mode of visualization of vertices of a TopoDS_Shape instance.
  //! By default, only stand-alone vertices (not belonging topologically to an edge) are drawn,
  //! that corresponds to Prs3d_VDM_Standalone mode. 
  //! Switching to Prs3d_VDM_Standalone mode makes all shape's vertices visible.
  //! To inherit this parameter from the global drawer instance ("the link") when it is present,
  //! Prs3d_VDM_Inherited value should be used.
  Standard_EXPORT void SetVertexDrawMode (const Prs3d_VertexDrawMode theMode);

  //! Returns the current mode of visualization of vertices of a TopoDS_Shape instance.
  Standard_EXPORT Prs3d_VertexDrawMode VertexDrawMode();

  //! Returns true if the vertex draw mode is not equal to <b>Prs3d_VDM_Inherited</b>. 
  //! This means that individual vertex draw mode value (i.e. not inherited from the global 
  //! drawer) is used for a specific interactive object.
  Standard_Boolean HasOwnVertexDrawMode() const { return (myVertexDrawMode != Prs3d_VDM_Inherited); }

  //! Returns settings for the appearance of datums.
  //! These settings can be edited. The default values for the three axes are:
  //!   Color: Quantity_NOC_PEACHPUFF
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  Standard_EXPORT const Handle(Prs3d_DatumAspect)& DatumAspect();

  //! Sets the modality theAspect for the display of datums.
  Standard_EXPORT void SetDatumAspect (const Handle(Prs3d_DatumAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! datum aspect that overrides the one in the link.
  Standard_Boolean HasOwnDatumAspect() const { return myHasOwnDatumAspect; }

  //! The LineAspect for the wire can be edited.
  //! The default values are:
  //!   Color: Quantity_NOC_ORANGE
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  //! These attributes are used by the algorithm Prs3d_WFShape.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& SectionAspect();

  //! Sets the parameter theAspect for display attributes of sections. 
  Standard_EXPORT void SetSectionAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! section aspect that overrides the one in the link.
  Standard_Boolean HasOwnSectionAspect() const { return myHasOwnSectionAspect; }

  //! Sets the parameter theAspect for the display of free boundaries.
  //! The method sets aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetFreeBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns the values for presentation of free boundaries,
  //! in other words, boundaries which are not shared.
  //! The LineAspect for the  free boundaries can be edited.
  //! The default values are:
  //!   Color: Quantity_NOC_GREEN
  //!   Type of line: Aspect_TOL_SOLID
  //!   Width: 1.0
  //! These attributes are used by the algorithm Prs3d_WFShape
  Standard_EXPORT const Handle(Prs3d_LineAspect)& FreeBoundaryAspect();

  //! Returns true if the drawer has its own attribute for
  //! free boundaries aspect that overrides the one in the link.
  Standard_Boolean HasOwnFreeBoundaryAspect() const { return myHasOwnFreeBoundaryAspect; }

  //! Enables or disables drawing of free boundaries for shading presentations.
  //! The method sets drawing flag owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theIsEnabled is a boolean flag indicating whether the free boundaries should be
  //! drawn or not.
  Standard_EXPORT void SetFreeBoundaryDraw (const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of the free boundaries is enabled
  //! True is the default setting.
  Standard_Boolean FreeBoundaryDraw() const
  {
    return HasOwnFreeBoundaryDraw() || myLink.IsNull()
         ? myFreeBoundaryDraw
         : myLink->FreeBoundaryDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw free boundaries" flag that overrides the one in the link.
  Standard_Boolean HasOwnFreeBoundaryDraw() const { return myHasOwnFreeBoundaryDraw; }

  //! Sets the parameter theAspect for the display of shared boundaries.
  //! The method sets aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetUnFreeBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns settings for shared boundary line aspects.
  //! The LineAspect for the unfree boundaries can be edited.
  //! The default values are:
  //! Color: Quantity_NOC_YELLOW
  //! Type of line: Aspect_TOL_SOLID
  //! Width: 1.
  //! These attributes are used by the algorithm Prs3d_WFShape
  Standard_EXPORT const Handle(Prs3d_LineAspect)& UnFreeBoundaryAspect();

  //! Returns true if the drawer has its own attribute for
  //! unfree boundaries aspect that overrides the one in the link.
  Standard_Boolean HasOwnUnFreeBoundaryAspect() const { return myHasOwnUnFreeBoundaryAspect; }

  //! Enables or disables drawing of shared boundaries for shading presentations.
  //! The method sets drawing flag owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theIsEnabled is a boolean flag indicating whether the shared boundaries should be drawn or not.
  Standard_EXPORT void SetUnFreeBoundaryDraw (const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of the shared boundaries is enabled.
  //! True is the default setting.
  Standard_Boolean UnFreeBoundaryDraw() const
  {
    return HasOwnUnFreeBoundaryDraw() || myLink.IsNull()
         ? myUnFreeBoundaryDraw
         : myLink->UnFreeBoundaryDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw shared boundaries" flag that overrides the one in the link.
  Standard_Boolean HasOwnUnFreeBoundaryDraw() const { return myHasOwnUnFreeBoundaryDraw; }

  //! Sets line aspect for face boundaries.
  //! The method sets line aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theAspect is the line aspect that determines the look of the face boundaries.
  Standard_EXPORT void SetFaceBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect);

  //! Returns line aspect of face boundaries.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& FaceBoundaryAspect();

  //! Returns true if the drawer has its own attribute for
  //! face boundaries aspect that overrides the one in the link.
  Standard_Boolean HasOwnFaceBoundaryAspect() const { return myHasOwnFaceBoundaryAspect; }

  //! Enables or disables face boundary drawing for shading presentations. 
  //! The method sets drawing flag owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theIsEnabled is a boolean flag indicating whether the face boundaries should be drawn or not.
  Standard_EXPORT void SetFaceBoundaryDraw (const Standard_Boolean theIsEnabled);

  //! Checks whether the face boundary drawing is enabled or not.
  Standard_Boolean FaceBoundaryDraw() const
  {
    return HasOwnFaceBoundaryDraw() || myLink.IsNull()
         ? myFaceBoundaryDraw
         : myLink->FaceBoundaryDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw face boundaries" flag that overrides the one in the link.
  Standard_Boolean HasOwnFaceBoundaryDraw() const { return myHasOwnFaceBoundaryDraw; }

  //! Returns settings for the appearance of dimensions. 
  Standard_EXPORT const Handle(Prs3d_DimensionAspect)& DimensionAspect();

  //! Sets the settings for the appearance of dimensions.
  //! The method sets aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theAspect);

  //! Returns true if the drawer has its own attribute for
  //! the appearance of dimensions that overrides the one in the link.
  Standard_Boolean HasOwnDimensionAspect() const { return myHasOwnDimensionAspect; }

  //! Sets dimension length model units for computing of dimension presentation.
  //! The method sets value owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetDimLengthModelUnits (const TCollection_AsciiString& theUnits);

  //! Sets dimension angle model units for computing of dimension presentation.
  //! The method sets value owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetDimAngleModelUnits (const TCollection_AsciiString& theUnits);

  //! Returns length model units for the dimension presentation. 
  const TCollection_AsciiString& DimLengthModelUnits() const
  {
    return HasOwnDimLengthModelUnits() || myLink.IsNull()
         ? myDimensionModelUnits.GetLengthUnits()
         : myLink->DimLengthModelUnits();
  }

  //! Returns angle model units for the dimension presentation. 
  const TCollection_AsciiString& DimAngleModelUnits() const
  {
    return HasOwnDimAngleModelUnits() || myLink.IsNull()
         ? myDimensionModelUnits.GetAngleUnits()
         : myLink->DimAngleModelUnits();
  }

  //! Returns true if the drawer has its own attribute for
  //! dimension length model units that overrides the one in the link.
  Standard_Boolean HasOwnDimLengthModelUnits() const { return myHasOwnDimLengthModelUnits; }

  //! Returns true if the drawer has its own attribute for
  //! dimension angle model units that overrides the one in the link.
  Standard_Boolean HasOwnDimAngleModelUnits() const { return myHasOwnDimAngleModelUnits; }

  //! Sets length units in which value for dimension presentation is displayed.
  //! The method sets value owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetDimLengthDisplayUnits (const TCollection_AsciiString& theUnits);

  //! Sets angle units in which value for dimension presentation is displayed.
  //! The method sets value owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  Standard_EXPORT void SetDimAngleDisplayUnits (const TCollection_AsciiString& theUnits);

  //! Returns length units in which dimension presentation is displayed.
  const TCollection_AsciiString& DimLengthDisplayUnits() const
  {
    return HasOwnDimLengthDisplayUnits() || myLink.IsNull()
         ? myDimensionDisplayUnits.GetLengthUnits()
         : myLink->DimLengthDisplayUnits();
  }

  //! Returns angle units in which dimension presentation is displayed.
  const TCollection_AsciiString& DimAngleDisplayUnits() const
  {
    return HasOwnDimAngleDisplayUnits() || myLink.IsNull()
         ? myDimensionDisplayUnits.GetAngleUnits()
         : myLink->DimAngleDisplayUnits();
  }

  //! Returns true if the drawer has its own attribute for
  //! length units in which dimension presentation is displayed
  //! that overrides the one in the link.
  Standard_Boolean HasOwnDimLengthDisplayUnits() const { return myHasOwnDimLengthDisplayUnits; }

  //! Returns true if the drawer has its own attribute for
  //! angle units in which dimension presentation is displayed
  //! that overrides the one in the link.
  Standard_Boolean HasOwnDimAngleDisplayUnits() const { return myHasOwnDimAngleDisplayUnits; }

  //! Returns the drawer to which the current object references.
  const Handle(Prs3d_Drawer)& Link() { return myLink; }

  //! Returns true if the current object has a link on the other drawer.
  Standard_Boolean HasLink() const { return !myLink.IsNull(); }

  //! Sets theDrawer as a link to which the current object references.
  void Link (const Handle(Prs3d_Drawer)& theDrawer)
  {
    myLink = theDrawer;
  }

  //! Removes local attributes. 
  Standard_EXPORT void ClearLocalAttributes();

protected:

  Handle(Prs3d_Drawer)          myLink;

  Standard_Integer              myNbPoints;
  Standard_Boolean              myHasOwnNbPoints;
  Standard_Real                 myMaximalParameterValue;
  Standard_Boolean              myHasOwnMaximalParameterValue;
  Quantity_Length               myChordialDeviation;
  Standard_Boolean              myHasOwnChordialDeviation;
  Aspect_TypeOfDeflection       myTypeOfDeflection;
  Standard_Boolean              myHasOwnTypeOfDeflection;
  Prs3d_TypeOfHLR               myTypeOfHLR;
  Standard_Real                 myDeviationCoefficient;
  Standard_Real                 myPreviousDeviationCoefficient;
  Standard_Boolean              myHasOwnDeviationCoefficient;
  Standard_Real                 myHLRDeviationCoefficient;
  Standard_Boolean              myHasOwnHLRDeviationCoefficient;
  Standard_Real                 myPreviousHLRDeviationCoefficient;
  Standard_Real                 myDeviationAngle;
  Standard_Boolean              myHasOwnDeviationAngle;
  Standard_Real                 myPreviousDeviationAngle;
  Standard_Real                 myHLRAngle;
  Standard_Boolean              myHasOwnHLRDeviationAngle;
  Standard_Real                 myPreviousHLRDeviationAngle;
  Standard_Boolean              myIsoOnPlane;
  Standard_Boolean              myHasOwnIsoOnPlane;
  Standard_Boolean              myIsAutoTriangulated;
  Standard_Boolean              myHasOwnIsAutoTriangulated;

  Handle(Prs3d_IsoAspect)       myUIsoAspect;
  Standard_Boolean              myHasOwnUIsoAspect;
  Handle(Prs3d_IsoAspect)       myVIsoAspect;
  Standard_Boolean              myHasOwnVIsoAspect;
  Handle(Prs3d_LineAspect)      myWireAspect;
  Standard_Boolean              myHasOwnWireAspect;
  Standard_Boolean              myWireDraw;
  Standard_Boolean              myHasOwnWireDraw;
  Handle(Prs3d_PointAspect)     myPointAspect;
  Standard_Boolean              myHasOwnPointAspect;
  Handle(Prs3d_LineAspect)      myLineAspect;
  Standard_Boolean              myHasOwnLineAspect;
  Handle(Prs3d_TextAspect)      myTextAspect;
  Standard_Boolean              myHasOwnTextAspect;
  Handle(Prs3d_ShadingAspect)   myShadingAspect;
  Standard_Boolean              myHasOwnShadingAspect;
  Standard_Boolean              myShadingAspectGlobal;
  Standard_Boolean              myHasOwnShadingAspectGlobal;
  Handle(Prs3d_PlaneAspect)     myPlaneAspect;
  Standard_Boolean              myHasOwnPlaneAspect;
  Handle(Prs3d_LineAspect)      mySeenLineAspect;
  Standard_Boolean              myHasOwnSeenLineAspect;
  Handle(Prs3d_ArrowAspect)     myArrowAspect;
  Standard_Boolean              myHasOwnArrowAspect;
  Standard_Boolean              myLineArrowDraw;
  Standard_Boolean              myHasOwnLineArrowDraw;
  Handle(Prs3d_LineAspect)      myHiddenLineAspect;
  Standard_Boolean              myHasOwnHiddenLineAspect;
  Standard_Boolean              myDrawHiddenLine;
  Standard_Boolean              myHasOwnDrawHiddenLine;
  Handle(Prs3d_LineAspect)      myVectorAspect;
  Standard_Boolean              myHasOwnVectorAspect;
  Prs3d_VertexDrawMode          myVertexDrawMode;
  Handle(Prs3d_DatumAspect)     myDatumAspect;
  Standard_Boolean              myHasOwnDatumAspect;
  Handle(Prs3d_LineAspect)      mySectionAspect;
  Standard_Boolean              myHasOwnSectionAspect;

  Handle(Prs3d_LineAspect)      myFreeBoundaryAspect;
  Standard_Boolean              myHasOwnFreeBoundaryAspect;
  Standard_Boolean              myFreeBoundaryDraw;
  Standard_Boolean              myHasOwnFreeBoundaryDraw;
  Handle(Prs3d_LineAspect)      myUnFreeBoundaryAspect;
  Standard_Boolean              myHasOwnUnFreeBoundaryAspect;
  Standard_Boolean              myUnFreeBoundaryDraw;
  Standard_Boolean              myHasOwnUnFreeBoundaryDraw;
  Handle(Prs3d_LineAspect)      myFaceBoundaryAspect;
  Standard_Boolean              myHasOwnFaceBoundaryAspect;
  Standard_Boolean              myFaceBoundaryDraw;
  Standard_Boolean              myHasOwnFaceBoundaryDraw;

  Handle(Prs3d_DimensionAspect) myDimensionAspect;
  Standard_Boolean              myHasOwnDimensionAspect;
  Prs3d_DimensionUnits          myDimensionModelUnits;
  Standard_Boolean              myHasOwnDimLengthModelUnits;
  Standard_Boolean              myHasOwnDimAngleModelUnits;
  Prs3d_DimensionUnits          myDimensionDisplayUnits;
  Standard_Boolean              myHasOwnDimLengthDisplayUnits;
  Standard_Boolean              myHasOwnDimAngleDisplayUnits;

public:

  DEFINE_STANDARD_RTTI(Prs3d_Drawer)

};

#endif // _Prs3d_Drawer_HeaderFile
