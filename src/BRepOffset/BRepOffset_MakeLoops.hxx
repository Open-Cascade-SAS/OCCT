// Created on: 1996-09-05
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepOffset_MakeLoops_HeaderFile
#define _BRepOffset_MakeLoops_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Boolean.hxx>
class BRepAlgo_AsDes;
class BRepAlgo_Image;
class BRepOffset_Analyse;



class BRepOffset_MakeLoops 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffset_MakeLoops();
  
  Standard_EXPORT void Build (const TopTools_ListOfShape& LF, const Handle(BRepAlgo_AsDes)& AsDes, BRepAlgo_Image& Image);
  
  Standard_EXPORT void BuildOnContext (const TopTools_ListOfShape& LContext, const BRepOffset_Analyse& Analyse, const Handle(BRepAlgo_AsDes)& AsDes, BRepAlgo_Image& Image, const Standard_Boolean InSide);
  
  Standard_EXPORT void BuildFaces (const TopTools_ListOfShape& LF, const Handle(BRepAlgo_AsDes)& AsDes, BRepAlgo_Image& Image);




protected:





private:



  TopTools_DataMapOfShapeShape myVerVerMap;


};







#endif // _BRepOffset_MakeLoops_HeaderFile
