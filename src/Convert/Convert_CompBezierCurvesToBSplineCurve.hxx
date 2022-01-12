// Created on: 1993-10-20
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Convert_CompBezierCurvesToBSplineCurve_HeaderFile
#define _Convert_CompBezierCurvesToBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Convert_SequenceOfArray1OfPoles.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>


//! An algorithm to convert a sequence of adjacent
//! non-rational Bezier curves into a BSpline curve.
//! A CompBezierCurvesToBSplineCurve object provides a framework for:
//! -   defining the sequence of adjacent non-rational Bezier
//! curves to be converted into a BSpline curve,
//! -   implementing the computation algorithm, and
//! -   consulting the results.
//! Warning
//! Do not attempt to convert rational Bezier curves using this type of algorithm.
class Convert_CompBezierCurvesToBSplineCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a framework for converting a sequence of
  //! adjacent non-rational Bezier curves into a BSpline curve.
  //! Knots will be created on the computed BSpline curve at
  //! each junction point of two consecutive Bezier curves. The
  //! degree of continuity of the BSpline curve will be increased at
  //! the junction point of two consecutive Bezier curves if their
  //! tangent vectors at this point are parallel. AngularTolerance
  //! (given in radians, and defaulted to 1.0 e-4) will be used
  //! to check the parallelism of the two tangent vectors.
  //! Use the following functions:
  //! -   AddCurve to define in sequence the adjacent Bezier
  //! curves to be converted,
  //! -   Perform to compute the data needed to build the BSpline curve,
  //! -   and the available consultation functions to access the
  //! computed data. This data may be used to construct the BSpline curve.
  Standard_EXPORT Convert_CompBezierCurvesToBSplineCurve(const Standard_Real AngularTolerance = 1.0e-4);
  
  //! Adds the Bezier curve defined by the table of poles Poles, to
  //! the sequence (still contained in this framework) of adjacent
  //! Bezier curves to be converted into a BSpline curve.
  //! Only polynomial (i.e. non-rational) Bezier curves are
  //! converted using this framework.
  //! If this is not the first call to the function (i.e. if this framework
  //! still contains data in its Bezier curve sequence), the degree
  //! of continuity of the BSpline curve will be increased at the
  //! time of computation at the first point of the added Bezier
  //! curve (i.e. the first point of the Poles table). This will be the
  //! case if the tangent vector of the curve at this point is
  //! parallel to the tangent vector at the end point of the
  //! preceding Bezier curve in the Bezier curve sequence still
  //! contained in this framework. An angular tolerance given at
  //! the time of construction of this framework will be used to
  //! check the parallelism of the two tangent vectors. This
  //! checking procedure and all related computations will be
  //! performed by the Perform function.
  //! When the adjacent Bezier curve sequence is complete, use
  //! the following functions:
  //! -   Perform to compute the data needed to build the BSpline curve,
  //! -   and the available consultation functions to access the
  //! computed data. This data may be used to construct the BSpline curve.
  //! Warning
  //! The Bezier curve sequence treated by this framework is
  //! automatically initialized with the first Bezier curve when the
  //! function is first called. During subsequent use of this function,
  //! ensure that the first point of the added Bezier curve (i.e. the
  //! first point of the Poles table) is coincident with the last point
  //! of the Bezier curve sequence (i.e. the last point of the
  //! preceding Bezier curve in the sequence) still contained in
  //! this framework. An error may occur at the time of
  //! computation if this condition is not satisfied. Particular care
  //! must be taken with respect to the above, as this condition is
  //! not checked either when defining the Bezier curve
  //! sequence or at the time of computation.
  Standard_EXPORT void AddCurve (const TColgp_Array1OfPnt& Poles);
  
  //! Computes all the data needed to build a BSpline curve
  //! equivalent to the adjacent Bezier curve sequence still
  //! contained in this framework.
  //! A knot is inserted on the computed BSpline curve at the
  //! junction point of two consecutive Bezier curves. The
  //! degree of continuity of the BSpline curve will be increased
  //! at the junction point of two consecutive Bezier curves if
  //! their tangent vectors at this point are parallel. An angular
  //! tolerance given at the time of construction of this
  //! framework is used to check the parallelism of the two
  //! tangent vectors.
  //! Use the available consultation functions to access the
  //! computed data. This data may then be used to construct
  //! the BSpline curve.
  //! Warning
  //! Make sure that the curves in the Bezier curve sequence
  //! contained in this framework are adjacent. An error may
  //! occur at the time of computation if this condition is not
  //! satisfied. Particular care must be taken with respect to the
  //! above as this condition is not checked, either when
  //! defining the Bezier curve sequence or at the time of computation.
  Standard_EXPORT void Perform();
  
  //! Returns the degree of the BSpline curve whose data is
  //! computed in this framework.
  //! Warning
  //! Take particular care not to use this function before the
  //! computation is performed (Perform function), as this
  //! condition is not checked and an error may  therefore occur.
  Standard_EXPORT Standard_Integer Degree() const;
  
  //! Returns the number of poles of the BSpline curve whose
  //! data is computed in this framework.
  //! Warning
  //! Take particular care not to use this function before the
  //! computation is performed (Perform function), as this
  //! condition is not checked and an error may therefore occur.
  Standard_EXPORT Standard_Integer NbPoles() const;
  
  //! Loads the Poles table with the poles of the BSpline curve
  //! whose data is computed in this framework.
  //! Warning
  //! -   Do not use this function before the computation is
  //! performed (Perform function).
  //! -   The length of the Poles array must be equal to the
  //! number of poles of the BSpline curve whose data is
  //! computed in this framework.
  //! Particular care must be taken with respect to the above, as
  //! these conditions are not checked, and an error may occur.
  Standard_EXPORT void Poles (TColgp_Array1OfPnt& Poles) const;
  
  //! Returns the number of knots of the BSpline curve whose
  //! data is computed in this framework.
  //! Warning
  //! Take particular care not to use this function before the
  //! computation is performed (Perform function), as this
  //! condition is not checked and an error may therefore occur.
  Standard_EXPORT Standard_Integer NbKnots() const;
  

  //! -   loads the Knots table with the knots,
  //! -   and loads the Mults table with the corresponding multiplicities
  //! of the BSpline curve whose data is computed in this framework.
  //! Warning
  //! -   Do not use this function before the computation is
  //! performed (Perform function).
  //! -   The length of the Knots and Mults arrays must be equal
  //! to the number of knots in the BSpline curve whose data is
  //! computed in this framework.
  //! Particular care must be taken with respect to the above as
  //! these conditions are not checked, and an error may occur.
  Standard_EXPORT void KnotsAndMults (TColStd_Array1OfReal& Knots, TColStd_Array1OfInteger& Mults) const;




protected:





private:



  Convert_SequenceOfArray1OfPoles mySequence;
  TColgp_SequenceOfPnt CurvePoles;
  TColStd_SequenceOfReal CurveKnots;
  TColStd_SequenceOfInteger KnotsMultiplicities;
  Standard_Integer myDegree;
  Standard_Real myAngular;
  Standard_Boolean myDone;


};







#endif // _Convert_CompBezierCurvesToBSplineCurve_HeaderFile
