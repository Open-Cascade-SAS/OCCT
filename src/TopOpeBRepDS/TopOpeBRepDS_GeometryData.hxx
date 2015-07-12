// Created on: 1994-05-26
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_GeometryData_HeaderFile
#define _TopOpeBRepDS_GeometryData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_ListOfInterference.hxx>
class TopOpeBRepDS_Interference;


//! mother-class of SurfaceData, CurveData, PointData
class TopOpeBRepDS_GeometryData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_GeometryData();
  
  Standard_EXPORT TopOpeBRepDS_GeometryData(const TopOpeBRepDS_GeometryData& Other);
  
  Standard_EXPORT void Assign (const TopOpeBRepDS_GeometryData& Other);
void operator= (const TopOpeBRepDS_GeometryData& Other)
{
  Assign(Other);
}
  
  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& Interferences() const;
  
  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeInterferences();
  
  Standard_EXPORT void AddInterference (const Handle(TopOpeBRepDS_Interference)& I);




protected:





private:



  TopOpeBRepDS_ListOfInterference myInterferences;


};







#endif // _TopOpeBRepDS_GeometryData_HeaderFile
