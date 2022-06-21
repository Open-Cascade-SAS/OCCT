// Created on: 1998-03-12
// Created by: Pierre BARRAS
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

#ifndef _ShapeUpgrade_SplitCurve_HeaderFile
#define _ShapeUpgrade_SplitCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <ShapeExtend_Status.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeUpgrade_SplitCurve;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_SplitCurve, Standard_Transient)

//! Splits a  curve with a  criterion.
class ShapeUpgrade_SplitCurve : public Standard_Transient
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitCurve();
  
  //! Initializes with curve first and last parameters.
  Standard_EXPORT void Init (const Standard_Real First, const Standard_Real Last);
  
  //! Sets the parameters where splitting has to be done.
  Standard_EXPORT void SetSplitValues (const Handle(TColStd_HSequenceOfReal)& SplitValues);
  
  //! If Segment is True, the result is composed with
  //! segments of the curve bounded by the SplitValues.  If
  //! Segment is False, the result is composed with trimmed
  //! Curves all based on the same complete curve.
  Standard_EXPORT virtual void Build (const Standard_Boolean Segment);
  
  //! returns all the splitting values including the
  //! First and Last parameters of the input curve
  //! Merges input split values and new ones into myGlobalKnots
  Standard_EXPORT const Handle(TColStd_HSequenceOfReal)& SplitValues() const;
  
  //! Calculates points for correction/splitting of the curve
  Standard_EXPORT virtual void Compute();
  
  //! Performs correction/splitting of the curve.
  //! First defines splitting values by method Compute(), then calls method Build().
  Standard_EXPORT void Perform (const Standard_Boolean Segment = Standard_True);
  
  //! Returns the status
  //! OK    - no splitting is needed
  //! DONE1 - splitting required and gives more than one segment
  //! DONE2 - splitting is required, but gives only one segment (initial)
  //! DONE3 - geometric form of the curve or parametrisation is modified
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve,Standard_Transient)

protected:


  Handle(TColStd_HSequenceOfReal) mySplitValues;
  Standard_Integer myNbCurves;
  Standard_Integer myStatus;


private:




};







#endif // _ShapeUpgrade_SplitCurve_HeaderFile
