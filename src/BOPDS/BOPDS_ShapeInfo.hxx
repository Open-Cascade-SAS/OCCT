// Created by: Peter KURNEV
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

#ifndef _BOPDS_ShapeInfo_HeaderFile
#define _BOPDS_ShapeInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS_Shape.hxx>


//! The class BOPDS_ShapeInfo is to store
//! handy information about shape
class BOPDS_ShapeInfo 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  BOPDS_ShapeInfo();

  virtual ~BOPDS_ShapeInfo();

  //! Constructor
  //! @param theAllocator the allocator to manage the memory
  BOPDS_ShapeInfo(const Handle(NCollection_BaseAllocator)& theAllocator);
  

  //! Modifier
  //! Sets the shape <theS>
    void SetShape (const TopoDS_Shape& theS);
  

  //! Selector
  //! Returns the shape
    const TopoDS_Shape& Shape() const;
  

  //! Modifier
  //! Sets the type of shape theType
    void SetShapeType (const TopAbs_ShapeEnum theType);
  

  //! Selector
  //! Returns the type of shape
    TopAbs_ShapeEnum ShapeType() const;
  

  //! Modifier
  //! Sets the boundung box of the shape theBox
    void SetBox (const Bnd_Box& theBox);
  

  //! Selector
  //! Returns the boundung box of the shape
    const Bnd_Box& Box() const;
  

  //! Selector/Modifier
  //! Returns the boundung box of the shape
    Bnd_Box& ChangeBox();
  

  //! Selector
  //! Returns the list of indices of sub-shapes
    const TColStd_ListOfInteger& SubShapes() const;
  

  //! Selector/ Modifier
  //! Returns the list of indices of sub-shapes
    TColStd_ListOfInteger& ChangeSubShapes();
  

  //! Query
  //! Returns true if the shape has sub-shape with
  //! index theI
    Standard_Boolean HasSubShape (const Standard_Integer theI) const;
  
    Standard_Boolean HasReference() const;
  

  //! Modifier
  //! Sets the index of a reference information
    void SetReference (const Standard_Integer theI);
  

  //! Selector
  //! Returns the index of a reference information
    Standard_Integer Reference() const;
  

  //! Query
  //! Returns true if the shape has boundary representation
    Standard_Boolean HasBRep() const;
  

  //! Returns true if the shape can be participant of
  //! an interference
  //!
  //! Flag
    Standard_Boolean IsInterfering() const;
  

  //! Query
  //! Returns true if there is flag.
    Standard_Boolean HasFlag() const;
  

  //! Query
  //! Returns true if there is flag.
  //! Returns the flag theFlag
    Standard_Boolean HasFlag (Standard_Integer& theFlag) const;
  

  //! Modifier
  //! Sets the flag
    void SetFlag (const Standard_Integer theI);
  

  //! Returns the flag
    Standard_Integer Flag() const;
  
  Standard_EXPORT void Dump() const;




protected:



  TopoDS_Shape myShape;
  TopAbs_ShapeEnum myType;
  Bnd_Box myBox;
  TColStd_ListOfInteger mySubShapes;
  Standard_Integer myReference;
  Standard_Integer myFlag;


private:





};


#include <BOPDS_ShapeInfo.lxx>





#endif // _BOPDS_ShapeInfo_HeaderFile
