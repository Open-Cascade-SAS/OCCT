// Created on: 1993-10-14
// Created by: Remi LEQUETTE
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

#ifndef _BRepAlgoAPI_Cut_HeaderFile
#define _BRepAlgoAPI_Cut_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <Standard_Boolean.hxx>
class BOPAlgo_PaveFiller;
class TopoDS_Shape;



//! The class Cut provides Boolean cut operation
//! between arguments and tools (Boolean Subtraction).
class BRepAlgoAPI_Cut  : public BRepAlgoAPI_BooleanOperation
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_Cut();
Standard_EXPORT virtual ~BRepAlgoAPI_Cut();
  
  //! Empty constructor
  //! <PF> - PaveFiller object that is carried out
  Standard_EXPORT BRepAlgoAPI_Cut(const BOPAlgo_PaveFiller& PF);
  
  //! Constructor with two shapes
  //! <S1>  -argument
  //! <S2>  -tool
  //! <anOperation> - the type of the operation
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Cut(const TopoDS_Shape& S1, const TopoDS_Shape& S2,
                                  const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Constructor with two shapes
  //! <S1>  -argument
  //! <S2>  -tool
  //! <anOperation> - the type of the operation
  //! <PF> - PaveFiller object that is carried out
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_Cut(const TopoDS_Shape& S1, const TopoDS_Shape& S2, const BOPAlgo_PaveFiller& aDSF, 
                                  const Standard_Boolean bFWD = Standard_True, 
                                  const Message_ProgressRange& theRange = Message_ProgressRange());




protected:





private:





};







#endif // _BRepAlgoAPI_Cut_HeaderFile
