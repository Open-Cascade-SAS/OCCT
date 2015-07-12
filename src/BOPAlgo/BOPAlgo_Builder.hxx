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

#ifndef _BOPAlgo_Builder_HeaderFile
#define _BOPAlgo_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPAlgo_PPaveFiller.hxx>
#include <BOPDS_PDS.hxx>
#include <Standard_Integer.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <Standard_Real.hxx>
#include <BOPAlgo_BuilderShape.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class IntTools_Context;
class TopoDS_Shape;
class BOPAlgo_PaveFiller;



class BOPAlgo_Builder  : public BOPAlgo_BuilderShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_Builder();
Standard_EXPORT virtual ~BOPAlgo_Builder();
  
  Standard_EXPORT BOPAlgo_Builder(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT virtual void Clear();
  
  Standard_EXPORT BOPAlgo_PPaveFiller PPaveFiller();
  
  Standard_EXPORT BOPDS_PDS PDS();
  
  Standard_EXPORT virtual void AddArgument (const TopoDS_Shape& theShape);
  
  Standard_EXPORT virtual void SetArguments (const BOPCol_ListOfShape& theLS);
  
  Standard_EXPORT const BOPCol_ListOfShape& Arguments() const;
  
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PerformWithFiller (const BOPAlgo_PaveFiller& theFiller);
  
  //! Returns the  list of shapes generated from the
  //! shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns the list of shapes modified from the shape
  //! theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns true if the shape theS has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  Standard_EXPORT const BOPCol_DataMapOfShapeListOfShape& Images() const;
  
  Standard_EXPORT Standard_Boolean IsInterferred (const TopoDS_Shape& theS) const;
  
  //! Returns myOrigins.
  Standard_EXPORT const BOPCol_DataMapOfShapeShape& Origins() const;
  
  //! Returns myShapesSD.
  Standard_EXPORT const BOPCol_DataMapOfShapeShape& ShapesSD() const;
  
  //! Returns mySplits.
  Standard_EXPORT const BOPCol_DataMapOfShapeListOfShape& Splits() const;
  
  //! Sets the additional tolerance
  Standard_EXPORT void SetFuzzyValue (const Standard_Real theFuzz);
  
  //! Returns the additional tolerance
  Standard_EXPORT Standard_Real FuzzyValue() const;




protected:

  
  //! Prepare information for history support
  Standard_EXPORT virtual void PrepareHistory() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PerformInternal (const BOPAlgo_PaveFiller& thePF);
  
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF);
  
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Prepare();
  
  Standard_EXPORT void FillImagesVertices();
  
  Standard_EXPORT void FillImagesEdges();
  
  Standard_EXPORT virtual void BuildResult (const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void FillImagesContainers (const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void FillImagesCompounds();
  
  Standard_EXPORT void FillImagesContainer (const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void FillImagesCompound (const TopoDS_Shape& theS, BOPCol_MapOfShape& theMF);
  
  Standard_EXPORT void FillImagesFaces();
  
  Standard_EXPORT virtual void BuildSplitFaces();
  
  Standard_EXPORT void FillSameDomainFaces();
  
  Standard_EXPORT void FillImagesFaces1();
  
  Standard_EXPORT void FillImagesSolids();
  
  Standard_EXPORT void BuildDraftSolid (const TopoDS_Shape& theSolid, TopoDS_Shape& theDraftSolid, BOPCol_ListOfShape& theLIF);
  
  Standard_EXPORT virtual void FillIn3DParts (BOPCol_DataMapOfShapeListOfShape& theInParts, BOPCol_DataMapOfShapeShape& theDraftSolids, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT void BuildSplitSolids (BOPCol_DataMapOfShapeListOfShape& theInParts, BOPCol_DataMapOfShapeShape& theDraftSolids, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT void FillInternalShapes();
  
  Standard_EXPORT virtual void PostTreat();


  BOPCol_ListOfShape myArguments;
  BOPCol_MapOfShape myMapFence;
  BOPAlgo_PPaveFiller myPaveFiller;
  BOPDS_PDS myDS;
  Handle(IntTools_Context) myContext;
  Standard_Integer myEntryPoint;
  BOPCol_DataMapOfShapeListOfShape myImages;
  BOPCol_DataMapOfShapeShape myShapesSD;
  BOPCol_DataMapOfShapeListOfShape mySplits;
  BOPCol_DataMapOfShapeShape myOrigins;
  Standard_Real myFuzzyValue;


private:





};







#endif // _BOPAlgo_Builder_HeaderFile
