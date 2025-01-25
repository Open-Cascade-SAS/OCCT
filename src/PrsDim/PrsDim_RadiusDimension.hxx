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

#ifndef _PrsDim_RadiusDimension_HeaderFile
#define _PrsDim_RadiusDimension_HeaderFile

#include <PrsDim_Dimension.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>

DEFINE_STANDARD_HANDLE(PrsDim_RadiusDimension, PrsDim_Dimension)

//! Radius dimension. Can be constructed:
//! - On generic circle.
//! - On generic circle with user-defined anchor point on that circle.
//! - On generic shape containing geometry that can be measured
//!   by diameter dimension: circle wire, arc, circular face, etc.
//! The anchor point is the location of left attachment point of
//! dimension on the circle. It can be user-specified, or computed as
//! middle point on the arc. The radius dimension always lies in the
//! plane of the measured circle. The dimension is considered as
//! invalid if the user-specified anchor point is not lying on the circle,
//! if the radius of the circle is less than Precision::Confusion().
//! In case if the dimension is built on the arbitrary shape,
//! it can be considered as invalid if the shape does not contain
//! circle geometry.
class PrsDim_RadiusDimension : public PrsDim_Dimension
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_RadiusDimension, PrsDim_Dimension)
public:
  //! Create radius dimension for the circle geometry.
  //! @param[in] theCircle  the circle to measure.
  Standard_EXPORT PrsDim_RadiusDimension(const gp_Circ& theCircle);

  //! Create radius dimension for the circle geometry and define its
  //! orientation by location of the first point on that circle.
  //! @param[in] theCircle  the circle to measure.
  //! @param[in] theAnchorPoint  the point to define the position
  //!        of the dimension attachment on the circle.
  Standard_EXPORT PrsDim_RadiusDimension(const gp_Circ& theCircle, const gp_Pnt& theAnchorPoint);

  //! Create radius dimension for the arbitrary shape (if possible).
  //! @param[in] theShape  the shape to measure.
  Standard_EXPORT PrsDim_RadiusDimension(const TopoDS_Shape& theShape);

public:
  //! @return measured geometry circle.
  const gp_Circ& Circle() const { return myCircle; }

  //! @return anchor point on circle for radius dimension.
  const gp_Pnt& AnchorPoint() const { return myAnchorPoint; }

  //! @return the measured shape.
  const TopoDS_Shape& Shape() const { return myShape; }

public:
  //! Measure radius of the circle.
  //! The dimension will become invalid if the radius of the circle
  //! is less than Precision::Confusion().
  //! @param[in] theCircle  the circle to measure.
  void SetMeasuredGeometry(const gp_Circ& theCircle)
  {
    SetMeasuredGeometry(theCircle, gp_Pnt(), Standard_False);
  }

  //! Measure radius of the circle and orient the dimension so
  //! the dimension lines attaches to anchor point on the circle.
  //! The dimension will become invalid if the radius of the circle
  //! is less than Precision::Confusion().
  //! @param[in] theCircle  the circle to measure.
  //! @param[in] theAnchorPoint  the point to attach the dimension lines, should be on the circle
  //! @param[in] theHasAnchor    should be set TRUE if theAnchorPoint should be used
  Standard_EXPORT void SetMeasuredGeometry(const gp_Circ&         theCircle,
                                           const gp_Pnt&          theAnchorPoint,
                                           const Standard_Boolean theHasAnchor = Standard_True);

  //! Measure radius on the passed shape, if applicable.
  //! The dimension will become invalid if the passed shape is not
  //! measurable or if measured diameter value is less than Precision::Confusion().
  //! @param[in] theShape  the shape to measure.
  void SetMeasuredGeometry(const TopoDS_Shape& theShape)
  {
    SetMeasuredGeometry(theShape, gp_Pnt(), Standard_False);
  }

  //! Measure radius on the passed shape, if applicable.
  //! The dimension will become invalid if the passed shape is not
  //! measurable or if measured diameter value is less than Precision::Confusion().
  //! @param[in] theShape  the shape to measure.
  //! @param[in] theAnchorPoint  the point to attach the dimension lines, should be on the circle
  //! @param[in] theHasAnchor    should be set TRUE if theAnchorPoint should be used
  Standard_EXPORT void SetMeasuredGeometry(const TopoDS_Shape&    theShape,
                                           const gp_Pnt&          theAnchorPoint,
                                           const Standard_Boolean theHasAnchor = Standard_True);

  //! @return the display units string.
  Standard_EXPORT virtual const TCollection_AsciiString& GetDisplayUnits() const Standard_OVERRIDE;

  //! @return the model units string.
  Standard_EXPORT virtual const TCollection_AsciiString& GetModelUnits() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetDisplayUnits(const TCollection_AsciiString& theUnits)
    Standard_OVERRIDE;

  Standard_EXPORT virtual void SetModelUnits(const TCollection_AsciiString& theUnits)
    Standard_OVERRIDE;

  Standard_EXPORT virtual void SetTextPosition(const gp_Pnt& theTextPos) Standard_OVERRIDE;

  Standard_EXPORT virtual gp_Pnt GetTextPosition() const Standard_OVERRIDE;

protected:
  Standard_EXPORT virtual void ComputePlane();

  //! Checks if anchor point and the center of the circle are on the plane.
  Standard_EXPORT virtual Standard_Boolean CheckPlane(const gp_Pln& thePlane) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Real ComputeValue() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                       const Handle(Prs3d_Presentation)&         thePrs,
                                       const Standard_Integer theMode) Standard_OVERRIDE;

protected:
  Standard_EXPORT Standard_Boolean IsValidCircle(const gp_Circ& theCircle) const;

  Standard_EXPORT Standard_Boolean IsValidAnchor(const gp_Circ& theCircle,
                                                 const gp_Pnt&  thePnt) const;

private:
  gp_Circ      myCircle;
  gp_Pnt       myAnchorPoint;
  TopoDS_Shape myShape;
};

#endif // _PrsDim_RadiusDimension_HeaderFile
