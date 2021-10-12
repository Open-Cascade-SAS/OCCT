// Created on: 1994-02-21
// Created by: Laurent PAINNOT
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

#ifndef _Approx_MCurvesToBSpCurve_HeaderFile
#define _Approx_MCurvesToBSpCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiBSpCurve.hxx>
#include <Standard_Boolean.hxx>
#include <AppParCurves_SequenceOfMultiCurve.hxx>
class AppParCurves_MultiCurve;



class Approx_MCurvesToBSpCurve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Approx_MCurvesToBSpCurve();
  
  Standard_EXPORT void Reset();
  
  Standard_EXPORT void Append (const AppParCurves_MultiCurve& MC);
  
  Standard_EXPORT void Perform();
  
  Standard_EXPORT void Perform (const AppParCurves_SequenceOfMultiCurve& TheSeq);
  
  //! return the composite MultiCurves as a MultiBSpCurve.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& Value() const;
  
  //! return the composite MultiCurves as a MultiBSpCurve.
  Standard_EXPORT const AppParCurves_MultiBSpCurve& ChangeValue();




protected:





private:



  AppParCurves_MultiBSpCurve mySpline;
  Standard_Boolean myDone;
  AppParCurves_SequenceOfMultiCurve myCurves;


};







#endif // _Approx_MCurvesToBSpCurve_HeaderFile
