// Created on: 1995-12-19
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

#ifndef _TopOpeBRepBuild_Loop_HeaderFile
#define _TopOpeBRepBuild_Loop_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_BlockIterator.hxx>
#include <Standard_Transient.hxx>
class TopoDS_Shape;
class TopOpeBRepBuild_BlockIterator;


class TopOpeBRepBuild_Loop;
DEFINE_STANDARD_HANDLE(TopOpeBRepBuild_Loop, Standard_Transient)


//! a Loop is an existing shape (Shell,Wire) or a set
//! of shapes (Faces,Edges) which are connex.
//! a set of connex shape is represented by a BlockIterator
class TopOpeBRepBuild_Loop : public Standard_Transient
{

public:

  
  Standard_EXPORT TopOpeBRepBuild_Loop(const TopoDS_Shape& S);
  
  Standard_EXPORT TopOpeBRepBuild_Loop(const TopOpeBRepBuild_BlockIterator& BI);
  
  Standard_EXPORT virtual Standard_Boolean IsShape() const;
  
  Standard_EXPORT virtual const TopoDS_Shape& Shape() const;
  
  Standard_EXPORT const TopOpeBRepBuild_BlockIterator& BlockIterator() const;
  
  Standard_EXPORT virtual void Dump() const;




  DEFINE_STANDARD_RTTIEXT(TopOpeBRepBuild_Loop,Standard_Transient)

protected:


  Standard_Boolean myIsShape;
  TopoDS_Shape myShape;
  TopOpeBRepBuild_BlockIterator myBlockIterator;


private:




};







#endif // _TopOpeBRepBuild_Loop_HeaderFile
