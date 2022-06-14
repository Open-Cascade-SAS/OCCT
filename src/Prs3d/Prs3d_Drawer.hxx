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

#include <Standard_Integer.hxx>
#include <Aspect_TypeOfDeflection.hxx>
#include <Graphic3d_GroupAspect.hxx>
#include <Graphic3d_PresentationAttributes.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Standard_Real.hxx>
#include <Prs3d_VertexDrawMode.hxx>
#include <Prs3d_DimensionUnits.hxx>
#include <Prs3d_TypeOfHLR.hxx>
#include <Standard_Transient.hxx>
#include <GeomAbs_Shape.hxx>

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

DEFINE_STANDARD_HANDLE(Prs3d_Drawer, Graphic3d_PresentationAttributes)

//! A graphic attribute manager which governs how
//! objects such as color, width, line thickness and deflection are displayed.
//! A drawer includes an instance of the Aspect classes with particular default values.
class Prs3d_Drawer : public Graphic3d_PresentationAttributes
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_Drawer, Graphic3d_PresentationAttributes)
public:

  //! Default constructor.
  Standard_EXPORT Prs3d_Drawer();

  //! Setup all own aspects with default values.
  Standard_EXPORT void SetupOwnDefaults();

  //! Sets the type of chordal deflection.
  //! This indicates whether the deflection value is absolute or relative to the size of the object.
  Standard_EXPORT void SetTypeOfDeflection (const Aspect_TypeOfDeflection theTypeOfDeflection);

  //! Returns the type of chordal deflection.
  //! This indicates whether the deflection value is absolute or relative to the size of the object.
  Aspect_TypeOfDeflection TypeOfDeflection() const
  {
    return myHasOwnTypeOfDeflection || myLink.IsNull()
         ? myTypeOfDeflection
         : myLink->TypeOfDeflection();
  }

  //! Returns true if the drawer has a type of deflection setting active.
  Standard_Boolean HasOwnTypeOfDeflection() const { return myHasOwnTypeOfDeflection; }

  //! Resets HasOwnTypeOfDeflection() flag, e.g. undoes SetTypeOfDeflection().
  void UnsetOwnTypeOfDeflection()
  {
    myHasOwnTypeOfDeflection = false;
    myTypeOfDeflection = Aspect_TOD_RELATIVE;
  }

  //! Defines the maximal chordial deviation when drawing any curve.
  //! Even if the type of deviation is set to TOD_Relative, this value is used by: 
  //!   Prs3d_DeflectionCurve
  //!   Prs3d_WFDeflectionSurface
  //!   Prs3d_WFDeflectionRestrictedFace
  void SetMaximalChordialDeviation (const Standard_Real theChordialDeviation)
  {
    myChordialDeviation = theChordialDeviation;
  }

  //! Returns the maximal chordal deviation. The default value is 0.0001.
  //! Drawings of curves or patches are made with respect to an absolute maximal chordal deviation.
  Standard_Real MaximalChordialDeviation() const
  {
    return myChordialDeviation > 0.0
         ? myChordialDeviation
         : (!myLink.IsNull()
           ? myLink->MaximalChordialDeviation()
           : 0.0001);
  }

  //! Returns true if the drawer has a maximal chordial deviation setting active.
  Standard_Boolean HasOwnMaximalChordialDeviation() const { return myChordialDeviation > 0.0; }

  //! Resets HasOwnMaximalChordialDeviation() flag, e.g. undoes SetMaximalChordialDeviation().
  void UnsetOwnMaximalChordialDeviation()
  {
    myChordialDeviation = -1.0;
  }

  //! Sets the type of HLR algorithm used by drawer's interactive objects
  Standard_EXPORT void SetTypeOfHLR (const Prs3d_TypeOfHLR theTypeOfHLR);

  //! Returns the type of HLR algorithm currently in use.
  Standard_EXPORT Prs3d_TypeOfHLR TypeOfHLR() const;

  //! Returns true if the type of HLR is not equal to Prs3d_TOH_NotSet.
  Standard_Boolean HasOwnTypeOfHLR() const { return (myTypeOfHLR != Prs3d_TOH_NotSet); }

  //! Defines the maximum value allowed for the first and last
  //! parameters of an infinite curve.
  void SetMaximalParameterValue (const Standard_Real theValue)
  {
    myMaximalParameterValue = theValue;
  }

  //! Sets the maximum value allowed for the first and last parameters of an infinite curve.
  //! By default, this value is 500000.
  Standard_Real MaximalParameterValue() const
  {
    return myMaximalParameterValue > 0.0
         ? myMaximalParameterValue
         : (!myLink.IsNull()
           ? myLink->MaximalParameterValue()
           : 500000.0);
  }

  //! Returns true if the drawer has a maximum value allowed for the first and last
  //! parameters of an infinite curve setting active.
  Standard_Boolean HasOwnMaximalParameterValue() const { return myMaximalParameterValue > 0.0; }

  //! Resets HasOwnMaximalParameterValue() flag, e.g. undoes SetMaximalParameterValue().
  void UnsetOwnMaximalParameterValue()
  {
    myMaximalParameterValue = -1.0;
  }

  //! Sets IsoOnPlane on or off by setting the parameter theIsEnabled to true or false.
  Standard_EXPORT void SetIsoOnPlane (const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of isos on planes is enabled.
  Standard_Boolean IsoOnPlane() const
  {
    return myHasOwnIsoOnPlane || myLink.IsNull()
         ? myIsoOnPlane
         : myLink->IsoOnPlane();
  }

  //! Returns true if the drawer has IsoOnPlane setting active.
  Standard_Boolean HasOwnIsoOnPlane() const { return myHasOwnIsoOnPlane; }

  //! Resets HasOwnIsoOnPlane() flag, e.g. undoes SetIsoOnPlane().
  void UnsetOwnIsoOnPlane()
  {
    myHasOwnIsoOnPlane = false;
    myIsoOnPlane = false;
  }

  //! Returns True if the drawing of isos on triangulation is enabled.
  Standard_Boolean IsoOnTriangulation() const
  {
    return myHasOwnIsoOnTriangulation || myLink.IsNull()
         ? myIsoOnTriangulation
         : myLink->IsoOnTriangulation();
  }

  //! Returns true if the drawer has IsoOnTriangulation setting active.
  Standard_Boolean HasOwnIsoOnTriangulation() const { return myHasOwnIsoOnTriangulation; }

  //! Resets HasOwnIsoOnTriangulation() flag, e.g. undoes SetIsoOnTriangulation().
  void UnsetOwnIsoOnTriangulation()
  {
    myHasOwnIsoOnTriangulation = false;
    myIsoOnTriangulation = false;
  }

  //! Enables or disables isolines on triangulation by setting the parameter theIsEnabled to true or false.
  Standard_EXPORT void SetIsoOnTriangulation (const Standard_Boolean theToEnable);

  //! Sets the discretisation parameter theValue.
  void SetDiscretisation (const Standard_Integer theValue)
  {
    myNbPoints = theValue;
  }

  //! Returns the discretisation setting. 
  Standard_Integer Discretisation() const
  {
    return myNbPoints != -1
         ? myNbPoints
         : (!myLink.IsNull()
           ? myLink->Discretisation()
           : 30);
  }

  //! Returns true if the drawer has discretisation setting active.
  Standard_Boolean HasOwnDiscretisation() const { return myNbPoints != -1; }

  //! Resets HasOwnDiscretisation() flag, e.g. undoes SetDiscretisation().
  void UnsetOwnDiscretisation()
  {
    myNbPoints = -1;
  }

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
    return myDeviationCoefficient > 0.0
         ? myDeviationCoefficient
         : (!myLink.IsNull()
           ? myLink->DeviationCoefficient()
           : 0.001);
  }

  //! Resets HasOwnDeviationCoefficient() flag, e.g. undoes previous SetDeviationCoefficient().
  void SetDeviationCoefficient()
  {
    myDeviationCoefficient = -1.0;
  }

  //! Returns true if there is a local setting for deviation
  //! coefficient in this framework for a specific interactive object.
  Standard_Boolean HasOwnDeviationCoefficient() const { return myDeviationCoefficient > 0.0; }

  //! Saves the previous value used for the chordal
  //! deviation coefficient. 
  Standard_Real PreviousDeviationCoefficient() const
  {
    return HasOwnDeviationCoefficient()
         ? myPreviousDeviationCoefficient
         : 0.0;
  }

  //! Updates the previous value used for the chordal deviation coefficient to the current state.
  void UpdatePreviousDeviationCoefficient()
  {
    if (HasOwnDeviationCoefficient())
    {
      myPreviousDeviationCoefficient = DeviationCoefficient();
    }
  }

  //! Sets the deviation angle theAngle.
  //! Also sets the hasOwnDeviationAngle flag to Standard_True, and myPreviousDeviationAngle.
  Standard_EXPORT void SetDeviationAngle (const Standard_Real theAngle);

  //! Returns the value for deviation angle in radians, 20 * M_PI / 180 by default.
  Standard_Real DeviationAngle() const
  {
    return myDeviationAngle > 0.0
         ? myDeviationAngle
         : (!myLink.IsNull()
           ? myLink->DeviationAngle()
           : 20.0 * M_PI / 180.0);
  }

  //! Resets HasOwnDeviationAngle() flag, e.g. undoes previous SetDeviationAngle().
  void SetDeviationAngle()
  {
    myDeviationAngle = -1.0;
  }

  //! Returns true if there is a local setting for deviation
  //! angle in this framework for a specific interactive object.
  Standard_Boolean HasOwnDeviationAngle() const { return myDeviationAngle > 0.0; }

  //! Returns the previous deviation angle
  Standard_Real PreviousDeviationAngle() const
  {
    return HasOwnDeviationAngle()
         ? myPreviousDeviationAngle
         : 0.0;
  }

  //! Updates the previous deviation angle to the current value
  void UpdatePreviousDeviationAngle()
  {
    if (HasOwnDeviationAngle())
    {
      myPreviousDeviationAngle = DeviationAngle();
    }
  }

  //! Sets IsAutoTriangulated on or off by setting the parameter theIsEnabled to true or false.
  //! If this flag is True automatic re-triangulation with deflection-check logic will be applied.
  //! Else this feature will be disable and triangulation is expected to be computed by application itself
  //! and no shading presentation at all if unavailable.
  Standard_EXPORT void SetAutoTriangulation (const Standard_Boolean theIsEnabled);

  //! Returns True if automatic triangulation is enabled.
  Standard_Boolean IsAutoTriangulation() const
  {
    return myHasOwnIsAutoTriangulated || myLink.IsNull()
         ? myIsAutoTriangulated
         : myLink->IsAutoTriangulation();
  }

  //! Returns true if the drawer has IsoOnPlane setting active.
  Standard_Boolean HasOwnIsAutoTriangulation() const { return myHasOwnIsAutoTriangulated; }

  //! Resets HasOwnIsAutoTriangulation() flag, e.g. undoes SetAutoTriangulation().
  void UnsetOwnIsAutoTriangulation()
  {
    myHasOwnIsAutoTriangulated = false;
    myIsAutoTriangulated = true;
  }

  //! Defines own attributes for drawing an U isoparametric curve of a face,
  //! settings from linked Drawer or NULL if neither was set.
  //!
  //! These attributes are used by the following algorithms:
  //!   Prs3d_WFDeflectionSurface
  //!   Prs3d_WFDeflectionRestrictedFace
  Standard_EXPORT const Handle(Prs3d_IsoAspect)& UIsoAspect() const;

  void SetUIsoAspect (const Handle(Prs3d_IsoAspect)& theAspect)
  {
    myUIsoAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! UIso aspect that overrides the one in the link.
  Standard_Boolean HasOwnUIsoAspect() const { return !myUIsoAspect.IsNull(); }

  //! Defines own attributes for drawing an V isoparametric curve of a face,
  //! settings from linked Drawer or NULL if neither was set.
  //!
  //! These attributes are used by the following algorithms:
  //!   Prs3d_WFDeflectionSurface
  //!   Prs3d_WFDeflectionRestrictedFace
  Standard_EXPORT const Handle(Prs3d_IsoAspect)& VIsoAspect() const;

  //! Sets the appearance of V isoparameters - theAspect.
  void SetVIsoAspect (const Handle(Prs3d_IsoAspect)& theAspect)
  {
    myVIsoAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! VIso aspect that overrides the one in the link.
  Standard_Boolean HasOwnVIsoAspect() const { return !myVIsoAspect.IsNull(); }

  //! Returns own wire aspect settings, settings from linked Drawer or NULL if neither was set.
  //! These attributes are used by the algorithm Prs3d_WFShape.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& WireAspect() const;

  //! Sets the parameter theAspect for display of wires.
  void SetWireAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myWireAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! wire aspect that overrides the one in the link.
  Standard_Boolean HasOwnWireAspect() const { return !myWireAspect.IsNull(); }

  //! Sets WireDraw on or off by setting the parameter theIsEnabled to true or false.
  Standard_EXPORT void SetWireDraw(const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of the wire is enabled.
  Standard_Boolean WireDraw() const
  {
    return myHasOwnWireDraw || myLink.IsNull()
         ? myWireDraw
         : myLink->WireDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw wires" flag that overrides the one in the link.
  Standard_Boolean HasOwnWireDraw() const { return myHasOwnWireDraw; }

  //! Resets HasOwnWireDraw() flag, e.g. undoes SetWireDraw().
  void UnsetOwnWireDraw()
  {
    myHasOwnWireDraw = false;
    myWireDraw = true;
  }

  //! Returns own point aspect setting, settings from linked Drawer or NULL if neither was set.
  //! These attributes are used by the algorithms Prs3d_Point.
  Standard_EXPORT const Handle(Prs3d_PointAspect)& PointAspect() const;

  //! Sets the parameter theAspect for display attributes of points
  void SetPointAspect (const Handle(Prs3d_PointAspect)& theAspect)
  {
    myPointAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! point aspect that overrides the one in the link.
  Standard_Boolean HasOwnPointAspect() const { return !myPointAspect.IsNull(); }

  //! Sets own point aspect, which is a yellow Aspect_TOM_PLUS marker by default.
  //! Returns FALSE if the drawer already has its own attribute for point aspect.
  Standard_EXPORT Standard_Boolean SetupOwnPointAspect (const Handle(Prs3d_Drawer)& theDefaults = Handle(Prs3d_Drawer)());

  //! Returns own settings for line aspects, settings from linked Drawer or NULL if neither was set.
  //! These attributes are used by the following algorithms:
  //!   Prs3d_Curve
  //!   Prs3d_Line
  //!   Prs3d_HLRShape
  Standard_EXPORT const Handle(Prs3d_LineAspect)& LineAspect() const;

  //! Sets the parameter theAspect for display attributes of lines.
  void SetLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myLineAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! line aspect that overrides the one in the link.
  Standard_Boolean HasOwnLineAspect() const { return !myLineAspect.IsNull(); }

  //! Sets own line aspects, which are
  //! single U and single V gray75 solid isolines (::UIsoAspect(), ::VIsoAspect()),
  //! red wire (::WireAspect()), yellow line (::LineAspect()),
  //! yellow seen line (::SeenLineAspect()), dashed yellow hidden line (::HiddenLineAspect()),
  //! green free boundary (::FreeBoundaryAspect()), yellow unfree boundary (::UnFreeBoundaryAspect()).
  //! Returns FALSE if own line aspect are already set.
  Standard_EXPORT Standard_Boolean SetOwnLineAspects (const Handle(Prs3d_Drawer)& theDefaults = Handle(Prs3d_Drawer)());

  //! Sets own line aspects for datums.
  //! Returns FALSE if own line for datums are already set.
  Standard_EXPORT Standard_Boolean SetOwnDatumAspects (const Handle(Prs3d_Drawer)& theDefaults = Handle(Prs3d_Drawer)());

  //! Returns own settings for text aspect, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_TextAspect)& TextAspect() const;

  //! Sets the parameter theAspect for display attributes of text.
  void SetTextAspect (const Handle(Prs3d_TextAspect)& theAspect)
  {
    myTextAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! text aspect that overrides the one in the link.
  Standard_Boolean HasOwnTextAspect() const { return !myTextAspect.IsNull(); }

  //! Returns own settings for shading aspects, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_ShadingAspect)& ShadingAspect() const;

  //! Sets the parameter theAspect for display attributes of shading.
  void SetShadingAspect (const Handle(Prs3d_ShadingAspect)& theAspect)
  {
    myShadingAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! shading aspect that overrides the one in the link.
  Standard_Boolean HasOwnShadingAspect() const { return !myShadingAspect.IsNull(); }

  //! Sets own shading aspect, which is Graphic3d_NameOfMaterial_Brass material by default.
  //! Returns FALSE if the drawer already has its own attribute for shading aspect.
  Standard_EXPORT Standard_Boolean SetupOwnShadingAspect (const Handle(Prs3d_Drawer)& theDefaults = Handle(Prs3d_Drawer)());

  //! Returns own settings for seen line aspects, settings of linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& SeenLineAspect() const;

  //! Sets the parameter theAspect for the display of seen lines in hidden line removal mode.
  void SetSeenLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    mySeenLineAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! seen line aspect that overrides the one in the link.
  Standard_Boolean HasOwnSeenLineAspect() const { return !mySeenLineAspect.IsNull(); }

  //! Returns own settings for the appearance of planes, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_PlaneAspect)& PlaneAspect() const;

  //! Sets the parameter theAspect for the display of planes. 
  void SetPlaneAspect (const Handle(Prs3d_PlaneAspect)& theAspect)
  {
    myPlaneAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! plane aspect that overrides the one in the link.
  Standard_Boolean HasOwnPlaneAspect() const { return !myPlaneAspect.IsNull(); }

  //! Returns own attributes for display of arrows, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_ArrowAspect)& ArrowAspect() const;

  //! Sets the parameter theAspect for display attributes of arrows.
  void SetArrowAspect (const Handle(Prs3d_ArrowAspect)& theAspect)
  {
    myArrowAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! arrow aspect that overrides the one in the link.
  Standard_Boolean HasOwnArrowAspect() const { return !myArrowAspect.IsNull(); }

  //! Enables the drawing of an arrow at the end of each line.
  //! By default the arrows are not drawn.
  Standard_EXPORT void SetLineArrowDraw (const Standard_Boolean theIsEnabled);

  //! Returns True if drawing an arrow at the end of each edge is enabled
  //! and False otherwise (the default).
  Standard_Boolean LineArrowDraw() const
  {
    return myHasOwnLineArrowDraw || myLink.IsNull()
         ? myLineArrowDraw
         : myLink->LineArrowDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw arrow" flag that overrides the one in the link.
  Standard_Boolean HasOwnLineArrowDraw() const { return myHasOwnLineArrowDraw; }

  //! Reset HasOwnLineArrowDraw() flag, e.g. undoes SetLineArrowDraw().
  void UnsetOwnLineArrowDraw()
  {
    myHasOwnLineArrowDraw = false;
    myLineArrowDraw = false;
  }

  //! Returns own settings for hidden line aspects, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& HiddenLineAspect() const;

  //! Sets the parameter theAspect for the display of hidden lines in hidden line removal mode.
  void SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myHiddenLineAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! hidden lines aspect that overrides the one in the link.
  Standard_Boolean HasOwnHiddenLineAspect() const { return !myHiddenLineAspect.IsNull(); }

  //! Returns Standard_True if the hidden lines are to be drawn.
  //! By default the hidden lines are not drawn.
  Standard_Boolean DrawHiddenLine() const
  {
    return myHasOwnDrawHiddenLine || myLink.IsNull()
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

  //! Resets HasOwnDrawHiddenLine() flag, e.g. unsets EnableDrawHiddenLine()/DisableDrawHiddenLine().
  void UnsetOwnDrawHiddenLine()
  {
    myHasOwnDrawHiddenLine = false;
    myDrawHiddenLine = false;
  }

  //! Returns own settings for the appearance of vectors, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& VectorAspect() const;

  //! Sets the modality theAspect for the display of vectors.
  void SetVectorAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myVectorAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! vector aspect that overrides the one in the link.
  Standard_Boolean HasOwnVectorAspect() const { return !myVectorAspect.IsNull(); }

  //! Sets the mode of visualization of vertices of a TopoDS_Shape instance.
  //! By default, only stand-alone vertices (not belonging topologically to an edge) are drawn,
  //! that corresponds to Prs3d_VDM_Standalone mode. 
  //! Switching to Prs3d_VDM_Standalone mode makes all shape's vertices visible.
  //! To inherit this parameter from the global drawer instance ("the link") when it is present,
  //! Prs3d_VDM_Inherited value should be used.
  Standard_EXPORT void SetVertexDrawMode (const Prs3d_VertexDrawMode theMode);

  //! Returns the current mode of visualization of vertices of a TopoDS_Shape instance.
  Standard_EXPORT Prs3d_VertexDrawMode VertexDrawMode() const;

  //! Returns true if the vertex draw mode is not equal to <b>Prs3d_VDM_Inherited</b>. 
  //! This means that individual vertex draw mode value (i.e. not inherited from the global 
  //! drawer) is used for a specific interactive object.
  Standard_Boolean HasOwnVertexDrawMode() const { return (myVertexDrawMode != Prs3d_VDM_Inherited); }

  //! Returns own settings for the appearance of datums, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_DatumAspect)& DatumAspect() const;

  //! Sets the modality theAspect for the display of datums.
  void SetDatumAspect (const Handle(Prs3d_DatumAspect)& theAspect)
  {
    myDatumAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! datum aspect that overrides the one in the link.
  Standard_Boolean HasOwnDatumAspect() const { return !myDatumAspect.IsNull(); }

  //! Returns own LineAspect for section wire, settings from linked Drawer or NULL if neither was set.
  //! These attributes are used by the algorithm Prs3d_WFShape.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& SectionAspect() const;

  //! Sets the parameter theAspect for display attributes of sections. 
  void SetSectionAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    mySectionAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! section aspect that overrides the one in the link.
  Standard_Boolean HasOwnSectionAspect() const { return !mySectionAspect.IsNull(); }

  //! Sets the parameter theAspect for the display of free boundaries.
  //! The method sets aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  void SetFreeBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myFreeBoundaryAspect = theAspect;
  }

  //! Returns own settings for presentation of free boundaries, settings from linked Drawer or NULL if neither was set.
  //! In other words, this settings affect boundaries which are not shared.
  //! These attributes are used by the algorithm Prs3d_WFShape
  Standard_EXPORT const Handle(Prs3d_LineAspect)& FreeBoundaryAspect() const;

  //! Returns true if the drawer has its own attribute for
  //! free boundaries aspect that overrides the one in the link.
  Standard_Boolean HasOwnFreeBoundaryAspect() const { return !myFreeBoundaryAspect.IsNull(); }

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
    return myHasOwnFreeBoundaryDraw || myLink.IsNull()
         ? myFreeBoundaryDraw
         : myLink->FreeBoundaryDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw free boundaries" flag that overrides the one in the link.
  Standard_Boolean HasOwnFreeBoundaryDraw() const { return myHasOwnFreeBoundaryDraw; }

  //! Resets HasOwnFreeBoundaryDraw() flag, e.g. undoes SetFreeBoundaryDraw().
  void UnsetOwnFreeBoundaryDraw()
  {
    myHasOwnFreeBoundaryDraw = false;
    myFreeBoundaryDraw = true;
  }

  //! Sets the parameter theAspect for the display of shared boundaries.
  //! The method sets aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  void SetUnFreeBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myUnFreeBoundaryAspect = theAspect;
  }

  //! Returns own settings for shared boundary line aspects, settings from linked Drawer or NULL if neither was set.
  //! These attributes are used by the algorithm Prs3d_WFShape
  Standard_EXPORT const Handle(Prs3d_LineAspect)& UnFreeBoundaryAspect() const;

  //! Returns true if the drawer has its own attribute for
  //! unfree boundaries aspect that overrides the one in the link.
  Standard_Boolean HasOwnUnFreeBoundaryAspect() const { return !myUnFreeBoundaryAspect.IsNull(); }

  //! Enables or disables drawing of shared boundaries for shading presentations.
  //! The method sets drawing flag owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theIsEnabled is a boolean flag indicating whether the shared boundaries should be drawn or not.
  Standard_EXPORT void SetUnFreeBoundaryDraw (const Standard_Boolean theIsEnabled);

  //! Returns True if the drawing of the shared boundaries is enabled.
  //! True is the default setting.
  Standard_Boolean UnFreeBoundaryDraw() const
  {
    return myHasOwnUnFreeBoundaryDraw || myLink.IsNull()
         ? myUnFreeBoundaryDraw
         : myLink->UnFreeBoundaryDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw shared boundaries" flag that overrides the one in the link.
  Standard_Boolean HasOwnUnFreeBoundaryDraw() const { return myHasOwnUnFreeBoundaryDraw; }

  //! Resets HasOwnUnFreeBoundaryDraw() flag, e.g. undoes SetUnFreeBoundaryDraw().
  void UnsetOwnUnFreeBoundaryDraw()
  {
    myHasOwnUnFreeBoundaryDraw = false;
    myUnFreeBoundaryDraw = true;
  }

  //! Sets line aspect for face boundaries.
  //! The method sets line aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theAspect is the line aspect that determines the look of the face boundaries.
  void SetFaceBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myFaceBoundaryAspect = theAspect;
  }

  //! Returns own line aspect of face boundaries, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_LineAspect)& FaceBoundaryAspect() const;

  //! Returns true if the drawer has its own attribute for
  //! face boundaries aspect that overrides the one in the link.
  Standard_Boolean HasOwnFaceBoundaryAspect() const { return !myFaceBoundaryAspect.IsNull(); }

  //! Sets own face boundary aspect, which is a black solid line by default.
  //! Returns FALSE if the drawer already has its own attribute for face boundary aspect.
  Standard_EXPORT Standard_Boolean SetupOwnFaceBoundaryAspect (const Handle(Prs3d_Drawer)& theDefaults = Handle(Prs3d_Drawer)());

  //! Enables or disables face boundary drawing for shading presentations. 
  //! The method sets drawing flag owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  //! theIsEnabled is a boolean flag indicating whether the face boundaries should be drawn or not.
  Standard_EXPORT void SetFaceBoundaryDraw (const Standard_Boolean theIsEnabled);

  //! Checks whether the face boundary drawing is enabled or not.
  Standard_Boolean FaceBoundaryDraw() const
  {
    return myHasOwnFaceBoundaryDraw || myLink.IsNull()
         ? myFaceBoundaryDraw
         : myLink->FaceBoundaryDraw();
  }

  //! Returns true if the drawer has its own attribute for
  //! "draw face boundaries" flag that overrides the one in the link.
  Standard_Boolean HasOwnFaceBoundaryDraw() const { return myHasOwnFaceBoundaryDraw; }

  //! Resets HasOwnFaceBoundaryDraw() flag, e.g. undoes SetFaceBoundaryDraw().
  void UnsetOwnFaceBoundaryDraw()
  {
    myHasOwnFaceBoundaryDraw = false;
    myFaceBoundaryDraw = false;
  }

  //! Returns true if the drawer has its own attribute for face boundaries upper edge continuity class that overrides the one in the link.
  Standard_Boolean HasOwnFaceBoundaryUpperContinuity() const { return myFaceBoundaryUpperContinuity != -1; }

  //! Get the most edge continuity class; GeomAbs_CN by default (all edges).
  GeomAbs_Shape FaceBoundaryUpperContinuity() const
  {
    return HasOwnFaceBoundaryUpperContinuity()
         ? (GeomAbs_Shape )myFaceBoundaryUpperContinuity
         : (!myLink.IsNull()
           ? myLink->FaceBoundaryUpperContinuity()
           : GeomAbs_CN);
  }

  //! Set the most edge continuity class for face boundaries.
  void SetFaceBoundaryUpperContinuity (GeomAbs_Shape theMostAllowedEdgeClass) { myFaceBoundaryUpperContinuity = theMostAllowedEdgeClass; }

  //! Unset the most edge continuity class for face boundaries.
  void UnsetFaceBoundaryUpperContinuity() { myFaceBoundaryUpperContinuity = -1; }

  //! Returns own settings for the appearance of dimensions, settings from linked Drawer or NULL if neither was set.
  Standard_EXPORT const Handle(Prs3d_DimensionAspect)& DimensionAspect() const;

  //! Sets the settings for the appearance of dimensions.
  //! The method sets aspect owned by the drawer that will be used during
  //! visualization instead of the one set in link.
  void SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theAspect)
  {
    myDimensionAspect = theAspect;
  }

  //! Returns true if the drawer has its own attribute for
  //! the appearance of dimensions that overrides the one in the link.
  Standard_Boolean HasOwnDimensionAspect() const { return !myDimensionAspect.IsNull(); }

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
    return myHasOwnDimLengthModelUnits || myLink.IsNull()
         ? myDimensionModelUnits.GetLengthUnits()
         : myLink->DimLengthModelUnits();
  }

  //! Returns angle model units for the dimension presentation. 
  const TCollection_AsciiString& DimAngleModelUnits() const
  {
    return myHasOwnDimAngleModelUnits || myLink.IsNull()
         ? myDimensionModelUnits.GetAngleUnits()
         : myLink->DimAngleModelUnits();
  }

  //! Returns true if the drawer has its own attribute for
  //! dimension length model units that overrides the one in the link.
  Standard_Boolean HasOwnDimLengthModelUnits() const { return myHasOwnDimLengthModelUnits; }

  //! Resets HasOwnDimLengthModelUnits() flag, e.g. undoes SetDimLengthModelUnits().
  void UnsetOwnDimLengthModelUnits()
  {
    myHasOwnDimLengthModelUnits = false;
    myDimensionModelUnits.SetLengthUnits ("m");
  }

  //! Returns true if the drawer has its own attribute for
  //! dimension angle model units that overrides the one in the link.
  Standard_Boolean HasOwnDimAngleModelUnits() const { return myHasOwnDimAngleModelUnits; }

  //! Resets HasOwnDimAngleModelUnits() flag, e.g. undoes SetDimAngleModelUnits().
  void UnsetOwnDimAngleModelUnits()
  {
    myHasOwnDimAngleModelUnits = false;
    myDimensionModelUnits.SetAngleUnits ("rad");
  }

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
    return myHasOwnDimLengthDisplayUnits || myLink.IsNull()
         ? myDimensionDisplayUnits.GetLengthUnits()
         : myLink->DimLengthDisplayUnits();
  }

  //! Returns angle units in which dimension presentation is displayed.
  const TCollection_AsciiString& DimAngleDisplayUnits() const
  {
    return myHasOwnDimAngleDisplayUnits || myLink.IsNull()
         ? myDimensionDisplayUnits.GetAngleUnits()
         : myLink->DimAngleDisplayUnits();
  }

  //! Returns true if the drawer has its own attribute for
  //! length units in which dimension presentation is displayed
  //! that overrides the one in the link.
  Standard_Boolean HasOwnDimLengthDisplayUnits() const { return myHasOwnDimLengthDisplayUnits; }

  //! Resets HasOwnDimLengthModelUnits() flag, e.g. undoes SetDimLengthDisplayUnits().
  void UnsetOwnDimLengthDisplayUnits()
  {
    myHasOwnDimLengthDisplayUnits = false;
    myDimensionDisplayUnits.SetLengthUnits ("m");
  }

  //! Returns true if the drawer has its own attribute for
  //! angle units in which dimension presentation is displayed
  //! that overrides the one in the link.
  Standard_Boolean HasOwnDimAngleDisplayUnits() const { return myHasOwnDimAngleDisplayUnits; }

  //! Resets HasOwnDimAngleDisplayUnits() flag, e.g. undoes SetDimLengthDisplayUnits().
  void UnsetOwnDimAngleDisplayUnits()
  {
    myHasOwnDimAngleDisplayUnits = false;
    myDimensionDisplayUnits.SetAngleUnits ("deg");
  }

public:

  //! Returns the drawer to which the current object references.
  const Handle(Prs3d_Drawer)& Link() const { return myLink; }

  //! Returns true if the current object has a link on the other drawer.
  Standard_Boolean HasLink() const { return !myLink.IsNull(); }

  //! Sets theDrawer as a link to which the current object references.
  void Link (const Handle(Prs3d_Drawer)& theDrawer) { SetLink (theDrawer); }

  //! Sets theDrawer as a link to which the current object references.
  void SetLink (const Handle(Prs3d_Drawer)& theDrawer) { myLink = theDrawer; }

  //! Removes local attributes. 
  Standard_EXPORT void ClearLocalAttributes();

  //! Assign shader program for specified type of primitives.
  //! @param theProgram new program to set (might be NULL)
  //! @param theAspect  the type of primitives
  //! @param theToOverrideDefaults if true then non-overridden attributes using defaults will be allocated and copied from the Link;
  //!                              otherwise, only already customized attributes will be changed
  //! @return TRUE if presentation should be recomputed after creating aspects not previously customized (if theToOverrideDefaults is also TRUE)
  Standard_EXPORT bool SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                         const Graphic3d_GroupAspect            theAspect,
                                         const bool                             theToOverrideDefaults = false);

  //! Sets Shading Model type for the shading aspect.
  Standard_EXPORT bool SetShadingModel (Graphic3d_TypeOfShadingModel theModel,
                                        bool theToOverrideDefaults = false);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

public: //! @name deprecated methods

  Standard_DEPRECATED("SetDeviationAngle() should be used instead")
  void SetHLRAngle (const Standard_Real theAngle) { SetDeviationAngle (theAngle); }

  Standard_DEPRECATED("DeviationAngle() should be used instead")
  Standard_Real HLRAngle() const { return DeviationAngle(); }

  Standard_DEPRECATED("SetDeviationAngle() should be used instead")
  void SetHLRAngle() { SetDeviationAngle(); }

  Standard_DEPRECATED("HasOwnDeviationAngle() should be used instead")
  Standard_Boolean HasOwnHLRDeviationAngle() const { return HasOwnDeviationAngle(); }

  Standard_DEPRECATED("PreviousDeviationAngle() should be used instead")
  Standard_Real PreviousHLRDeviationAngle() const { return PreviousDeviationAngle(); }

protected:

  Handle(Prs3d_Drawer)          myLink;

  Standard_Integer              myNbPoints;
  Standard_Real                 myMaximalParameterValue;
  Standard_Real                 myChordialDeviation;
  Aspect_TypeOfDeflection       myTypeOfDeflection;
  Standard_Boolean              myHasOwnTypeOfDeflection;
  Prs3d_TypeOfHLR               myTypeOfHLR;
  Standard_Real                 myDeviationCoefficient;
  Standard_Real                 myPreviousDeviationCoefficient;
  Standard_Real                 myDeviationAngle;
  Standard_Real                 myPreviousDeviationAngle;
  Standard_Boolean              myIsoOnPlane;
  Standard_Boolean              myHasOwnIsoOnPlane;
  Standard_Boolean              myIsoOnTriangulation;
  Standard_Boolean              myHasOwnIsoOnTriangulation;
  Standard_Boolean              myIsAutoTriangulated;
  Standard_Boolean              myHasOwnIsAutoTriangulated;

  Handle(Prs3d_IsoAspect)       myUIsoAspect;
  Handle(Prs3d_IsoAspect)       myVIsoAspect;
  Handle(Prs3d_LineAspect)      myWireAspect;
  Standard_Boolean              myWireDraw;
  Standard_Boolean              myHasOwnWireDraw;
  Handle(Prs3d_PointAspect)     myPointAspect;
  Handle(Prs3d_LineAspect)      myLineAspect;
  Handle(Prs3d_TextAspect)      myTextAspect;
  Handle(Prs3d_ShadingAspect)   myShadingAspect;
  Handle(Prs3d_PlaneAspect)     myPlaneAspect;
  Handle(Prs3d_LineAspect)      mySeenLineAspect;
  Handle(Prs3d_ArrowAspect)     myArrowAspect;
  Standard_Boolean              myLineArrowDraw;
  Standard_Boolean              myHasOwnLineArrowDraw;
  Handle(Prs3d_LineAspect)      myHiddenLineAspect;
  Standard_Boolean              myDrawHiddenLine;
  Standard_Boolean              myHasOwnDrawHiddenLine;
  Handle(Prs3d_LineAspect)      myVectorAspect;
  Prs3d_VertexDrawMode          myVertexDrawMode;
  Handle(Prs3d_DatumAspect)     myDatumAspect;
  Handle(Prs3d_LineAspect)      mySectionAspect;

  Handle(Prs3d_LineAspect)      myFreeBoundaryAspect;
  Standard_Boolean              myFreeBoundaryDraw;
  Standard_Boolean              myHasOwnFreeBoundaryDraw;
  Handle(Prs3d_LineAspect)      myUnFreeBoundaryAspect;
  Standard_Boolean              myUnFreeBoundaryDraw;
  Standard_Boolean              myHasOwnUnFreeBoundaryDraw;
  Handle(Prs3d_LineAspect)      myFaceBoundaryAspect;
  Standard_Integer              myFaceBoundaryUpperContinuity; //!< the most edge continuity class (GeomAbs_Shape) to be included to face boundaries presentation, or -1 if undefined
  Standard_Boolean              myFaceBoundaryDraw;
  Standard_Boolean              myHasOwnFaceBoundaryDraw;

  Handle(Prs3d_DimensionAspect) myDimensionAspect;
  Prs3d_DimensionUnits          myDimensionModelUnits;
  Standard_Boolean              myHasOwnDimLengthModelUnits;
  Standard_Boolean              myHasOwnDimAngleModelUnits;
  Prs3d_DimensionUnits          myDimensionDisplayUnits;
  Standard_Boolean              myHasOwnDimLengthDisplayUnits;
  Standard_Boolean              myHasOwnDimAngleDisplayUnits;
};

#endif // _Prs3d_Drawer_HeaderFile
