// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//! A framework to define display of angles. <br>
//! These displays are particularly useful in viewing draft prisms. <br>
//! The angle displayed may define an intersection <br>
//! can be between two edges or two faces of a shape <br>
//! or a plane. The display consists of arrows and text. <br>

#ifndef _AIS_AngleDimension_HeaderFile
#define _AIS_AngleDimension_HeaderFile

#include <AIS_Dimension.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom_Transformation.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Projector.hxx>
#include <Prs3d_Presentation.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

DEFINE_STANDARD_HANDLE (AIS_AngleDimension, AIS_Dimension)

class AIS_AngleDimension : public AIS_Dimension
{
public:

  //! Constructs angle dimension between two edges
  //! with automatic working plane computing
  //! if it is possible. In case of PI angle please
  //! set custom working plane or use constructor with 3 parameters.
  Standard_EXPORT  AIS_AngleDimension (const TopoDS_Edge& theFirstEdge,
                                       const TopoDS_Edge& theSecondEdge);
  //!  Constructs the angle display object defined by the <br>
  //! two edges and custom working plane.
  //! ATTENTION :In case if the working plane is custom and one edge is out of the
  //!            working plane it tries to project this edge line on the plane.
  //!            To avoid this case you can reset the working plane
  //!            using <ResetWorkingPlane ()> method.
  Standard_EXPORT  AIS_AngleDimension (const TopoDS_Edge& theFirstEdge,
                                       const TopoDS_Edge& theSecondEdge,
                                       const gp_Pln& thePlane);

  //! Constructs the angle display object defined by three points.
  Standard_EXPORT  AIS_AngleDimension (const gp_Pnt& theFirstPoint,
                                       const gp_Pnt& theSecondPoint,
                                       const gp_Pnt& theThirdPoint);

  //! Angle of cone
  Standard_EXPORT  AIS_AngleDimension (const TopoDS_Face& theCone);

  //! TwoPlanarFaceAngle dimension
  Standard_EXPORT  AIS_AngleDimension (const TopoDS_Face& theFirstFace,
                                       const TopoDS_Face& theSecondFace,
                                       const gp_Ax1& theAxis);

  //! Sets first shape
  Standard_EXPORT  void SetFirstShape (const TopoDS_Shape& theShape,
                                       const Standard_Boolean isSingleShape = Standard_False);

public:

  DEFINE_STANDARD_RTTI (AIS_AngleDimension)

protected:

  //! Computes dimension value in display units
  Standard_EXPORT virtual void computeValue();

  Standard_EXPORT  void init();

  Standard_EXPORT  gp_Pnt getCenterOnArc (const gp_Pnt& theFirstAttach,
                                          const gp_Pnt& theSecondAttach);

  Standard_EXPORT  void drawArc (const Handle(Prs3d_Presentation)& thePresentation,
                                 const gp_Pnt& theFirstAttach,
                                 const gp_Pnt& theSecondAttach,
                                 const gp_Pnt& theCenter,
                                 const Standard_Real theRadius,
                                 const AIS_DimensionDisplayMode theMode);

  Standard_EXPORT  void drawArcWithText (const Handle(Prs3d_Presentation)& thePresentation,
                                         const gp_Pnt& theFirstAttach,
                                         const gp_Pnt& theSecondAttach,
                                         const TCollection_ExtendedString& theText,
                                         const AIS_DimensionDisplayMode theMode,
                                         const Standard_Integer theLabelPosition);

  Standard_EXPORT  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                         const Handle(Prs3d_Presentation)& thePresentation,
                                         const Standard_Integer theMode = 0);

  Standard_EXPORT  Standard_Boolean initConeAngle (const TopoDS_Face& theCone);

  Standard_EXPORT  Standard_Boolean initTwoFacesAngle();

  Standard_EXPORT  Standard_Boolean initTwoEdgesAngle();

  //! Auxiliary method to get position of the angle dimension
  //! if the cone is trimmed
  //! Returns 1 if <theC> center is above of <theCMin> center;
  //!         0 if <theC> center is between <theCMin> and <theCMax> centers;
  //!        -1 if <theC> center is below <theCMax> center.
  Standard_EXPORT Standard_Integer aboveInBelowCone (const gp_Circ &theCMax,
                                                     const gp_Circ &theCMin,
                                                     const gp_Circ &theC);

  //! Fills default plane object if it is possible to count plane automatically.
  Standard_EXPORT virtual void countDefaultPlane ();
  
 //! Fills sensitive entity for flyouts and adds it to the selection
 Standard_EXPORT virtual void computeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                      const Handle(AIS_DimensionOwner)& theOwner);

protected:

  //! Shows if there is necessarily to draw extensions on angle dimension
  //! It is set to the true value if the attachment point are out of the edges.
  Standard_Boolean myIsFlyoutLines;

  //! The center of dimension arc
  gp_Pnt myCenter;
};

#endif
