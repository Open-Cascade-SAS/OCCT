// Created on: 1998-08-25
// Created by: Pavel DURANDIN <pdn@nnov.matra-dtv.fr>
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

#ifndef _ShapeAnalysis_FreeBoundData_HeaderFile
#define _ShapeAnalysis_FreeBoundData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Wire.hxx>
#include <Standard_Real.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Wire;


class ShapeAnalysis_FreeBoundData;
DEFINE_STANDARD_HANDLE(ShapeAnalysis_FreeBoundData, Standard_Transient)

//! This class is intended to represent free bound and to store
//! its properties.
//!
//! This class is used by ShapeAnalysis_FreeBoundsProperties
//! class when storing each free bound and its properties.
//!
//! The properties stored in this class are the following:
//! - area of the contour,
//! - perimeter of the contour,
//! - ratio of average length to average width of the contour,
//! - average width of contour,
//! - notches (narrow 'V'-like sub-contours) on the contour and
//! their maximum width.
//!
//! This class provides methods for setting and getting fields
//! only.
class ShapeAnalysis_FreeBoundData : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_FreeBoundData();
  
  //! Creates object with contour given in the form of TopoDS_Wire
  Standard_EXPORT ShapeAnalysis_FreeBoundData(const TopoDS_Wire& freebound);
  
  //! Clears all properties of the contour.
  //! Contour bound itself is not cleared.
  Standard_EXPORT void Clear();
  
  //! Sets contour
    void SetFreeBound (const TopoDS_Wire& freebound);
  
  //! Sets area of the contour
    void SetArea (const Standard_Real area);
  
  //! Sets perimeter of the contour
    void SetPerimeter (const Standard_Real perimeter);
  
  //! Sets ratio of average length to average width of the contour
    void SetRatio (const Standard_Real ratio);
  
  //! Sets average width of the contour
    void SetWidth (const Standard_Real width);
  
  //! Adds notch on the contour with its maximum width
  Standard_EXPORT void AddNotch (const TopoDS_Wire& notch, const Standard_Real width);
  
  //! Returns contour
    TopoDS_Wire FreeBound() const;
  
  //! Returns area of the contour
    Standard_Real Area() const;
  
  //! Returns perimeter of the contour
    Standard_Real Perimeter() const;
  
  //! Returns ratio of average length to average width of the contour
    Standard_Real Ratio() const;
  
  //! Returns average width of the contour
    Standard_Real Width() const;
  
  //! Returns number of notches on the contour
    Standard_Integer NbNotches() const;
  
  //! Returns sequence of notches on the contour
    Handle(TopTools_HSequenceOfShape) Notches() const;
  
  //! Returns notch on the contour
    TopoDS_Wire Notch (const Standard_Integer index) const;
  
  //! Returns maximum width of notch specified by its rank number
  //! on the contour
  Standard_EXPORT Standard_Real NotchWidth (const Standard_Integer index) const;
  
  //! Returns maximum width of notch specified as TopoDS_Wire
  //! on the contour
  Standard_EXPORT Standard_Real NotchWidth (const TopoDS_Wire& notch) const;




  DEFINE_STANDARD_RTTIEXT(ShapeAnalysis_FreeBoundData,Standard_Transient)

protected:




private:


  TopoDS_Wire myBound;
  Standard_Real myArea;
  Standard_Real myPerimeter;
  Standard_Real myRatio;
  Standard_Real myWidth;
  Handle(TopTools_HSequenceOfShape) myNotches;
  TopTools_DataMapOfShapeReal myNotchesParams;


};


#include <ShapeAnalysis_FreeBoundData.lxx>





#endif // _ShapeAnalysis_FreeBoundData_HeaderFile
