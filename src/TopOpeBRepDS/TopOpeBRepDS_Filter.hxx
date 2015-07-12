// Created on: 1997-04-21
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Filter_HeaderFile
#define _TopOpeBRepDS_Filter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepTool_PShapeClassifier.hxx>
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepDS_HDataStructure;



class TopOpeBRepDS_Filter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_Filter(const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopOpeBRepTool_PShapeClassifier& pClassif = 0);
  
  Standard_EXPORT void ProcessInterferences();
  
  Standard_EXPORT void ProcessFaceInterferences (const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEsp);
  
  Standard_EXPORT void ProcessFaceInterferences (const Standard_Integer I, const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEsp);
  
  Standard_EXPORT void ProcessEdgeInterferences();
  
  Standard_EXPORT void ProcessEdgeInterferences (const Standard_Integer I);
  
  Standard_EXPORT void ProcessCurveInterferences();
  
  Standard_EXPORT void ProcessCurveInterferences (const Standard_Integer I);




protected:





private:



  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  TopOpeBRepTool_PShapeClassifier myPShapeClassif;


};







#endif // _TopOpeBRepDS_Filter_HeaderFile
