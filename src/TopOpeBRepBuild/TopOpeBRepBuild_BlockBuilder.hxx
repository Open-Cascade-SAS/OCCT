// Created on: 1993-02-25
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

#ifndef _TopOpeBRepBuild_BlockBuilder_HeaderFile
#define _TopOpeBRepBuild_BlockBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepBuild_ShapeSet;
class TopOpeBRepBuild_BlockIterator;
class TopoDS_Shape;



class TopOpeBRepBuild_BlockBuilder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_BlockBuilder();
  
  Standard_EXPORT TopOpeBRepBuild_BlockBuilder(TopOpeBRepBuild_ShapeSet& SS);
  
  Standard_EXPORT void MakeBlock (TopOpeBRepBuild_ShapeSet& SS);
  
  Standard_EXPORT void InitBlock();
  
  Standard_EXPORT Standard_Boolean MoreBlock() const;
  
  Standard_EXPORT void NextBlock();
  
  Standard_EXPORT TopOpeBRepBuild_BlockIterator BlockIterator() const;
  
  //! Returns the current element of <BI>.
  Standard_EXPORT const TopoDS_Shape& Element (const TopOpeBRepBuild_BlockIterator& BI) const;
  
  Standard_EXPORT const TopoDS_Shape& Element (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Integer Element (const TopoDS_Shape& S) const;
  
  Standard_EXPORT Standard_Boolean ElementIsValid (const TopOpeBRepBuild_BlockIterator& BI) const;
  
  Standard_EXPORT Standard_Boolean ElementIsValid (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Integer AddElement (const TopoDS_Shape& S);
  
  Standard_EXPORT void SetValid (const TopOpeBRepBuild_BlockIterator& BI, const Standard_Boolean isvalid);
  
  Standard_EXPORT void SetValid (const Standard_Integer I, const Standard_Boolean isvalid);
  
  Standard_EXPORT Standard_Boolean CurrentBlockIsRegular();




protected:





private:



  TColStd_DataMapOfIntegerInteger myOrientedShapeMapIsValid;
  TopTools_IndexedMapOfOrientedShape myOrientedShapeMap;
  TColStd_SequenceOfInteger myBlocks;
  Standard_Integer myBlockIndex;
  Standard_Boolean myIsDone;
  TColStd_SequenceOfInteger myBlocksIsRegular;


};







#endif // _TopOpeBRepBuild_BlockBuilder_HeaderFile
