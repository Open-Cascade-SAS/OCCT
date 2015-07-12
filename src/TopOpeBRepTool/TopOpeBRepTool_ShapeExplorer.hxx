// Created on: 1995-07-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_ShapeExplorer_HeaderFile
#define _TopOpeBRepTool_ShapeExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopExp_Explorer.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TopoDS_Shape;



class TopOpeBRepTool_ShapeExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty explorer, becomes usefull after Init.
  Standard_EXPORT TopOpeBRepTool_ShapeExplorer();
  
  //! Creates an Explorer on the Shape <S>.
  //!
  //! <ToFind> is the type of shapes to search.
  //! TopAbs_VERTEX, TopAbs_EDGE, ...
  //!
  //! <ToAvoid>   is the type   of shape to  skip in the
  //! exploration.   If   <ToAvoid>  is  equal  or  less
  //! complex than <ToFind> or if  <ToAVoid> is SHAPE it
  //! has no effect on the exploration.
  Standard_EXPORT TopOpeBRepTool_ShapeExplorer(const TopoDS_Shape& S, const TopAbs_ShapeEnum ToFind, const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE);
  
  Standard_EXPORT void Init (const TopoDS_Shape& S, const TopAbs_ShapeEnum ToFind, const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE);
  
  //! Returns  True if  there are   more  shapes in  the
  //! exploration.
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Moves to the next Shape in the exploration.
  Standard_EXPORT void Next();
  
  //! Returns the current shape in the exploration.
  Standard_EXPORT const TopoDS_Shape& Current() const;
  
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  Standard_EXPORT Standard_Integer Index() const;
  
  Standard_EXPORT Standard_OStream& DumpCurrent (Standard_OStream& OS) const;




protected:





private:



  TopExp_Explorer myExplorer;
  Standard_Integer myIndex;
  Standard_Integer myNbShapes;


};







#endif // _TopOpeBRepTool_ShapeExplorer_HeaderFile
