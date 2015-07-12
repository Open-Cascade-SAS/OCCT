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

#ifndef _TopOpeBRepDS_ShapeData_HeaderFile
#define _TopOpeBRepDS_ShapeData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepDS_DataStructure;



class TopOpeBRepDS_ShapeData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_ShapeData();
  
  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& Interferences() const;
  
  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeInterferences();
  
  Standard_EXPORT Standard_Boolean Keep() const;
  
  Standard_EXPORT void ChangeKeep (const Standard_Boolean B);


friend class TopOpeBRepDS_DataStructure;


protected:





private:



  TopOpeBRepDS_ListOfInterference myInterferences;
  TopTools_ListOfShape mySameDomain;
  Standard_Integer mySameDomainRef;
  TopOpeBRepDS_Config mySameDomainOri;
  Standard_Integer mySameDomainInd;
  TopAbs_Orientation myOrientation;
  Standard_Boolean myOrientationDef;
  Standard_Integer myAncestorRank;
  Standard_Boolean myKeep;


};







#endif // _TopOpeBRepDS_ShapeData_HeaderFile
