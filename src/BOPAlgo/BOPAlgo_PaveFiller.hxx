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

#ifndef _BOPAlgo_PaveFiller_HeaderFile
#define _BOPAlgo_PaveFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_PIterator.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <Standard_Real.hxx>
#include <BOPAlgo_Algo.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
#include <BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks.hxx>
#include <Standard_Boolean.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_DataMapOfIntegerReal.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPCol_DataMapOfIntegerInteger.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPCol_IndexedDataMapOfShapeInteger.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
class IntTools_Context;
class BOPDS_DS;
class BOPAlgo_SectionAttribute;
class BOPDS_PaveBlock;
class TopoDS_Vertex;
class gp_Pnt;
class BOPDS_Curve;
class TopoDS_Face;



class BOPAlgo_PaveFiller  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_PaveFiller();
Standard_EXPORT virtual ~BOPAlgo_PaveFiller();
  
  Standard_EXPORT BOPAlgo_PaveFiller(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT const BOPDS_DS& DS();
  
  Standard_EXPORT BOPDS_PDS PDS();
  
  Standard_EXPORT const BOPDS_PIterator& Iterator();
  
  Standard_EXPORT void SetArguments (const BOPCol_ListOfShape& theLS);
  
  Standard_EXPORT const BOPCol_ListOfShape& Arguments() const;
  
  Standard_EXPORT Handle(IntTools_Context) Context();
  
  Standard_EXPORT void SetSectionAttribute (const BOPAlgo_SectionAttribute& theSecAttr);
  
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  //! Sets the additional tolerance
  Standard_EXPORT void SetFuzzyValue (const Standard_Real theFuzz);
  
  //! Returns the additional tolerance
  Standard_EXPORT Standard_Real FuzzyValue() const;




protected:

  
  Standard_EXPORT virtual void PerformInternal();
  
  Standard_EXPORT virtual void Clear();
  
  Standard_EXPORT virtual void Init();
  
  Standard_EXPORT void Prepare();
  
  Standard_EXPORT virtual void PerformVV();
  
  Standard_EXPORT virtual void PerformVE();
  
  Standard_EXPORT virtual void PerformVF();
  
  Standard_EXPORT virtual void PerformEE();
  
  Standard_EXPORT virtual void PerformEF();
  
  Standard_EXPORT virtual void PerformFF();
  
  Standard_EXPORT virtual void PerformVZ();
  
  Standard_EXPORT virtual void PerformEZ();
  
  Standard_EXPORT virtual void PerformFZ();
  
  Standard_EXPORT virtual void PerformZZ();

  Standard_EXPORT virtual void PerformSZ(const TopAbs_ShapeEnum aTS);
  
  Standard_EXPORT void TreatVerticesEE();
  
  Standard_EXPORT void MakeSplitEdges();
  
  Standard_EXPORT void MakeBlocks();
  
  Standard_EXPORT void MakePCurves();
  
  Standard_EXPORT void ProcessDE();
  
  Standard_EXPORT void FillShrunkData (Handle(BOPDS_PaveBlock)& thePB);
  
  Standard_EXPORT void FillShrunkData (const TopAbs_ShapeEnum theType1, const TopAbs_ShapeEnum theType2);
  
  Standard_EXPORT Standard_Integer PerformVerticesEE (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT Standard_Integer PerformVerticesEF (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT Standard_Boolean CheckFacePaves (const TopoDS_Vertex& theVnew, const BOPCol_MapOfInteger& theMIF);
  
  Standard_EXPORT static Standard_Boolean CheckFacePaves (const Standard_Integer theN, const BOPCol_MapOfInteger& theMIFOn, const BOPCol_MapOfInteger& theMIFIn);
  
  Standard_EXPORT Standard_Boolean IsExistingVertex (const gp_Pnt& theP, const Standard_Real theTol, const BOPCol_MapOfInteger& theMVOn) const;
  

  //! Checks and puts paves from <theMVOn> on the curve <theNC>.
  Standard_EXPORT void PutPavesOnCurve (const BOPCol_MapOfInteger& theMVOn, const Standard_Real theTolR3D, BOPDS_Curve& theNC, const Standard_Integer nF1, const Standard_Integer nF2, const BOPCol_MapOfInteger& theMI, const BOPCol_MapOfInteger& theMVEF, BOPCol_DataMapOfIntegerReal& theMVTol);
  

  //! Depending on the parameter aType it checks whether
  //! the vertex nV was created in EE or EF intersections.
  //! If so, it increases aTolVExt from tolerance value of vertex to
  //! the max distance from vertex nV to the ends of the range of common part.
  //! Possible values of aType:
  //! 1 - checks only EE;
  //! 2 - checks only EF;
  //! other - checks both types of intersections.
  Standard_EXPORT Standard_Boolean ExtendedTolerance (const Standard_Integer nV, const BOPCol_MapOfInteger& aMI, Standard_Real& aTolVExt, const Standard_Integer aType = 0);
  
  Standard_EXPORT void PutBoundPaveOnCurve (const TopoDS_Face& theF1, const TopoDS_Face& theF2, const Standard_Real theTolR3D, BOPDS_Curve& theNC, BOPCol_ListOfInteger& theLBV);
  
  Standard_EXPORT Standard_Boolean IsExistingPaveBlock (const Handle(BOPDS_PaveBlock)& thePB, const BOPDS_Curve& theNC, const Standard_Real theTolR3D, const BOPDS_IndexedMapOfPaveBlock& theMPB, Handle(BOPDS_PaveBlock)& thePBOut);
  
  Standard_EXPORT Standard_Boolean IsExistingPaveBlock (const Handle(BOPDS_PaveBlock)& thePB, const BOPDS_Curve& theNC, const Standard_Real theTolR3D, const BOPCol_ListOfInteger& theLSE);
  

  //! Treatment of section edges.
  Standard_EXPORT Standard_Integer PostTreatFF (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB, BOPCol_DataMapOfShapeInteger& theMVI, BOPDS_DataMapOfPaveBlockListOfPaveBlock& theDMExEdges, BOPCol_DataMapOfIntegerInteger& theDMI, const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT void FindPaveBlocks (const Standard_Integer theV, const Standard_Integer theF, BOPDS_ListOfPaveBlock& theLPB);
  
  Standard_EXPORT void FillPaves (const Standard_Integer theV, const Standard_Integer theE, const Standard_Integer theF, const BOPDS_ListOfPaveBlock& theLPB, const Handle(BOPDS_PaveBlock)& thePB);
  
  Standard_EXPORT void MakeSplitEdge (const Standard_Integer theV, const Standard_Integer theF);
  
  Standard_EXPORT void GetEFPnts (const Standard_Integer nF1, const Standard_Integer nF2, IntSurf_ListOfPntOn2S& aListOfPnts);
  

  //! Checks and puts paves created in EF intersections on the curve <theNC>.
  Standard_EXPORT void PutEFPavesOnCurve (BOPDS_Curve& theNC, const BOPCol_MapOfInteger& theMI, const BOPCol_MapOfInteger& theMVEF, BOPCol_DataMapOfIntegerReal& theMVTol);
  

  //! Puts stick paves on the curve <theNC>
  Standard_EXPORT void PutStickPavesOnCurve (const TopoDS_Face& aF1, const TopoDS_Face& aF2, const BOPCol_MapOfInteger& theMI, BOPDS_Curve& theNC, const BOPCol_MapOfInteger& theMVStick, BOPCol_DataMapOfIntegerReal& theMVTol);
  

  //! Collects indices of vertices created in all intersections between
  //! two faces (<nF1> and <nF2>) to the map <theMVStick>.
  //! Also, it collects indices of EF vertices to the <theMVEF> map
  //! and indices of all subshapes of these two faces to the <theMI> map.
  Standard_EXPORT void GetStickVertices (const Standard_Integer nF1, const Standard_Integer nF2, BOPCol_MapOfInteger& theMVStick, BOPCol_MapOfInteger& theMVEF, BOPCol_MapOfInteger& theMI);
  

  //! Collects index nF and indices of all subshapes of the shape with index <nF>
  //! to the map <theMI>.
  Standard_EXPORT void GetFullShapeMap (const Standard_Integer nF, BOPCol_MapOfInteger& theMI);
  

  //! Removes indices of vertices that are already on the
  //! curve <theNC> from the map <theMV>.
  //! It is used in PutEFPavesOnCurve and PutStickPavesOnCurve methods.
  Standard_EXPORT void RemoveUsedVertices (BOPDS_Curve& theNC, BOPCol_MapOfInteger& theMV);
  

  //! Puts the pave nV on the curve theNC.
  //! Parameter aType defines whether to check the pave with
  //! extended tolerance:
  //! 0 - do not perform the check;
  //! other - perform the check (aType goes to ExtendedTolerance).
  Standard_EXPORT void PutPaveOnCurve (const Standard_Integer nV, const Standard_Real theTolR3D, BOPDS_Curve& theNC, const BOPCol_MapOfInteger& theMI, BOPCol_DataMapOfIntegerReal& theMVTol, const Standard_Integer aType = 0);
  

  //! Adds the existing edges from the map <theMPBOnIn> which interfere
  //! with the vertices from <theMVB> map to the post treatment of section edges.
  Standard_EXPORT void ProcessExistingPaveBlocks (const Standard_Integer theInt, const BOPDS_IndexedMapOfPaveBlock& theMPBOnIn, const BOPCol_DataMapOfIntegerListOfInteger& theDMBV, BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB, BOPCol_DataMapOfShapeInteger& theMVI, BOPDS_MapOfPaveBlock& theMPB);
  

  //! Replaces existing pave block <thePB> with new pave blocks <theLPB>.
  //! The list <theLPB> contains images of <thePB> which were created in
  //! the post treatment of section edges.
  Standard_EXPORT void UpdateExistingPaveBlocks (const Handle(BOPDS_PaveBlock)& thePB, BOPDS_ListOfPaveBlock& theLPB, const Standard_Integer nF1, const Standard_Integer nF2);
  

  //! Treatment of vertices that were created in EE intersections.
  Standard_EXPORT void TreatNewVertices (const BOPCol_IndexedDataMapOfShapeInteger& theMVI, BOPCol_IndexedDataMapOfShapeListOfShape& theImages);
  

  //! Put paves on the curve <aBC> in case when <aBC>
  //! is closed 3D-curve
  Standard_EXPORT void PutClosingPaveOnCurve (BOPDS_Curve& aNC);
  

  //! Keeps data for post treatment
  Standard_EXPORT void PreparePostTreatFF (const Standard_Integer aInt, const Standard_Integer aCur, const Handle(BOPDS_PaveBlock)& aPB, BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB, BOPCol_DataMapOfShapeInteger& aMVI, BOPDS_ListOfPaveBlock& aLPB);
  

  //! Refines the state On for the all faces having
  //! state information
  Standard_EXPORT void RefineFaceInfoOn();
  

  //! Updates the information about faces
  Standard_EXPORT void UpdateFaceInfo (BOPDS_DataMapOfPaveBlockListOfPaveBlock& theDME, const BOPCol_DataMapOfIntegerInteger& theDMV);
  

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with edge
  Standard_EXPORT void ForceInterfVE (const Standard_Integer nV, Handle(BOPDS_PaveBlock)& aPB, BOPDS_MapOfPaveBlock& aMPB);
  

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with face with index <nF>
  Standard_EXPORT Standard_Boolean ForceInterfVF (const Standard_Integer nV, const Standard_Integer nF);
  

  //! Checks if there are any common or intersecting sub shapes
  //! between two planar faces.
  Standard_EXPORT Standard_Boolean CheckPlanes (const Standard_Integer nF1, const Standard_Integer nF2) const;
  

  //! Creates new edge from the edge nE with vertices nV1 and nV2
  //! and returns the index of that new edge in the DS.
  Standard_EXPORT Standard_Integer SplitEdge (const Standard_Integer nE, const Standard_Integer nV1, const Standard_Real aT1, const Standard_Integer nV2, const Standard_Real aT2);
  

  //! Updates pave blocks which have the paves with indices contained
  //! in the map <theDMI>.
  Standard_EXPORT void UpdatePaveBlocks (const BOPCol_DataMapOfIntegerInteger& theDMI);


  BOPCol_ListOfShape myArguments;
  BOPDS_PDS myDS;
  BOPDS_PIterator myIterator;
  Handle(IntTools_Context) myContext;
  BOPAlgo_SectionAttribute mySectionAttribute;
  Standard_Real myFuzzyValue;


private:





};







#endif // _BOPAlgo_PaveFiller_HeaderFile
