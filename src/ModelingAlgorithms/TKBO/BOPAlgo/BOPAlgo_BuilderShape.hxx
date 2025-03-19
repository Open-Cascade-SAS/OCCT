// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPAlgo_BuilderShape_HeaderFile
#define _BOPAlgo_BuilderShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Algo.hxx>
#include <BRepTools_History.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
class TopoDS_Shape;

//! Root class for algorithms that has shape as result.
//!
//! The class provides the History mechanism, which allows
//! tracking the modification of the input shapes during
//! the operation. It uses the *BRepTools_History* tool
//! as a storer for history objects.
class BOPAlgo_BuilderShape : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Getting the result
  //! Returns the result of algorithm
  const TopoDS_Shape& Shape() const { return myShape; }

public: //! @name History methods
  //! Returns the list of shapes Modified from the shape theS.
  const TopTools_ListOfShape& Modified(const TopoDS_Shape& theS)
  {
    if (myFillHistory && myHistory)
      return myHistory->Modified(theS);
    myHistShapes.Clear();
    return myHistShapes;
  }

  //! Returns the list of shapes Generated from the shape theS.
  const TopTools_ListOfShape& Generated(const TopoDS_Shape& theS)
  {
    if (myFillHistory && myHistory)
      return myHistory->Generated(theS);
    myHistShapes.Clear();
    return myHistShapes;
  }

  //! Returns true if the shape theS has been deleted.
  //! In this case the shape will have no Modified elements,
  //! but can have Generated elements.
  Standard_Boolean IsDeleted(const TopoDS_Shape& theS)
  {
    return (myFillHistory && myHistory ? myHistory->IsRemoved(theS) : Standard_False);
  }

  //! Returns true if any of the input shapes has been modified during operation.
  Standard_Boolean HasModified() const
  {
    return (myFillHistory && myHistory ? myHistory->HasModified() : Standard_False);
  }

  //! Returns true if any of the input shapes has generated shapes during operation.
  Standard_Boolean HasGenerated() const
  {
    return (myFillHistory && myHistory ? myHistory->HasGenerated() : Standard_False);
  }

  //! Returns true if any of the input shapes has been deleted during operation.
  Standard_Boolean HasDeleted() const
  {
    return (myFillHistory && myHistory ? myHistory->HasRemoved() : Standard_False);
  }

  //! History Tool
  Handle(BRepTools_History) History()
  {
    if (myFillHistory)
    {
      if (myHistory.IsNull())
        // It seems the algorithm has exited with error before filling
        // the history. Initialize the History tool to return the empty
        // History instead of NULL.
        myHistory = new BRepTools_History();

      return myHistory;
    }

    // If the History has not been requested to be filled, return the NULL
    // explicitly as the History may be partially filled for the algorithm's
    // internal needs.
    return NULL;
  }

public: //! @name Enabling/Disabling the history collection.
  //! Allows disabling the history collection
  void SetToFillHistory(const Standard_Boolean theHistFlag) { myFillHistory = theHistFlag; }

  //! Returns flag of history availability
  Standard_Boolean HasHistory() const { return myFillHistory; }

protected: //! @name Constructors
  //! Empty constructor
  BOPAlgo_BuilderShape()
      : BOPAlgo_Algo(),
        myFillHistory(Standard_True)
  {
  }

  //! Constructor with allocator
  BOPAlgo_BuilderShape(const Handle(NCollection_BaseAllocator)& theAllocator)
      : BOPAlgo_Algo(theAllocator),
        myFillHistory(Standard_True)
  {
  }

protected: //! @name Clearing
  //! Clears the content of the algorithm.
  virtual void Clear() Standard_OVERRIDE
  {
    BOPAlgo_Algo::Clear();
    myHistory.Nullify();
    myMapShape.Clear();
  }

protected:              //! @name Fields
  TopoDS_Shape myShape; //!< Result of the operation

  TopTools_ListOfShape myHistShapes; //!< Storer for the history shapes
  TopTools_MapOfShape  myMapShape;   //!< cached map of all arguments shapes

  Standard_Boolean          myFillHistory; //!< Controls the history filling
  Handle(BRepTools_History) myHistory;     //!< History tool
};

#endif // _BOPAlgo_BuilderShape_HeaderFile
