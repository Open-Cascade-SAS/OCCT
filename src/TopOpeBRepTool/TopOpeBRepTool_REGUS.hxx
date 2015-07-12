// Created on: 1999-01-04
// Created by: Xuan PHAM PHU
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_REGUS_HeaderFile
#define _TopOpeBRepTool_REGUS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;



class TopOpeBRepTool_REGUS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepTool_REGUS();
  
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT const TopoDS_Shape& S() const;
  
  Standard_EXPORT Standard_Boolean MapS();
  
  Standard_EXPORT static Standard_Boolean WireToFace (const TopoDS_Face& Fanc, const TopTools_ListOfShape& nWs, TopTools_ListOfShape& nFs);
  
  Standard_EXPORT static Standard_Boolean SplitF (const TopoDS_Face& Fanc, TopTools_ListOfShape& FSplits);
  
  Standard_EXPORT Standard_Boolean SplitFaces();
  
  Standard_EXPORT Standard_Boolean REGU();
  
  Standard_EXPORT void SetFsplits (TopTools_DataMapOfShapeListOfShape& Fsplits);
  
  Standard_EXPORT void GetFsplits (TopTools_DataMapOfShapeListOfShape& Fsplits) const;
  
  Standard_EXPORT void SetOshNsh (TopTools_DataMapOfShapeListOfShape& OshNsh);
  
  Standard_EXPORT void GetOshNsh (TopTools_DataMapOfShapeListOfShape& OshNsh) const;
  
  Standard_EXPORT Standard_Boolean InitBlock();
  
  Standard_EXPORT Standard_Boolean NextinBlock();
  
  Standard_EXPORT Standard_Boolean NearestF (const TopoDS_Edge& e, const TopTools_ListOfShape& lof, TopoDS_Face& ffound) const;




protected:





private:



  Standard_Boolean hasnewsplits;
  TopTools_DataMapOfShapeListOfShape myFsplits;
  TopTools_DataMapOfShapeListOfShape myOshNsh;
  TopoDS_Shape myS;
  TopTools_DataMapOfShapeListOfShape mymapeFsstatic;
  TopTools_DataMapOfShapeListOfShape mymapeFs;
  TopTools_IndexedMapOfShape mymapemult;
  Standard_Integer mynF;
  Standard_Integer myoldnF;
  TopoDS_Shape myf;
  TopTools_MapOfShape myedstoconnect;
  TopTools_ListOfShape mylFinBlock;


};







#endif // _TopOpeBRepTool_REGUS_HeaderFile
