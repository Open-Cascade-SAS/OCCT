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

#ifndef _BOPAlgo_WireEdgeSet_HeaderFile
#define _BOPAlgo_WireEdgeSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Face;
class TopoDS_Shape;



class BOPAlgo_WireEdgeSet 
{
public:

  DEFINE_STANDARD_ALLOC

  
    BOPAlgo_WireEdgeSet();
 virtual ~BOPAlgo_WireEdgeSet();
  
    BOPAlgo_WireEdgeSet(const Handle(NCollection_BaseAllocator)& theAllocator);
  
    void Clear();
  
    void SetFace (const TopoDS_Face& aF);
  
    const TopoDS_Face& Face() const;
  
    void AddStartElement (const TopoDS_Shape& sS);
  
    const TopTools_ListOfShape& StartElements() const;
  
    void AddShape (const TopoDS_Shape& sS);
  
    const TopTools_ListOfShape& Shapes() const;




protected:



  TopoDS_Face myFace;
  TopTools_ListOfShape myStartShapes;
  TopTools_ListOfShape myShapes;


private:





};


#include <BOPAlgo_WireEdgeSet.lxx>





#endif // _BOPAlgo_WireEdgeSet_HeaderFile
