// Created on: 1996-01-05
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_CompositeClassifier_HeaderFile
#define _TopOpeBRepBuild_CompositeClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <TopOpeBRepBuild_LoopClassifier.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>
class TopOpeBRepBuild_BlockBuilder;
class TopOpeBRepBuild_Loop;
class TopoDS_Shape;



//! classify composite Loops, i.e, loops that can be either a Shape, or
//! a block of Elements.
class TopOpeBRepBuild_CompositeClassifier  : public TopOpeBRepBuild_LoopClassifier
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT virtual TopAbs_State Compare (const Handle(TopOpeBRepBuild_Loop)& L1, const Handle(TopOpeBRepBuild_Loop)& L2) Standard_OVERRIDE;
  
  //! classify shape <B1> with shape <B2>
  Standard_EXPORT virtual TopAbs_State CompareShapes (const TopoDS_Shape& B1, const TopoDS_Shape& B2) = 0;
  
  //! classify element <E> with shape <B>
  Standard_EXPORT virtual TopAbs_State CompareElementToShape (const TopoDS_Shape& E, const TopoDS_Shape& B) = 0;
  
  //! prepare classification involving shape <B>
  //! calls ResetElement on first element of <B>
  Standard_EXPORT virtual void ResetShape (const TopoDS_Shape& B) = 0;
  
  //! prepare classification involving element <E>.
  Standard_EXPORT virtual void ResetElement (const TopoDS_Shape& E) = 0;
  
  //! Add element <E> in the set of elements used in classification.
  //! Returns FALSE if the element <E> has been already added to the set of elements,
  //! otherwise returns TRUE.
  Standard_EXPORT virtual Standard_Boolean CompareElement (const TopoDS_Shape& E) = 0;
  
  //! Returns state of classification of 2D point, defined by
  //! ResetElement, with the current set of elements, defined by Compare.
  Standard_EXPORT virtual TopAbs_State State() = 0;




protected:

  
  Standard_EXPORT TopOpeBRepBuild_CompositeClassifier(const TopOpeBRepBuild_BlockBuilder& BB);


  Standard_Address myBlockBuilder;


private:





};







#endif // _TopOpeBRepBuild_CompositeClassifier_HeaderFile
