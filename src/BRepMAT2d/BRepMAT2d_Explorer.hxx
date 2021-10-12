// Created on: 1994-10-04
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepMAT2d_Explorer_HeaderFile
#define _BRepMAT2d_Explorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <MAT2d_SequenceOfSequenceOfCurve.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <Standard_Boolean.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
class TopoDS_Face;
class TopoDS_Wire;
class Geom2d_Curve;


//! Construct an explorer from wires, face, set of curves
//! from Geom2d to compute the bisecting Locus.
class BRepMAT2d_Explorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepMAT2d_Explorer();
  
  Standard_EXPORT BRepMAT2d_Explorer(const TopoDS_Face& aFace);
  
  //! Clear the contents of <me>.
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void Perform (const TopoDS_Face& aFace);
  
  //! Returns the Number of contours.
  Standard_EXPORT Standard_Integer NumberOfContours() const;
  
  //! Returns the Number of Curves in the Contour  number
  //! <IndexContour>.
  Standard_EXPORT Standard_Integer NumberOfCurves (const Standard_Integer IndexContour) const;
  
  //! Initialisation of  an Iterator on  the curves of
  //! the Contour number <IndexContour>.
  Standard_EXPORT void Init (const Standard_Integer IndexContour);
  
  //! Return False if there is no more curves on the Contour
  //! initialised by the method Init.
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Move to the next curve of the current Contour.
  Standard_EXPORT void Next();
  
  //! Returns the current curve on the current Contour.
  Standard_EXPORT Handle(Geom2d_Curve) Value() const;
  
  Standard_EXPORT TopoDS_Shape Shape() const;
  
  Standard_EXPORT const TColGeom2d_SequenceOfCurve& Contour (const Standard_Integer IndexContour) const;
  
  Standard_EXPORT Standard_Boolean IsModified (const TopoDS_Shape& aShape) const;
  
  //! If the shape is not modified, returns the shape itself.
  Standard_EXPORT TopoDS_Shape ModifiedShape (const TopoDS_Shape& aShape) const;
  
  Standard_EXPORT const TColStd_SequenceOfBoolean& GetIsClosed() const;




protected:





private:

  
  //! Construction from a set of curves from Geom2d.
  //! Assume  the   orientation  of  the  closed   lines are
  //! compatible. (ie if A is in B, the orientation of A and B
  //! has to be different.
  //!
  //! Assume the explo contains only lines located in the
  //! area where the bisecting locus will be computed.
  //!
  //! Assume a line don't cross itself or an other line.
  //!
  //! A contour has to be construct in adding each curve in
  //! respect to the sense of the contour.
  //!
  //! afirst point of a curve in a contour is equal to the last
  //! point of the precedent curve.
  //!
  //! No  control of this  rules is done in the construction
  //! of the explorer
  Standard_EXPORT void Add (const TopoDS_Wire& Spine, const TopoDS_Face& aFace, TopoDS_Face& aNewFace);
  
  Standard_EXPORT void NewContour();
  
  //! Add the curve <aCurve> at me.
  Standard_EXPORT void Add (const Handle(Geom2d_Curve)& aCurve);


  MAT2d_SequenceOfSequenceOfCurve theCurves;
  Standard_Integer current;
  Standard_Integer currentContour;
  TopoDS_Shape myShape;
  TColStd_SequenceOfBoolean myIsClosed;
  TopTools_IndexedDataMapOfShapeShape myModifShapes;


};







#endif // _BRepMAT2d_Explorer_HeaderFile
