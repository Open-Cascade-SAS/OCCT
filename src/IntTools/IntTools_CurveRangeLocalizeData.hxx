// Created on: 2005-10-14
// Created by: Mikhail KLOKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_CurveRangeLocalizeData_HeaderFile
#define _IntTools_CurveRangeLocalizeData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <IntTools_MapOfCurveSample.hxx>
#include <IntTools_DataMapOfCurveSampleBox.hxx>
#include <IntTools_ListOfCurveRangeSample.hxx>

class IntTools_CurveRangeSample;
class Bnd_Box;

class IntTools_CurveRangeLocalizeData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntTools_CurveRangeLocalizeData(const Standard_Integer theNbSample, const Standard_Real theMinRange);
  
  Standard_Integer GetNbSample() const { return myNbSampleC; }

  Standard_Real GetMinRange() const { return myMinRangeC; }

  Standard_EXPORT void AddOutRange (const IntTools_CurveRangeSample& theRange);

  Standard_EXPORT void AddBox (const IntTools_CurveRangeSample& theRange, const Bnd_Box& theBox);

  Standard_EXPORT Standard_Boolean FindBox (const IntTools_CurveRangeSample& theRange, Bnd_Box& theBox) const;

  Standard_EXPORT Standard_Boolean IsRangeOut (const IntTools_CurveRangeSample& theRange) const;

  Standard_EXPORT void ListRangeOut (IntTools_ListOfCurveRangeSample& theList) const;

private:

  Standard_Integer myNbSampleC;
  Standard_Real myMinRangeC;
  IntTools_MapOfCurveSample myMapRangeOut;
  IntTools_DataMapOfCurveSampleBox myMapBox;

};

#endif // _IntTools_CurveRangeLocalizeData_HeaderFile
