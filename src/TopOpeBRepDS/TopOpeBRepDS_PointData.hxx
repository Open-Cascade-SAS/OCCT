// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_PointData_HeaderFile
#define _TopOpeBRepDS_PointData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_Point.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_GeometryData.hxx>
class TopOpeBRepDS_DataStructure;
class TopOpeBRepDS_Point;



class TopOpeBRepDS_PointData  : public TopOpeBRepDS_GeometryData
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_PointData();
  
  Standard_EXPORT TopOpeBRepDS_PointData(const TopOpeBRepDS_Point& P);
  
  Standard_EXPORT TopOpeBRepDS_PointData(const TopOpeBRepDS_Point& P, const Standard_Integer I1, const Standard_Integer I2);
  
  Standard_EXPORT void SetShapes (const Standard_Integer I1, const Standard_Integer I2);
  
  Standard_EXPORT void GetShapes (Standard_Integer& I1, Standard_Integer& I2) const;


friend class TopOpeBRepDS_DataStructure;


protected:





private:



  TopOpeBRepDS_Point myPoint;
  Standard_Integer myS1;
  Standard_Integer myS2;


};







#endif // _TopOpeBRepDS_PointData_HeaderFile
