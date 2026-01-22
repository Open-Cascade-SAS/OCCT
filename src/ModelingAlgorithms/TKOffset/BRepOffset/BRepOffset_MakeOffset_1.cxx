// Created on: 2016
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

// This is the implementation of the extension of the 3D offset algorithm
// to work in mode Complete and Join Type Intersection.
// Currently only the Planar cases are supported.

#include <BRepOffset_MakeOffset.hxx>

#include <Precision.hxx>

#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepLib.hxx>
#include <BRepTools.hxx>

#include <BRepAdaptor_Curve.hxx>

#include <BRepOffset_Tool.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#include <BOPDS_DS.hxx>

#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>
#include <BOPAlgo_Splitter.hxx>
#include <BOPAlgo_BOP.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>

#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Set.hxx>

#include <IntTools_Context.hxx>
#include <IntTools_ShrunkRange.hxx>

#ifdef OFFSET_DEBUG
  #include <BRepAlgoAPI_Check.hxx>
#endif

typedef NCollection_DataMap<TopoDS_Shape,
                            NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>,
                            TopTools_ShapeMapHasher>
  BRepOffset_DataMapOfShapeIndexedMapOfShape;

namespace
{

//=======================================================================
// function : AddToContainer
// purpose  : Set of methods to add a shape into container
//=======================================================================
static void AddToContainer(const TopoDS_Shape& theS, NCollection_List<TopoDS_Shape>& theList)
{
  theList.Append(theS);
}

static bool AddToContainer(const TopoDS_Shape&                                     theS,
                           NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  return theMap.Add(theS);
}

static bool AddToContainer(const TopoDS_Shape&                                            theS,
                           NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  const int aNb   = theMap.Extent();
  const int anInd = theMap.Add(theS);
  return anInd > aNb;
}

static void AddToContainer(const TopoDS_Shape& theS, TopoDS_Shape& theSOut)
{
  BRep_Builder().Add(theSOut, theS);
}

static void AddToContainer(
  const TopoDS_Shape& theKey,
  const TopoDS_Shape& theValue,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMap)
{
  if (NCollection_List<TopoDS_Shape>* pList = theMap.ChangeSeek(theKey))
  {
    pList->Append(theValue);
  }
  else
  {
    theMap.Bound(theKey, NCollection_List<TopoDS_Shape>())->Append(theValue);
  }
}

//=======================================================================
// function : TakeModified
// purpose  : Check if the shape has images in the given images map.
//           Puts in the output map either the images or the shape itself.
//=======================================================================
template <class ContainerType, class FenceMapType>
static bool TakeModified(
  const TopoDS_Shape& theS,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                 theImages,
  ContainerType& theContainer,
  FenceMapType*  theMFence)
{
  const NCollection_List<TopoDS_Shape>* pLSIm = theImages.Seek(theS);
  if (pLSIm)
  {
    NCollection_List<TopoDS_Shape>::Iterator itLSIm(*pLSIm);
    for (; itLSIm.More(); itLSIm.Next())
    {
      const TopoDS_Shape& aSIm = itLSIm.Value();
      if (!theMFence || AddToContainer(aSIm, *theMFence))
        AddToContainer(aSIm, theContainer);
    }
    return true;
  }
  else
  {
    if (!theMFence || AddToContainer(theS, *theMFence))
      AddToContainer(theS, theContainer);
    return false;
  }
}

template <class ContainerType>
static bool TakeModified(
  const TopoDS_Shape& theS,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                 theImages,
  ContainerType& theMapOut)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* aDummy = nullptr;
  return TakeModified(theS, theImages, theMapOut, aDummy);
}

//=======================================================================
// function : hasData
// purpose  : Checks if container has any data in it
//=======================================================================
template <class Container>
static bool hasData(const Container* theData)
{
  return (theData && !theData->IsEmpty());
}

//=======================================================================
// function : AppendToList
// purpose  : Add to a list only unique elements
//=======================================================================
static void AppendToList(NCollection_List<TopoDS_Shape>& theList, const TopoDS_Shape& theShape)
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(theList); it.More(); it.Next())
  {
    const TopoDS_Shape& aS = it.Value();
    if (aS.IsSame(theShape))
    {
      return;
    }
  }
  theList.Append(theShape);
}

//=======================================================================
// function : ProcessMicroEdge
// purpose  : Checking if the edge is micro edge
//=======================================================================
static bool ProcessMicroEdge(const TopoDS_Edge&                   theEdge,
                             const occ::handle<IntTools_Context>& theCtx)
{
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(theEdge, aV1, aV2);
  if (aV1.IsNull() || aV2.IsNull())
  {
    return false;
  }

  bool bMicro = BOPTools_AlgoTools::IsMicroEdge(theEdge, theCtx);
  if (bMicro && BRepAdaptor_Curve(theEdge).GetType() == GeomAbs_Line)
  {
    double aLen = BRep_Tool::Pnt(aV1).Distance(BRep_Tool::Pnt(aV2));
    BRep_Builder().UpdateVertex(aV1, aLen / 2.);
    BRep_Builder().UpdateVertex(aV2, aLen / 2.);
  }

  return bMicro;
}

//=================================================================================================

static void UpdateOrigins(
  const NCollection_List<TopoDS_Shape>& theLA,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                   theOrigins,
  BOPAlgo_Builder& theGF)
{
  for (NCollection_List<TopoDS_Shape>::Iterator aItA(theLA); aItA.More(); aItA.Next())
  {
    const TopoDS_Shape& aS = aItA.Value();

    const NCollection_List<TopoDS_Shape>& aLSIm = theGF.Modified(aS);
    if (aLSIm.IsEmpty())
    {
      continue;
    }

    NCollection_List<TopoDS_Shape>  aLSEmpt;
    NCollection_List<TopoDS_Shape>* pLS = theOrigins.ChangeSeek(aS);
    if (!pLS)
    {
      pLS = &aLSEmpt;
      pLS->Append(aS);
    }

    for (NCollection_List<TopoDS_Shape>::Iterator aIt(aLSIm); aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (NCollection_List<TopoDS_Shape>* pLSOr = theOrigins.ChangeSeek(aSIm))
      {
        // merge two lists
        for (NCollection_List<TopoDS_Shape>::Iterator aIt1(*pLS); aIt1.More(); aIt1.Next())
        {
          AppendToList(*pLSOr, aIt1.Value());
        }
      }
      else
      {
        theOrigins.Bind(aSIm, *pLS);
      }
    }
  }
}

//=======================================================================
// function : UpdateImages
// purpose  : Updating images of the shapes
//=======================================================================
static void UpdateImages(
  const NCollection_List<TopoDS_Shape>& theLA,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                          theImages,
  BOPAlgo_Builder&                                        theGF,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theModified)
{
  for (NCollection_List<TopoDS_Shape>::Iterator aIt(theLA); aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();

    NCollection_List<TopoDS_Shape>* pLSIm = theImages.ChangeSeek(aS);
    if (!pLSIm)
    {
      const NCollection_List<TopoDS_Shape>& aLSIm = theGF.Modified(aS);
      if (aLSIm.Extent())
      {
        theImages.Bind(aS, aLSIm);
        theModified.Add(aS);
      }
      continue;
    }

    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
    NCollection_List<TopoDS_Shape>                         aLSImNew;

    bool bModified = false;

    // check modifications of the images
    for (NCollection_List<TopoDS_Shape>::Iterator aIt1(*pLSIm); aIt1.More(); aIt1.Next())
    {
      const TopoDS_Shape& aSIm = aIt1.Value();
      bModified |= TakeModified(aSIm, theGF.Images(), aLSImNew, &aMFence);
    }

    if (bModified)
    {
      *pLSIm = aLSImNew;
      theModified.Add(aS);
    }
  }
}

//=======================================================================
// function : FindCommonParts
// purpose  : Looking for the parts of type <theType> contained in both lists
//=======================================================================
static void FindCommonParts(const NCollection_List<TopoDS_Shape>& theLS1,
                            const NCollection_List<TopoDS_Shape>& theLS2,
                            NCollection_List<TopoDS_Shape>&       theLSC,
                            const TopAbs_ShapeEnum                theType = TopAbs_EDGE)
{
  // map shapes in the first list
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMS1;
  for (NCollection_List<TopoDS_Shape>::Iterator aIt(theLS1); aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    TopExp::MapShapes(aS, theType, aMS1);
  }
  if (aMS1.IsEmpty())
  {
    return;
  }

  // check for such shapes in the other list
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  for (NCollection_List<TopoDS_Shape>::Iterator aIt(theLS2); aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();

    TopExp_Explorer aExp(aS, theType);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aST = aExp.Current();
      if (aMS1.Contains(aST) && aMFence.Add(aST))
      {
        theLSC.Append(aST);
      }
    }
  }
}

//=======================================================================
// function : NbPoints
// purpose  : Defines number of sample points to get average direction of the edge
//=======================================================================
static int NbPoints(const TopoDS_Edge& theEdge)
{
  BRepAdaptor_Curve aBAC(theEdge);
  switch (aBAC.GetType())
  {
    case GeomAbs_Line:
      return 1;
    default:
      return 11;
  }
}

//=======================================================================
// function : FindShape
// purpose  : Looking for the same sub-shape in the shape
//=======================================================================
static bool FindShape(const TopoDS_Shape&       theSWhat,
                      const TopoDS_Shape&       theSWhere,
                      const BRepOffset_Analyse* theAnalyse,
                      TopoDS_Shape&             theRes)
{
  bool             bFound = false;
  TopAbs_ShapeEnum aType  = theSWhat.ShapeType();
  TopExp_Explorer  aExp(theSWhere, aType);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aS = aExp.Current();
    if (aS.IsSame(theSWhat))
    {
      theRes = aS;
      bFound = true;
      break;
    }
  }

  if (!bFound && theAnalyse)
  {
    const NCollection_List<TopoDS_Shape>* pLD = theAnalyse->Descendants(theSWhere);
    if (pLD)
    {
      for (NCollection_List<TopoDS_Shape>::Iterator it(*pLD); it.More(); it.Next())
      {
        const TopoDS_Shape& aS = it.Value();
        if (aS.IsSame(theSWhat))
        {
          theRes = aS;
          bFound = true;
          break;
        }
      }
    }
  }

  return bFound;
}

//=======================================================================
// function : BuildSplitsOfTrimmedFace
// purpose  : Building the splits of offset face
//=======================================================================
static void BuildSplitsOfTrimmedFace(const TopoDS_Face&              theFace,
                                     const TopoDS_Shape&             theEdges,
                                     NCollection_List<TopoDS_Shape>& theLFImages,
                                     const Message_ProgressRange&    theRange)
{
  BOPAlgo_Splitter aSplitter;
  //
  aSplitter.AddArgument(theFace);
  aSplitter.AddArgument(theEdges);
  aSplitter.SetToFillHistory(false);
  aSplitter.Perform(theRange);
  if (aSplitter.HasErrors())
  {
    return;
  }
  //
  // splits of the offset shape
  for (TopExp_Explorer anExp(aSplitter.Shape(), TopAbs_FACE); anExp.More(); anExp.Next())
  {
    theLFImages.Append(anExp.Current());
  }
}

//=======================================================================
// function : BuildSplitsOfFace
// purpose  : Building the splits of offset face
//=======================================================================
static void BuildSplitsOfFace(
  const TopoDS_Face&                                                        theFace,
  const TopoDS_Shape&                                                       theEdges,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFacesOrigins,
  NCollection_List<TopoDS_Shape>&                                           theLFImages)
{
  theLFImages.Clear();
  //
  // take edges to split the face
  NCollection_List<TopoDS_Shape> aLE;
  TopExp_Explorer                aExp(theEdges, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    TopoDS_Edge aE = TopoDS::Edge(aExp.Current());
    aE.Orientation(TopAbs_FORWARD);
    aLE.Append(aE);
    aE.Orientation(TopAbs_REVERSED);
    aLE.Append(aE);
  }
  //
  TopoDS_Face        aFF  = theFace;
  TopAbs_Orientation anOr = theFace.Orientation();
  aFF.Orientation(TopAbs_FORWARD);
  //
  // build pcurves for edges on the face
  BRepLib::BuildPCurveForEdgesOnPlane(aLE, aFF);
  //
  // build splits of faces
  BOPAlgo_BuilderFace aBF;
  aBF.SetFace(aFF);
  aBF.SetShapes(aLE);
  aBF.Perform();
  if (aBF.HasErrors())
  {
    return;
  }
  //
  const NCollection_List<TopoDS_Shape>&    aLFSp = aBF.Areas();
  NCollection_List<TopoDS_Shape>::Iterator aItLF(aLFSp);
  for (; aItLF.More(); aItLF.Next())
  {
    TopoDS_Shape& aFSp = aItLF.ChangeValue();
    aFSp.Orientation(anOr);
    theLFImages.Append(aFSp);
    //
    theFacesOrigins.Bind(aFSp, theFace);
  }
}

//=======================================================================
// function : GetAverageTangent
// purpose  : Computes average tangent vector along the curve
//=======================================================================
static gp_Vec GetAverageTangent(const TopoDS_Shape& theS, const int theNbP)
{
  gp_Vec          aVA;
  TopExp_Explorer aExp(theS, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExp.Current();
    //
    double                         aT1, aT2;
    const occ::handle<Geom_Curve>& aC = BRep_Tool::Curve(aE, aT1, aT2);
    //
    gp_Pnt aP;
    gp_Vec aV, aVSum;
    double aT  = aT1;
    double aDt = (aT2 - aT1) / theNbP;
    while (aT <= aT2)
    {
      aC->D1(aT, aP, aV);
      aVSum += aV.Normalized();
      aT += aDt;
    }
    //
    if (aE.Orientation() == TopAbs_REVERSED)
    {
      aVSum.Reverse();
    }
    //
    aVA += aVSum;
  }
  return aVA;
}
} // namespace

//=======================================================================
// function : BRepOffset_BuildOffsetFaces
// purpose  : Auxiliary local class that is used here for building splits
//           of offset faces, that are further used for building volumes.
//=======================================================================
class BRepOffset_BuildOffsetFaces
{
public: //! @name Constructor
  //! Constructor, taking the history tool to be filled
  BRepOffset_BuildOffsetFaces(BRepAlgo_Image& theImage)
      : myFaces(nullptr),
        myAnalyzer(nullptr),
        myEdgesOrigins(nullptr),
        myFacesOrigins(nullptr),
        myETrimEInf(nullptr),
        myImage(&theImage)
  {
    myContext = new IntTools_Context();
  }

public: //! @name Setting data
  //! Sets faces to build splits
  void SetFaces(const NCollection_List<TopoDS_Shape>& theFaces) { myFaces = &theFaces; }

  //! Sets ascendants/descendants information
  void SetAsDesInfo(const occ::handle<BRepAlgo_AsDes>& theAsDes) { myAsDes = theAsDes; }

  //! Sets the analysis info of the input shape
  void SetAnalysis(const BRepOffset_Analyse& theAnalyse) { myAnalyzer = &theAnalyse; }

  //! Sets origins of the offset edges (from original shape)
  void SetEdgesOrigins(
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEdgesOrigins)
  {
    myEdgesOrigins = &theEdgesOrigins;
  }

  //! Sets origins of the offset faces (from original shape)
  void SetFacesOrigins(
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFacesOrigins)
  {
    myFacesOrigins = &theFacesOrigins;
  }

  //! Sets infinite (extended) edges for the trimmed ones
  void SetInfEdges(
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theETrimEInf)
  {
    myETrimEInf = &theETrimEInf;
  }

public: //! @name Public methods to build the splits
  //! Build splits of already trimmed faces
  void BuildSplitsOfTrimmedFaces(const Message_ProgressRange& theRange);

  //! Building splits of not-trimmed offset faces.
  //! For the cases in which invalidities will be found, these invalidities will be rebuilt.
  void BuildSplitsOfExtendedFaces(const Message_ProgressRange& theRange);

private: //! @name private methods performing the job
private: //! @name Intersection and post-treatment of edges
  //! Intersection of the trimmed edges among themselves
  void IntersectTrimmedEdges(const Message_ProgressRange& theRange);

  //! Saving connection from trimmed edges to not trimmed ones
  void UpdateIntersectedEdges(const NCollection_List<TopoDS_Shape>& theLA, BOPAlgo_Builder& theGF);

  //! Getting edges from AsDes map to build the splits of faces
  bool GetEdges(const TopoDS_Face&                                             theFace,
                TopoDS_Shape&                                                  theEdges,
                NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* theInv = nullptr);

  //! Looks for the invalid edges (edges with changed orientation)
  //! in the splits of offset faces
  void FindInvalidEdges(
    const TopoDS_Face&                            theF,
    const NCollection_List<TopoDS_Shape>&         theLFImages,
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>& theDMFMVE,
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>& theDMFMNE,
    BRepOffset_DataMapOfShapeIndexedMapOfShape&   theDMFMIE,
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>& theDMFMVIE,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                            theDMEOrLEIm,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theEdgesInvalidByVertex,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theEdgesValidByVertex,
    const Message_ProgressRange&                            theRange);

  //! Additional method to look for invalid edges
  void FindInvalidEdges(const NCollection_List<TopoDS_Shape>&         theLFOffset,
                        BRepOffset_DataMapOfShapeIndexedMapOfShape&   theLocInvEdges,
                        NCollection_DataMap<TopoDS_Shape,
                                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                            TopTools_ShapeMapHasher>& theLocValidEdges,
                        NCollection_DataMap<TopoDS_Shape,
                                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                            TopTools_ShapeMapHasher>& theNeutralEdges);

  //! Checks if the edge has been inverted
  bool CheckInverted(
    const TopoDS_Edge&                                                   theEIm,
    const TopoDS_Face&                                                   theFOr,
    const NCollection_IndexedDataMap<TopoDS_Shape,
                                     NCollection_List<TopoDS_Shape>,
                                     TopTools_ShapeMapHasher>&           theDMVE,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEdges);

  //! Looking for the invalid faces containing inverted edges that can be safely removed
  void RemoveInvalidSplitsByInvertedEdges(
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved);

  //! Makes inverted edges located inside loop of invalid edges, invalid as well
  void MakeInvertedEdgesInvalid(const NCollection_List<TopoDS_Shape>& theLFOffset);

  //! Checks if it is possible to remove the block containing inverted edges
  bool CheckInvertedBlock(
    const TopoDS_Shape&                           theCB,
    const NCollection_List<TopoDS_Shape>&         theLCBF,
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>& theDMCBVInverted,
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>& theDMCBVAll);

  //! Updating the maps of images and origins of the offset edges
  void FilterEdgesImages(const TopoDS_Shape& theS);

  //! Checks additionally the unchecked edges originated from vertices
  void CheckEdgesCreatedByVertex();

  //! Filtering the invalid edges according to currently invalid faces
  void FilterInvalidEdges(
    const BRepOffset_DataMapOfShapeIndexedMapOfShape&                    theDMFMIE,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInside,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theMEUseInRebuild);

private: //! @name Checking faces
  //! Build splits of faces
  void BuildSplitsOfFaces(const Message_ProgressRange& theRange);

  //! Looking for the invalid faces by analyzing their invalid edges
  void FindInvalidFaces(
    NCollection_List<TopoDS_Shape>&                                theLFImages,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                              TopTools_ShapeMapHasher>&            theDMFMVE,
    const BRepOffset_DataMapOfShapeIndexedMapOfShape&              theDMFMIE,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theMENeutral,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theEdgesInvalidByVertex,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theEdgesValidByVertex,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theMFHoles,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInvInHole,
    NCollection_List<TopoDS_Shape>&                                theInvFaces,
    NCollection_List<TopoDS_Shape>&                                theInvertedFaces);

  //! Find faces inside holes wires from the original face
  void FindFacesInsideHoleWires(const TopoDS_Face&                                  theFOrigin,
                                const TopoDS_Face&                                  theFOffset,
                                const NCollection_List<TopoDS_Shape>&               theLFImages,
                                const NCollection_DataMap<TopoDS_Shape,
                                                          NCollection_List<TopoDS_Shape>,
                                                          TopTools_ShapeMapHasher>& theDMEOrLEIm,
                                const NCollection_IndexedDataMap<TopoDS_Shape,
                                                                 NCollection_List<TopoDS_Shape>,
                                                                 TopTools_ShapeMapHasher>& theEFMap,
                                NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFHoles);

  //! Removing invalid splits of faces from valid
  void RemoveInvalidSplitsFromValid(
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                              TopTools_ShapeMapHasher>& theDMFMVIE);

  //! Looking for the inside faces that can be safely removed
  void RemoveInsideFaces(
    const NCollection_List<TopoDS_Shape>&                                theInvertedFaces,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFToCheckInt,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInvInHole,
    const TopoDS_Shape&                                                  theFHoles,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMERemoved,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMEInside,
    const Message_ProgressRange&                                         theRange);

  //! Looking for the connections between faces not to miss some necessary intersection
  void ShapesConnections(
    const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDMFOr,
    BOPAlgo_Builder&                                                                theBuilder);

  //! Remove isolated invalid hanging parts
  void RemoveHangingParts(
    const BOPAlgo_MakerVolume&                                                      theMV,
    const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDMFImF,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&            theMFInv,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                         theMFToRem);

  //! Removing valid splits according to results of intersection
  void RemoveValidSplits(
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theSpRem,
    BOPAlgo_Builder&                                               theGF,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved);

  //! Removing invalid splits according to the results of intersection
  void RemoveInvalidSplits(
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theSpRem,
    BOPAlgo_Builder&                                               theGF,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved);

  //! Filtering of the invalid faces
  void FilterInvalidFaces(
    const NCollection_IndexedDataMap<TopoDS_Shape,
                                     NCollection_List<TopoDS_Shape>,
                                     TopTools_ShapeMapHasher>&           theDMEF,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved);

  //! Checks if the face is artificially invalid
  bool CheckIfArtificial(
    const TopoDS_Shape&                                                  theF,
    const NCollection_List<TopoDS_Shape>&                                theLFImages,
    const TopoDS_Shape&                                                  theCE,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapEInv,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theMENInv);

  //! Looking for the faces that have to be rebuilt:
  //! * Faces close to invalidity
  //! * Faces containing some invalid parts
  void FindFacesToRebuild();

  //! Intersection of the faces that should be rebuild to resolve all invalidities
  void IntersectFaces(NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsToAvoid,
                      const Message_ProgressRange&                            theRange);

  //! Preparation of the maps for analyzing intersections of the faces
  void PrepareFacesForIntersection(
    const bool                                           theLookVertToAvoid,
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>& theFLE,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theMDone,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theDMSF,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theMEInfETrim,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                         theDMVEFull,
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>& theDMEFInv);

  //! Looking for the invalid vertices
  void FindVerticesToAvoid(const NCollection_IndexedDataMap<TopoDS_Shape,
                                                            NCollection_List<TopoDS_Shape>,
                                                            TopTools_ShapeMapHasher>& theDMEFInv,
                           const NCollection_DataMap<TopoDS_Shape,
                                                     NCollection_List<TopoDS_Shape>,
                                                     TopTools_ShapeMapHasher>&        theDMVEFull,
                           NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&    theMVRInv);

  //! Looking for the faces around each invalidity for intersection
  void FindFacesForIntersection(
    const TopoDS_Shape&                                                  theFInv,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theME,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&                  theDMSF,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theMVInvAll,
    const bool                                                           theArtCase,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMFAvoid,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMFInt,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMFIntExt,
    NCollection_List<TopoDS_Shape>&                                      theLFImInt);

  //! Analyzing the common edges between splits of offset faces
  void ProcessCommonEdges(
    const NCollection_List<TopoDS_Shape>&                                theLEC,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theME,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&                  theMEInfETrim,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theAllInvs,
    const bool                                                           theForceUse,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECV,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theMECheckExt,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                   theDMEETrim,
    NCollection_List<TopoDS_Shape>&                                theLFEi,
    NCollection_List<TopoDS_Shape>&                                theLFEj,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEToInt);

  //! Updating the already interfered faces
  void UpdateIntersectedFaces(
    const TopoDS_Shape&                                            theFInv,
    const TopoDS_Shape&                                            theFi,
    const TopoDS_Shape&                                            theFj,
    const NCollection_List<TopoDS_Shape>&                          theLFInv,
    const NCollection_List<TopoDS_Shape>&                          theLFImi,
    const NCollection_List<TopoDS_Shape>&                          theLFImj,
    const NCollection_List<TopoDS_Shape>&                          theLFEi,
    const NCollection_List<TopoDS_Shape>&                          theLFEj,
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEToInt);

  //! Intersection of the pair of faces
  void IntersectFaces(const TopoDS_Shape&                                            theFInv,
                      const TopoDS_Shape&                                            theFi,
                      const TopoDS_Shape&                                            theFj,
                      const NCollection_List<TopoDS_Shape>&                          theLFInv,
                      const NCollection_List<TopoDS_Shape>&                          theLFImi,
                      const NCollection_List<TopoDS_Shape>&                          theLFImj,
                      NCollection_List<TopoDS_Shape>&                                theLFEi,
                      NCollection_List<TopoDS_Shape>&                                theLFEj,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMECV,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEToInt);

  //! Intersection of the new intersection edges among themselves
  void IntersectAndTrimEdges(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInt,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInt,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&                  theDMEETrim,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMSInv,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVE,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theVertsToAvoid,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theNewVertsToAvoid,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theMECheckExt,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>*                  theSSInterfs,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theMVBounds,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEImages);

  //! Looking for the invalid edges by intersecting with invalid vertices
  void GetInvalidEdges(
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsToAvoid,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
    BOPAlgo_Builder&                                              theGF,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMEInv);

  //! Making the new splits and updating the maps
  void UpdateValidEdges(
    const NCollection_IndexedDataMap<TopoDS_Shape,
                                     NCollection_List<TopoDS_Shape>,
                                     TopTools_ShapeMapHasher>&    theFLE,
    const NCollection_IndexedDataMap<TopoDS_Shape,
                                     NCollection_List<TopoDS_Shape>,
                                     TopTools_ShapeMapHasher>&    theOENEdges,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInvOnArt,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECheckExt,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theVertsToAvoid,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEImages,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                 theEETrim,
    const Message_ProgressRange& theRange);

  //! Trims intersection edges
  void TrimNewIntersectionEdges(
    const NCollection_List<TopoDS_Shape>&                         theLE,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&           theEETrim,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECheckExt,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                            theEImages,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEB,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVOld,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMENew,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theDMEOr,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theMELF);

  //! Intersecting the trimmed edges to avoid self-intersections
  void IntersectEdges(
    const NCollection_List<TopoDS_Shape>&                         theLA,
    const NCollection_List<TopoDS_Shape>&                         theLE,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsToAvoid,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMENew,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECheckExt,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEImages,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theDMEOr,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                  theMELF,
    TopoDS_Shape& theSplits);

  //! Getting edges from the splits of offset faces
  void GetBounds(const NCollection_List<TopoDS_Shape>&                         theLFaces,
                 const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEB,
                 TopoDS_Shape&                                                 theBounds);

  //! Get bounding edges that should be updated
  void GetBoundsToUpdate(const NCollection_List<TopoDS_Shape>&                         theLF,
                         const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEB,
                         NCollection_List<TopoDS_Shape>&                               theLABounds,
                         NCollection_List<TopoDS_Shape>&                               theLAValid,
                         TopoDS_Shape&                                                 theBounds);

  //! Filter new splits by intersection with bounds
  void GetInvalidEdgesByBounds(
    const TopoDS_Shape&                                           theSplits,
    const TopoDS_Shape&                                           theBounds,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVOld,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMENew,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&           theDMEOr,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&           theMELF,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>&           theEImages,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMECheckExt,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInvOnArt,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theVertsToAvoid,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMEInv);

  //! Filter the images of edges from the invalid edges
  void FilterSplits(
    const NCollection_List<TopoDS_Shape>&                         theLE,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEFilter,
    const bool                                                    theIsInv,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                  theEImages,
    TopoDS_Shape& theSplits);

  //! Updating the maps of images and origins of the offset edges
  void UpdateNewIntersectionEdges(
    const NCollection_List<TopoDS_Shape>&               theLE,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>& theMELF,
    const NCollection_DataMap<TopoDS_Shape,
                              NCollection_List<TopoDS_Shape>,
                              TopTools_ShapeMapHasher>& theEImages,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      theEETrim);

private:
  //! Fill possible gaps (holes) in the splits of the offset faces
  void FillGaps(const Message_ProgressRange& theRange);

  //! Saving obtained results in history tools
  void FillHistory();

private:
  // Input data
  const NCollection_List<TopoDS_Shape>* myFaces;    //!< Input faces which have to be split
  occ::handle<BRepAlgo_AsDes>           myAsDes;    //!< Ascendants/descendants of the edges faces
  const BRepOffset_Analyse*             myAnalyzer; //!< Analyzer of the input parameters

  // clang-format off
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>* myEdgesOrigins; //!< Origins of the offset edges (binding between offset edge and original edge)
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>* myFacesOrigins;       //!< Origins of the offset faces (binding between offset face and original face)
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>* myETrimEInf;          //!< Binding between trimmed and infinite edge

  // Intermediate data
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myOEImages;        //!< Images of the offset edges 
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myOEOrigins;       //!< Origins of the splits of offset edges
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myOFImages; //!< Images of the offset edges

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myInvalidEdges;  //!< Edges considered invalid (orientation is changed) in some split of face
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myValidEdges;    //!< Edges considered valid (orientation is kept) in some split of face
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myInvertedEdges; //!< Edges considered inverted (vertices swapped) in some split of face
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myEdgesToAvoid;  //!< Splits of edges to be avoided when building splits of faces
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myLastInvEdges;         //!< Edges marked invalid on the current step and to be avoided on the next step
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myModifiedEdges;        //!< Edges to be used for building splits
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myInsideEdges;   //!< Edges located fully inside solids built from the splits of offset faces

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myInvalidFaces; //!< Invalid faces - splits of offset faces consisting of invalid edges
  BRepOffset_DataMapOfShapeIndexedMapOfShape myArtInvalidFaces; //!< Artificially invalid faces - valid faces intentionally marked invalid
                                                                //!  to be rebuilt on the future steps in the situations when invalid edges
                                                                //!  are present, but invalid faces not
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> myAlreadyInvFaces;         //!< Count number of the same face being marked invalid to avoid infinite
                                                            //!  rebuilding of the same face
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myFNewHoles;           //!< Images of the hole faces of the original face

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mySSInterfs; //!< Intersection information, used to collect intersection pairs during rebuild
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mySSInterfsArt; //!< Intersection information, used to collect intersection pairs during rebuild
  NCollection_DataMap <TopoDS_Shape,
    NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>, TopTools_ShapeMapHasher>,
    TopTools_ShapeMapHasher> myIntersectionPairs; //!< All possible intersection pairs, used to avoid some of the intersections

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myFacesToRebuild; //!< Faces that have to be rebuilt (invalid and close to invalid faces)
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myFSelfRebAvoid;                        //!< Faces that have to be avoided when rebuilding splits of the same offset face
  // clang-format on

  TopoDS_Shape mySolids; //!< Solids built from the splits of faces

  // Auxiliary tools
  occ::handle<IntTools_Context> myContext;

  // Output
  BRepAlgo_Image* myImage; //!< History of modifications
};

//=================================================================================================

void BRepOffset_BuildOffsetFaces::BuildSplitsOfTrimmedFaces(const Message_ProgressRange& theRange)
{
  if (!hasData(myFaces))
  {
    return;
  }

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEdgesOrigins;
  if (!myEdgesOrigins)
  {
    myEdgesOrigins = &anEdgesOrigins;
  }

  Message_ProgressScope aPS(theRange, "Building splits of trimmed faces", 5);

  // Fuse all edges
  IntersectTrimmedEdges(aPS.Next(1));

  Message_ProgressScope aPSLoop(aPS.Next(4), nullptr, myFaces->Extent());
  for (NCollection_List<TopoDS_Shape>::Iterator aItLF(*myFaces); aItLF.More(); aItLF.Next())
  {
    if (!aPSLoop.More())
    {
      return;
    }
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();

    TopoDS_Shape aCE;
    bool         bFound = GetEdges(aF, aCE);

    // split the face by the edges
    if (!bFound)
    {
      if (!myImage->HasImage(aF))
      {
        myOFImages(myOFImages.Add(aF, NCollection_List<TopoDS_Shape>())).Append(aF);
      }
      continue;
    }

    NCollection_List<TopoDS_Shape> aLFImages;
    BuildSplitsOfTrimmedFace(aF, aCE, aLFImages, aPSLoop.Next());

    myOFImages.Add(aF, aLFImages);
  }
  // Fill history for faces and edges
  FillHistory();
}

//=================================================================================================

void BRepOffset_BuildOffsetFaces::BuildSplitsOfExtendedFaces(const Message_ProgressRange& theRange)
{
  // Check input data
  if (!hasData(myFaces) || !hasData(myEdgesOrigins) || !hasData(myFacesOrigins)
      || !hasData(myETrimEInf))
  {
    return;
  }

  Message_ProgressScope aPS(theRange, "Building splits of extended faces", 100.);
  // Scope has to be added into a loop of undefined size.
  // In general there are about 2 to 5 loops performed, each time
  // decreasing complexity. So reserve for each next loop smaller time.
  // Reserve also 4% on filling gaps after the faces are built.
  double aWhole = 100. - 4.;

  // Fusing all trimmed offset edges to avoid self-intersections in the splits
  IntersectTrimmedEdges(aPS.Next());
  if (!aPS.More())
  {
    return;
  }
  // vertices to avoid
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aVertsToAvoid;

  // Limit total number of attempts by 10. This should be extra, as each invalid face can be
  // rebuilt only three times. So, in general, there are about 2-5 loops done.
  const int aNbMaxAttempts = 10;
  // First progress portion is the half of the whole. Each next step is half of the previous:
  // 48%, 24%, 12%, 6% and so on. This way in three loops it will already be 84%,
  // and in four - 90%. So even if the loop will stop earlier, the not advanced scope
  // will be acceptable.
  double aPart = aWhole / 2.;
  for (int iCount = 1; iCount <= aNbMaxAttempts; ++iCount, aPart /= 2.)
  {
    if (!aPS.More())
    {
      return;
    }
    // Clear the data before further faces construction
    myInvalidFaces.Clear();
    myArtInvalidFaces.Clear();
    myInvalidEdges.Clear();
    myInvertedEdges.Clear();
    mySSInterfs.Clear();
    mySSInterfsArt.Clear();
    myIntersectionPairs.Clear();
    mySolids.Nullify();
    myFacesToRebuild.Clear();
    myFSelfRebAvoid.Clear();

    // Split progress range on
    // * building faces basing on currently available edges and
    // * rebuilding faces basing on edges classification
    Message_ProgressScope aPSLoop(aPS.Next(aPart), nullptr, 10.);

    // Build splits of the faces having new intersection edges
    BuildSplitsOfFaces(aPSLoop.Next(7.));
    if (myInvalidFaces.IsEmpty())
    {
      break;
    }

    // Find faces to rebuild
    FindFacesToRebuild();
    if (myFacesToRebuild.IsEmpty())
    {
      break;
    }

    // Perform new intersections
    myModifiedEdges.Clear();
    IntersectFaces(aVertsToAvoid, aPSLoop.Next(3.));
  }

  // Fill possible gaps in the splits of offset faces to increase possibility of
  // creating closed volume from these splits
  FillGaps(aPS.Next(4.));

  // Fill history for faces and edges
  FillHistory();
}

//=======================================================================
// function : UpdateIntersectedEdges
// purpose  : Saving connection from trimmed edges to not trimmed ones
//=======================================================================
void BRepOffset_BuildOffsetFaces::UpdateIntersectedEdges(
  const NCollection_List<TopoDS_Shape>& theLA,
  BOPAlgo_Builder&                      theGF)
{
  for (NCollection_List<TopoDS_Shape>::Iterator aItA(theLA); aItA.More(); aItA.Next())
  {
    const TopoDS_Shape& aS    = aItA.Value();
    const TopoDS_Shape* pEInf = myETrimEInf->Seek(aS);
    if (!pEInf)
    {
      continue;
    }

    const NCollection_List<TopoDS_Shape>& aLSIm = theGF.Modified(aS);
    if (aLSIm.IsEmpty())
    {
      continue;
    }

    for (NCollection_List<TopoDS_Shape>::Iterator aIt(aLSIm); aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aEIm = aIt.Value();
      if (!myETrimEInf->IsBound(aEIm))
      {
        myETrimEInf->Bind(aEIm, *pEInf);
      }
    }
  }
}

//=================================================================================================

void BRepOffset_BuildOffsetFaces::IntersectTrimmedEdges(const Message_ProgressRange& theRange)
{
  // get edges to intersect from descendants of the offset faces
  NCollection_List<TopoDS_Shape> aLS;
  //
  Message_ProgressScope                    aPS(theRange, nullptr, 2);
  NCollection_List<TopoDS_Shape>::Iterator aItLF(*myFaces);
  for (; aItLF.More(); aItLF.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    const NCollection_List<TopoDS_Shape>&    aLE = myAsDes->Descendant(aF);
    NCollection_List<TopoDS_Shape>::Iterator aItLE(aLE);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Edge& aE = *(TopoDS_Edge*)&aItLE.Value();
      //
      if (ProcessMicroEdge(aE, myContext))
      {
        continue;
      }
      //
      if (myModifiedEdges.Add(aE))
      {
        aLS.Append(aE);
      }
    }
  }
  //
  if (aLS.Extent() < 2)
  {
    // nothing to intersect
    return;
  }
  //
  // perform intersection of the edges
  BOPAlgo_Builder aGFE;
  aGFE.SetArguments(aLS);
  aGFE.Perform(aPS.Next());
  if (aGFE.HasErrors())
  {
    return;
  }
  //
  NCollection_List<TopoDS_Shape> aLA;
  // fill map with edges images
  Message_ProgressScope aPSLoop(aPS.Next(), nullptr, aLS.Size());
  for (NCollection_List<TopoDS_Shape>::Iterator aIt(aLS); aIt.More(); aIt.Next(), aPSLoop.Next())
  {
    if (!aPSLoop.More())
    {
      return;
    }
    const TopoDS_Shape&                   aE    = aIt.Value();
    const NCollection_List<TopoDS_Shape>& aLEIm = aGFE.Modified(aE);
    if (aLEIm.IsEmpty())
    {
      continue;
    }
    //
    aLA.Append(aE);
    // save images
    myOEImages.Bind(aE, aLEIm);
    // save origins
    NCollection_List<TopoDS_Shape>::Iterator aItLE(aLEIm);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Shape& aEIm = aItLE.Value();
      if (NCollection_List<TopoDS_Shape>* pLEOr = myOEOrigins.ChangeSeek(aEIm))
      {
        AppendToList(*pLEOr, aE);
      }
      else
      {
        myOEOrigins.Bound(aEIm, NCollection_List<TopoDS_Shape>())->Append(aE);
      }
    }
  }
  //
  UpdateOrigins(aLA, *myEdgesOrigins, aGFE);
  UpdateIntersectedEdges(aLA, aGFE);
}

namespace
{
//=======================================================================
// function : CheckConnectionsOfFace
// purpose  : Checks number of connections for theFace with theLF
//           Returns true if number of connections more than 1
//=======================================================================
static bool checkConnectionsOfFace(const TopoDS_Shape&                   theFace,
                                   const NCollection_List<TopoDS_Shape>& theLF)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeVert;
  for (NCollection_List<TopoDS_Shape>::Iterator aFImIterator(theLF); aFImIterator.More();
       aFImIterator.Next())
  {
    const TopoDS_Shape& aShape = aFImIterator.Value();
    if (aShape.IsSame(theFace))
    {
      continue;
    }
    TopExp::MapShapes(aShape, TopAbs_VERTEX, aShapeVert);
  }
  int                                                           aNbConnections = 0;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaceVertices;
  TopExp::MapShapes(theFace, TopAbs_VERTEX, aFaceVertices);
  for (NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aVertIter(
         aFaceVertices);
       aVertIter.More();
       aVertIter.Next())
  {
    const TopoDS_Shape& aVert = aVertIter.Value();
    if (aShapeVert.Contains(aVert))
    {
      ++aNbConnections;
    }
    if (aNbConnections > 1)
    {
      return true;
    }
  }
  return false;
}
} // namespace

//=======================================================================
// function : BuildSplitsOfFaces
// purpose  : Building the splits of offset faces and
//           looking for the invalid splits
//=======================================================================
void BRepOffset_BuildOffsetFaces::BuildSplitsOfFaces(const Message_ProgressRange& theRange)
{
  BRep_Builder aBB;
  int          i, aNb;
  //
  // processed faces
  NCollection_List<TopoDS_Shape> aLFDone;
  // extended face - map of neutral edges, i.e. in one split - valid and in other - invalid
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>
    aDMFMNE;
  // map of valid edges for each face
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>
    aDMFMVE;
  // map of invalid edges for each face
  BRepOffset_DataMapOfShapeIndexedMapOfShape aDMFMIE;
  // map of valid inverted edges for the face
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>
    aDMFMVIE;
  // map of splits to check for internals
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFToCheckInt;
  // map of edges created from vertex and marked as invalid
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEdgeInvalidByVertex;
  // map of edges created from vertex and marked as valid
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEdgeValidByVertex;
  // connection map from old edges to new ones
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMEOrLEIm;
  //
  // Outer range
  Message_ProgressScope aPSOuter(theRange, nullptr, 10.);
  // build splits of faces
  Message_ProgressScope aPSBF(aPSOuter.Next(3.), "Building faces", 2 * myFaces->Extent());
  NCollection_List<TopoDS_Shape>::Iterator aItLF(*myFaces);
  for (; aItLF.More(); aItLF.Next(), aPSBF.Next())
  {
    if (!aPSBF.More())
    {
      return;
    }
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    NCollection_List<TopoDS_Shape>* pLFIm = myOFImages.ChangeSeek(aF);
    if (pLFIm && pLFIm->IsEmpty())
    {
      continue;
    }
    // get edges by which the face should be split
    TopoDS_Shape                                                  aCE;
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapEInv;
    bool bFound = GetEdges(aF, aCE, &aMapEInv);
    if (!bFound)
    {
      continue;
    }
    //
#ifdef OFFSET_DEBUG
    // check the found edges on self-intersection
    BRepAlgoAPI_Check aChecker(aCE);
    if (!aChecker.IsValid())
    {
      std::cout << "Offset_i_c Error: set of edges to build faces is self-intersecting\n";
    }
#endif
    // build splits
    NCollection_List<TopoDS_Shape> aLFImages;
    BuildSplitsOfFace(aF, aCE, *myFacesOrigins, aLFImages);
    //
    if (aMapEInv.Extent())
    {
      // check if all possible faces are built
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMENInv;
      bool                                                   bArtificialCase =
        aLFImages.IsEmpty() || CheckIfArtificial(aF, aLFImages, aCE, aMapEInv, aMENInv);
      //
      // try to build splits using invalid edges
      TopoDS_Compound aCE1;
      aBB.MakeCompound(aCE1);
      aBB.Add(aCE1, aCE);
      for (i = 1; i <= aMapEInv.Extent(); ++i)
      {
        aBB.Add(aCE1, aMapEInv(i));
      }
      //
      NCollection_List<TopoDS_Shape> aLFImages1;
      BuildSplitsOfFace(aF, aCE1, *myFacesOrigins, aLFImages1);
      //
      // check if the rebuilding has added some new faces to the splits
      for (NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFImages1); aItLFIm.More();)
      {
        bool bAllInv = true;
        // Additional check for artificial case
        // if current image face consist only of edges from aMapEInv and aMENInv
        // then recheck current face for the further processing
        bool                aToReCheckFace = bArtificialCase;
        const TopoDS_Shape& aFIm           = aItLFIm.Value();
        TopExp_Explorer     aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next())
        {
          const TopoDS_Shape& aE = aExpE.Current();
          if (!aMapEInv.Contains(aE))
          {
            bAllInv = false;
            if (!aMENInv.Contains(aE))
            {
              aToReCheckFace = false;
              break;
            }
          }
        }
        // if current image face is to recheck then check number of connections for this face
        // with other image faces for current face
        if (!aExpE.More() && aToReCheckFace)
        {
          aToReCheckFace = checkConnectionsOfFace(aFIm, aLFImages1);
        }
        // do not delete image face from further processing if aToReCheckFace is true
        if (!aExpE.More() && !aToReCheckFace)
        {
          if (bAllInv)
          {
            aMFToCheckInt.Add(aFIm);
          }
          aLFImages1.Remove(aItLFIm);
        }
        else
        {
          aItLFIm.Next();
        }
      }
      //
      if (bArtificialCase)
      {
        if (aLFImages.Extent() == aLFImages1.Extent())
        {
          bArtificialCase = false;
        }
        else
        {
          aLFImages = aLFImages1;
        }
      }
      //
      if (bArtificialCase)
      {
        // make the face invalid
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
        //
        *pLFIm = aLFImages;
        NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFImages);
        for (; aItLFIm.More(); aItLFIm.Next())
        {
          const TopoDS_Shape& aFIm = aItLFIm.Value();
          TopExp_Explorer     aExpE(aFIm, TopAbs_EDGE);
          for (; aExpE.More(); aExpE.Next())
          {
            const TopoDS_Shape& aE = aExpE.Current();
            if (aMapEInv.Contains(aE))
            {
              myInvalidEdges.Add(aE);
              aMEInv.Add(aE);
            }
            else
            {
              myValidEdges.Add(aE);
            }
          }
        }
        //
        myArtInvalidFaces.Bind(aF, aMEInv);
        aDMFMIE.Bind(aF, aMEInv);
        aLFDone.Append(aF);
        //
        continue;
      }
    }

    // find invalid edges
    FindInvalidEdges(aF,
                     aLFImages,
                     aDMFMVE,
                     aDMFMNE,
                     aDMFMIE,
                     aDMFMVIE,
                     aDMEOrLEIm,
                     aMEdgeInvalidByVertex,
                     aMEdgeValidByVertex,
                     aPSBF.Next());

    // save the new splits
    if (!pLFIm)
    {
      pLFIm = &myOFImages(myOFImages.Add(aF, NCollection_List<TopoDS_Shape>()));
    }
    else
    {
      pLFIm->Clear();
    }
    pLFIm->Append(aLFImages);
    //
    aLFDone.Append(aF);
  }
  //
  if (myInvalidEdges.IsEmpty() && myArtInvalidFaces.IsEmpty() && aDMFMIE.IsEmpty())
  {
    return;
  }

  // Additional step to find invalid edges by checking unclassified edges
  // in the splits of SD faces
  FindInvalidEdges(aLFDone, aDMFMIE, aDMFMVE, aDMFMNE);

  // Additional step to mark inverted edges located inside loops
  // of invalid edges as invalid as well
  MakeInvertedEdgesInvalid(aLFDone);

#ifdef OFFSET_DEBUG
  // show invalid edges
  TopoDS_Compound aCEInv1;
  BRep_Builder().MakeCompound(aCEInv1);
  int aNbEInv = myInvalidEdges.Extent();
  for (i = 1; i <= aNbEInv; ++i)
  {
    const TopoDS_Shape& aE = myInvalidEdges(i);
    BRep_Builder().Add(aCEInv1, aE);
  }
  // show valid edges
  TopoDS_Compound aCEVal1;
  BRep_Builder().MakeCompound(aCEVal1);
  aNbEInv = myValidEdges.Extent();
  for (i = 1; i <= aNbEInv; ++i)
  {
    const TopoDS_Shape& aE = myValidEdges(i);
    BRep_Builder().Add(aCEVal1, aE);
  }
  // show inverted edges
  TopoDS_Compound aCEInverted;
  BRep_Builder().MakeCompound(aCEInverted);
  for (i = 1; i <= myInvertedEdges.Extent(); ++i)
  {
    BRep_Builder().Add(aCEInverted, myInvertedEdges(i));
  }
#endif

#ifdef OFFSET_DEBUG
  // Show all obtained splits of faces
  TopoDS_Compound aCFIm1;
  BRep_Builder().MakeCompound(aCFIm1);
#endif

  // Build Edge-Face connectivity map to find faces which removal
  // may potentially lead to creation of the holes in the faces
  // preventing from obtaining closed volume in the result
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
            anEFMap;
  const int aNbF = myOFImages.Extent();
  for (i = 1; i <= aNbF; ++i)
  {
    NCollection_List<TopoDS_Shape>::Iterator itLFIm(myOFImages(i));
    for (; itLFIm.More(); itLFIm.Next())
    {
      TopExp::MapShapesAndAncestors(itLFIm.Value(), TopAbs_EDGE, TopAbs_FACE, anEFMap);
#ifdef OFFSET_DEBUG
      BRep_Builder().Add(aCFIm1, itLFIm.Value());
#endif
    }
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anEmptyMap;
  // invalid faces inside the holes
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFInvInHole;
  // all hole faces
  TopoDS_Compound aFHoles;
  aBB.MakeCompound(aFHoles);
  // Find the faces containing only the inverted edges and the invalid ones
  NCollection_List<TopoDS_Shape> anInvertedFaces;

  Message_ProgressScope aPSIF(aPSOuter.Next(2.), "Checking validity of faces", aLFDone.Extent());
  // find invalid faces
  // considering faces containing only invalid edges as invalid
  aItLF.Initialize(aLFDone);
  for (; aItLF.More(); aItLF.Next(), aPSIF.Next())
  {
    if (!aPSIF.More())
    {
      return;
    }
    const TopoDS_Face&              aF        = TopoDS::Face(aItLF.Value());
    NCollection_List<TopoDS_Shape>& aLFImages = myOFImages.ChangeFromKey(aF);
    //
    NCollection_List<TopoDS_Shape> aLFInv;
    bool                           bArtificialCase = myArtInvalidFaces.IsBound(aF);
    if (bArtificialCase)
    {
      aLFInv = aLFImages;
    }
    else
    {
      // neutral edges
      const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMNE = aDMFMNE.ChangeSeek(aF);
      if (!pMNE)
      {
        pMNE = &anEmptyMap;
      }
      // find faces inside holes wires
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFHoles;
      const TopoDS_Face& aFOr = TopoDS::Face(myFacesOrigins->Find(aF));
      FindFacesInsideHoleWires(aFOr, aF, aLFImages, aDMEOrLEIm, anEFMap, aMFHoles);
      //
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItMH(aMFHoles);
      for (; aItMH.More(); aItMH.Next())
      {
        aBB.Add(aFHoles, aItMH.Value());
      }
      //
      // find invalid faces
      FindInvalidFaces(aLFImages,
                       aDMFMVE,
                       aDMFMIE,
                       *pMNE,
                       aMEdgeInvalidByVertex,
                       aMEdgeValidByVertex,
                       aMFHoles,
                       aMFInvInHole,
                       aLFInv,
                       anInvertedFaces);
    }
    //
    if (aLFInv.Extent())
    {
      if (myAlreadyInvFaces.IsBound(aF))
      {
        if (myAlreadyInvFaces.Find(aF) > 2)
        {
          if (aLFInv.Extent() == aLFImages.Extent() && !bArtificialCase)
          {
            aLFImages.Clear();
          }
          //
          aLFInv.Clear();
        }
      }
      myInvalidFaces.Add(aF, aLFInv);
    }
  }
  //
  if (myInvalidFaces.IsEmpty())
  {
    myInvalidEdges.Clear();
    return;
  }
  //
#ifdef OFFSET_DEBUG
  // show invalid faces
  TopoDS_Compound aCFInv1;
  BRep_Builder().MakeCompound(aCFInv1);
  int aNbFInv = myInvalidFaces.Extent();
  for (i = 1; i <= aNbFInv; ++i)
  {
    const NCollection_List<TopoDS_Shape>&    aLFInv = myInvalidFaces(i);
    NCollection_List<TopoDS_Shape>::Iterator aItLFInv(aLFInv);
    for (; aItLFInv.More(); aItLFInv.Next())
    {
      const TopoDS_Shape& aFIm = aItLFInv.Value();
      BRep_Builder().Add(aCFInv1, aFIm);
    }
  }
#endif
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMERemoved;
  // remove invalid splits of faces using inverted edges
  RemoveInvalidSplitsByInvertedEdges(aMERemoved);
  if (myInvalidFaces.IsEmpty())
  {
    myInvalidEdges.Clear();
    return;
  }
  //
  // remove invalid splits from valid splits
  RemoveInvalidSplitsFromValid(aDMFMVIE);
  //
  // remove inside faces
  RemoveInsideFaces(anInvertedFaces,
                    aMFToCheckInt,
                    aMFInvInHole,
                    aFHoles,
                    aMERemoved,
                    myInsideEdges,
                    aPSOuter.Next(5.));
  //
  // make compound of valid splits
  TopoDS_Compound aCFIm;
  aBB.MakeCompound(aCFIm);
  //
  aNb = myOFImages.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLFIm = myOFImages(i);
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aBB.Add(aCFIm, aFIm);
    }
  }
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMEF;
  TopExp::MapShapesAndAncestors(aCFIm, TopAbs_EDGE, TopAbs_FACE, aDMEF);
  //
  // filter maps of images and origins
  FilterEdgesImages(aCFIm);
  //
  // filter invalid faces
  FilterInvalidFaces(aDMEF, aMERemoved.Extent() ? myInsideEdges : aMERemoved);
  aNb = myInvalidFaces.Extent();
  if (!aNb)
  {
    myInvalidEdges.Clear();
    return;
  }
  //
#ifdef OFFSET_DEBUG
  // show invalid faces
  TopoDS_Compound aCFInv;
  BRep_Builder().MakeCompound(aCFInv);
  aNbFInv = myInvalidFaces.Extent();
  for (i = 1; i <= aNbFInv; ++i)
  {
    const NCollection_List<TopoDS_Shape>&    aLFInv = myInvalidFaces(i);
    NCollection_List<TopoDS_Shape>::Iterator aItLFInv(aLFInv);
    for (; aItLFInv.More(); aItLFInv.Next())
    {
      const TopoDS_Shape& aFIm = aItLFInv.Value();
      BRep_Builder().Add(aCFInv, aFIm);
    }
  }
#endif
  //
  // filter invalid edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEUseInRebuild;
  FilterInvalidEdges(aDMFMIE,
                     aMERemoved,
                     aMERemoved.Extent() ? myInsideEdges : aMERemoved,
                     aMEUseInRebuild);
  //
  // Check additionally validity of edges originated from vertices.
  CheckEdgesCreatedByVertex();

#ifdef OFFSET_DEBUG
  // show invalid edges
  TopoDS_Compound aCEInv;
  BRep_Builder().MakeCompound(aCEInv);
  aNbEInv = myInvalidEdges.Extent();
  for (i = 1; i <= aNbEInv; ++i)
  {
    const TopoDS_Shape& aE = myInvalidEdges(i);
    BRep_Builder().Add(aCEInv, aE);
  }
#endif
  //
  myLastInvEdges.Clear();
  aNb = myInvalidEdges.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aE = myInvalidEdges(i);
    myLastInvEdges.Add(aE);
    if (!aMEUseInRebuild.Contains(aE))
    {
      myEdgesToAvoid.Add(aE);
    }
  }
  //
  aNb = myInvalidFaces.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aF = myInvalidFaces.FindKey(i);
    if (myAlreadyInvFaces.IsBound(aF))
    {
      myAlreadyInvFaces.ChangeFind(aF)++;
    }
    else
    {
      myAlreadyInvFaces.Bind(aF, 1);
    }
  }
}

//=======================================================================
// function : GetEdges
// purpose  : Getting edges from AsDes map to build the splits of faces
//=======================================================================
bool BRepOffset_BuildOffsetFaces::GetEdges(
  const TopoDS_Face&                                             theFace,
  TopoDS_Shape&                                                  theEdges,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* theInvMap)
{
  // get boundary edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFBounds;
  TopExp_Explorer                                        aExp(theFace, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aE = aExp.Current();
    if (const NCollection_List<TopoDS_Shape>* pLEIm = myOEImages.Seek(aE))
    {
      for (NCollection_List<TopoDS_Shape>::Iterator aItLE(*pLEIm); aItLE.More(); aItLE.Next())
      {
        aMFBounds.Add(aItLE.Value());
      }
    }
    else
    {
      aMFBounds.Add(aE);
    }
  }

  BRep_Builder aBB;
  bool         bFound(false), bUpdate(false);
  // the resulting edges
  TopoDS_Compound anEdges;
  aBB.MakeCompound(anEdges);
  // Fence map
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEFence;
  // the edges by which the offset face should be split
  const NCollection_List<TopoDS_Shape>&    aLE = myAsDes->Descendant(theFace);
  NCollection_List<TopoDS_Shape>::Iterator aItLE(aLE);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
    //
    if (!bUpdate)
    {
      bUpdate = myModifiedEdges.Contains(aE);
    }
    //
    const NCollection_List<TopoDS_Shape>* pLEIm = myOEImages.Seek(aE);
    if (pLEIm)
    {
      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        const TopoDS_Edge& aEIm = TopoDS::Edge(aItLEIm.Value());
        //
        if (!aMEFence.Add(aEIm))
          continue;

        if (myEdgesToAvoid.Contains(aEIm))
        {
          if (theInvMap)
          {
            theInvMap->Add(aEIm);
          }
          if (!bUpdate)
          {
            bUpdate = myLastInvEdges.Contains(aEIm);
          }
          continue;
        }
        // check for micro edge
        if (ProcessMicroEdge(aEIm, myContext))
        {
          continue;
        }
        //
        aBB.Add(anEdges, aEIm);
        if (!bFound)
        {
          bFound = !aMFBounds.Contains(aEIm);
        }
        //
        if (!bUpdate)
        {
          bUpdate = myModifiedEdges.Contains(aEIm);
        }
      }
    }
    else
    {
      if (myEdgesToAvoid.Contains(aE))
      {
        if (theInvMap)
        {
          theInvMap->Add(aE);
        }
        if (!bUpdate)
        {
          bUpdate = myLastInvEdges.Contains(aE);
        }
        continue;
      }
      //
      if (ProcessMicroEdge(aE, myContext))
      {
        continue;
      }
      aBB.Add(anEdges, aE);
      if (!bFound)
      {
        bFound = !aMFBounds.Contains(aE);
      }
    }
  }
  //
  theEdges = anEdges;
  return bFound && bUpdate;
}

//=======================================================================
// function : CheckIfArtificial
// purpose  : Checks if the face is artificially invalid
//=======================================================================
bool BRepOffset_BuildOffsetFaces::CheckIfArtificial(
  const TopoDS_Shape&                                                  theF,
  const NCollection_List<TopoDS_Shape>&                                theLFImages,
  const TopoDS_Shape&                                                  theCE,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapEInv,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theMENInv)
{
  // all boundary edges should be used
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEUsed;
  NCollection_List<TopoDS_Shape>::Iterator                      aItLFIm(theLFImages);
  for (; aItLFIm.More(); aItLFIm.Next())
  {
    const TopoDS_Shape& aFIm = aItLFIm.Value();
    TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEUsed);
    TopExp::MapShapes(aFIm, TopAbs_VERTEX, aMEUsed);
  }
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMVE;
  TopExp::MapShapesAndAncestors(theCE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  //
  int i, aNb = theMapEInv.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aEInv = theMapEInv(i);
    TopExp_Explorer     aExpV(aEInv, TopAbs_VERTEX);
    for (; aExpV.More(); aExpV.Next())
    {
      const TopoDS_Shape&                   aVEInv  = aExpV.Current();
      const NCollection_List<TopoDS_Shape>* pLENInv = aMVE.Seek(aVEInv);
      if (pLENInv)
      {
        NCollection_List<TopoDS_Shape>::Iterator aItLEInv(*pLENInv);
        for (; aItLEInv.More(); aItLEInv.Next())
        {
          const TopoDS_Shape& aENInv = aItLEInv.Value();
          if (!aMEUsed.Contains(aENInv))
          {
            theMENInv.Add(aENInv);
          }
        }
      }
    }
  }
  //
  if (theMENInv.IsEmpty())
  {
    return false;
  }
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEFound;
  TopExp::MapShapes(theCE, TopAbs_EDGE, aMEFound);
  //
  const NCollection_List<TopoDS_Shape>&    aLE = myAsDes->Descendant(theF);
  NCollection_List<TopoDS_Shape>::Iterator aItLE(aLE);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
    //
    if (const NCollection_List<TopoDS_Shape>* pLEIm = myOEImages.Seek(aE))
    {
      bool                                     bChecked = false;
      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        const TopoDS_Edge& aEIm = TopoDS::Edge(aItLEIm.Value());
        if (!aMEFound.Contains(aEIm) || theMENInv.Contains(aEIm))
        {
          continue;
        }
        //
        bChecked = true;
        if (aMEUsed.Contains(aEIm))
        {
          break;
        }
      }
      //
      if (bChecked && !aItLEIm.More())
      {
        break;
      }
    }
    else
    {
      if (aMEFound.Contains(aE) && !theMENInv.Contains(aE) && !aMEUsed.Contains(aE))
      {
        break;
      }
    }
  }
  //
  return aItLE.More();
}

//=======================================================================
// function : FindInvalidEdges
// purpose  : Looking for the invalid edges
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindInvalidEdges(
  const TopoDS_Face&                            theF,
  const NCollection_List<TopoDS_Shape>&         theLFImages,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theDMFMVE,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theDMFMNE,
  BRepOffset_DataMapOfShapeIndexedMapOfShape&   theDMFMIE,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theDMFMVIE,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                          theDMEOrLEIm,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theEdgesInvalidByVertex,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theEdgesValidByVertex,
  const Message_ProgressRange&                            theRange)
{
  // Edge is considered as invalid in the following cases:
  // 1. Its orientation on the face has changed comparing to the originals edge and face;
  // 2. The vertices of the edge have changed places comparing to the originals edge and face.
  //
  // The edges created from vertices, i.e. as intersection between two faces connected only
  // by VERTEX, will also be checked on validity. For these edges the correct orientation will
  // be defined by the edges on the original face adjacent to the connection vertex

  // original face
  const TopoDS_Face& aFOr = *(TopoDS_Face*)&myFacesOrigins->Find(theF);
  // invalid edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
  // valid edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEVal;
  // internal edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInt;
  //
  // maps for checking the inverted edges
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                aDMVE, aDMEF;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEdges;
  // back map from the original shapes to their offset images
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anImages;
  //
  Message_ProgressScope aPS(theRange, "Checking validity of edges", 2 * theLFImages.Extent());
  NCollection_List<TopoDS_Shape>::Iterator aItLF(theLFImages);
  for (; aItLF.More(); aItLF.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aE = aExp.Current();
      // keep all edges
      aMEdges.Add(aE);
      //
      // keep connection from edges to faces
      NCollection_List<TopoDS_Shape>* pLF = aDMEF.ChangeSeek(aE);
      if (!pLF)
      {
        pLF = &aDMEF(aDMEF.Add(aE, NCollection_List<TopoDS_Shape>()));
      }
      AppendToList(*pLF, aFIm);
      //
      // keep connection from vertices to edges
      TopoDS_Iterator aItV(aE);
      for (; aItV.More(); aItV.Next())
      {
        const TopoDS_Shape& aV = aItV.Value();
        //
        NCollection_List<TopoDS_Shape>* pLE = aDMVE.ChangeSeek(aV);
        if (!pLE)
        {
          pLE = &aDMVE(aDMVE.Add(aV, NCollection_List<TopoDS_Shape>()));
        }
        AppendToList(*pLE, aE);
      }

      // back map from original edges to their offset images
      const NCollection_List<TopoDS_Shape>* pLOr = myEdgesOrigins->Seek(aE);
      if (!pLOr)
        continue;
      for (NCollection_List<TopoDS_Shape>::Iterator itOr(*pLOr); itOr.More(); itOr.Next())
      {
        const TopoDS_Shape& aSOr = itOr.Value();
        TopoDS_Shape        aSInF;
        if (!FindShape(aSOr, aFOr, myAnalyzer, aSInF))
          continue;
        NCollection_List<TopoDS_Shape>* pImages = anImages.ChangeSeek(aSInF);
        if (!pImages)
          pImages = anImages.Bound(aSInF, NCollection_List<TopoDS_Shape>());
        AppendToList(*pImages, aE);
      }
    }
  }
  //
  // the map will be used to find the edges on the original face
  // adjacent to the same vertex. It will be filled at first necessity;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVEFOr;
  //
  aItLF.Initialize(theLFImages);
  for (; aItLF.More(); aItLF.Next(), aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    // valid edges for this split
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVE;
    // invalid edges for this split
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMIE;
    //
    TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Edge& aEIm = *(TopoDS_Edge*)&aExp.Current();
      //
      if (aEIm.Orientation() == TopAbs_INTERNAL)
      {
        aMEInt.Add(aEIm);
        continue;
      }
      //
      const NCollection_List<TopoDS_Shape>* pLEOr = myEdgesOrigins->Seek(aEIm);
      if (!pLEOr || pLEOr->IsEmpty())
      {
        continue;
      }
      //
      int                                      aNbVOr = 0;
      NCollection_List<TopoDS_Shape>::Iterator aItLEO(*pLEOr);
      for (; aItLEO.More(); aItLEO.Next())
      {
        if (aItLEO.Value().ShapeType() == TopAbs_VERTEX)
        {
          ++aNbVOr;
        }
      }
      if (aNbVOr > 1 && (pLEOr->Extent() - aNbVOr) > 1)
        continue;
      //
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aME, aMV, aMF;
      bool                                                   bInvalid = false, bChecked = false;
      int                                                    aNbP = NbPoints(aEIm), aNbInv = 0;
      bool                                                   bUseVertex = !aNbVOr ? false
                                                                                  : (aNbVOr == 1 && aDMEF.FindFromKey(aEIm).Extent() == 1
                                   && !myOEOrigins.IsBound(aEIm));
      //
      aItLEO.Initialize(*pLEOr);
      for (; aItLEO.More(); aItLEO.Next())
      {
        const TopoDS_Shape& aSOr    = aItLEO.Value();
        bool                bVertex = (aSOr.ShapeType() == TopAbs_VERTEX);
        //
        TopoDS_Shape aEOrF;
        if (bVertex)
        {
          // for some cases it is impossible to check the validity of the edge
          if (!bUseVertex)
          {
            continue;
          }
          // find edges on the original face adjacent to this vertex
          if (aDMVEFOr.IsEmpty())
          {
            // fill the map
            TopExp::MapShapesAndAncestors(aFOr, TopAbs_VERTEX, TopAbs_EDGE, aDMVEFOr);
          }
          //
          NCollection_List<TopoDS_Shape>* pLEFOr = aDMVEFOr.ChangeSeek(aSOr);
          if (pLEFOr)
          {
            TopoDS_Compound aCEOr;
            BRep_Builder().MakeCompound(aCEOr);
            // Avoid classification of edges originated from vertices
            // located between tangent edges
            bool                                     bAllTgt = true;
            NCollection_List<TopoDS_Shape>::Iterator aItLEFOr(*pLEFOr);
            gp_Vec aVRef = GetAverageTangent(aItLEFOr.Value(), aNbP);
            for (; aItLEFOr.More(); aItLEFOr.Next())
            {
              const TopoDS_Shape& aEOr = aItLEFOr.Value();
              BRep_Builder().Add(aCEOr, aEOr);

              gp_Vec aVCur = GetAverageTangent(aEOr, aNbP);
              if (!aVRef.IsParallel(aVCur, Precision::Angular()))
                bAllTgt = false;
            }
            if (!bAllTgt)
              aEOrF = aCEOr;
          }
        }
        else
        {
          FindShape(aSOr, aFOr, myAnalyzer, aEOrF);
          //
          NCollection_List<TopoDS_Shape>* pLEIm = theDMEOrLEIm.ChangeSeek(aSOr);
          if (!pLEIm)
          {
            pLEIm = theDMEOrLEIm.Bound(aSOr, NCollection_List<TopoDS_Shape>());
          }
          AppendToList(*pLEIm, aEIm);
        }
        //
        if (aEOrF.IsNull())
        {
          // the edge has not been found
          continue;
        }

        if (bVertex)
        {
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVTotal;
          int                                                    aNbChecked = 0;
          // Just check if the original edges sharing the vertex do not share it any more.
          for (TopoDS_Iterator it(aEOrF); it.More(); it.Next())
          {
            const TopoDS_Shape&                   aEOr = it.Value();
            const NCollection_List<TopoDS_Shape>* aLIm = anImages.Seek(aEOr);
            if (!aLIm)
              continue;
            ++aNbChecked;
            NCollection_IndexedDataMap<TopoDS_Shape,
                                       NCollection_List<TopoDS_Shape>,
                                       TopTools_ShapeMapHasher>
              aMVLoc;
            for (NCollection_List<TopoDS_Shape>::Iterator itLIM(*aLIm); itLIM.More(); itLIM.Next())
              TopExp::MapShapesAndAncestors(itLIM.Value(), TopAbs_VERTEX, TopAbs_EDGE, aMVLoc);
            for (int i = 1; i <= aMVLoc.Extent(); ++i)
            {
              if (aMVLoc(i).Extent() > 1 && !aMVTotal.Add(aMVLoc.FindKey(i)))
              {
                bInvalid = true;
                theEdgesInvalidByVertex.Add(aEIm);
                break;
              }
            }
            if (bInvalid)
              break;
          }
          if (!bInvalid && aNbChecked < 2)
            continue;
          else
            theEdgesValidByVertex.Add(aEIm);
        }
        else
        {
          //
          // Check orientations of the image edge and original edge.
          // In case the 3d curves are having the same direction the orientations
          // must be the same. Otherwise the orientations should also be different.
          //
          // get average tangent vector for each curve taking into account
          // the orientations of the edges, i.e. the edge is reversed
          // the vector is reversed as well
          gp_Vec aVSum1 = GetAverageTangent(aEIm, aNbP);
          gp_Vec aVSum2 = GetAverageTangent(aEOrF, aNbP);
          //
          aVSum1.Normalize();
          aVSum2.Normalize();
          //
          double aCos = aVSum1.Dot(aVSum2);
          if (std::abs(aCos) < 0.9999)
          {
            continue;
          }
          //
          aME.Add(aEOrF);
          TopExp_Explorer aExpE(aEOrF, TopAbs_VERTEX);
          for (; aExpE.More(); aExpE.Next())
          {
            const TopoDS_Shape& aV = aExpE.Current();
            aMV.Add(aV);
          }
          if (myAnalyzer)
          {
            for (NCollection_List<TopoDS_Shape>::Iterator itFA(myAnalyzer->Ancestors(aEOrF));
                 itFA.More();
                 itFA.Next())
              aMF.Add(itFA.Value());
          }
          //
          if (aCos < Precision::Confusion())
          {
            bInvalid = true;
            aNbInv++;
          }
        }
        bChecked = true;
      }
      //
      if (!bChecked)
      {
        continue;
      }
      //
      bool bLocalOnly = (aNbVOr > 1 && (pLEOr->Extent() - aNbVOr) > 1);
      int  aNbE = aME.Extent(), aNbV = aMV.Extent();
      if (aNbE > 1 && aNbV == 2 * aNbE)
      {
        bool bSkip = true;

        // It seems the edge originated from not connected edges and cannot be
        // considered as correctly classified as it may fill some undesired parts.
        // Still, allow the edge to be accounted for local analysis if it is:
        // * originated from more than two faces
        // * unanimously considered valid or invalid
        // * not a boundary edge in the splits
        if (aMF.Extent() > 2 && (aNbInv == 0 || aNbInv == aNbE))
        {
          if (theLFImages.Extent() > 2)
          {
            TopoDS_Iterator itV(aEIm);
            for (; itV.More(); itV.Next())
            {
              NCollection_List<TopoDS_Shape>::Iterator itE(aDMVE.FindFromKey(itV.Value()));
              for (; itE.More(); itE.Next())
                if (aDMEF.FindFromKey(itE.Value()).Extent() < 2)
                  break;
              if (itE.More())
                break;
            }
            bSkip = itV.More();
          }
        }
        if (bSkip)
          continue;
        else
          bLocalOnly = true;
      }
      //
      if (bInvalid)
      {
        if (!bLocalOnly)
          myInvalidEdges.Add(aEIm);
        aMIE.Add(aEIm);
        aMEInv.Add(aEIm);
        continue;
      }
      //
      // check if the edge has been inverted
      bool bInverted = !aNbE || bLocalOnly ? false : CheckInverted(aEIm, aFOr, aDMVE, aMEdges);
      //
      if (!bInverted || !aNbVOr)
      {
        if (!bLocalOnly)
          myValidEdges.Add(aEIm);
        aMVE.Add(aEIm);
        aMEVal.Add(aEIm);
      }
    }
    //
    // valid edges
    if (aMVE.Extent())
    {
      theDMFMVE.Bind(aFIm, aMVE);
    }
    //
    // invalid edges
    if (aMIE.Extent())
    {
      theDMFMIE.Bind(aFIm, aMIE);
    }
  }
  //
  // process invalid edges:
  // check for the inverted edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVIE;
  // fill neutral edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMNE;
  //
  int i, aNbEInv = aMEInv.Extent();
  for (i = 1; i <= aNbEInv; ++i)
  {
    const TopoDS_Shape& aEIm = aMEInv(i);
    //
    // neutral edges - on the splits of the same offset face
    // it is valid for one split and invalid for other
    if (aMEVal.Contains(aEIm))
    {
      aMNE.Add(aEIm);
      continue;
    }
    //
    // the inverted images of the origins of invalid edges should also be invalid
    if (!myInvertedEdges.Contains(aEIm))
    {
      continue;
    }
    //
    const NCollection_List<TopoDS_Shape>* pLOEOr = myOEOrigins.Seek(aEIm);
    if (!pLOEOr)
    {
      continue;
    }
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLOEOr(*pLOEOr);
    for (; aItLOEOr.More(); aItLOEOr.Next())
    {
      const TopoDS_Shape&                   aOEOr  = aItLOEOr.Value();
      const NCollection_List<TopoDS_Shape>& aLEIm1 = myOEImages.Find(aOEOr);
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLEIm1(aLEIm1);
      for (; aItLEIm1.More(); aItLEIm1.Next())
      {
        const TopoDS_Shape& aEIm1 = aItLEIm1.Value();
        if (aMEdges.Contains(aEIm1) && !aMEInv.Contains(aEIm1) && !aMEInt.Contains(aEIm1)
            && myInvertedEdges.Contains(aEIm1))
        {
          myInvalidEdges.Add(aEIm1);
          aMVIE.Add(aEIm1);
        }
      }
    }
  }
  //
  if (aMNE.Extent())
  {
    theDMFMNE.Bind(theF, aMNE);
  }
  //
  if (aMVIE.Extent())
  {
    theDMFMVIE.Bind(theF, aMVIE);
  }
}

namespace
{
//=======================================================================
// function : addAsNeutral
// purpose  : Adds as the edge into corresponding maps making it neutral
//=======================================================================
static void addAsNeutral(const TopoDS_Shape&                           theE,
                         const TopoDS_Shape&                           theFInv,
                         const TopoDS_Shape&                           theFVal,
                         BRepOffset_DataMapOfShapeIndexedMapOfShape&   theLocInvEdges,
                         NCollection_DataMap<TopoDS_Shape,
                                             NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                             TopTools_ShapeMapHasher>& theLocValidEdges)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* pMEInv =
    theLocInvEdges.ChangeSeek(theFInv);
  if (!pMEInv)
    pMEInv = theLocInvEdges.Bound(theFInv,
                                  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>());
  pMEInv->Add(theE);

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMEVal =
    theLocValidEdges.ChangeSeek(theFVal);
  if (!pMEVal)
    pMEVal =
      theLocValidEdges.Bound(theFVal, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
  pMEVal->Add(theE);
}

} // namespace

//=======================================================================
// function : FindInvalidEdges
// purpose  : Additional method to look for invalid edges
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindInvalidEdges(
  const NCollection_List<TopoDS_Shape>&         theLFOffset,
  BRepOffset_DataMapOfShapeIndexedMapOfShape&   theLocInvEdges,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theLocValidEdges,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theNeutralEdges)
{
  // 1. Find edges unclassified in faces
  // 2. Find SD faces in which the same edge is classified
  // 3. Check if the edge is neutral in face in which it wasn't classified

  NCollection_IndexedDataMap<TopoDS_Shape,
                             NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                             TopTools_ShapeMapHasher>
                                                                           aMEUnclassified;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aFSplitFOffset;

  // Avoid artificial faces
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aNewFaces;
  if (myAnalyzer)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapNewTmp;
    for (NCollection_List<TopoDS_Shape>::Iterator it(myAnalyzer->NewFaces()); it.More(); it.Next())
      aMapNewTmp.Add(it.Value());

    for (NCollection_List<TopoDS_Shape>::Iterator it(theLFOffset); it.More(); it.Next())
    {
      const TopoDS_Shape& aFOffset = it.Value();
      const TopoDS_Shape& aFOrigin = myFacesOrigins->Find(aFOffset);
      if (aMapNewTmp.Contains(aFOrigin))
        aNewFaces.Add(aFOffset);
    }
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEFMap;
  for (NCollection_List<TopoDS_Shape>::Iterator itLFO(theLFOffset); itLFO.More(); itLFO.Next())
  {
    const TopoDS_Shape& aF = itLFO.Value();
    if (aNewFaces.Contains(aF))
      continue;

    const NCollection_List<TopoDS_Shape>& aLFImages = myOFImages.FindFromKey(aF);
    for (NCollection_List<TopoDS_Shape>::Iterator itLF(aLFImages); itLF.More(); itLF.Next())
    {
      const TopoDS_Shape& aFIm = itLF.Value();

      TopExp::MapShapesAndAncestors(aFIm, TopAbs_EDGE, TopAbs_FACE, anEFMap);

      const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* pMEInvalid =
        theLocInvEdges.Seek(aFIm);
      const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMEValid =
        theLocValidEdges.Seek(aFIm);

      for (TopExp_Explorer expE(aFIm, TopAbs_EDGE); expE.More(); expE.Next())
      {
        const TopoDS_Shape& aE = expE.Current();
        if (myInvalidEdges.Contains(aE) != myValidEdges.Contains(aE))
        {
          // edge is classified in some face

          if ((!pMEInvalid || !pMEInvalid->Contains(aE)) && (!pMEValid || !pMEValid->Contains(aE)))
          {
            // but not in the current one
            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMap =
              aMEUnclassified.ChangeSeek(aE);
            if (!pMap)
              pMap = &aMEUnclassified(
                aMEUnclassified.Add(aE, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>()));
            pMap->Add(aFIm);

            aFSplitFOffset.Bind(aFIm, aF);
          }
        }
      }
    }
  }

  if (aMEUnclassified.IsEmpty())
    return;

  // Analyze unclassified edges
  const int aNbE = aMEUnclassified.Extent();
  for (int iE = 1; iE <= aNbE; ++iE)
  {
    const TopoDS_Shape&                                           aE = aMEUnclassified.FindKey(iE);
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMFUnclassified =
      aMEUnclassified(iE);

    const NCollection_List<TopoDS_Shape>& aLF = anEFMap.FindFromKey(aE);

    for (NCollection_List<TopoDS_Shape>::Iterator itLF(aLF); itLF.More(); itLF.Next())
    {
      const TopoDS_Shape& aFClassified = itLF.Value();
      if (aMFUnclassified.Contains(aFClassified))
        continue;

      BOPTools_Set anEdgeSetClass;
      anEdgeSetClass.Add(aFClassified, TopAbs_EDGE);

      TopoDS_Shape aEClassified;
      FindShape(aE, aFClassified, nullptr, aEClassified);
      TopAbs_Orientation anOriClass = aEClassified.Orientation();

      gp_Dir aDNClass;
      BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(TopoDS::Edge(aEClassified),
                                                  TopoDS::Face(aFClassified),
                                                  aDNClass);

      const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* pMEInvalid =
        theLocInvEdges.Seek(aFClassified);
      bool isInvalid = pMEInvalid && pMEInvalid->Contains(aE);

      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itM(aMFUnclassified);
           itM.More();
           itM.Next())
      {
        const TopoDS_Shape& aFUnclassified = itM.Value();

        BOPTools_Set anEdgeSetUnclass;
        anEdgeSetUnclass.Add(aFUnclassified, TopAbs_EDGE);

        if (anEdgeSetClass.IsEqual(anEdgeSetUnclass))
        {
          gp_Dir aDNUnclass;
          BOPTools_AlgoTools3D::GetNormalToFaceOnEdge(TopoDS::Edge(aE),
                                                      TopoDS::Face(aFUnclassified),
                                                      aDNUnclass);

          bool isSameOri = aDNClass.IsEqual(aDNUnclass, Precision::Angular());

          // Among other splits of the same face find those where the edge is contained with
          // different orientation
          const TopoDS_Shape&                      aFOffset  = aFSplitFOffset.Find(aFUnclassified);
          const NCollection_List<TopoDS_Shape>&    aLFSplits = myOFImages.FindFromKey(aFOffset);
          NCollection_List<TopoDS_Shape>::Iterator itLFSp(aLFSplits);
          for (; itLFSp.More(); itLFSp.Next())
          {
            const TopoDS_Shape& aFSp = itLFSp.Value();

            if (!aFSp.IsSame(aFUnclassified) && aMFUnclassified.Contains(aFSp))
            {
              TopoDS_Shape aEUnclassified;
              FindShape(aE, aFSp, nullptr, aEUnclassified);

              TopAbs_Orientation anOriUnclass = aEUnclassified.Orientation();
              if (!isSameOri)
                anOriUnclass = TopAbs::Reverse(anOriUnclass);

              if (anOriClass != anOriUnclass)
              {
                // make the edge neutral for the face
                NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMENeutral =
                  theNeutralEdges.ChangeSeek(aFOffset);
                if (!pMENeutral)
                  pMENeutral =
                    theNeutralEdges.Bound(aFOffset,
                                          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
                pMENeutral->Add(aE);

                if (isInvalid && isSameOri)
                {
                  // make edge invalid in aFUnclassified and valid in aFSp
                  addAsNeutral(aE, aFClassified, aFSp, theLocInvEdges, theLocValidEdges);
                }
                else
                {
                  // make edge invalid in aFSp and valid in aFUnclassified
                  addAsNeutral(aE, aFSp, aFClassified, theLocInvEdges, theLocValidEdges);
                }
              }
            }
          }
          if (itLFSp.More())
            break;
        }
      }
    }
  }
}

//=======================================================================
// function : MakeInvertedEdgesInvalid
// purpose  : Makes inverted edges located inside loop of invalid edges, invalid as well
//=======================================================================
void BRepOffset_BuildOffsetFaces::MakeInvertedEdgesInvalid(
  const NCollection_List<TopoDS_Shape>& theLFOffset)
{
  if (myInvalidEdges.IsEmpty() || myInvertedEdges.IsEmpty())
    return;

  // Map all invalid edges
  TopoDS_Compound aCBEInv;
  BRep_Builder().MakeCompound(aCBEInv);
  for (int i = 1; i <= myInvalidEdges.Extent(); ++i)
  {
    BRep_Builder().Add(aCBEInv, myInvalidEdges(i));
  }

  // Make loops of invalid edges
  NCollection_List<TopoDS_Shape> aLCB;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCBEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCB);

  // Analyze each loop on closeness and use only closed ones
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDMVCB;

  for (NCollection_List<TopoDS_Shape>::Iterator itLCB(aLCB); itLCB.More(); itLCB.Next())
  {
    const TopoDS_Shape& aCB = itLCB.Value();

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aDMVE;
    TopExp::MapShapesAndAncestors(aCB, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
    bool isClosed = true;
    for (int iV = 1; iV <= aDMVE.Extent(); ++iV)
    {
      if (aDMVE(iV).Extent() != 2)
      {
        isClosed = false;
        break;
      }
    }
    if (!isClosed)
      continue;

    // Bind loop to each vertex of the loop
    for (int iV = 1; iV <= aDMVE.Extent(); ++iV)
    {
      aDMVCB.Bind(aDMVE.FindKey(iV), aCB);
    }
  }

  // Check if any inverted edges of offset faces are locked inside the loops of invalid edges.
  // Make such edges invalid as well.
  for (NCollection_List<TopoDS_Shape>::Iterator itLF(theLFOffset); itLF.More(); itLF.Next())
  {
    const NCollection_List<TopoDS_Shape>& aLFIm = myOFImages.FindFromKey(itLF.Value());
    for (NCollection_List<TopoDS_Shape>::Iterator itLFIm(aLFIm); itLFIm.More(); itLFIm.Next())
    {
      for (TopExp_Explorer expE(itLFIm.Value(), TopAbs_EDGE); expE.More(); expE.Next())
      {
        const TopoDS_Edge& aE = TopoDS::Edge(expE.Current());
        if (!myInvalidEdges.Contains(aE) && myInvertedEdges.Contains(aE))
        {
          const TopoDS_Shape* pCB1 = aDMVCB.Seek(TopExp::FirstVertex(aE));
          const TopoDS_Shape* pCB2 = aDMVCB.Seek(TopExp::LastVertex(aE));
          if (pCB1 && pCB2 && pCB1->IsSame(*pCB2))
          {
            myInvalidEdges.Add(aE);
          }
        }
      }
    }
  }
}

//=======================================================================
// function : FindInvalidFaces
// purpose  : Looking for the invalid faces by analyzing their invalid edges
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindInvalidFaces(
  NCollection_List<TopoDS_Shape>&                                theLFImages,
  const NCollection_DataMap<TopoDS_Shape,
                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                            TopTools_ShapeMapHasher>&            theDMFMVE,
  const BRepOffset_DataMapOfShapeIndexedMapOfShape&              theDMFMIE,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theMENeutral,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theEdgesInvalidByVertex,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theEdgesValidByVertex,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theMFHoles,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInvInHole,
  NCollection_List<TopoDS_Shape>&                                theInvFaces,
  NCollection_List<TopoDS_Shape>&                                theInvertedFaces)
{
  // The face should be considered as invalid in the following cases:
  // 1. It has been reverted, i.e. at least two not connected edges
  //    have changed orientation (i.e. invalid). In this case all edges,
  //    should be invalid for that face, because edges have also been reverted;
  // 2. All checked edges of the face are invalid for this face;
  // The face should be removed from the splits in the following cases:
  // 1. All checked edges of the face are invalid for this one, but valid for
  //    some other face in this list of splits.
  // The face will be kept in the following cases:
  // 1. Some of the edges are valid for this face.
  bool bHasValid, bAllValid, bAllInvalid, bHasReallyInvalid, bAllInvNeutral;
  bool bValid, bValidLoc, bInvalid, bInvalidLoc, bNeutral, bInverted;
  bool bIsInvalidByInverted, bHasInverted;
  int  aNbChecked;
  //
  bool bTreatInvertedAsInvalid = (theLFImages.Extent() == 1);
  //
  // neutral edges to remove
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMENRem;
  //
  // faces for post treat
  NCollection_List<TopoDS_Shape> aLFPT;
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                           aDMEF;
  NCollection_List<TopoDS_Shape>::Iterator aItLF(theLFImages);
  for (; aItLF.More(); aItLF.Next())
  {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    TopExp::MapShapesAndAncestors(aFIm, TopAbs_EDGE, TopAbs_FACE, aDMEF);
  }

  aItLF.Initialize(theLFImages);
  for (; aItLF.More();)
  {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    // valid edges for this split
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMVE = theDMFMVE.Seek(aFIm);
    // invalid edges for this split
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* pMIE =
      theDMFMIE.Seek(aFIm);
    //
    bHasValid            = false;
    bAllValid            = true;
    bAllInvalid          = true;
    bHasReallyInvalid    = false;
    bAllInvNeutral       = true;
    bIsInvalidByInverted = true;
    bHasInverted         = false;
    aNbChecked           = 0;
    //
    const TopoDS_Wire& aWIm = BRepTools::OuterWire(aFIm);
    TopExp_Explorer    aExp(aWIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      bValid   = myValidEdges.Contains(aEIm);
      bInvalid = myInvalidEdges.Contains(aEIm);
      bNeutral = theMENeutral.Contains(aEIm);
      //
      if (!bValid && !bInvalid && !bNeutral)
      {
        // edge has not been checked for some reason
        continue;
      }

      // skip not-boundary edges originated from vertex
      if ((theEdgesInvalidByVertex.Contains(aEIm) || theEdgesValidByVertex.Contains(aEIm))
          && aDMEF.FindFromKey(aEIm).Extent() != 1)
        continue;

      ++aNbChecked;
      //
      bInvalidLoc = pMIE && pMIE->Contains(aEIm);
      bHasReallyInvalid =
        bInvalid && bInvalidLoc && !bValid && !theEdgesInvalidByVertex.Contains(aEIm);
      if (bHasReallyInvalid)
      {
        break;
      }
      //
      bValidLoc = pMVE && pMVE->Contains(aEIm);
      bInverted = myInvertedEdges.Contains(aEIm);
      if (!bInvalid && !bInvalidLoc && bTreatInvertedAsInvalid)
      {
        bInvalid = bInverted;
      }
      //
      if (bValidLoc && bNeutral)
      {
        bHasValid = true;
      }
      //
      bAllValid &= bValidLoc;
      bAllInvalid &= (bInvalid || bInvalidLoc);
      bAllInvNeutral &= (bAllInvalid && bNeutral);
      bIsInvalidByInverted &= (bInvalidLoc || bInverted);
      bHasInverted |= bInverted;
    }
    //
    if (!aNbChecked)
    {
      aItLF.Next();
      continue;
    }
    //
    if (!bHasReallyInvalid && (bAllInvNeutral && !bHasValid) && (aNbChecked > 1))
    {
      if (bHasInverted)
      {
        // The part seems to be filled due to overlapping of parts rather than
        // due to multi-connection of faces. No need to remove the part.
        aItLF.Next();
        continue;
      }
      // remove edges from neutral
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMENRem);
      // remove face
      theLFImages.Remove(aItLF);
      continue;
    }
    //
    if (bHasReallyInvalid
        || (bAllInvalid && !(bHasValid || bAllValid) && (!bAllInvNeutral || (aNbChecked != 1))))
    {
      theInvFaces.Append(aFIm);
      if (theMFHoles.Contains(aFIm))
      {
        theMFInvInHole.Add(aFIm);
      }
      aItLF.Next();
      continue;
    }
    //
    if (theMFHoles.Contains(aFIm))
    {
      // remove edges from neutral
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMENRem);
      // remove face
      theLFImages.Remove(aItLF);
      continue;
    }
    //
    if (bIsInvalidByInverted && !(bHasValid || bAllValid))
    {
      // The face contains only the inverted and locally invalid edges
      theInvertedFaces.Append(aFIm);
    }

    if (!bAllInvNeutral)
    {
      aLFPT.Append(aFIm);
    }
    else
    {
      // remove edges from neutral
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMENRem);
    }
    aItLF.Next();
  }
  //
  if (aLFPT.IsEmpty() || aMENRem.IsEmpty())
  {
    return;
  }

  // check the splits once more
  aItLF.Initialize(aLFPT);
  for (; aItLF.More(); aItLF.Next())
  {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    // valid edges for this split
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMVE = theDMFMVE.Seek(aFIm);
    //
    bHasValid   = false;
    bAllValid   = true;
    bAllInvalid = true;
    //
    const TopoDS_Wire& aWIm = BRepTools::OuterWire(aFIm);
    TopExp_Explorer    aExp(aWIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      bValid    = myValidEdges.Contains(aEIm);
      bInvalid  = myInvalidEdges.Contains(aEIm);
      bNeutral  = theMENeutral.Contains(aEIm) && !aMENRem.Contains(aEIm);
      bValidLoc = pMVE && pMVE->Contains(aEIm);
      //
      if (!bInvalid && bTreatInvertedAsInvalid)
      {
        bInvalid = myInvertedEdges.Contains(aEIm);
      }
      //
      if (bValidLoc && bNeutral)
      {
        bHasValid = true;
      }
      //
      bAllValid   = bAllValid && bValidLoc;
      bAllInvalid = bAllInvalid && bInvalid;
    }
    //
    if (bAllInvalid && !bHasValid && !bAllValid)
    {
      theInvFaces.Append(aFIm);
    }
  }
}

//=======================================================================
// function : FindFacesInsideHoleWires
// purpose  : Find faces inside holes wires from the original face
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindFacesInsideHoleWires(
  const TopoDS_Face&                    theFOrigin,
  const TopoDS_Face&                    theFOffset,
  const NCollection_List<TopoDS_Shape>& theLFImages,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                             theDMEOrLEIm,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& theEFMap,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&    theMFHoles)
{
  if (theLFImages.IsEmpty())
  {
    return;
  }
  //
  // find all hole wires in the original face
  NCollection_List<TopoDS_Shape> aLHoleWires;
  const TopoDS_Wire&             anOuterWire = BRepTools::OuterWire(theFOrigin);
  TopExp_Explorer                aExpW(theFOrigin, TopAbs_WIRE);
  for (; aExpW.More(); aExpW.Next())
  {
    const TopoDS_Wire& aHoleWire = TopoDS::Wire(aExpW.Current());
    if (!aHoleWire.IsSame(anOuterWire) && aHoleWire.Orientation() != TopAbs_INTERNAL)
    {
      aLHoleWires.Append(aHoleWire);
    }
  }
  //
  if (aLHoleWires.IsEmpty())
  {
    // no holes in the face
    return;
  }
  //
  NCollection_List<TopoDS_Shape>* pLFNewHoles = myFNewHoles.ChangeSeek(theFOrigin);
  //
  if (!pLFNewHoles)
  {
    pLFNewHoles = myFNewHoles.Bound(theFOrigin, NCollection_List<TopoDS_Shape>());
  }
  if (pLFNewHoles->IsEmpty())
  {
    //
    // find the faces representing holes in the images of the faces:
    // 1. for each original hole wire try to build its image
    // 2. build the new planar face from the images
    //
    // map vertices and edges of the splits
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMESplits;
    NCollection_List<TopoDS_Shape>::Iterator                      aItLF(theLFImages);
    for (; aItLF.More(); aItLF.Next())
    {
      TopExp::MapShapes(aItLF.Value(), TopAbs_EDGE, aMESplits);
    }
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLW(aLHoleWires);
    for (; aItLW.More(); aItLW.Next())
    {
      const TopoDS_Wire& aHoleWire = TopoDS::Wire(aItLW.Value());
      // find images of all edges of the original wire
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEImWire;
      TopoDS_Iterator                                               aItE(aHoleWire);
      for (; aItE.More(); aItE.Next())
      {
        const TopoDS_Shape&                   aEOr  = aItE.Value();
        const NCollection_List<TopoDS_Shape>* pLEIm = theDMEOrLEIm.Seek(aEOr);
        if (!pLEIm || pLEIm->IsEmpty())
        {
          continue;
        }
        NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
        for (; aItLEIm.More(); aItLEIm.Next())
        {
          const TopoDS_Shape& aEIm = aItLEIm.Value();
          if (aMESplits.Contains(aEIm))
          {
            aMEImWire.Add(aEIm);
          }
        }
      }
      //
      if (aMEImWire.IsEmpty())
      {
        continue;
      }
      //
      // build new planar face using these edges
      NCollection_List<TopoDS_Shape> aLE;
      int                            i, aNbE = aMEImWire.Extent();
      for (i = 1; i <= aNbE; ++i)
      {
        aLE.Append(aMEImWire(i).Oriented(TopAbs_FORWARD));
        aLE.Append(aMEImWire(i).Oriented(TopAbs_REVERSED));
      }
      //
      BOPAlgo_BuilderFace aBF;
      aBF.SetFace(TopoDS::Face(theFOffset.Oriented(TopAbs_FORWARD)));
      aBF.SetShapes(aLE);
      aBF.Perform();
      //
      const NCollection_List<TopoDS_Shape>& aLFNew = aBF.Areas();
      if (aLFNew.IsEmpty())
      {
        continue;
      }
      //
      // check if outer edges in the new faces are not inverted
      // because the inverted edges mean that the hole has been
      // filled during offset and there will be no faces to remove
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
                                               aDMEFNew;
      NCollection_List<TopoDS_Shape>::Iterator aItLFNew(aLFNew);
      for (; aItLFNew.More(); aItLFNew.Next())
      {
        TopExp::MapShapesAndAncestors(aItLFNew.Value(), TopAbs_EDGE, TopAbs_FACE, aDMEFNew);
      }
      //
      aNbE = aDMEFNew.Extent();
      for (i = 1; i <= aNbE; ++i)
      {
        if (aDMEFNew(i).Extent() == 1)
        {
          const TopoDS_Shape& aE = aDMEFNew.FindKey(i);
          if (myInvertedEdges.Contains(aE))
          {
            break;
          }
        }
      }
      //
      if (i <= aNbE)
      {
        continue;
      }
      //
      aItLFNew.Initialize(aLFNew);
      for (; aItLFNew.More(); aItLFNew.Next())
      {
        pLFNewHoles->Append(aItLFNew.Value());
      }
    }
  }

  // Build Edge-Face map for splits of current offset face
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEFSplitsMap;
  // Build Edge-Face map for holes
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEFHolesMap;

  // among the splits of the offset face find those that are
  // located inside the hole faces
  NCollection_List<TopoDS_Shape>::Iterator aItLF(theLFImages);
  for (; aItLF.More(); aItLF.Next())
  {
    const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
    TopExp::MapShapesAndAncestors(aFIm, TopAbs_EDGE, TopAbs_FACE, anEFSplitsMap);
    // get the point inside the face and classify it relatively hole faces
    gp_Pnt   aP3D;
    gp_Pnt2d aP2D;
    int      iErr = BOPTools_AlgoTools3D::PointInFace(aFIm, aP3D, aP2D, myContext);
    if (iErr)
    {
      continue;
    }
    //
    double aTol = BRep_Tool::Tolerance(aFIm);
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLFNew(*pLFNewHoles);
    for (; aItLFNew.More(); aItLFNew.Next())
    {
      const TopoDS_Face& aFNew = TopoDS::Face(aItLFNew.Value());
      if (myContext->IsValidPointForFace(aP3D, aFNew, aTol))
      {
        // the face is classified as IN
        theMFHoles.Add(aFIm);
        TopExp::MapShapesAndAncestors(aFIm, TopAbs_EDGE, TopAbs_FACE, anEFHolesMap);
        break;
      }
    }
  }

  // Out of all found holes find those which cannot be removed
  // by checking their connectivity to splits of other offset faces.
  // These are the faces, which will create uncovered holes if removed.
  const int aNbE = anEFHolesMap.Extent();
  for (int i = 1; i <= aNbE; ++i)
  {
    const TopoDS_Shape&                   anEdge   = anEFHolesMap.FindKey(i);
    const NCollection_List<TopoDS_Shape>& aLFHoles = anEFHolesMap(i);
    // Check if the edge is outer for holes
    if (aLFHoles.Extent() != 1)
      continue;

    const TopoDS_Shape& aFHole = aLFHoles.First();
    if (!theMFHoles.Contains(aFHole))
      // Already removed
      continue;

    // Check if the edge is not outer for splits
    const NCollection_List<TopoDS_Shape>& aLSplits = anEFSplitsMap.FindFromKey(anEdge);
    if (aLSplits.Extent() == 1)
      continue;

    // Check if edge is only connected to splits of the current offset face
    const NCollection_List<TopoDS_Shape>& aLFAll = theEFMap.FindFromKey(anEdge);
    if (aLFAll.Extent() == 2)
      // Avoid removal of the hole from the splits
      theMFHoles.Remove(aFHole);
  }
}

//=======================================================================
// function : CheckInverted
// purpose  : Checks if the edge has been inverted
//=======================================================================
bool BRepOffset_BuildOffsetFaces::CheckInverted(
  const TopoDS_Edge&                                                   theEIm,
  const TopoDS_Face&                                                   theFOr,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&           theDMVE,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEdges)
{
  // It is necessary to compare the direction from first vertex
  // to the last vertex on the original edge with the
  // same direction on the new edge. If the directions
  // will be different - the edge has been inverted.
  //
  TopoDS_Vertex aVI1, aVI2; // vertices on the offset edge
  TopoDS_Vertex aVO1, aVO2; // vertices on the original edge
  //
  int i;
  // find vertices of the offset shape
  TopExp::Vertices(theEIm, aVI1, aVI2);
  //
  // find images
  NCollection_List<TopoDS_Shape> aLEImages;
  if (myOEOrigins.IsBound(theEIm))
  {
    TopoDS_Wire anImages;
    BRep_Builder().MakeWire(anImages);
    //
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMImFence;
    const NCollection_List<TopoDS_Shape>&                  aLOffsetOr = myOEOrigins.Find(theEIm);
    NCollection_List<TopoDS_Shape>::Iterator               aItOffset(aLOffsetOr);
    for (; aItOffset.More(); aItOffset.Next())
    {
      const TopoDS_Shape&                   aEOffsetOr = aItOffset.Value();
      const NCollection_List<TopoDS_Shape>& aLImages   = myOEImages.Find(aEOffsetOr);
      //
      NCollection_List<TopoDS_Shape>::Iterator aItImages(aLImages);
      for (; aItImages.More(); aItImages.Next())
      {
        const TopoDS_Edge& anIm = *(TopoDS_Edge*)&aItImages.Value();
        if (theMEdges.Contains(anIm) && aMImFence.Add(anIm))
        {
          BRep_Builder().Add(anImages, anIm);
          aLEImages.Append(anIm);
        }
      }
    }
    //
    // find alone vertices
    TopoDS_Vertex aVW1, aVW2;
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aDMImVE;
    TopExp::MapShapesAndAncestors(anImages, TopAbs_VERTEX, TopAbs_EDGE, aDMImVE);
    //
    NCollection_List<TopoDS_Shape> aLVAlone;
    int                            aNb = aDMImVE.Extent();
    for (i = 1; i <= aNb; ++i)
    {
      const NCollection_List<TopoDS_Shape>& aLImE = aDMImVE(i);
      if (aLImE.Extent() == 1)
      {
        aLVAlone.Append(aDMImVE.FindKey(i));
      }
    }
    //
    if (aLVAlone.Extent() > 1)
    {
      aVW1 = *(TopoDS_Vertex*)&aLVAlone.First();
      aVW2 = *(TopoDS_Vertex*)&aLVAlone.Last();
      //
      // check distances
      const gp_Pnt& aPI1 = BRep_Tool::Pnt(aVI1);
      const gp_Pnt& aPW1 = BRep_Tool::Pnt(aVW1);
      const gp_Pnt& aPW2 = BRep_Tool::Pnt(aVW2);
      //
      double aDist1 = aPI1.SquareDistance(aPW1);
      double aDist2 = aPI1.SquareDistance(aPW2);
      //
      if (aDist1 < aDist2)
      {
        aVI1 = aVW1;
        aVI2 = aVW2;
      }
      else
      {
        aVI1 = aVW2;
        aVI2 = aVW1;
      }
    }
  }
  else
  {
    aLEImages.Append(theEIm);
  }
  //
  // Find edges connected to these vertices
  const NCollection_List<TopoDS_Shape>& aLIE1 = theDMVE.FindFromKey(aVI1);
  const NCollection_List<TopoDS_Shape>& aLIE2 = theDMVE.FindFromKey(aVI2);
  //
  // Find vertices on the original face corresponding to vertices on the offset edge
  //
  // find original edges for both lists
  NCollection_List<TopoDS_Shape> aLOE1, aLOE2;
  for (i = 0; i < 2; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLIE = !i ? aLIE1 : aLIE2;
    NCollection_List<TopoDS_Shape>&       aLOE = !i ? aLOE1 : aLOE2;
    //
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLIE(aLIE);
    for (; aItLIE.More(); aItLIE.Next())
    {
      const TopoDS_Shape& aEI = aItLIE.Value();
      if (myEdgesOrigins->IsBound(aEI))
      {
        const NCollection_List<TopoDS_Shape>& aLEOrigins = myEdgesOrigins->Find(aEI);
        //
        NCollection_List<TopoDS_Shape>::Iterator aItLOE(aLEOrigins);
        for (; aItLOE.More(); aItLOE.Next())
        {
          const TopoDS_Shape& aEO = aItLOE.Value();
          if (aEO.ShapeType() == TopAbs_EDGE && aMFence.Add(aEO))
          {
            TopoDS_Shape aEOin;
            if (FindShape(aEO, theFOr, nullptr, aEOin))
            {
              AppendToList(aLOE, aEO);
            }
          }
        }
      }
    }
  }
  //
  if (aLOE1.Extent() < 2 || aLOE2.Extent() < 2)
  {
    return false;
  }
  //
  // find vertices common for the max number of edges in the lists
  for (i = 0; i < 2; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLOE = !i ? aLOE1 : aLOE2;
    TopoDS_Vertex&                        aVO  = !i ? aVO1 : aVO2;

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aDMVELoc;
    for (NCollection_List<TopoDS_Shape>::Iterator itLOE(aLOE); itLOE.More(); itLOE.Next())
    {
      TopExp::MapShapesAndAncestors(itLOE.Value(), TopAbs_VERTEX, TopAbs_EDGE, aDMVELoc);
    }

    int aNbEMax = 0;
    for (int j = 1; j <= aDMVELoc.Extent(); ++j)
    {
      int aNbE = aDMVELoc(j).Extent();
      if (aNbE > 1 && aNbE > aNbEMax)
      {
        aVO     = TopoDS::Vertex(aDMVELoc.FindKey(j));
        aNbEMax = aNbE;
      }
    }
    if (aVO.IsNull())
    {
      return false;
    }
  }

  if (aVO1.IsSame(aVO2))
  {
    return false;
  }
  //
  // check positions of the offset and original vertices
  const gp_Pnt& aPI1 = BRep_Tool::Pnt(aVI1);
  const gp_Pnt& aPI2 = BRep_Tool::Pnt(aVI2);
  const gp_Pnt& aPO1 = BRep_Tool::Pnt(aVO1);
  const gp_Pnt& aPO2 = BRep_Tool::Pnt(aVO2);
  //
  gp_Vec aVI(aPI1, aPI2);
  gp_Vec aVO(aPO1, aPO2);
  //
  double anAngle   = aVI.Angle(aVO);
  bool   bInverted = std::abs(anAngle - M_PI) < 1.e-4;
  if (bInverted)
  {
    NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEImages);
    for (; aItLEIm.More(); aItLEIm.Next())
    {
      const TopoDS_Shape& aEInvr = aItLEIm.Value();
      myInvertedEdges.Add(aEInvr);
    }
  }
  return bInverted;
}

namespace
{
//=======================================================================
// function : GetVerticesOnEdges
// purpose  : Get vertices from the given shape belonging to the given edges
//=======================================================================
static void GetVerticesOnEdges(
  const TopoDS_Shape&                                                  theCB,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theEdges,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theVerticesOnEdges,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theAllVertices)
{
  TopExp_Explorer aExp(theCB, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aE             = aExp.Current();
    bool                isOnGivenEdges = theEdges.Contains(aE);
    for (TopoDS_Iterator aItV(aE); aItV.More(); aItV.Next())
    {
      theAllVertices.Add(aItV.Value());
      if (isOnGivenEdges)
      {
        theVerticesOnEdges.Add(aItV.Value());
      }
    }
  }
}
} // namespace

//=======================================================================
// function : CheckInvertedBlock
// purpose  : Checks if it is possible to remove the block containing inverted edges
//=======================================================================
bool BRepOffset_BuildOffsetFaces::CheckInvertedBlock(
  const TopoDS_Shape&                           theCB,
  const NCollection_List<TopoDS_Shape>&         theLCBF,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theDMCBVInverted,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>& theDMCBVAll)
{
  // For possible removal of the block:
  // 1. There should be more than just one face in the block
  if (theCB.NbChildren() < 2)
  {
    return false;
  }
  //
  // 2. The block should at least contain two connected inverted edges with
  //    different origins (not just two images/splits of the same edge)
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMECBInv;
  TopoDS_Compound                                        aCECBInv;
  BRep_Builder().MakeCompound(aCECBInv);
  //
  TopExp_Explorer aExp(theCB, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aE = aExp.Current();
    if (myInvertedEdges.Contains(aE))
    {
      if (aMECBInv.Add(aE))
      {
        BRep_Builder().Add(aCECBInv, aE);
      }
    }
  }
  //
  if (aMECBInv.Extent() < 2)
  {
    return false;
  }
  //
  // check that the edges are connected and different
  NCollection_List<TopoDS_Shape> aLCBE;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCECBInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
  //
  NCollection_List<TopoDS_Shape>::Iterator aItLCBE(aLCBE);
  for (; aItLCBE.More(); aItLCBE.Next())
  {
    const TopoDS_Shape& aCBE = aItLCBE.Value();
    // count the unique edges in the block
    int                                                    aNbUnique = 0;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEOrigins;
    TopoDS_Iterator                                        aItE(aCBE);
    for (; aItE.More(); aItE.Next())
    {
      const TopoDS_Shape&                   aE    = aItE.Value();
      const NCollection_List<TopoDS_Shape>* pLEOr = myOEOrigins.Seek(aE);
      if (!pLEOr)
      {
        aMEOrigins.Add(aE);
        ++aNbUnique;
        continue;
      }
      NCollection_List<TopoDS_Shape>::Iterator aItLEOr(*pLEOr);
      for (; aItLEOr.More(); aItLEOr.Next())
      {
        const TopoDS_Shape& aEOr = aItLEOr.Value();
        if (aMEOrigins.Add(aEOr))
        {
          ++aNbUnique;
        }
      }
    }
    //
    if (aNbUnique >= 2)
    {
      break;
    }
  }
  //
  if (!aItLCBE.More())
  {
    return false;
  }
  //
  // 3. the block should not contain inverted edges which vertices
  //    are contained in other blocks
  //
  // collect vertices from inverted edges and compare them with
  // vertices from other blocks
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMVInverted =
    theDMCBVInverted.ChangeSeek(theCB);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMVAll = theDMCBVAll.ChangeSeek(theCB);
  if (!pMVInverted)
  {
    pMVInverted =
      theDMCBVInverted.Bound(theCB, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
    pMVAll = theDMCBVAll.Bound(theCB, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
    //
    GetVerticesOnEdges(theCB, myInvertedEdges, *pMVInverted, *pMVAll);
  }
  //
  NCollection_List<TopoDS_Shape>::Iterator aItLCB1(theLCBF);
  for (; aItLCB1.More(); aItLCB1.Next())
  {
    const TopoDS_Shape& aCB1 = aItLCB1.Value();
    if (aCB1.IsSame(theCB))
    {
      continue;
    }
    //
    // collect vertices from inverted edges
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMVInverted1 =
      theDMCBVInverted.ChangeSeek(aCB1);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pMVAll1 = theDMCBVAll.ChangeSeek(aCB1);
    if (!pMVInverted1)
    {
      pMVInverted1 =
        theDMCBVInverted.Bound(aCB1, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
      pMVAll1 = theDMCBVAll.Bound(aCB1, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
      //
      GetVerticesOnEdges(aCB1, myInvertedEdges, *pMVInverted1, *pMVAll1);
    }
    //
    if (NCollection_MapAlgo::HasIntersection(*pMVInverted, *pMVAll1))
    {
      return false;
    }
  }
  //
  return true;
}

//=======================================================================
// function : RemoveInvalidSplitsByInvertedEdges
// purpose  : Looking for the invalid faces containing inverted edges
//           that can be safely removed
//=======================================================================
void BRepOffset_BuildOffsetFaces::RemoveInvalidSplitsByInvertedEdges(
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved)
{
  if (myInvertedEdges.IsEmpty())
  {
    return;
  }
  //
  // check the faces on regularity, i.e. the splits of the same face
  // should not be connected only by vertex. Such irregular splits
  // will have to be rebuilt and cannot be removed.
  //
  BRep_Builder                                                  aBB;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEAvoid;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aDMVF;
  int aNb = myOFImages.Extent(), i;
  for (i = 1; i <= aNb; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLFIm = myOFImages(i);
    //
    TopoDS_Compound aCFIm;
    aBB.MakeCompound(aCFIm);
    //
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                             aDMEF;
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLFIm);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aF = aIt.Value();
      aBB.Add(aCFIm, aF);
      //
      // make a map to use only outer edges
      TopExp_Explorer aExp(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        //
        NCollection_List<TopoDS_Shape>* pLF = aDMEF.ChangeSeek(aE);
        if (!pLF)
        {
          pLF = aDMEF.Bound(aE, NCollection_List<TopoDS_Shape>());
        }
        else
        {
          // internal edges should not be used
          aMEAvoid.Add(aE);
        }
        AppendToList(*pLF, aF);
      }
      //
      // fill connection map of the vertices of inverted edges to faces
      aExp.Init(aF, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aV = aExp.Current();
        //
        NCollection_List<TopoDS_Shape>* pLF = aDMVF.ChangeSeek(aV);
        if (!pLF)
        {
          pLF = aDMVF.Bound(aV, NCollection_List<TopoDS_Shape>());
        }
        AppendToList(*pLF, aF);
      }
    }
    //
    // for the splits to be regular they should form only one block
    NCollection_List<TopoDS_Shape> aLCBF;
    BOPTools_AlgoTools::MakeConnexityBlocks(aCFIm, TopAbs_EDGE, TopAbs_FACE, aLCBF);
    if (aLCBF.Extent() == 1)
    {
      continue;
    }
    //
    // check if the inverted edges create the irregularity
    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>
      aDMCBVInverted, aDMCBVAll;
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLCB(aLCBF);
    for (; aItLCB.More(); aItLCB.Next())
    {
      const TopoDS_Shape& aCB = aItLCB.Value();
      //
      // check if it is possible to remove the block
      if (!CheckInvertedBlock(aCB, aLCBF, aDMCBVInverted, aDMCBVAll))
      {
        // non of the edges in this block should be removed
        TopExp::MapShapes(aCB, TopAbs_EDGE, aMEAvoid);
        continue;
      }
    }
  }
  //
  // all edges not included in aMEAvoid can be removed
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMERem;
  for (int iInverted = 1; iInverted <= myInvertedEdges.Extent(); ++iInverted)
  {
    const TopoDS_Shape& aE = myInvertedEdges(iInverted);
    if (!aMEAvoid.Contains(aE))
    {
      TopoDS_Iterator aIt(aE);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape&                   aV  = aIt.Value();
        const NCollection_List<TopoDS_Shape>* pLF = aDMVF.Seek(aV);
        if (pLF && (pLF->Extent() > 3))
        {
          aMERem.Add(aE);
          break;
        }
      }
    }
  }
  //
  if (aMERem.IsEmpty())
  {
    return;
  }
  //
  // all invalid faces containing these edges can be removed
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                aInvFaces;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        aMFRem;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFToUpdate;
  aNb = myInvalidFaces.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape&             aF    = myInvalidFaces.FindKey(i);
    NCollection_List<TopoDS_Shape>& aLFIm = myInvalidFaces(i);
    //
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLFIm);
    for (; aIt.More();)
    {
      const TopoDS_Shape& aFIm = aIt.Value();
      //
      // to be removed the face should have at least two not connected
      // inverted edges
      TopoDS_Compound aCEInv;
      aBB.MakeCompound(aCEInv);
      TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        if (aMERem.Contains(aE))
        {
          aBB.Add(aCEInv, aE);
        }
      }
      //
      // check connectivity
      NCollection_List<TopoDS_Shape> aLCBE;
      BOPTools_AlgoTools::MakeConnexityBlocks(aCEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
      //
      if (aLCBE.Extent() >= 2)
      {
        aMFToUpdate.Add(aF);
        aMFRem.Add(aFIm);
        aLFIm.Remove(aIt);
      }
      else
      {
        aIt.Next();
      }
    }
    //
    if (aLFIm.Extent())
    {
      aInvFaces.Add(aF, aLFIm);
    }
  }
  //
  if (aMFRem.IsEmpty())
  {
    return;
  }
  //
  myInvalidFaces = aInvFaces;
  // remove from splits
  aNb = aMFToUpdate.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape&             aF    = aMFToUpdate(i);
    NCollection_List<TopoDS_Shape>& aLFIm = myOFImages.ChangeFromKey(aF);
    //
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLFIm);
    for (; aIt.More();)
    {
      const TopoDS_Shape& aFIm = aIt.Value();
      if (aMFRem.Contains(aFIm))
      {
        TopExp::MapShapes(aFIm, TopAbs_EDGE, theMERemoved);
        aLFIm.Remove(aIt);
      }
      else
      {
        aIt.Next();
      }
    }
  }
}

//=======================================================================
// function : RemoveInvalidSplitsFromValid
// purpose  : Removing invalid splits of faces from valid
//=======================================================================
void BRepOffset_BuildOffsetFaces::RemoveInvalidSplitsFromValid(
  const NCollection_DataMap<TopoDS_Shape,
                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                            TopTools_ShapeMapHasher>& theDMFMVIE)
{
  // Decide whether to remove the found invalid faces or not.
  // The procedure is the following:
  // 1. Make connexity blocks from invalid faces;
  // 2. Find free edges in this blocks;
  // 3. If all free edges are valid for the faces - remove block.
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence, aMFToRem;
  TopoDS_Compound                                        aCFInv;
  BRep_Builder                                           aBB;
  aBB.MakeCompound(aCFInv);
  NCollection_List<TopoDS_Shape>::Iterator aItLF;
  //
  // make compound of invalid faces
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDMIFOF;
  int i, aNb = myInvalidFaces.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aF = myInvalidFaces.FindKey(i);
    // artificially invalid faces should not be removed
    if (myArtInvalidFaces.IsBound(aF))
    {
      continue;
    }
    const NCollection_List<TopoDS_Shape>& aLFInv = myInvalidFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFIm = aItLF.Value();
      if (aMFence.Add(aFIm))
      {
        aBB.Add(aCFInv, aFIm);
        aDMIFOF.Bind(aFIm, aF);
      }
    }
  }
  //
  // make connexity blocks
  NCollection_List<TopoDS_Shape> aLCBInv;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFInv, TopAbs_EDGE, TopAbs_FACE, aLCBInv);
  //
  // analyze each block
  aItLF.Initialize(aLCBInv);
  for (; aItLF.More(); aItLF.Next())
  {
    const TopoDS_Shape& aCB = aItLF.Value();
    //
    // if connexity block contains only one face - it should be removed;
    TopExp_Explorer aExp(aCB, TopAbs_FACE);
    aExp.Next();
    if (aExp.More())
    {
      // check if there are valid images left
      aExp.Init(aCB, TopAbs_FACE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aFIm = aExp.Current();
        const TopoDS_Shape& aF   = aDMIFOF.Find(aFIm);
        //
        const NCollection_List<TopoDS_Shape>& aLFIm  = myOFImages.FindFromKey(aF);
        const NCollection_List<TopoDS_Shape>& aLFInv = myInvalidFaces.FindFromKey(aF);
        //
        if (aLFIm.Extent() == aLFInv.Extent())
        {
          break;
        }
      }
    }
    //
    if (!aExp.More())
    {
      aExp.Init(aCB, TopAbs_FACE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aF = aExp.Current();
        aMFToRem.Add(aF);
      }
      continue;
    }
    //
    // remove faces connected by inverted edges
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aDMEF;
    TopExp::MapShapesAndAncestors(aCB, TopAbs_EDGE, TopAbs_FACE, aDMEF);
    //
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      aDMFF;
    aExp.Init(aCB, TopAbs_FACE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape&             aFCB  = aExp.Current();
      const TopoDS_Shape&             aF    = aDMIFOF.Find(aFCB);
      NCollection_List<TopoDS_Shape>* pList = aDMFF.ChangeSeek(aF);
      if (!pList)
        pList = aDMFF.Bound(aF, NCollection_List<TopoDS_Shape>());
      pList->Append(aFCB);
    }

    for (NCollection_DataMap<TopoDS_Shape,
                             NCollection_List<TopoDS_Shape>,
                             TopTools_ShapeMapHasher>::Iterator itM(aDMFF);
         itM.More();
         itM.Next())
    {
      const TopoDS_Shape&                                           aF = itM.Key();
      const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pValidInverted =
        theDMFMVIE.Seek(aF);

      // either remove all of these faces or none.
      const NCollection_List<TopoDS_Shape>&    aLFCB = itM.Value();
      NCollection_List<TopoDS_Shape>::Iterator itL(aLFCB);
      for (; itL.More(); itL.Next())
      {
        const TopoDS_Shape& aFCB = itL.Value();
        TopExp_Explorer     aExpE(aFCB, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next())
        {
          const TopoDS_Shape& aECB = aExpE.Current();
          if (pValidInverted && pValidInverted->Contains(aECB))
            break;
          if (aDMEF.FindFromKey(aECB).Extent() > 1)
          {
            if (!myInvertedEdges.Contains(aECB))
              break;
          }
        }
        if (!aExpE.More())
          // if one removed - remove all
          break;
      }
      if (itL.More())
      {
        for (itL.Initialize(aLFCB); itL.More(); itL.Next())
        {
          aMFToRem.Add(itL.Value());
        }
      }
    }
  }
  //
  if (aMFToRem.Extent())
  {
    // remove invalid faces from images
    aNb = myInvalidFaces.Extent();
    for (i = 1; i <= aNb; ++i)
    {
      const TopoDS_Shape&             aF        = myInvalidFaces.FindKey(i);
      NCollection_List<TopoDS_Shape>& aLFImages = myOFImages.ChangeFromKey(aF);
      aItLF.Initialize(aLFImages);
      for (; aItLF.More();)
      {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (aMFToRem.Contains(aFIm))
        {
          aLFImages.Remove(aItLF);
        }
        else
        {
          aItLF.Next();
        }
      }
    }
  }
}

namespace
{
//=======================================================================
// function : buildPairs
// purpose  : builds pairs of shapes
//=======================================================================
static void buildPairs(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theSMap,
                       NCollection_DataMap<TopoDS_Shape,
                                           NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                           TopTools_ShapeMapHasher>& theIntPairs)
{
  const int aNbS = theSMap.Extent();
  if (aNbS < 2)
    return;
  for (int it1 = 1; it1 <= aNbS; ++it1)
  {
    const TopoDS_Shape& aS = theSMap(it1);
    theIntPairs.TryBound(aS, NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>());
  }

  for (int it1 = 1; it1 <= aNbS; ++it1)
  {
    const TopoDS_Shape&                                     aS1   = theSMap(it1);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMap1 = theIntPairs(aS1);
    for (int it2 = it1 + 1; it2 <= aNbS; ++it2)
    {
      const TopoDS_Shape& aS2 = theSMap(it2);
      aMap1.Add(aS2);
      theIntPairs(aS2).Add(aS1);
    }
  }
}

//=======================================================================
// function : buildIntersectionPairs
// purpose  : builds intersection pairs
//=======================================================================
static void buildIntersectionPairs(
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                      myOFImages,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                      myInvalidFaces,
  const BOPAlgo_Builder&                                                          theBuilder,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                   theMFRemoved,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFOrigins,
  NCollection_DataMap<TopoDS_Shape,
                      NCollection_DataMap<TopoDS_Shape,
                                          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                          TopTools_ShapeMapHasher>,
                      TopTools_ShapeMapHasher>&                                   theIntPairs)
{
  TopAbs_ShapeEnum aCType = TopAbs_VERTEX;
  // Build connection map from vertices to faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVF;
  TopExp::MapShapesAndAncestors(theBuilder.Shape(), aCType, TopAbs_FACE, aDMVF);

  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    anImages = theBuilder.Images();
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    anOrigins = theBuilder.Origins();

  // Find all faces connected to the not removed faces and build intersection pairs among them.
  // For removed faces intersect only those connected to each other.

  for (int iF = 1; iF <= myInvalidFaces.Extent(); ++iF)
  {
    const TopoDS_Shape& aFInv = myInvalidFaces.FindKey(iF);

    TopoDS_Compound aCF, aCFRem;
    BRep_Builder().MakeCompound(aCF);
    BRep_Builder().MakeCompound(aCFRem);

    for (int iC = 0; iC < 2; ++iC)
    {
      const NCollection_List<TopoDS_Shape>& aLF =
        !iC ? myInvalidFaces(iF) : myOFImages.FindFromKey(aFInv);

      for (NCollection_List<TopoDS_Shape>::Iterator it(aLF); it.More(); it.Next())
      {
        NCollection_List<TopoDS_Shape> aLFIm;
        TakeModified(it.Value(), anImages, aLFIm);

        for (NCollection_List<TopoDS_Shape>::Iterator itIm(aLFIm); itIm.More(); itIm.Next())
        {
          const TopoDS_Shape& aFIm = itIm.Value();

          if (theMFRemoved.Contains(aFIm))
            BRep_Builder().Add(aCFRem, aFIm);
          else
            BRep_Builder().Add(aCF, aFIm);
        }
      }
    }

    NCollection_List<TopoDS_Shape> aLCB;
    BOPTools_AlgoTools::MakeConnexityBlocks(aCF, TopAbs_EDGE, TopAbs_FACE, aLCB);

    if (aLCB.IsEmpty())
      continue;

    NCollection_DataMap<TopoDS_Shape,
                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                        TopTools_ShapeMapHasher>* pFInterMap =
      theIntPairs.Bound(aFInv,
                        NCollection_DataMap<TopoDS_Shape,
                                            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                            TopTools_ShapeMapHasher>());

    // build pairs for not removed faces
    for (NCollection_List<TopoDS_Shape>::Iterator itCB(aLCB); itCB.More(); itCB.Next())
    {
      const TopoDS_Shape& aCB = itCB.Value();

      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFInter;
      for (TopExp_Explorer exp(aCB, aCType); exp.More(); exp.Next())
      {
        const TopoDS_Shape&                   aCS  = exp.Current();
        const NCollection_List<TopoDS_Shape>* pLFV = aDMVF.Seek(aCS);
        if (!pLFV)
          continue;

        for (NCollection_List<TopoDS_Shape>::Iterator itFV(*pLFV); itFV.More(); itFV.Next())
        {
          const TopoDS_Shape& aFConnected = itFV.Value();

          NCollection_List<TopoDS_Shape> aLFOr;
          TakeModified(aFConnected, anOrigins, aLFOr);
          for (NCollection_List<TopoDS_Shape>::Iterator itOr(aLFOr); itOr.More(); itOr.Next())
          {
            const TopoDS_Shape* pFOr = theFOrigins.Seek(itOr.Value());
            if (pFOr)
              aMFInter.Add(*pFOr);
          }
        }
      }

      // build intersection pairs
      buildPairs(aMFInter, *pFInterMap);
    }

    aLCB.Clear();
    BOPTools_AlgoTools::MakeConnexityBlocks(aCFRem, TopAbs_EDGE, TopAbs_FACE, aLCB);

    if (aLCB.IsEmpty())
      continue;

    for (NCollection_List<TopoDS_Shape>::Iterator itCB(aLCB); itCB.More(); itCB.Next())
    {
      const TopoDS_Shape& aCB = itCB.Value();

      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        aDMEF;
      for (TopExp_Explorer exp(aCB, aCType); exp.More(); exp.Next())
      {
        const TopoDS_Shape&                   aCS  = exp.Current();
        const NCollection_List<TopoDS_Shape>* pLFV = aDMVF.Seek(aCS);
        if (!pLFV)
          continue;

        for (NCollection_List<TopoDS_Shape>::Iterator itFV(*pLFV); itFV.More(); itFV.Next())
        {
          const TopoDS_Shape& aFConnected = itFV.Value();
          TopExp::MapShapesAndAncestors(aFConnected, TopAbs_EDGE, TopAbs_FACE, aDMEF);
        }
      }

      for (int iE = 1; iE <= aDMEF.Extent(); ++iE)
      {
        const NCollection_List<TopoDS_Shape>& aLFConnected = aDMEF(iE);
        if (aLFConnected.Extent() < 2)
          continue;

        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFInter;
        for (NCollection_List<TopoDS_Shape>::Iterator itLF(aLFConnected); itLF.More(); itLF.Next())
        {
          const TopoDS_Shape& aFConnected = itLF.Value();

          NCollection_List<TopoDS_Shape> aLFOr;
          TakeModified(aFConnected, anOrigins, aLFOr);
          for (NCollection_List<TopoDS_Shape>::Iterator itOr(aLFOr); itOr.More(); itOr.Next())
          {
            const TopoDS_Shape* pFOr = theFOrigins.Seek(itOr.Value());
            if (pFOr)
              aMFInter.Add(*pFOr);
          }
        }

        buildPairs(aMFInter, *pFInterMap);
      }
    }
  }
}

} // namespace

//=======================================================================
// function : RemoveInsideFaces
// purpose  : Looking for the inside faces that can be safely removed
//=======================================================================
void BRepOffset_BuildOffsetFaces::RemoveInsideFaces(
  const NCollection_List<TopoDS_Shape>&                                theInvertedFaces,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFToCheckInt,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInvInHole,
  const TopoDS_Shape&                                                  theFHoles,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMERemoved,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMEInside,
  const Message_ProgressRange&                                         theRange)
{
  NCollection_List<TopoDS_Shape>                                           aLS;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                   aMFence;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            aMFInv;
  NCollection_List<TopoDS_Shape>::Iterator                                 aItLF;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDMFImF;
  //
  Message_ProgressScope aPS(theRange, "Looking for inside faces", 10);
  int                   i, aNb = myOFImages.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Shape& aF = myOFImages.FindKey(i);
    // to avoid intersection of the splits of the same
    // offset faces among themselves make compound of the
    // splits and use it as one argument
    TopoDS_Compound aCFImi;
    BRep_Builder().MakeCompound(aCFImi);
    //
    for (int j = 0; j < 2; ++j)
    {
      const NCollection_List<TopoDS_Shape>* pLFSp = !j ? myInvalidFaces.Seek(aF) : &myOFImages(i);
      if (!pLFSp)
      {
        continue;
      }
      //
      aItLF.Initialize(*pLFSp);
      for (; aItLF.More(); aItLF.Next())
      {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (aMFence.Add(aFIm))
        {
          BRep_Builder().Add(aCFImi, aFIm);
          aDMFImF.Bind(aFIm, aF);
          if (!j)
          {
            aMFInv.Add(aFIm);
          }
        }
      }
    }
    //
    aLS.Append(aCFImi);
  }
  //
  // to make the solids more complete add for intersection also the faces
  // consisting only of invalid edges and not included into splits
  aNb = theMFToCheckInt.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aFSp = theMFToCheckInt(i);
    if (aMFence.Add(aFSp))
    {
      aLS.Append(aFSp);
    }
  }
  //
  BOPAlgo_MakerVolume aMV;
  aMV.SetArguments(aLS);
  aMV.SetIntersect(true);
  aMV.Perform(aPS.Next(9));
  if (aMV.HasErrors())
    return;

  //
  // get shapes connection for using in the rebuilding process
  // for the cases in which some of the intersection left undetected
  ShapesConnections(aDMFImF, aMV);
  //
  // find faces to remove
  const TopoDS_Shape& aSols = aMV.Shape();
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMFS;
  TopExp::MapShapesAndAncestors(aSols, TopAbs_FACE, TopAbs_SOLID, aDMFS);
  //
  aNb = aDMFS.Extent();
  if (!aNb)
  {
    return;
  }
  //
  // To use the created solids for classifications, firstly, it is necessary
  // to check them on validity - the created solids should be complete,
  // i.e. all faces should be included.
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFToRem;
  // Check completeness
  if (aMV.HasDeleted())
  {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEHoles;
    TopExp::MapShapes(theFHoles, TopAbs_EDGE, aMEHoles);

    // Map edges of the solids to check the connectivity
    // of the removed invalid splits
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMESols;
    TopExp::MapShapes(aSols, TopAbs_EDGE, aMESols);

    // perform additional check on faces
    aNb = myOFImages.Extent();
    for (i = 1; i <= aNb; ++i)
    {
      if (!aPS.More())
      {
        return;
      }
      const NCollection_List<TopoDS_Shape>& aLFIm = myOFImages(i);
      if (aLFIm.IsEmpty())
      {
        continue;
      }

      const TopoDS_Shape& aF       = myOFImages.FindKey(i);
      bool                bInvalid = myInvalidFaces.Contains(aF);
      // For invalid faces it is allowed to be at least connected
      // to the solids, otherwise the solids are considered as broken
      bool bConnected = false;

      bool bFaceKept = false;
      aItLF.Initialize(aLFIm);
      for (; aItLF.More(); aItLF.Next())
      {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (!aMV.IsDeleted(aFIm))
        {
          bFaceKept = true;
          continue;
        }
        //
        TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next())
        {
          if (aMEHoles.Contains(aExpE.Current()))
          {
            bFaceKept = true;
            aMFToRem.Add(aFIm);
            break;
          }
          if (!bFaceKept && bInvalid && !bConnected)
            bConnected = aMESols.Contains(aExpE.Current());
        }
      }
      //
      if (!bFaceKept && !bConnected)
      {
        return;
      }
    }
  }
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEBoundary;
  aNb = aDMFS.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape&                   aFIm  = aDMFS.FindKey(i);
    const NCollection_List<TopoDS_Shape>& aLSol = aDMFS(i);
    if (aLSol.Extent() > 1)
    {
      aMFToRem.Add(aFIm);
    }
    else if (aFIm.Orientation() != TopAbs_INTERNAL)
    {
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEBoundary);
    }
  }

  // Tool for getting the splits of faces
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    aMVIms = aMV.Images();

  // update invalid faces with images
  aNb = aMFInv.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aFInv = aMFInv(i);
    TakeModified(aFInv, aMVIms, aMFInv);
  }

  // Take into account the faces invalid by inverted edges
  for (NCollection_List<TopoDS_Shape>::Iterator itLF(theInvertedFaces); itLF.More(); itLF.Next())
    TakeModified(itLF.Value(), aMVIms, aMFInv);

  // check if the invalid faces inside the holes are really invalid:
  // check its normal direction - if it has changed relatively the
  // original face the offset face is invalid and should be kept for rebuilding
  int aNbFH = theMFInvInHole.Extent();
  for (i = 1; i <= aNbFH; ++i)
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Shape&            aFInv    = theMFInvInHole(i);
    NCollection_List<TopoDS_Shape> aLFInvIm = aMV.Modified(aFInv);
    if (aLFInvIm.IsEmpty())
    {
      aLFInvIm.Append(aFInv);
    }
    //
    const TopoDS_Shape* pFOffset = aDMFImF.Seek(aFInv);
    if (!pFOffset)
    {
      continue;
    }
    NCollection_List<TopoDS_Shape>::Iterator aItLFInv(aLFInvIm);
    for (; aItLFInv.More(); aItLFInv.Next())
    {
      const TopoDS_Shape&                   aFInvIm = aItLFInv.Value();
      const NCollection_List<TopoDS_Shape>* pLSols  = aDMFS.Seek(aFInvIm);
      if (!pLSols || pLSols->Extent() != 1)
      {
        continue;
      }
      //
      const TopoDS_Shape& aFSol = pLSols->First();
      //
      TopoDS_Shape aFx;
      if (!FindShape(aFInvIm, aFSol, nullptr, aFx))
      {
        continue;
      }
      //
      if (BRepOffset_Tool::CheckPlanesNormals(TopoDS::Face(aFx), TopoDS::Face(*pFOffset)))
      {
        // the normal direction has not changed, thus the face can be removed
        aMFToRem.Add(aFInvIm);
      }
    }
  }
  //
  TopoDS_Compound aSolids;
  BRep_Builder().MakeCompound(aSolids);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFKeep;
  //
  TopExp_Explorer aExpS(aSols, TopAbs_SOLID);
  for (; aExpS.More(); aExpS.Next())
  {
    if (!aPS.More())
    {
      return;
    }
    const TopoDS_Shape& aSol = aExpS.Current();
    //
    bool bAllInv(true), bAllRemoved(true);

    for (TopExp_Explorer aExpF(aSol, TopAbs_FACE); aExpF.More(); aExpF.Next())
    {
      const TopoDS_Shape& aFS = aExpF.Current();
      //
      if (aFS.Orientation() == TopAbs_INTERNAL)
      {
        aMFToRem.Add(aFS);
        continue;
      }

      if (aMFToRem.Contains(aFS))
        continue;

      bAllRemoved = false;
      bAllInv &= aMFInv.Contains(aFS);
    }
    //
    if (bAllInv && !bAllRemoved)
    {
      // remove invalid faces but keep those that have already been marked for removal
      TopExp_Explorer aExpF(aSol, TopAbs_FACE);
      for (; aExpF.More(); aExpF.Next())
      {
        const TopoDS_Shape& aFS = aExpF.Current();
        //
        if (aMFToRem.Contains(aFS))
        {
          if (!aMFKeep.Add(aFS))
          {
            aMFKeep.Remove(aFS);
          }
        }
        else
        {
          aMFToRem.Add(aFS);
        }
      }
    }
    else
    {
      BRep_Builder().Add(aSolids, aSol);
      mySolids = aSolids;
    }
  }
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItM(aMFKeep);
  for (; aItM.More(); aItM.Next())
  {
    aMFToRem.Remove(aItM.Value());
  }

  // Remove the invalid hanging parts external to the solids
  RemoveHangingParts(aMV, aDMFImF, aMFInv, aMFToRem);

  // Remove newly found internal and hanging faces
  RemoveValidSplits(aMFToRem, aMV, theMERemoved);
  RemoveInvalidSplits(aMFToRem, aMV, theMERemoved);
  //
  // Get inside faces from the removed ones comparing them with boundary edges
  theMEInside.Clear();
  aNb = theMERemoved.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aE = theMERemoved(i);
    if (!aMEBoundary.Contains(aE))
    {
      theMEInside.Add(aE);
    }
  }

  // build all possible intersection pairs basing on the intersection results
  // taking into account removed faces.
  if (aMFToRem.Extent())
    buildIntersectionPairs(myOFImages, myInvalidFaces, aMV, aMFToRem, aDMFImF, myIntersectionPairs);
}

//=======================================================================
// function : ShapesConnections
// purpose  : Looking for the connections between faces not to miss
//           some necessary intersection
//=======================================================================
void BRepOffset_BuildOffsetFaces::ShapesConnections(
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDMFOr,
  BOPAlgo_Builder&                                                                theBuilder)
{
  // Make connexity blocks from invalid edges to use the whole block
  // to which the edge is connected instead of the single edge.
  TopoDS_Compound aCEInv;
  BRep_Builder().MakeCompound(aCEInv);
  for (int i = 1; i <= myInvalidEdges.Extent(); ++i)
  {
    AddToContainer(myInvalidEdges(i), aCEInv);
  }

  NCollection_List<TopoDS_Shape> aLCB;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCB);

  // Binding from the edge to the block
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aECBMap;
  for (NCollection_List<TopoDS_Shape>::Iterator itCB(aLCB); itCB.More(); itCB.Next())
  {
    for (TopoDS_Iterator itE(itCB.Value()); itE.More(); itE.Next())
    {
      aECBMap.Bind(itE.Value(), itCB.Value());
    }
  }

  // update invalid edges with images and keep connection to original edge
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aDMEOr;
  int aNb = myInvalidEdges.Extent();
  for (int i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape&                   aEInv = myInvalidEdges(i);
    const NCollection_List<TopoDS_Shape>& aLEIm = theBuilder.Modified(aEInv);
    if (aLEIm.IsEmpty())
    {
      aDMEOr.Bound(aEInv, NCollection_List<TopoDS_Shape>())->Append(aEInv);
      continue;
    }
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEIm);
    for (; aItLEIm.More(); aItLEIm.Next())
    {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      AddToContainer(aEIm, aEInv, aDMEOr);
    }
  }
  //
  // get shapes connections for using in the rebuilding process
  const BOPDS_PDS& pDS = theBuilder.PDS();
  // analyze all Face/Face intersections
  const NCollection_Vector<BOPDS_InterfFF>& aFFs = pDS->InterfFF();
  int                                       iInt, aNbFF = aFFs.Length();
  for (iInt = 0; iInt < aNbFF; ++iInt)
  {
    const BOPDS_InterfFF&                  aFF  = aFFs(iInt);
    const NCollection_Vector<BOPDS_Curve>& aVNC = aFF.Curves();
    int                                    aNbC = aVNC.Length();
    if (!aNbC)
    {
      continue;
    }
    //
    const TopoDS_Shape& aFIm1 = pDS->Shape(aFF.Index1());
    const TopoDS_Shape& aFIm2 = pDS->Shape(aFF.Index2());
    //
    const TopoDS_Shape* pF1 = theDMFOr.Seek(aFIm1);
    const TopoDS_Shape* pF2 = theDMFOr.Seek(aFIm2);
    //
    if (!pF1 || !pF2)
    {
      continue;
    }
    //
    if (pF1->IsSame(*pF2))
    {
      continue;
    }
    //
    bool bInv1 = myInvalidFaces.Contains(*pF1);
    bool bInv2 = myInvalidFaces.Contains(*pF2);
    //
    if (!bInv1 && !bInv2)
    {
      continue;
    }
    //
    // check if it is real Face/Face intersection
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInt;
    for (int iC = 0; iC < aNbC; ++iC)
    {
      const BOPDS_Curve&                                       aNC  = aVNC(iC);
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>&    aLPB = aNC.PaveBlocks();
      NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB(aLPB);
      for (; aItLPB.More(); aItLPB.Next())
      {
        const occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.Value();
        int                                 nEInt;
        if (aPB->HasEdge(nEInt))
        {
          const TopoDS_Shape& aEInt = pDS->Shape(nEInt);
          aMEInt.Add(aEInt);
        }
      }
    }
    //
    if (aMEInt.IsEmpty())
    {
      continue;
    }
    //
    // check if invalid edges of the face are in the same splits with intersection edges
    for (int i = 0; i < 2; ++i)
    {
      if ((!i && !bInv1) || (i && !bInv2))
      {
        continue;
      }
      //
      const TopoDS_Shape& aF   = !i ? *pF1 : *pF2;
      const TopoDS_Shape& aFOp = !i ? *pF2 : *pF1;
      const TopoDS_Shape& aFIm = !i ? aFIm1 : aFIm2;
      //
      bool bFound = false;
      //
      NCollection_List<TopoDS_Shape> aLFIm = theBuilder.Modified(aFIm);
      if (aLFIm.IsEmpty())
      {
        aLFIm.Append(aFIm);
      }
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next())
      {
        const TopoDS_Shape& aFImIm = aItLFIm.Value();
        //
        bool            bInv(false), bInt(false);
        TopExp_Explorer aExpE(aFImIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next())
        {
          const TopoDS_Shape& aE = aExpE.Current();
          if (!bInv)
          {
            bInv = aDMEOr.IsBound(aE);
          }
          if (!bInt)
          {
            bInt = aMEInt.Contains(aE);
          }
          if (bInv && bInt)
          {
            break;
          }
        }
        //
        if (!bInt || !bInv)
        {
          continue;
        }
        //
        bFound = true;
        //
        // append opposite face to all invalid edges in the split
        aExpE.Init(aFImIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next())
        {
          const TopoDS_Shape&                   aE    = aExpE.Current();
          const NCollection_List<TopoDS_Shape>* pLEOr = aDMEOr.Seek(aE);
          if (!pLEOr)
          {
            continue;
          }
          //
          NCollection_List<TopoDS_Shape>::Iterator aItLE(*pLEOr);
          for (; aItLE.More(); aItLE.Next())
          {
            const TopoDS_Shape&             aEOr = aItLE.Value();
            NCollection_List<TopoDS_Shape>* pLFE = mySSInterfs.ChangeSeek(aEOr);
            if (!pLFE)
            {
              pLFE = mySSInterfs.Bound(aEOr, NCollection_List<TopoDS_Shape>());
            }
            AppendToList(*pLFE, aFOp);
          }
        }
      }
      if (bFound)
      {
        // save connection between offset faces
        NCollection_List<TopoDS_Shape>* pLF = mySSInterfs.ChangeSeek(aF);
        if (!pLF)
        {
          pLF = mySSInterfs.Bound(aF, NCollection_List<TopoDS_Shape>());
        }
        AppendToList(*pLF, aFOp);
      }
    }

    // Treatment for the artificial case - check if one of the faces is artificially invalid
    for (int iF = 0; iF < 2; ++iF)
    {
      const TopoDS_Shape& aFArt      = !iF ? *pF1 : *pF2;
      const TopoDS_Shape& aFOpposite = !iF ? *pF2 : *pF1;

      if (!myArtInvalidFaces.IsBound(aFArt))
        continue;

      if (myInvalidFaces.Contains(aFOpposite) && !myArtInvalidFaces.IsBound(aFOpposite))
        continue;

      // Collect own invalid edges of the face and the invalid edges connected to those
      // own invalid edges to be avoided in the check for intersection.
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEAvoid;
      if (const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* pFEInv =
            myArtInvalidFaces.Seek(aFOpposite))
      {
        for (int iE = 1; iE <= pFEInv->Extent(); ++iE)
        {
          if (const TopoDS_Shape* pCB = aECBMap.Seek(pFEInv->FindKey(iE)))
          {
            TopExp::MapShapes(*pCB, TopAbs_EDGE, aMEAvoid);
          }
        }
      }
      else if (const NCollection_List<TopoDS_Shape>* pLFIm = myOFImages.Seek(aFOpposite))
      {
        for (NCollection_List<TopoDS_Shape>::Iterator itLFIm(*pLFIm); itLFIm.More(); itLFIm.Next())
        {
          for (TopExp_Explorer expE(itLFIm.Value(), TopAbs_EDGE); expE.More(); expE.Next())
          {
            if (const TopoDS_Shape* pCB = aECBMap.Seek(expE.Current()))
            {
              TopExp::MapShapes(*pCB, TopAbs_EDGE, aMEAvoid);
            }
          }
        }
      }

      const TopoDS_Shape& aFArtIm      = !iF ? aFIm1 : aFIm2;
      const TopoDS_Shape& aFOppositeIm = !iF ? aFIm2 : aFIm1;

      // Check if there are any intersections between edges of artificially
      // invalid face and opposite face
      const int nFOp = pDS->Index(aFOppositeIm);

      for (TopExp_Explorer expE(aFArtIm, TopAbs_EDGE); expE.More(); expE.Next())
      {
        const TopoDS_Shape& aE = expE.Current();
        if (!myInvalidEdges.Contains(aE) || myInvertedEdges.Contains(aE) || aMEAvoid.Contains(aE))
        {
          continue;
        }
        const int nE = pDS->Index(aE);
        if (nE < 0)
        {
          continue;
        }

        if (!pDS->HasInterf(nE, nFOp))
        {
          continue;
        }

        NCollection_List<TopoDS_Shape>            aLV;
        const NCollection_Vector<BOPDS_InterfEF>& aEFs = pDS->InterfEF();
        for (int iEF = 0; iEF < aEFs.Size(); ++iEF)
        {
          const BOPDS_InterfEF& aEF = aEFs(iEF);
          if (aEF.Contains(nE) && aEF.Contains(nFOp))
          {
            if (aEF.CommonPart().Type() == TopAbs_VERTEX)
              aLV.Append(pDS->Shape(aEF.IndexNew()));
          }
        }

        if (aLV.IsEmpty())
        {
          continue;
        }

        // Make sure that there is an opposite intersection exists, i.e. some of the edges
        // of the opposite face intersect the artificially invalid face.
        const int       nFArt = pDS->Index(aFArtIm);
        TopExp_Explorer expEOp(aFOppositeIm, TopAbs_EDGE);
        for (; expEOp.More(); expEOp.Next())
        {
          const TopoDS_Shape& aEOp = expEOp.Current();
          const int           nEOp = pDS->Index(aEOp);
          if (pDS->HasInterf(nEOp, nFArt))
          {
            break;
          }
        }
        if (!expEOp.More())
        {
          continue;
        }

        // Intersection is present - add connection between offset faces.
        AddToContainer(aFArt, aFOpposite, mySSInterfsArt);

        // Add connection between edge and opposite face
        AddToContainer(aE, aFOpposite, mySSInterfsArt);

        // Along with the opposite face, save the intersection vertices to
        // be used for trimming the intersection edge in the rebuilding process
        for (NCollection_List<TopoDS_Shape>::Iterator itLV(aLV); itLV.More(); itLV.Next())
        {
          // Add connection to intersection vertex
          AddToContainer(aE, itLV.Value(), mySSInterfsArt);
        }
      }
    }
  }
}

//=======================================================================
// function : RemoveHangingParts
// purpose  : Remove isolated invalid hanging parts
//=======================================================================
void BRepOffset_BuildOffsetFaces::RemoveHangingParts(
  const BOPAlgo_MakerVolume&                                                      theMV,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDMFImF,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&            theMFInv,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                         theMFToRem)
{
  // Map the faces of the result solids to filter them from avoided faces
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFS;
  TopExp::MapShapes(theMV.Shape(), TopAbs_FACE, aMFS);

  BRep_Builder aBB;
  // Build compound of all faces not included into solids
  TopoDS_Compound aCFHangs;
  aBB.MakeCompound(aCFHangs);

  // Tool for getting the splits of faces
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    aMVIms = theMV.Images();

  NCollection_List<TopoDS_Shape>::Iterator aItLArgs(theMV.Arguments());
  for (; aItLArgs.More(); aItLArgs.Next())
  {
    TopExp_Explorer anExpF(aItLArgs.Value(), TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      const TopoDS_Shape& aF = anExpF.Current();
      TakeModified(aF, aMVIms, aCFHangs, &aMFS);
    }
  }

  // Make connexity blocks of all hanging parts and check that they are isolated
  NCollection_List<TopoDS_Shape> aLCBHangs;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFHangs, TopAbs_EDGE, TopAbs_FACE, aLCBHangs);
  if (aLCBHangs.IsEmpty())
    return;

  // To be removed, the block should contain invalid splits of offset faces and should
  // meet one of the following conditions:
  // 1. The block should not be connected to any invalid parts (Faces or Edges)
  //    contained in solids;
  // 2. The block should be isolated from other faces, i.e. it should consist of
  //    the splits of the single offset face.

  // Map the edges and vertices of the result solids to check connectivity
  // of the hanging blocks to invalid parts contained in solids
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMEF, aDMVE;
  TopExp::MapShapesAndAncestors(theMV.Shape(), TopAbs_EDGE, TopAbs_FACE, aDMEF);
  TopExp::MapShapesAndAncestors(theMV.Shape(), TopAbs_VERTEX, TopAbs_EDGE, aDMVE);

  // Update invalid edges with intersection results
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
  int                                                    i, aNbE = myInvalidEdges.Extent();
  for (i = 1; i <= aNbE; ++i)
    TakeModified(myInvalidEdges(i), aMVIms, aMEInv);

  // Update inverted edges with intersection results
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInverted;
  for (int iInv = 1; iInv <= myInvertedEdges.Extent(); ++iInv)
    TakeModified(myInvertedEdges(iInv), aMVIms, aMEInverted);

  // Tool for getting the origins of the splits
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    aMVOrs = theMV.Origins();

  // Find hanging blocks to remove
  NCollection_List<TopoDS_Shape> aBlocksToRemove;

  NCollection_List<TopoDS_Shape>::Iterator aItLCBH(aLCBHangs);
  for (; aItLCBH.More(); aItLCBH.Next())
  {
    const TopoDS_Shape& aCBH = aItLCBH.Value();

    // Remove the block containing the inverted edges
    bool            bHasInverted = false;
    TopExp_Explorer anExpE(aCBH, TopAbs_EDGE);
    for (; anExpE.More() && !bHasInverted; anExpE.Next())
    {
      const TopoDS_Shape& aE = anExpE.Current();
      bHasInverted           = !aDMEF.Contains(aE) && aMEInverted.Contains(aE);
    }

    if (bHasInverted)
    {
      aBlocksToRemove.Append(aCBH);
      continue;
    }

    // Check the block to contain invalid split
    bool bHasInvalidFace = false;
    // Check connectivity to invalid parts
    bool                                                          bIsConnected = false;
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aBlockME;
    TopExp::MapShapes(aCBH, TopAbs_EDGE, aBlockME);
    // Map to collect all original faces
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMOffsetF;

    TopExp_Explorer anExpF(aCBH, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
    {
      const TopoDS_Shape& aF = anExpF.Current();
      // Check block to contain invalid face
      if (!bHasInvalidFace)
        bHasInvalidFace = theMFInv.Contains(aF);

      // Check block for connectivity to invalid parts
      if (!bIsConnected)
      {
        // check edges
        anExpE.Init(aF, TopAbs_EDGE);
        for (; anExpE.More() && !bIsConnected; anExpE.Next())
        {
          const TopoDS_Shape&                   aE  = anExpE.Current();
          const NCollection_List<TopoDS_Shape>* pLF = aDMEF.Seek(aE);
          if (pLF)
          {
            NCollection_List<TopoDS_Shape>::Iterator aItLF(*pLF);
            for (; aItLF.More() && !bIsConnected; aItLF.Next())
              bIsConnected = theMFInv.Contains(aItLF.Value());
          }
        }
        // check vertices
        if (!bIsConnected)
        {
          TopExp_Explorer anExpV(aF, TopAbs_VERTEX);
          for (; anExpV.More() && !bIsConnected; anExpV.Next())
          {
            const TopoDS_Shape&                   aV  = anExpV.Current();
            const NCollection_List<TopoDS_Shape>* pLE = aDMVE.Seek(aV);
            if (pLE)
            {
              NCollection_List<TopoDS_Shape>::Iterator aItLE(*pLE);
              for (; aItLE.More() && !bIsConnected; aItLE.Next())
                bIsConnected = !aBlockME.Contains(aItLE.Value()) && aMEInv.Contains(aItLE.Value());
            }
          }
        }
      }

      // Check block to be isolated
      const NCollection_List<TopoDS_Shape>* pLFOr = aMVOrs.Seek(aF);
      if (pLFOr)
      {
        NCollection_List<TopoDS_Shape>::Iterator aItLFOr(*pLFOr);
        for (; aItLFOr.More(); aItLFOr.Next())
        {
          const TopoDS_Shape* pFOffset = theDMFImF.Seek(aItLFOr.Value());
          if (pFOffset)
            aMOffsetF.Add(*pFOffset);
        }
      }
      else
      {
        const TopoDS_Shape* pFOffset = theDMFImF.Seek(aF);
        if (pFOffset)
          aMOffsetF.Add(*pFOffset);
      }
    }

    bool bRemove = bHasInvalidFace && (!bIsConnected || aMOffsetF.Extent() == 1);

    if (bRemove)
      aBlocksToRemove.Append(aCBH);
  }

  // remove the invalidated blocks
  aItLCBH.Initialize(aBlocksToRemove);
  for (; aItLCBH.More(); aItLCBH.Next())
  {
    const TopoDS_Shape& aCBH = aItLCBH.Value();
    TopExp_Explorer     anExpF(aCBH, TopAbs_FACE);
    for (; anExpF.More(); anExpF.Next())
      theMFToRem.Add(anExpF.Current());
  }
}

//=======================================================================
// function : RemoveValidSplits
// purpose  : Removing valid splits according to results of intersection
//=======================================================================
void BRepOffset_BuildOffsetFaces::RemoveValidSplits(
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theSpRem,
  BOPAlgo_Builder&                                               theGF,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved)
{
  int i, aNb = myOFImages.Extent();
  if (!aNb)
  {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i)
  {
    NCollection_List<TopoDS_Shape>&          aLSIm = myOFImages(i);
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLSIm);
    for (; aIt.More();)
    {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (theSpRem.Contains(aSIm))
      {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
        continue;
      }
      //
      // check if all its images are have to be removed
      const NCollection_List<TopoDS_Shape>& aLSImIm = theGF.Modified(aSIm);
      if (aLSImIm.Extent())
      {
        bool                                     bAllRem = true;
        NCollection_List<TopoDS_Shape>::Iterator aIt1(aLSImIm);
        for (; aIt1.More(); aIt1.Next())
        {
          const TopoDS_Shape& aSImIm = aIt1.Value();
          if (theSpRem.Contains(aSImIm))
          {
            TopExp::MapShapes(aSImIm, TopAbs_EDGE, theMERemoved);
          }
          else
          {
            bAllRem = false;
          }
        }
        //
        if (bAllRem)
        {
          TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
          aLSIm.Remove(aIt);
          continue;
        }
      }
      aIt.Next();
    }
  }
}

//=======================================================================
// function : RemoveInvalidSplits
// purpose  : Removing invalid splits according to the results of intersection
//=======================================================================
void BRepOffset_BuildOffsetFaces::RemoveInvalidSplits(
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theSpRem,
  BOPAlgo_Builder&                                               theGF,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved)
{
  int i, aNb = myInvalidFaces.Extent();
  if (!aNb)
  {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aS   = myInvalidFaces.FindKey(i);
    bool                bArt = myArtInvalidFaces.IsBound(aS);
    //
    NCollection_List<TopoDS_Shape>&          aLSIm = myInvalidFaces(i);
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLSIm);
    for (; aIt.More();)
    {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (theSpRem.Contains(aSIm))
      {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
        continue;
      }
      //
      // check if all its images are have to be removed
      const NCollection_List<TopoDS_Shape>& aLSImIm = theGF.Modified(aSIm);
      if (aLSImIm.IsEmpty())
      {
        aIt.Next();
        continue;
      }
      //
      bool                                                          bAllRem = true;
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMERemoved;
      NCollection_List<TopoDS_Shape>::Iterator                      aIt1(aLSImIm);
      for (; aIt1.More(); aIt1.Next())
      {
        const TopoDS_Shape& aSImIm = aIt1.Value();
        if (theSpRem.Contains(aSImIm))
        {
          TopExp::MapShapes(aSImIm, TopAbs_EDGE, aMERemoved);
        }
        else
        {
          bAllRem = false;
        }
      }
      //
      if (bAllRem)
      {
        aLSIm.Remove(aIt);
        continue;
      }
      //
      if (bArt)
      {
        aIt.Next();
        continue;
      }
      //
      // remove the face from invalid if all invalid edges of this face
      // have been marked for removal
      TopExp_Explorer aExpE(aSIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next())
      {
        const TopoDS_Shape& aEInv = aExpE.Current();
        if (myInvalidEdges.Contains(aEInv) && !aMERemoved.Contains(aEInv))
        {
          break;
        }
      }
      if (!aExpE.More())
      {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
      }
      else
      {
        aIt.Next();
      }
    }
  }
}

//=======================================================================
// function : FilterEdgesImages
// purpose  : Updating the maps of images and origins of the offset edges
//=======================================================================
void BRepOffset_BuildOffsetFaces::FilterEdgesImages(const TopoDS_Shape& theS)
{
  // map edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aME;
  TopExp::MapShapes(theS, TopAbs_EDGE, aME);
  //
  myOEOrigins.Clear();
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator aItDM(myOEImages);
  for (; aItDM.More(); aItDM.Next())
  {
    const TopoDS_Shape&             aE    = aItDM.Key();
    NCollection_List<TopoDS_Shape>& aLEIm = aItDM.ChangeValue();
    //
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLEIm);
    for (; aIt.More();)
    {
      const TopoDS_Shape& aEIm = aIt.Value();
      // filter images
      if (!aME.Contains(aEIm))
      {
        // remove the image
        // edges with no images left should be kept in the map
        // to avoid their usage when building the splits of faces
        aLEIm.Remove(aIt);
        continue;
      }
      //
      // save origins
      if (myOEOrigins.IsBound(aEIm))
      {
        AppendToList(myOEOrigins.ChangeFind(aEIm), aE);
      }
      else
      {
        NCollection_List<TopoDS_Shape> aLOr;
        aLOr.Append(aE);
        myOEOrigins.Bind(aEIm, aLOr);
      }
      //
      aIt.Next();
    }
  }
}

//=======================================================================
// function : FilterInvalidFaces
// purpose  : Filtering of the invalid faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::FilterInvalidFaces(
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&           theDMEF,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved)
{
  //
  // filter invalid faces, considering faces having only valid
  // images left with non-free edges as valid
  // do not remove invalid faces if it creates free edges
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aReallyInvFaces;
  // Edge-Face connexity map of all splits, both invalid and valid
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                           aDMEFAll;
  NCollection_List<TopoDS_Shape>::Iterator aItLF;
  //
  const int aNb = myInvalidFaces.Extent();
  for (int i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape&                   aF     = myInvalidFaces.FindKey(i);
    const NCollection_List<TopoDS_Shape>& aLFInv = myInvalidFaces(i);
    //
    if (myArtInvalidFaces.IsBound(aF))
    {
      if (aLFInv.IsEmpty())
      {
        myArtInvalidFaces.UnBind(aF);
      }
      else
      {
        aReallyInvFaces.Add(aF, aLFInv);
      }
      continue;
    }
    //
    if (aLFInv.IsEmpty())
    {
      continue;
    }
    //
    NCollection_List<TopoDS_Shape>& aLFIm    = myOFImages.ChangeFromKey(aF);
    bool                            bInvalid = aLFIm.IsEmpty();
    //
    if (!bInvalid)
    {
      // check two lists on common splits
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next())
      {
        const TopoDS_Shape& aFInv = aItLF.Value();
        //
        NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
        for (; aItLFIm.More(); aItLFIm.Next())
        {
          const TopoDS_Shape& aFIm = aItLFIm.Value();
          //
          if (aFInv.IsSame(aFIm))
          {
            break;
          }
        }
        //
        if (aItLFIm.More())
        {
          break;
        }
      }
      //
      bInvalid = aItLF.More();
    }
    //
    if (!bInvalid)
    {
      // check for free edges
      for (int j = 0; !bInvalid && j < 2; ++j)
      {
        const NCollection_List<TopoDS_Shape>& aLI = !j ? aLFIm : aLFInv;
        aItLF.Initialize(aLI);
        for (; aItLF.More(); aItLF.Next())
        {
          const TopoDS_Shape& aFIm = aItLF.Value();
          //
          TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
          for (; aExp.More(); aExp.Next())
          {
            const TopoDS_Shape& aE = aExp.Current();
            if (!theMERemoved.Contains(aE))
            {
              const NCollection_List<TopoDS_Shape>* pLEF = theDMEF.Seek(aE);
              if (pLEF && pLEF->Extent() == 1)
              {
                break;
              }
            }
          }
          //
          if (aExp.More())
          {
            break;
          }
        }
        bInvalid = aItLF.More();
      }
    }
    if (bInvalid)
    {
      if (aDMEFAll.IsEmpty())
      {
        aDMEFAll = theDMEF;
        for (int iF = 1; iF <= aNb; ++iF)
          for (NCollection_List<TopoDS_Shape>::Iterator itLFInv(myInvalidFaces(iF)); itLFInv.More();
               itLFInv.Next())
            TopExp::MapShapesAndAncestors(itLFInv.Value(), TopAbs_EDGE, TopAbs_FACE, aDMEFAll);
      }

      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aLocalSplits;
      for (int j = 0; j < 2; ++j)
        for (aItLF.Initialize((!j ? aLFIm : aLFInv)); aItLF.More(); aItLF.Next())
          aLocalSplits.Add(aItLF.Value());

      // Check if all invalid edges are located inside the split and do not touch
      // any other faces both invalid and valid
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next())
      {
        const TopoDS_Shape& aFIm = aItLF.Value();
        TopExp_Explorer     aExp(aFIm, TopAbs_EDGE);
        for (; aExp.More(); aExp.Next())
        {
          const TopoDS_Shape& aE = aExp.Current();
          if (myInvalidEdges.Contains(aE) && !theMERemoved.Contains(aE))
          {
            const NCollection_List<TopoDS_Shape>&    aLF = aDMEFAll.FindFromKey(aE);
            NCollection_List<TopoDS_Shape>::Iterator itLF(aLF);
            for (; itLF.More(); itLF.Next())
            {
              if (!aLocalSplits.Contains(itLF.Value()))
                break;
            }
            if (itLF.More())
              break;
          }
        }
        if (aExp.More())
          break;
      }
      bInvalid = aItLF.More();
      if (!bInvalid)
      {
        aItLF.Initialize(aLFInv);
        for (; aItLF.More(); aItLF.Next())
          AppendToList(aLFIm, aItLF.Value());
      }
    }
    //
    if (bInvalid)
    {
      aReallyInvFaces.Add(aF, aLFInv);
    }
  }
  //
  myInvalidFaces = aReallyInvFaces;
}

//=======================================================================
// function : CheckEdgesCreatedByVertex
// purpose  : Checks additionally the unchecked edges originated from vertices
//=======================================================================
void BRepOffset_BuildOffsetFaces::CheckEdgesCreatedByVertex()
{
  // Mark the unchecked edges contained in invalid faces as invalid
  const int aNbF = myInvalidFaces.Extent();
  for (int i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Shape& aF = myInvalidFaces.FindKey(i);
    if (myArtInvalidFaces.IsBound(aF))
      continue;

    const NCollection_List<TopoDS_Shape>& aLFIm = myInvalidFaces(i);
    for (NCollection_List<TopoDS_Shape>::Iterator it(aLFIm); it.More(); it.Next())
    {
      const TopoDS_Shape& aFIm = it.Value();
      for (TopExp_Explorer expE(aFIm, TopAbs_EDGE); expE.More(); expE.Next())
      {
        const TopoDS_Shape& aE = expE.Current();
        if (myInvalidEdges.Contains(aE) || myValidEdges.Contains(aE))
        {
          continue;
        }

        // check if this edges is not created from vertex and mark it as invalid
        const NCollection_List<TopoDS_Shape>* pLEOr = myEdgesOrigins->Seek(aE);
        if (!pLEOr)
          continue;
        NCollection_List<TopoDS_Shape>::Iterator itLEO(*pLEOr);
        for (; itLEO.More(); itLEO.Next())
        {
          if (itLEO.Value().ShapeType() != TopAbs_VERTEX)
            break;
        }
        if (!itLEO.More())
        {
          myInvalidEdges.Add(aE);
        }
      }
    }
  }
}

//=======================================================================
// function : FilterInvalidEdges
// purpose  : Filtering the invalid edges according to currently invalid faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::FilterInvalidEdges(
  const BRepOffset_DataMapOfShapeIndexedMapOfShape&                    theDMFMIE,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMERemoved,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInside,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&              theMEUseInRebuild)
{
  TopoDS_Compound                                               aCEInv;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
  BRep_Builder                                                  aBB;
  aBB.MakeCompound(aCEInv);
  NCollection_List<TopoDS_Shape>::Iterator aItLF;
  //
  int i, aNb = myInvalidFaces.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLFInv = myInvalidFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFIm = aItLF.Value();
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEInv);
      //
      TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next())
      {
        const TopoDS_Shape& aE = aExpE.Current();
        if (myInvalidEdges.Contains(aE))
        {
          aBB.Add(aCEInv, aE);
        }
      }
    }
  }
  //
  // remove edges which have been marked for removal
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInvToAvoid;
  NCollection_List<TopoDS_Shape>                                aLCBE;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
  //
  NCollection_List<TopoDS_Shape>::Iterator aItLCBE(aLCBE);
  for (; aItLCBE.More(); aItLCBE.Next())
  {
    const TopoDS_Shape& aCBE = aItLCBE.Value();
    TopExp_Explorer     aExpCB(aCBE, TopAbs_EDGE);
    for (; aExpCB.More(); aExpCB.Next())
    {
      const TopoDS_Shape& aE = aExpCB.Current();
      if (!theMERemoved.Contains(aE))
      {
        break;
      }
    }
    //
    if (!aExpCB.More())
    {
      TopExp::MapShapes(aCBE, TopAbs_EDGE, aMEInvToAvoid);
    }
  }
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aReallyInvEdges;
  //
  aNb = myInvalidFaces.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aF = myInvalidFaces.FindKey(i);
    if (myArtInvalidFaces.IsBound(aF))
    {
      if (const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* aMIE =
            theDMFMIE.Seek(aF))
      {
        const int aNbIE = aMIE->Extent();
        for (int iE = 1; iE <= aNbIE; ++iE)
        {
          const TopoDS_Shape& aE = aMIE->FindKey(iE);
          if (aMEInv.Contains(aE) && !aMEInvToAvoid.Contains(aE))
          {
            aReallyInvEdges.Add(aE);
          }
        }
      }
    }
    else
    {
      const NCollection_List<TopoDS_Shape>& aLFInv = myInvalidFaces(i);
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next())
      {
        const TopoDS_Shape& aFIm = aItLF.Value();
        TopExp_Explorer     aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next())
        {
          const TopoDS_Shape& aE = aExpE.Current();
          if (myInvalidEdges.Contains(aE) && !aMEInvToAvoid.Contains(aE))
          {
            aReallyInvEdges.Add(aE);
          }
        }
      }
    }
  }

  myInvalidEdges = aReallyInvEdges;

  // Check if any of the currently invalid edges may be used for
  // rebuilding splits of invalid faces.
  // For that the edge should be inside and not connected to invalid
  // boundary edges of the same origin.

  aNb = myInvalidEdges.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aE = myInvalidEdges(i);
    if (!theMEInside.Contains(aE) || !myValidEdges.Contains(aE))
    {
      continue;
    }

    const NCollection_List<TopoDS_Shape>* pEOrigins = myOEOrigins.Seek(aE);
    if (!pEOrigins)
    {
      continue;
    }

    bool bHasInvOutside = false;
    for (NCollection_List<TopoDS_Shape>::Iterator itEOr(*pEOrigins);
         !bHasInvOutside && itEOr.More();
         itEOr.Next())
    {
      if (const NCollection_List<TopoDS_Shape>* pEIms = myOEImages.Seek(itEOr.Value()))
      {
        for (NCollection_List<TopoDS_Shape>::Iterator itEIms(*pEIms);
             !bHasInvOutside && itEIms.More();
             itEIms.Next())
        {
          bHasInvOutside =
            myInvalidEdges.Contains(itEIms.Value()) && !theMEInside.Contains(itEIms.Value());
        }
      }
    }
    if (!bHasInvOutside)
    {
      theMEUseInRebuild.Add(aE);
    }
  }
}

//=======================================================================
// function : FindFacesToRebuild
// purpose  : Looking for the faces that have to be rebuilt:
//           1. Faces close to invalidity
//           2. Faces containing some invalid parts
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindFacesToRebuild()
{
  int i, aNb = myOFImages.Extent();
  if (!aNb)
  {
    return;
  }
  //
  bool                                                   bRebuild;
  NCollection_List<TopoDS_Shape>::Iterator               aItLF;
  NCollection_List<TopoDS_Shape>                         aLEValid;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence, aMEReb, aMFReb;
  TopExp_Explorer                                        aExp;
  //
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aDMFLV;
  // get edges from invalid faces
  aNb = myInvalidFaces.Extent();
  for (i = 1; i <= aNb; i++)
  {
    const TopoDS_Shape& aF = myInvalidFaces.FindKey(i);
    aMFence.Clear();
    NCollection_List<TopoDS_Shape>        aLVAvoid;
    const NCollection_List<TopoDS_Shape>& aLFIm = myInvalidFaces(i);
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aExp.Init(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        aMEReb.Add(aE);
        if (myInvalidEdges.Contains(aE))
        {
          TopExp_Explorer aExpV(aE, TopAbs_VERTEX);
          for (; aExpV.More(); aExpV.Next())
          {
            const TopoDS_Shape& aV = aExpV.Current();
            if (aMFence.Add(aV))
            {
              aLVAvoid.Append(aV);
              aMEReb.Add(aV);
            }
          }
        }
      }
    }
    //
    if (aLVAvoid.Extent())
    {
      aDMFLV.Bind(aF, aLVAvoid);
    }
    //
    const NCollection_List<TopoDS_Shape>* pLF =
      !myArtInvalidFaces.IsBound(aF) ? mySSInterfs.Seek(aF) : mySSInterfsArt.Seek(aF);
    if (pLF)
    {
      NCollection_List<TopoDS_Shape>::Iterator aItLFE(*pLF);
      for (; aItLFE.More(); aItLFE.Next())
      {
        const TopoDS_Shape& aFE = aItLFE.Value();
        aMFReb.Add(aFE);
      }
    }
  }
  //
  // get face to rebuild
  aNb = myOFImages.Extent();
  for (i = 1; i <= aNb; i++)
  {
    const TopoDS_Shape&                                    aF    = myOFImages.FindKey(i);
    const NCollection_List<TopoDS_Shape>&                  aLFIm = myOFImages(i);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVAvoid;
    if (aDMFLV.IsBound(aF))
    {
      const NCollection_List<TopoDS_Shape>&    aLVAvoid = aDMFLV.Find(aF);
      NCollection_List<TopoDS_Shape>::Iterator aItLV(aLVAvoid);
      for (; aItLV.More(); aItLV.Next())
      {
        const TopoDS_Shape& aV = aItLV.Value();
        aMVAvoid.Add(aV);
      }
    }
    //
    bRebuild = aMFReb.Contains(aF);
    aLEValid.Clear();
    aMFence.Clear();
    //
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aExp.Init(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Edge& anEIm = TopoDS::Edge(aExp.Current());
        if (!myInvalidEdges.Contains(anEIm))
        {
          if (aMFence.Add(anEIm))
          {
            aLEValid.Append(anEIm);
          }
        }
        //
        if (!bRebuild)
        {
          bRebuild = aMEReb.Contains(anEIm);
        }
        //
        if (!bRebuild)
        {
          // check vertices
          TopExp_Explorer aExpV(anEIm, TopAbs_VERTEX);
          for (; aExpV.More() && !bRebuild; aExpV.Next())
          {
            const TopoDS_Shape& aV = aExpV.Current();
            if (!aMVAvoid.Contains(aV))
            {
              bRebuild = aMEReb.Contains(aV);
            }
          }
        }
      }
    }
    //
    if (!bRebuild)
    {
      bRebuild = aLFIm.Extent() && myInvalidFaces.Contains(aF);
      if (bRebuild)
      {
        myFSelfRebAvoid.Add(aF);
      }
    }
    //
    if (bRebuild)
    {
      myFacesToRebuild.Add(aF, aLEValid);
    }
  }
}

namespace
{
//=======================================================================
// function : mapShapes
// purpose  : Collect theVecShapes into theMap with theType set
//=======================================================================
template <class Container>
static void mapShapes(const Container&                                        theVecShapes,
                      const TopAbs_ShapeEnum                                  theType,
                      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  for (const auto& aShape : theVecShapes)
  {
    for (TopExp_Explorer anExp(aShape, theType); anExp.More(); anExp.Next())
    {
      theMap.Add(anExp.Current());
    }
  }
}
} // namespace

//=======================================================================
// function : IntersectFaces
// purpose  : Intersection of the faces that should be rebuild to resolve all invalidities
//=======================================================================
void BRepOffset_BuildOffsetFaces::IntersectFaces(
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsToAvoid,
  const Message_ProgressRange&                            theRange)
{
  int aNbFR = myFacesToRebuild.Extent();
  if (!aNbFR)
  {
    return;
  }

  Message_ProgressScope aPSOuter(theRange, "Rebuilding invalid faces", 10);
  //
  int                                      i, j, k, aNbInv;
  NCollection_List<TopoDS_Shape>::Iterator aItLF, aItLE;
  //
  // get vertices from invalid edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVInv, aMVInvAll;
  aNbInv = myInvalidEdges.Extent();
  for (i = 1; i <= aNbInv; ++i)
  {
    const TopoDS_Shape& aEInv  = myInvalidEdges(i);
    bool                bValid = myValidEdges.Contains(aEInv);
    for (TopExp_Explorer aExp(aEInv, TopAbs_VERTEX); aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aV = aExp.Current();
      aMVInvAll.Add(aV);
      if (!bValid)
      {
        aMVInv.Add(aV);
      }
    }
  }
  //
  bool bLookVertToAvoid = (aMVInv.Extent() > 0);
  //
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aDMSF,
    aMDone, aMEInfETrim, aDMVEFull;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aFLE, aDMEFInv;
  //
  // Add all faces to rebuild to outgoing map <aFLE>,
  // plus link edges and vertices to the faces to
  // define intersection faces
  PrepareFacesForIntersection(bLookVertToAvoid,
                              aFLE,
                              aMDone,
                              aDMSF,
                              aMEInfETrim,
                              aDMVEFull,
                              aDMEFInv);

  // Find vertices to avoid while trimming the edges.
  // These vertices are taken from the invalid edges common between
  // splits of different invalid, but not artificially, faces.
  // Additional condition for these vertices is that all
  // edges adjacent to this vertex must be either invalid
  // or contained in invalid faces
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVRInv = theVertsToAvoid;
  FindVerticesToAvoid(aDMEFInv, aDMVEFull, aMVRInv);
  //
  aPSOuter.Next();
  if (!aPSOuter.More())
  {
    return;
  }

  // The faces should be intersected selectively -
  // intersect only faces neighboring to the same invalid face
  // and connected to its invalid edges;
  // when dealing with artificially invalid faces for intersection to be
  // complete we need to use not only invalid edges, but also the
  // edges connected to invalid ones

  // find blocks of artificially invalid faces
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDMFImF;
  TopoDS_Compound                                                          aCFArt;
  BRep_Builder().MakeCompound(aCFArt);
  BRepOffset_DataMapOfShapeIndexedMapOfShape::Iterator aItM(myArtInvalidFaces);
  for (; aItM.More(); aItM.Next())
  {
    const TopoDS_Shape&                   aF     = aItM.Key();
    const NCollection_List<TopoDS_Shape>& aLFInv = myInvalidFaces.FindFromKey(aF);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next())
    {
      BRep_Builder().Add(aCFArt, aItLF.Value());
      aDMFImF.Bind(aItLF.Value(), aF);
    }
  }
  //
  // make connexity blocks
  NCollection_List<TopoDS_Shape> aLCBArt;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFArt, TopAbs_VERTEX, TopAbs_FACE, aLCBArt);
  //
  // alone edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEAlone, aMEInvOnArt;
  //
  Message_ProgressScope                    aPSArt(aPSOuter.Next(), nullptr, aLCBArt.Extent());
  NCollection_List<TopoDS_Shape>::Iterator aItLCBArt(aLCBArt);
  for (; aItLCBArt.More(); aItLCBArt.Next(), aPSArt.Next())
  {
    if (!aPSArt.More())
    {
      return;
    }
    const TopoDS_Shape& aCB = aItLCBArt.Value();
    //
    // check if aCB contains splits of only one offset face
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFArt;
    TopExp_Explorer                                        aExpF(aCB, TopAbs_FACE);
    for (; aExpF.More(); aExpF.Next())
    {
      aMFArt.Add(aDMFImF.Find(aExpF.Current()));
    }
    //
    bool bAlone = (aMFArt.Extent() == 1);
    //
    // vertices on invalid edges
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVEInv;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
    // edges that should not be marked as alone - edges having same origins as invalid ones
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEAvoid;
    // map to find alone edges by looking for free vertices
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aDMVEVal;
    //
    TopExp_Explorer aExpE(aCB, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next())
    {
      const TopoDS_Shape& aE = aExpE.Current();
      if (myInvalidEdges.Contains(aE))
      {
        aMEInvOnArt.Add(aE);
        for (TopoDS_Iterator aItV(aE); aItV.More(); aItV.Next())
        {
          aMVEInv.Add(aItV.Value());
        }
        //
        if (bAlone)
        {
          const NCollection_List<TopoDS_Shape>* pLEOr = myOEOrigins.Seek(aE);
          if (pLEOr)
          {
            NCollection_List<TopoDS_Shape>::Iterator aItLEOr(*pLEOr);
            for (; aItLEOr.More(); aItLEOr.Next())
            {
              NCollection_List<TopoDS_Shape>::Iterator aItLEIm(myOEImages.Find(aItLEOr.Value()));
              for (; aItLEIm.More(); aItLEIm.Next())
              {
                aMEAvoid.Add(aItLEIm.Value());
              }
            }
          }
        }
        continue;
      }
      //
      if (aMFence.Add(aE))
      {
        TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aDMVEVal);
      }
    }
    //
    // find edges with free vertices
    int aNbV = aDMVEVal.Extent();
    for (i = 1; i <= aNbV; ++i)
    {
      const TopoDS_Shape& aV = aDMVEVal.FindKey(i);
      if (!aMVEInv.Contains(aV))
      {
        continue;
      }
      //
      const NCollection_List<TopoDS_Shape>& aLEV = aDMVEVal(i);
      if (aLEV.Extent() > 1)
      {
        continue;
      }
      //
      const TopoDS_Shape& aE = aLEV.First();
      if (aMEAvoid.Contains(aE))
      {
        continue;
      }
      //
      aMEAlone.Add(aE);
      //
      // if this alone edge adds nothing to the intersection list
      // it means that the origin of this edge has been split and we need to
      // add the neighboring images of the same origins
      if (aDMSF.Find(aE).Extent() > 1)
      {
        continue;
      }
      //
      // check also its vertices
      TopoDS_Iterator aItE(aE);
      for (; aItE.More(); aItE.Next())
      {
        const TopoDS_Shape& aVE = aItE.Value();
        if (aDMSF.Find(aVE).Extent() > 2)
        {
          break;
        }
      }
      //
      if (aItE.More())
      {
        continue;
      }
      //
      // the edge is useless - look for other images
      const NCollection_List<TopoDS_Shape>* pLEOr = myOEOrigins.Seek(aE);
      if (!pLEOr)
      {
        continue;
      }
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLEOr(*pLEOr);
      for (; aItLEOr.More(); aItLEOr.Next())
      {
        const TopoDS_Shape& aEOr = aItLEOr.Value();
        //
        const NCollection_List<TopoDS_Shape>&    aLEIm = myOEImages.Find(aEOr);
        NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEIm);
        for (; aItLEIm.More(); aItLEIm.Next())
        {
          const TopoDS_Shape& aEIm = aItLEIm.Value();
          //
          if (aMFence.Contains(aEIm))
          {
            aMEAlone.Add(aEIm);
          }
        }
      }
    }
  }
  //
  // Get all invalidities from all faces to be used for avoiding
  // repeated usage of the common edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMAllInvs;
  aNbInv = myInvalidFaces.Extent();
  for (k = 1; k <= aNbInv; ++k)
  {
    NCollection_List<TopoDS_Shape>::Iterator aIt(myInvalidFaces(k));
    for (; aIt.More(); aIt.Next())
    {
      TopExp_Explorer aExp(aIt.Value(), TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        if (myInvalidEdges.Contains(aE) || aMEAlone.Contains(aE))
        {
          aMAllInvs.Add(aE);
          TopoDS_Iterator aItV(aE);
          for (; aItV.More(); aItV.Next())
          {
            aMAllInvs.Add(aItV.Value());
          }
        }
      }
    }
  }
  //
  // Bounding vertices of not trimmed edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVBounds;
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMECheckExt;
  // Save connections between not trimmed edge and its trimmed parts
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMEETrim;
  // Splits of the new edges
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
               aEImages;
  BRep_Builder aBB;

  // Keep connection between blocks of invalid edges to the lists of
  // found edges to be intersected for its treatment
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMOENEdges;

  aNbInv = myInvalidFaces.Extent();
  Message_ProgressScope aPSInter(aPSOuter.Next(5), nullptr, aNbInv);
  for (k = 1; k <= aNbInv; ++k)
  {
    if (!aPSInter.More())
    {
      return;
    }
    const TopoDS_Shape&                   aFInv         = myInvalidFaces.FindKey(k);
    bool                                  bSelfRebAvoid = myFSelfRebAvoid.Contains(aFInv);
    const NCollection_List<TopoDS_Shape>& aLFInv        = myInvalidFaces(k);
    //
    NCollection_List<TopoDS_Shape> aLCB;
    if (aLFInv.Extent() > 1)
    {
      // make compound of invalid faces
      TopoDS_Compound aCFInv;
      aBB.MakeCompound(aCFInv);
      //
      NCollection_List<TopoDS_Shape>::Iterator aIt(aLFInv);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aFIm = aIt.Value();
        aBB.Add(aCFInv, aFIm);
      }
      //
      // make connexity blocks
      BOPTools_AlgoTools::MakeConnexityBlocks(aCFInv, TopAbs_EDGE, TopAbs_FACE, aLCB);
    }
    else
    {
      aLCB = aLFInv;
    }
    //
    Message_ProgressScope                    aPSCB(aPSInter.Next(), nullptr, aLCB.Extent());
    bool                                     bArtificial = myArtInvalidFaces.IsBound(aFInv);
    NCollection_List<TopoDS_Shape>::Iterator aItLCB(aLCB);
    for (; aItLCB.More(); aItLCB.Next())
    {
      if (!aPSCB.More())
      {
        return;
      }
      const TopoDS_Shape& aCBInv = aItLCB.Value();
      //
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEFence;
      //
      TopoDS_Compound aCBE;
      aBB.MakeCompound(aCBE);
      //
      // remember inside edges and vertices to further check
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anInsideEdges;
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anInsideVertices;
      TopExp_Explorer                                        aExp(aCBInv, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        if (myInvalidEdges.Contains(aE) || (bArtificial && aMEAlone.Contains(aE)))
        {
          if (aMEFence.Add(aE))
          {
            aBB.Add(aCBE, aE);
            if (!myEdgesToAvoid.Contains(aE) && myInvalidEdges.Contains(aE))
            {
              anInsideEdges.Add(aE);
              TopoDS_Iterator anIt(aE);
              for (; anIt.More(); anIt.Next())
              {
                anInsideVertices.Add(anIt.Value());
              }
            }
          }
        }
      }
      //
      // make connexity blocks of edges
      NCollection_List<TopoDS_Shape> aLCBE;
      BOPTools_AlgoTools::MakeConnexityBlocks(aCBE, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
      //
      Message_ProgressScope                    aPSCBE(aPSCB.Next(), nullptr, aLCBE.Extent());
      NCollection_List<TopoDS_Shape>::Iterator aItLCBE(aLCBE);
      for (; aItLCBE.More(); aItLCBE.Next())
      {
        if (!aPSCBE.More())
        {
          return;
        }
        const TopoDS_Shape& aCBELoc = aItLCBE.Value();
        //
        // map of edges and vertices of processing invalidity
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aME;
        // map of vertices to trim the new edges
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMECV;
        TopExp::MapShapes(aCBELoc, TopAbs_EDGE, aME);
        aMECV = aME;
        TopExp::MapShapes(aCBELoc, TopAbs_VERTEX, aME);
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFInt;
        // additional faces for intersection
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFIntExt;
        // splits of faces for intersection
        NCollection_List<TopoDS_Shape> aLFInt;
        // faces to avoid intersection
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMFAvoid;
        //
        FindFacesForIntersection(aFInv,
                                 aME,
                                 aDMSF,
                                 aMVInvAll,
                                 bArtificial,
                                 aMFAvoid,
                                 aMFInt,
                                 aMFIntExt,
                                 aLFInt);
        if (aMFInt.Extent() < 3)
        {
          // nothing to intersect
          aPSCBE.Next();
          continue;
        }
        //
        const NCollection_DataMap<TopoDS_Shape,
                                  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>,
                                  TopTools_ShapeMapHasher>* pMFInter =
          myIntersectionPairs.Seek(aFInv);
        // intersect the faces, but do not intersect the invalid ones
        // among each other (except for the artificially invalid faces)
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEToInt;
        int                                                           aNb = aMFInt.Extent();
        Message_ProgressScope aPSIntPair(aPSCBE.Next(), nullptr, aNb);
        for (i = 1; i <= aNb; ++i, aPSIntPair.Next())
        {
          if (!aPSIntPair.More())
          {
            return;
          }
          const TopoDS_Face& aFi = TopoDS::Face(aMFInt(i));
          if (bSelfRebAvoid && aFi.IsSame(aFInv))
          {
            continue;
          }
          //
          const NCollection_List<TopoDS_Shape>* aLFImi = myOFImages.Seek(aFi);
          if (!aLFImi)
            continue;
          //
          NCollection_List<TopoDS_Shape>* aLFEi = aFLE.ChangeSeek(aFi);
          if (!aLFEi)
            continue;
          //
          NCollection_List<TopoDS_Shape>& aLFDone = aMDone.ChangeFind(aFi);
          //
          const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* pInterFi =
            !pMFInter ? nullptr : pMFInter->Seek(aFi);
          if (pMFInter && !pInterFi)
            continue;

          // create map of edges and vertices for aLFImi
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEVIm;
          mapShapes(*aLFImi, TopAbs_EDGE, aMEVIm);
          mapShapes(*aLFImi, TopAbs_VERTEX, aMEVIm);

          bool isIContainsE = NCollection_MapAlgo::HasIntersection(aMEVIm, anInsideEdges);
          bool isIContainsV = NCollection_MapAlgo::HasIntersection(aMEVIm, anInsideVertices);

          for (j = i + 1; j <= aNb; ++j)
          {
            const TopoDS_Face& aFj = TopoDS::Face(aMFInt(j));
            if (bSelfRebAvoid && aFj.IsSame(aFInv))
            {
              continue;
            }
            //
            if (pInterFi && !pInterFi->Contains(aFj))
              continue;

            const NCollection_List<TopoDS_Shape>* aLFImj = myOFImages.Seek(aFj);
            if (!aLFImj)
              continue;
            //
            NCollection_List<TopoDS_Shape>* aLFEj = aFLE.ChangeSeek(aFj);
            if (!aLFEj)
              continue;

            // create map of edges and vertices for aLFImi
            aMEVIm.Clear();
            mapShapes(*aLFImj, TopAbs_EDGE, aMEVIm);
            mapShapes(*aLFImj, TopAbs_VERTEX, aMEVIm);
            // check images of both faces contain anInsideEdges and anInsideVertices
            // not process if false and true
            bool isJContainsE = NCollection_MapAlgo::HasIntersection(aMEVIm, anInsideEdges);
            bool isJContainsV = NCollection_MapAlgo::HasIntersection(aMEVIm, anInsideVertices);

            // Check if one face is connected to inside edge then
            // the other must be also connected
            if ((isIContainsE && !isJContainsV) || (isJContainsE && !isIContainsV))
            {
              NCollection_List<TopoDS_Shape> aLVC;
              // it is necessary to process the images if they already have
              // common vertices
              FindCommonParts(*aLFImi, *aLFImj, aLVC, TopAbs_VERTEX);

              if (aLVC.IsEmpty())
                continue;
            }
            //
            // if there are some common edges between faces
            // we should use these edges and do not intersect again.
            NCollection_List<TopoDS_Shape> aLEC;
            FindCommonParts(*aLFImi, *aLFImj, aLEC);
            //
            if (aLEC.Extent())
            {
              // no need to intersect if we have common edges between faces
              bool bForceUse = aMFIntExt.Contains(aFi) || aMFIntExt.Contains(aFj);
              ProcessCommonEdges(aLEC,
                                 aME,
                                 aMEInfETrim,
                                 aMAllInvs,
                                 bForceUse,
                                 aMECV,
                                 aMECheckExt,
                                 aDMEETrim,
                                 *aLFEi,
                                 *aLFEj,
                                 aMEToInt);

              // Add common vertices not belonging to the common edges for trimming the intersection
              // edges
              NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMVOnCE;
              for (NCollection_List<TopoDS_Shape>::Iterator itE(aLEC); itE.More(); itE.Next())
              {
                TopExp::MapShapes(itE.Value(), TopAbs_VERTEX, aMVOnCE);
              }

              NCollection_List<TopoDS_Shape> aLEV;
              FindCommonParts(*aLFImi, *aLFImj, aLEV, TopAbs_VERTEX);
              for (NCollection_List<TopoDS_Shape>::Iterator itV(aLEV); itV.More(); itV.Next())
              {
                if (!aMVOnCE.Contains(itV.Value()))
                {
                  aMECV.Add(itV.Value());
                }
              }
              continue;
            }
            //
            // check if both these faces are invalid and sharing edges
            if (myInvalidFaces.Contains(aFi) && myInvalidFaces.Contains(aFj)
                && !myArtInvalidFaces.IsBound(aFi) && !myArtInvalidFaces.IsBound(aFj))
            {
              continue;
            }
            //
            // check if these two faces have already been treated
            aItLE.Initialize(aLFDone);
            for (; aItLE.More(); aItLE.Next())
            {
              const TopoDS_Shape& aF = aItLE.Value();
              if (aF.IsSame(aFj))
              {
                break;
              }
            }
            //
            if (aItLE.More())
            {
              // use intersection line obtained on the previous steps
              // plus, find new origins for these lines
              UpdateIntersectedFaces(aFInv,
                                     aFi,
                                     aFj,
                                     aLFInv,
                                     *aLFImi,
                                     *aLFImj,
                                     *aLFEi,
                                     *aLFEj,
                                     aMEToInt);
              continue;
            }
            //
            if (aMFAvoid.Contains(aFi) || aMFAvoid.Contains(aFj))
            {
              continue;
            }
            //
            aLFDone.Append(aFj);
            aMDone.ChangeFind(aFj).Append(aFi);
            //
            IntersectFaces(aFInv,
                           aFi,
                           aFj,
                           aLFInv,
                           *aLFImi,
                           *aLFImj,
                           *aLFEi,
                           *aLFEj,
                           aMECV,
                           aMEToInt);
          }
        }
        //
        // intersect and trim edges for this chain
        IntersectAndTrimEdges(aMFInt,
                              aMEToInt,
                              aDMEETrim,
                              aME,
                              aMECV,
                              aMVInv,
                              aMVRInv,
                              aMECheckExt,
                              bArtificial ? &mySSInterfsArt : nullptr,
                              aMVBounds,
                              aEImages);
        //
        int iE, aNbEToInt = aMEToInt.Extent();
        for (iE = 1; iE <= aNbEToInt; ++iE)
        {
          const TopoDS_Shape& aEInt = aMEToInt(iE);
          TopExp_Explorer     anExpE(aCBELoc, TopAbs_EDGE);
          for (; anExpE.More(); anExpE.Next())
          {
            const TopoDS_Shape&             aE       = anExpE.Current();
            NCollection_List<TopoDS_Shape>* pLEToInt = aDMOENEdges.ChangeSeek(aE);
            if (!pLEToInt)
              pLEToInt = &aDMOENEdges(aDMOENEdges.Add(aE, NCollection_List<TopoDS_Shape>()));
            AppendToList(*pLEToInt, aEInt);
          }
        }
      }
    }
  }
  //
  // filter the obtained edges
  UpdateValidEdges(aFLE,
                   aDMOENEdges,
                   aMVBounds,
                   aMEInvOnArt,
                   aMECheckExt,
                   theVertsToAvoid,
                   aEImages,
                   aDMEETrim,
                   aPSOuter.Next(3));
}

//=======================================================================
// function : PrepareFacesForIntersection
// purpose  : Preparation of the maps for analyzing intersections of the faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::PrepareFacesForIntersection(
  const bool theLookVertToAvoid,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theFLE,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMDone,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMSF,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMEInfETrim,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMVEFull,
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMEFInv)
{
  int i, aNb = myFacesToRebuild.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aF = myFacesToRebuild.FindKey(i);
    //
    NCollection_List<TopoDS_Shape> aLE;
    theFLE.Add(aF, aLE);
    theMDone.Bind(aF, aLE);
    //
    const NCollection_List<TopoDS_Shape>&    aLFIm = myOFImages.FindFromKey(aF);
    NCollection_List<TopoDS_Shape>::Iterator aItLF(aLFIm);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aFIm = aItLF.Value();
      TopExp_Explorer     aExp(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        // save connection to untrimmed face
        NCollection_List<TopoDS_Shape>* pLF = theDMSF.ChangeSeek(aE);
        if (!pLF)
        {
          pLF = theDMSF.Bound(aE, NCollection_List<TopoDS_Shape>());
        }
        AppendToList(*pLF, aF);
        //
        // save connection to untrimmed edge
        const TopoDS_Shape&             aEInf   = myETrimEInf->Find(aE);
        NCollection_List<TopoDS_Shape>* pLETrim = theMEInfETrim.ChangeSeek(aEInf);
        if (!pLETrim)
        {
          pLETrim = theMEInfETrim.Bound(aEInf, NCollection_List<TopoDS_Shape>());
        }
        AppendToList(*pLETrim, aE);
        //
        TopExp_Explorer aExpV(aE, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next())
        {
          const TopoDS_Shape& aV = aExpV.Current();
          // save connection to face
          NCollection_List<TopoDS_Shape>* pLFV = theDMSF.ChangeSeek(aV);
          if (!pLFV)
          {
            pLFV = theDMSF.Bound(aV, NCollection_List<TopoDS_Shape>());
          }
          AppendToList(*pLFV, aF);
          //
          if (theLookVertToAvoid)
          {
            // save connection to edges
            NCollection_List<TopoDS_Shape>* pLEV = theDMVEFull.ChangeSeek(aV);
            if (!pLEV)
            {
              pLEV = theDMVEFull.Bound(aV, NCollection_List<TopoDS_Shape>());
            }
            AppendToList(*pLEV, aE);
          }
        }
      }
    }
    //
    if (theLookVertToAvoid)
    {
      // get edges of invalid faces (from invalid splits only)
      const NCollection_List<TopoDS_Shape>* pLFInv = myInvalidFaces.Seek(aF);
      if (!pLFInv || myArtInvalidFaces.IsBound(aF))
      {
        continue;
      }
      //
      aItLF.Initialize(*pLFInv);
      for (; aItLF.More(); aItLF.Next())
      {
        const TopoDS_Shape& aFInv = aItLF.Value();
        TopExp_Explorer     aExp(aFInv, TopAbs_EDGE);
        for (; aExp.More(); aExp.Next())
        {
          const TopoDS_Shape&             aE  = aExp.Current();
          NCollection_List<TopoDS_Shape>* pLF = theDMEFInv.ChangeSeek(aE);
          if (!pLF)
          {
            pLF = &theDMEFInv(theDMEFInv.Add(aE, NCollection_List<TopoDS_Shape>()));
          }
          AppendToList(*pLF, aF);
        }
      }
    }
  }
}

//=======================================================================
// function : FindVerticesToAvoid
// purpose  : Looking for the invalid vertices
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindVerticesToAvoid(
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>& theDMEFInv,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                          theDMVEFull,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVRInv)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  int                                                    i, aNb = theDMEFInv.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLFInv = theDMEFInv(i);
    if (aLFInv.Extent() == 1)
    {
      continue;
    }
    //
    const TopoDS_Shape& aE = theDMEFInv.FindKey(i);
    if (!myInvalidEdges.Contains(aE) || myValidEdges.Contains(aE))
    {
      continue;
    }

    if (!aMFence.Add(aE))
      continue;

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      aMVEEdges;
    // Do not check the splitting vertices, but check only the ending ones
    const NCollection_List<TopoDS_Shape>* pLEOr = myOEOrigins.Seek(aE);
    if (pLEOr)
    {
      NCollection_List<TopoDS_Shape>::Iterator aItLEOr(*pLEOr);
      for (; aItLEOr.More(); aItLEOr.Next())
      {
        const NCollection_List<TopoDS_Shape>&    aLEIm = myOEImages.Find(aItLEOr.Value());
        NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEIm);
        for (; aItLEIm.More(); aItLEIm.Next())
        {
          aMFence.Add(aItLEIm.Value());
          TopExp::MapShapesAndAncestors(aItLEIm.Value(), TopAbs_VERTEX, TopAbs_EDGE, aMVEEdges);
        }
      }
    }
    else
    {
      TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVEEdges);
    }

    int j, aNbV = aMVEEdges.Extent();
    for (j = 1; j <= aNbV; ++j)
    {
      if (aMVEEdges(j).Extent() != 1)
        continue;

      const TopoDS_Shape& aV = aMVEEdges.FindKey(j);
      if (!aMFence.Add(aV))
        continue;
      const NCollection_List<TopoDS_Shape>* pLE = theDMVEFull.Seek(aV);
      if (!pLE)
      {
        // isolated vertex
        theMVRInv.Add(aV);
        continue;
      }
      //
      // If all edges sharing the vertex are either invalid or
      // the vertex is connected to at least two inverted edges
      // mark the vertex to be avoided in the new splits
      int                                      iNbEInverted = 0;
      bool                                     bAllEdgesInv = true;
      NCollection_List<TopoDS_Shape>::Iterator aItLE(*pLE);
      for (; aItLE.More(); aItLE.Next())
      {
        const TopoDS_Shape& aEV = aItLE.Value();

        if (myInvertedEdges.Contains(aEV))
          ++iNbEInverted;

        if (bAllEdgesInv)
          bAllEdgesInv = myInvalidEdges.Contains(aEV);
      }

      if (iNbEInverted > 1 || bAllEdgesInv)
      {
        theMVRInv.Add(aV);
      }
    }
  }
}

//=======================================================================
// function : FindFacesForIntersection
// purpose  : Looking for the faces around each invalidity for intersection
//=======================================================================
void BRepOffset_BuildOffsetFaces::FindFacesForIntersection(
  const TopoDS_Shape&                                                  theFInv,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theME,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                 theDMSF,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theMVInvAll,
  const bool                                                     theArtCase,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFAvoid,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInt,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFIntExt,
  NCollection_List<TopoDS_Shape>&                                theLFImInt)
{
  int i, aNbE = theME.Extent();
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMShapes;
  //
  for (i = 1; i <= aNbE; ++i)
  {
    const TopoDS_Shape& aS = theME(i);
    if (!theDMSF.IsBound(aS))
    {
      continue;
    }
    //
    // in artificial case we intersect the faces which are close to invalidity
    bool bAvoid =
      theArtCase ? ((aS.ShapeType() == TopAbs_VERTEX) && !theMVInvAll.Contains(aS)) : false;
    //
    const NCollection_List<TopoDS_Shape>&    aLF = theDMSF.Find(aS);
    NCollection_List<TopoDS_Shape>::Iterator aItLF(aLF);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aF = aItLF.Value();
      if (theMFInt.Contains(aF))
      {
        continue;
      }
      //
      if (bAvoid && myArtInvalidFaces.IsBound(aF))
      {
        theMFAvoid.Add(aF);
      }
      //
      theMFInt.Add(aF);
      //
      bool bUse = !aF.IsSame(theFInv);
      //
      const NCollection_List<TopoDS_Shape>&    aLFIm = myOFImages.FindFromKey(aF);
      NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next())
      {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        theLFImInt.Append(aFIm);
        if (bUse)
        {
          TopExp::MapShapes(aFIm, TopAbs_EDGE, aMShapes);
        }
      }
    }
  }
  //
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                        aSSInterfsMap = theArtCase ? mySSInterfsArt : mySSInterfs;
  const NCollection_List<TopoDS_Shape>* pLFInv        = aSSInterfsMap.Seek(theFInv);
  if (!pLFInv)
  {
    return;
  }
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMF;
  NCollection_List<TopoDS_Shape>::Iterator               aItLF(*pLFInv);
  for (; aItLF.More(); aItLF.Next())
  {
    const TopoDS_Shape& aF = aItLF.Value();
    aMF.Add(aF);
  }
  //
  // the faces should be unique in each place
  TopoDS_Compound aCF;
  BRep_Builder().MakeCompound(aCF);
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            aMFToAdd;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDMFOr;
  //
  for (i = 1; i <= aNbE; ++i)
  {
    const TopoDS_Shape&                   aS  = theME(i);
    const NCollection_List<TopoDS_Shape>* pLF = aSSInterfsMap.Seek(aS);
    if (!pLF)
    {
      continue;
    }
    //
    aItLF.Initialize(*pLF);
    for (; aItLF.More(); aItLF.Next())
    {
      const TopoDS_Shape& aF = aItLF.Value();
      if (theMFInt.Contains(aF) || aMFToAdd.Contains(aF) || !aMF.Contains(aF))
      {
        continue;
      }
      //
      // check if the face has some connection to already added for intersection faces
      const NCollection_List<TopoDS_Shape>& aLFIm = myOFImages.FindFromKey(aF);
      if (!theArtCase)
      {
        NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
        for (; aItLFIm.More(); aItLFIm.Next())
        {
          const TopoDS_Shape& aFIm = aItLFIm.Value();
          TopExp_Explorer     aExp(aFIm, TopAbs_EDGE);
          for (; aExp.More(); aExp.Next())
          {
            if (aMShapes.Contains(aExp.Current()))
            {
              break;
            }
          }
          if (aExp.More())
          {
            break;
          }
        }
        if (!aItLFIm.More())
        {
          continue;
        }
      }
      //
      aMFToAdd.Add(aF);
      NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next())
      {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        aDMFOr.Bind(aFIm, aF);
        BRep_Builder().Add(aCF, aFIm);
      }
    }
  }
  //
  if (aMFToAdd.IsEmpty())
  {
    return;
  }
  //
  NCollection_List<TopoDS_Shape> aLCB;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCF, TopAbs_EDGE, TopAbs_FACE, aLCB);
  //
  if ((aLCB.Extent() == 1) && (aMFToAdd.Extent() > 1))
  {
    return;
  }
  //
  NCollection_List<TopoDS_Shape>::Iterator aItLCB(aLCB);
  for (; aItLCB.More(); aItLCB.Next())
  {
    const TopoDS_Shape& aCB = aItLCB.Value();
    aMFToAdd.Clear();
    TopExp_Explorer aExpF(aCB, TopAbs_FACE);
    for (; aExpF.More(); aExpF.Next())
    {
      const TopoDS_Shape& aFIm = aExpF.Current();
      aMFToAdd.Add(aDMFOr.Find(aFIm));
    }
    //
    if (aMFToAdd.Extent() == 1)
    {
      const TopoDS_Shape& aF = aMFToAdd(1);
      //
      theMFInt.Add(aF);
      theMFIntExt.Add(aF);
      //
      const NCollection_List<TopoDS_Shape>&    aLFIm = myOFImages.FindFromKey(aF);
      NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next())
      {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        theLFImInt.Append(aFIm);
      }
    }
  }
}

//=======================================================================
// function : ProcessCommonEdges
// purpose  : Analyzing the common edges between splits of offset faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::ProcessCommonEdges(
  const NCollection_List<TopoDS_Shape>&                                theLEC,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theME,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                 theMEInfETrim,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&  theAllInvs,
  const bool                                                     theForceUse,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMECV,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theMECheckExt,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                 theDMEETrim,
  NCollection_List<TopoDS_Shape>&                                theLFEi,
  NCollection_List<TopoDS_Shape>&                                theLFEj,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEToInt)
{
  NCollection_List<TopoDS_Shape> aLEC;
  // process common edges
  NCollection_List<TopoDS_Shape>::Iterator aItLE(theLEC);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aEC = aItLE.Value();
    //
    // check first if common edges are valid
    if (myInvalidEdges.Contains(aEC) && !myValidEdges.Contains(aEC))
    {
      continue;
    }
    //
    // common edge should have connection to current invalidity
    if (theME.Contains(aEC))
    {
      aLEC.Append(aEC);
      continue;
    }
    //
    TopoDS_Iterator aItV(aEC);
    for (; aItV.More(); aItV.Next())
    {
      const TopoDS_Shape& aVE = aItV.Value();
      if (theME.Contains(aVE))
      {
        aLEC.Append(aEC);
        break;
      }
    }
  }
  //
  bool bUseOnlyInf = aLEC.IsEmpty();
  if (bUseOnlyInf)
  {
    if (theForceUse)
    {
      aLEC = theLEC;
    }
    else
    {
      aItLE.Initialize(theLEC);
      for (; aItLE.More(); aItLE.Next())
      {
        const TopoDS_Shape& aEC = aItLE.Value();
        // check if all images of the origin of this edge
        // are not connected to any invalidity
        const TopoDS_Shape&                      aEInt = myETrimEInf->Find(aEC);
        const NCollection_List<TopoDS_Shape>&    aLVE  = theMEInfETrim.Find(aEInt);
        NCollection_List<TopoDS_Shape>::Iterator aItLVE(aLVE);
        for (; aItLVE.More(); aItLVE.Next())
        {
          const TopoDS_Shape& aECx = aItLVE.Value();
          if (theAllInvs.Contains(aECx) || myInvalidEdges.Contains(aECx))
          {
            return;
          }
          //
          TopoDS_Iterator aItV(aECx);
          for (; aItV.More(); aItV.Next())
          {
            if (theAllInvs.Contains(aItV.Value()))
            {
              return;
            }
          }
        }
        // use only one element
        if (aLEC.IsEmpty())
        {
          aLEC.Append(aEC);
        }
      }
    }
  }
  //
  aItLE.Initialize(aLEC);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aEC = aItLE.Value();
    //
    const TopoDS_Shape& aEInt = myETrimEInf->Find(aEC);
    if (!bUseOnlyInf)
    {
      // find the edges of the same original edge
      // and take their vertices as well
      const NCollection_List<TopoDS_Shape>&    aLVE = theMEInfETrim.Find(aEInt);
      NCollection_List<TopoDS_Shape>::Iterator aItLVE(aLVE);
      for (; aItLVE.More(); aItLVE.Next())
      {
        const TopoDS_Shape& aECx = aItLVE.Value();
        //
        const NCollection_List<TopoDS_Shape>* pLEOr = myOEOrigins.Seek(aECx);
        if (!pLEOr || (pLEOr->Extent() == 1))
        {
          TopExp::MapShapes(aECx, TopAbs_VERTEX, theMECV);
        }
      }
      //
      // bind unlimited edge to its trimmed part in face to update maps of
      // images and origins in the future
      NCollection_List<TopoDS_Shape>* pLTAdded = theDMEETrim.ChangeSeek(aEInt);
      if (!pLTAdded)
      {
        pLTAdded = theDMEETrim.Bound(aEInt, NCollection_List<TopoDS_Shape>());
      }
      AppendToList(*pLTAdded, aEC);
    }
    else if (!theForceUse)
    {
      theMECheckExt.Add(aEInt);
    }
    //
    AppendToList(theLFEi, aEInt);
    AppendToList(theLFEj, aEInt);
    theMEToInt.Add(aEInt);
  }
}

namespace
{
//=======================================================================
// function : FindOrigins
// purpose  : Looking for the origin edges
//=======================================================================
static void FindOrigins(
  const NCollection_List<TopoDS_Shape>&                                theLFIm1,
  const NCollection_List<TopoDS_Shape>&                                theLFIm2,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theME,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                  theOrigins,
  NCollection_List<TopoDS_Shape>& theLEOr)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  for (int i = 0; i < 2; ++i)
  {
    const NCollection_List<TopoDS_Shape>&    aLF = !i ? theLFIm1 : theLFIm2;
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLF);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aF = aIt.Value();
      //
      TopExp_Explorer aExp(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aE = aExp.Current();
        //
        if (theME.Contains(aE) && theOrigins.IsBound(aE))
        {
          const NCollection_List<TopoDS_Shape>&    aLEOr = theOrigins.Find(aE);
          NCollection_List<TopoDS_Shape>::Iterator aItLE(aLEOr);
          for (; aItLE.More(); aItLE.Next())
          {
            const TopoDS_Shape& aEOr = aItLE.Value();
            //
            if (aMFence.Add(aEOr) && (aEOr.ShapeType() == TopAbs_EDGE))
            {
              theLEOr.Append(aEOr);
            }
          }
        }
      }
    }
  }
}
} // namespace

//=======================================================================
// function : UpdateIntersectedFaces
// purpose  : Updating the already interfered faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::UpdateIntersectedFaces(
  const TopoDS_Shape&                                            theFInv,
  const TopoDS_Shape&                                            theFi,
  const TopoDS_Shape&                                            theFj,
  const NCollection_List<TopoDS_Shape>&                          theLFInv,
  const NCollection_List<TopoDS_Shape>&                          theLFImi,
  const NCollection_List<TopoDS_Shape>&                          theLFImj,
  const NCollection_List<TopoDS_Shape>&                          theLFEi,
  const NCollection_List<TopoDS_Shape>&                          theLFEj,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEToInt)
{
  // Find common edges in these two lists
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEi;
  NCollection_List<TopoDS_Shape>::Iterator               aItLE(theLFEi);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aE = aItLE.Value();
    aMEi.Add(aE);
  }
  //
  // find origins
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEToFindOrigins;
  NCollection_List<TopoDS_Shape>                                aLEToFindOrigins;
  if (!theFi.IsSame(theFInv))
  {
    FindCommonParts(theLFImi, theLFInv, aLEToFindOrigins);
  }
  if (!theFj.IsSame(theFInv))
  {
    FindCommonParts(theLFImj, theLFInv, aLEToFindOrigins);
  }
  //
  NCollection_List<TopoDS_Shape> aLEOrInit;
  aItLE.Initialize(aLEToFindOrigins);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aEC = aItLE.Value();
    aMEToFindOrigins.Add(aEC);
  }
  //
  FindOrigins(theLFImi, theLFImj, aMEToFindOrigins, *myEdgesOrigins, aLEOrInit);
  //
  aItLE.Initialize(theLFEj);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aE = aItLE.Value();
    if (aMEi.Contains(aE))
    {
      theMEToInt.Add(aE);
      if (aLEOrInit.Extent())
      {
        if (myEdgesOrigins->IsBound(aE))
        {
          NCollection_List<TopoDS_Shape>&          aLEOr = myEdgesOrigins->ChangeFind(aE);
          NCollection_List<TopoDS_Shape>::Iterator aItLEOr(aLEOrInit);
          for (; aItLEOr.More(); aItLEOr.Next())
          {
            const TopoDS_Shape& aEOr = aItLEOr.Value();
            AppendToList(aLEOr, aEOr);
          }
        }
        else
        {
          myEdgesOrigins->Bind(aE, aLEOrInit);
        }
      }
    }
  }
}

//=======================================================================
// function : IntersectFaces
// purpose  : Intersection of the pair of faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::IntersectFaces(
  const TopoDS_Shape&                                            theFInv,
  const TopoDS_Shape&                                            theFi,
  const TopoDS_Shape&                                            theFj,
  const NCollection_List<TopoDS_Shape>&                          theLFInv,
  const NCollection_List<TopoDS_Shape>&                          theLFImi,
  const NCollection_List<TopoDS_Shape>&                          theLFImj,
  NCollection_List<TopoDS_Shape>&                                theLFEi,
  NCollection_List<TopoDS_Shape>&                                theLFEj,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMECV,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEToInt)
{
  // intersect faces
  TopAbs_State                   aSide = TopAbs_OUT;
  NCollection_List<TopoDS_Shape> aLInt1, aLInt2;
  TopoDS_Edge                    aNullEdge;
  TopoDS_Face                    aNullFace;
  BRepOffset_Tool::Inter3D(TopoDS::Face(theFi),
                           TopoDS::Face(theFj),
                           aLInt1,
                           aLInt2,
                           aSide,
                           aNullEdge,
                           aNullFace,
                           aNullFace);
  //
  if (aLInt1.IsEmpty())
  {
    return;
  }
  //
  // find common vertices for trimming edges
  NCollection_List<TopoDS_Shape>           aLCV;
  NCollection_List<TopoDS_Shape>::Iterator aItLE;
  FindCommonParts(theLFImi, theLFImj, aLCV, TopAbs_VERTEX);
  if (aLCV.Extent() > 1)
  {
    aItLE.Initialize(aLCV);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Shape& aCV = aItLE.Value();
      theMECV.Add(aCV);
    }
  }
  //
  // find origins
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEToFindOrigins;
  NCollection_List<TopoDS_Shape>                                aLEToFindOrigins;
  if (!theFi.IsSame(theFInv))
  {
    FindCommonParts(theLFImi, theLFInv, aLEToFindOrigins);
  }
  if (!theFj.IsSame(theFInv))
  {
    FindCommonParts(theLFImj, theLFInv, aLEToFindOrigins);
  }
  NCollection_List<TopoDS_Shape> aLEOrInit;
  aItLE.Initialize(aLEToFindOrigins);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aEC = aItLE.Value();
    aMEToFindOrigins.Add(aEC);
  }
  //
  FindOrigins(theLFImi, theLFImj, aMEToFindOrigins, *myEdgesOrigins, aLEOrInit);
  //
  aItLE.Initialize(aLInt1);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape& aEInt = aItLE.Value();
    theLFEi.Append(aEInt);
    theLFEj.Append(aEInt);
    //
    if (aLEOrInit.Extent())
    {
      myEdgesOrigins->Bind(aEInt, aLEOrInit);
    }
    //
    theMEToInt.Add(aEInt);
  }
}

//=======================================================================
// function : IntersectAndTrimEdges
// purpose  : Intersection of the new intersection edges among themselves
//=======================================================================
void BRepOffset_BuildOffsetFaces::IntersectAndTrimEdges(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFInt,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInt,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                       theDMEETrim,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMSInv,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVE,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theVertsToAvoid,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theNewVertsToAvoid,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&        theMECheckExt,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>*
                                                          theSSInterfs,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEImages)
{
  int i, aNb = theMEInt.Extent();
  if (!aNb)
  {
    return;
  }
  //
  NCollection_List<TopoDS_Shape>                         aLArgs;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  NCollection_List<TopoDS_Shape>::Iterator               aIt, aIt1;
  TopExp_Explorer                                        aExp;
  //
  // get vertices from the splits of intersected faces.
  // vertices are taken from the edges close to invalidity
  //
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVE;
  aNb = theMFInt.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape&                   aF  = theMFInt(i);
    const NCollection_List<TopoDS_Shape>& aLE = myFacesToRebuild.FindFromKey(aF);
    //
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aE = aIt.Value();
      TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
      //
      aExp.Init(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next())
      {
        const TopoDS_Shape& aV1 = aExp.Current();
        if (!theVertsToAvoid.Contains(aV1) && theMVE.Contains(aV1) && aMFence.Add(aV1))
        {
          aLArgs.Append(aV1);
        }
      }
    }
  }
  //
  aNb = theMSInv.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aS = theMSInv(i);
    // edge case
    if (theSSInterfs)
    {
      if (const NCollection_List<TopoDS_Shape>* pLV = theSSInterfs->Seek(aS))
      {
        // Add vertices from intersection info to trim section edges of artificial faces
        for (NCollection_List<TopoDS_Shape>::Iterator itLV(*pLV); itLV.More(); itLV.Next())
        {
          if (itLV.Value().ShapeType() == TopAbs_VERTEX)
          {
            aLArgs.Append(itLV.Value());
          }
        }
      }
    }

    // vertex case
    if (const NCollection_List<TopoDS_Shape>* pLVE = aDMVE.ChangeSeek(aS))
    {
      aIt.Initialize(*pLVE);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aE = aIt.Value();
        //
        aExp.Init(aE, TopAbs_VERTEX);
        for (; aExp.More(); aExp.Next())
        {
          const TopoDS_Shape& aV1 = aExp.Current();
          if (!theVertsToAvoid.Contains(aV1) && aMFence.Add(aV1))
          {
            aLArgs.Append(aV1);
          }
        }
      }
    }
  }
  //
  // bounding vertices of untrimmed edges
  NCollection_List<TopoDS_Shape> aLVBounds;
  // new intersection edges
  NCollection_List<TopoDS_Shape> aLENew;
  // get edges to intersect
  NCollection_List<TopoDS_Shape> aLEInt;
  // Common intersection edges. Should be intersected separately
  NCollection_List<TopoDS_Shape> aLCE;
  //
  aNb = theMEInt.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aE = theMEInt(i);
    if (theMECheckExt.Contains(aE))
    {
      // avoid trimming of the intersection edges by additional common edges
      aLCE.Append(aE);
      continue;
    }
    //
    if (!theDMEETrim.IsBound(aE))
    {
      aLENew.Append(aE);
    }
    //
    aLEInt.Append(aE);
    aLArgs.Append(aE);
    //
    aExp.Init(aE, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aV = aExp.Current();
      aLVBounds.Append(aV);
    }
  }
  //
  // Intersect Edges
  BOPAlgo_Builder aGF;
  aGF.SetArguments(aLArgs);
  aGF.Perform();
  if (aGF.HasErrors())
  {
    return;
  }
  //
  // update vertices to avoid with SD vertices
  aIt.Initialize(aLVBounds);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape&                   aV    = aIt.Value();
    const NCollection_List<TopoDS_Shape>& aLVIm = aGF.Modified(aV);
    if (aLVIm.IsEmpty())
    {
      theMVBounds.Add(aV);
    }
    else
    {
      const TopoDS_Shape& aVIm = aLVIm.First();
      theMVBounds.Add(aVIm);
    }
  }
  //
  // find invalid splits of edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
  GetInvalidEdges(theNewVertsToAvoid, theMVBounds, aGF, aMEInv);
  //
  BRep_Builder aBB;
  // get valid splits to intersect with the commons
  TopoDS_Compound aCEIm;
  aBB.MakeCompound(aCEIm);
  //
  // remove the splits containing vertices from invalid edges
  aIt.Initialize(aLEInt);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aE = aIt.Value();
    //
    NCollection_List<TopoDS_Shape> aLEIm = aGF.Modified(aE);
    if (aLEIm.IsEmpty())
    {
      continue;
    }
    //
    aIt1.Initialize(aLEIm);
    for (; aIt1.More();)
    {
      const TopoDS_Shape& aEIm = aIt1.Value();
      //
      if (aMEInv.Contains(aEIm))
      {
        aLEIm.Remove(aIt1);
      }
      else
      {
        aBB.Add(aCEIm, aEIm);
        aIt1.Next();
      }
    }
    //
    if (aLEIm.Extent())
    {
      NCollection_List<TopoDS_Shape>* pLEIm = theEImages.ChangeSeek(aE);
      if (!pLEIm)
      {
        pLEIm = theEImages.Bound(aE, NCollection_List<TopoDS_Shape>());
      }
      pLEIm->Append(aLEIm);
    }
  }
  //
  if (!aLCE.Extent())
  {
    return;
  }
  //
  // trim common edges by other intersection edges
  BOPAlgo_Builder aGFCE;
  aGFCE.SetArguments(aLCE);
  aGFCE.AddArgument(aCEIm);
  aGFCE.Perform();
  //
  if (aGFCE.HasErrors())
  {
    return;
  }
  //
  const BOPDS_PDS&                         pDS = aGFCE.PDS();
  NCollection_List<TopoDS_Shape>::Iterator aItLCE(aLCE);
  for (; aItLCE.More(); aItLCE.Next())
  {
    const TopoDS_Shape&            aE    = aItLCE.Value();
    NCollection_List<TopoDS_Shape> aLEIm = aGFCE.Modified(aE);
    if (aLEIm.IsEmpty())
    {
      continue;
    }
    //
    // check if it's not coincide with some intersection edge
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB(
      pDS->PaveBlocks(pDS->Index(aE)));
    for (; aItLPB.More(); aItLPB.Next())
    {
      if (pDS->IsCommonBlock(aItLPB.Value()))
      {
        // find with what it is a common
        const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPBC =
          pDS->CommonBlock(aItLPB.Value())->PaveBlocks();
        NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPBC(aLPBC);
        for (; aItLPBC.More(); aItLPBC.Next())
        {
          const TopoDS_Shape& aEC = pDS->Shape(aItLPBC.Value()->OriginalEdge());
          if (!theMECheckExt.Contains(aEC))
          {
            break;
          }
        }
        if (aItLPBC.More())
        {
          break;
        }
      }
    }
    if (aItLPB.More())
    {
      // avoid creation of unnecessary splits from commons which
      // coincide with intersection edges
      continue;
    }
    //
    // save the images
    NCollection_List<TopoDS_Shape>* pLEIm = theEImages.ChangeSeek(aE);
    if (!pLEIm)
    {
      pLEIm = theEImages.Bound(aE, NCollection_List<TopoDS_Shape>());
    }
    pLEIm->Append(aLEIm);
    //
    // save bounding vertices
    for (TopoDS_Iterator aItV(aE); aItV.More(); aItV.Next())
    {
      const TopoDS_Shape&                   aV    = aItV.Value();
      const NCollection_List<TopoDS_Shape>& aLVIm = aGFCE.Modified(aV);
      theMVBounds.Add(aLVIm.IsEmpty() ? aV : aLVIm.First());
    }
  }
}

//=======================================================================
// function : GetInvalidEdges
// purpose  : Looking for the invalid edges by intersecting with invalid vertices
//=======================================================================
void BRepOffset_BuildOffsetFaces::GetInvalidEdges(
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsToAvoid,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
  BOPAlgo_Builder&                                              theGF,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMEInv)
{
  if (theVertsToAvoid.IsEmpty())
  {
    return;
  }
  //
  NCollection_List<TopoDS_Shape>::Iterator aIt, aIt1;
  // get vertices created with intersection edges
  const TopoDS_Shape& aRes = theGF.Shape();
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVE;
  TopExp::MapShapesAndAncestors(aRes, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  const BOPDS_PDS& pDS = theGF.PDS();
  //
  // find invalid splits of edges
  // check if the vertex is invalid:
  // a. it may be the vertex SD with the vertices to avoid
  // b. or it may be the vertex which is created by the intersection
  //    of only existing edges, i.e. no new intersection edges goes
  //    through this vertex
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVInv;
  int                                                    i, aNb = aDMVE.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aDMVE.FindKey(i));
    if (theMVBounds.Contains(aV))
    {
      continue;
    }
    //
    int nV = pDS->Index(aV);
    if ((nV >= 0) && !pDS->IsNewShape(nV))
    {
      continue;
    }
    //
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItM(theVertsToAvoid);
    for (; aItM.More(); aItM.Next())
    {
      const TopoDS_Vertex& aVInv = *(TopoDS_Vertex*)&aItM.Value();
      int                  iFlag = BOPTools_AlgoTools::ComputeVV(aV, aVInv);
      if (!iFlag)
      {
        aMVInv.Add(aV);
        break;
      }
    }
    //
    if (aItM.More())
    {
      const NCollection_List<TopoDS_Shape>& aLVE = aDMVE.FindFromKey(aV);
      aIt.Initialize(aLVE);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aE = aIt.Value();
        theMEInv.Add(aE);
      }
    }
  }
}

//=======================================================================
// function : UpdateValidEdges
// purpose  : Making the new splits and updating the maps
//=======================================================================
void BRepOffset_BuildOffsetFaces::UpdateValidEdges(
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&    theFLE,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&    theOENEdges,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInvOnArt,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECheckExt,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theVertsToAvoid,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEImages,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                               theEETrim,
  const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPSOuter(theRange, "Updating edges", 10);
  // update images and origins of edges, plus update AsDes
  //
  // new edges
  NCollection_List<TopoDS_Shape> aLE;
  // back connection from edges to faces
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aMELF;
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMETmp;
  int                                                    i, aNb = theFLE.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Face& aF = TopoDS::Face(theFLE.FindKey(i));
    //
    const NCollection_List<TopoDS_Shape>&    aLEInt = theFLE(i);
    NCollection_List<TopoDS_Shape>::Iterator aItLE(aLEInt);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Shape& aE = aItLE.Value();
      if ((theMECheckExt.Contains(aE) || aMETmp.Contains(aE)) && !theEImages.IsBound(aE))
      {
        theMECheckExt.Remove(aE);
        aMETmp.Add(aE);
        continue;
      }
      NCollection_List<TopoDS_Shape>* pLF = aMELF.ChangeSeek(aE);
      if (!pLF)
      {
        pLF = aMELF.Bound(aE, NCollection_List<TopoDS_Shape>());
        aLE.Append(aE);
      }
      pLF->Append(aF);
    }
  }
  //
  if (aLE.IsEmpty())
  {
    return;
  }
  //
  // bounding edges, that are going to be replaced
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEB;
  //
  // new intersection edges
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMENew;
  // map of old vertices
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVOld;
  // back connection to untrimmed edges
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aDMEOr;
  //
  // trim the new intersection edges
  TrimNewIntersectionEdges(aLE,
                           theEETrim,
                           theMVBounds,
                           theMECheckExt,
                           theEImages,
                           aMEB,
                           aMVOld,
                           aMENew,
                           aDMEOr,
                           aMELF);
  //
  if (theEImages.IsEmpty())
  {
    // No new splits is preserved
    // update intersection edges and exit
    UpdateNewIntersectionEdges(aLE, aMELF, theEImages, theEETrim);
    return;
  }

  aPSOuter.Next();
  if (!aPSOuter.More())
  {
    return;
  }

  BRep_Builder aBB;

  // Make connexity blocks of the invalid edges
  // and treat each block separately

  // Compound of all invalid edges to make the blocks
  TopoDS_Compound aCEAll;
  aBB.MakeCompound(aCEAll);

  int aNbE = theOENEdges.Extent();
  for (i = 1; i <= aNbE; ++i)
    aBB.Add(aCEAll, theOENEdges.FindKey(i));

  // Separate the edges into blocks
  NCollection_List<TopoDS_Shape> aLBlocks;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCEAll, TopAbs_VERTEX, TopAbs_EDGE, aLBlocks);

  // Perform intersection of the new splits for each block

  // Intersected splits
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMBlocksSp;

  Message_ProgressScope                    aPSB(aPSOuter.Next(), nullptr, aLBlocks.Extent());
  NCollection_List<TopoDS_Shape>::Iterator aItLB(aLBlocks);
  for (; aItLB.More(); aItLB.Next(), aPSB.Next())
  {
    if (!aPSB.More())
    {
      return;
    }
    const TopoDS_Shape& aBlock = aItLB.Value();

    // Get the list of new edges for the block
    NCollection_List<TopoDS_Shape> aBlockLENew;
    {
      // Fence map
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEFence;
      TopExp_Explorer                                        anExpE(aBlock, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        const TopoDS_Shape&                      aE     = anExpE.Current();
        const NCollection_List<TopoDS_Shape>&    aLEInt = theOENEdges.FindFromKey(aE);
        NCollection_List<TopoDS_Shape>::Iterator aItLEInt(aLEInt);
        for (; aItLEInt.More(); aItLEInt.Next())
        {
          if (aMEFence.Add(aItLEInt.Value()))
            aBlockLENew.Append(aItLEInt.Value());
        }
      }
    }

    if (aBlockLENew.IsEmpty())
      continue;

    // Get the splits of new edges to intersect
    NCollection_List<TopoDS_Shape> aLSplits;

    NCollection_List<TopoDS_Shape>::Iterator aItLE(aBlockLENew);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Shape&             aE    = aItLE.Value();
      NCollection_List<TopoDS_Shape>* pLEIm = theEImages.ChangeSeek(aE);
      if (!pLEIm || pLEIm->IsEmpty())
        continue;

      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
        aLSplits.Append(aItLEIm.Value());
    }

    if (aLSplits.IsEmpty())
      continue;

    TopoDS_Shape aCE;
    if (aLSplits.Extent() > 1)
      // Intersect the new splits among themselves to avoid self-intersections
      IntersectEdges(aLSplits,
                     aBlockLENew,
                     theMVBounds,
                     theVertsToAvoid,
                     aMENew,
                     theMECheckExt,
                     theEImages,
                     aDMEOr,
                     aMELF,
                     aCE);
    else
      aCE = aLSplits.First();

    aMBlocksSp.Add(aCE, aBlockLENew);
  }

  // Perform filtering of the edges in two steps:
  // - Check each block separately using localized bounds
  //   taken only from the splits of faces of the current block;
  // - Intersect all splits together and filter the splits by all bounds.

  // FIRST STAGE - separate treatment of the blocks

  // Valid splits to be preserved on the first stage
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEVal;

  // Blocks of valid edges on the first stage
  NCollection_List<TopoDS_Shape> aLValBlocks;

  int                   aNbB = aMBlocksSp.Extent();
  Message_ProgressScope aPSBSp(aPSOuter.Next(), nullptr, aNbB);
  for (i = 1; i <= aNbB; ++i, aPSBSp.Next())
  {
    if (!aPSBSp.More())
    {
      return;
    }
    const TopoDS_Shape&                   aCE         = aMBlocksSp.FindKey(i);
    const NCollection_List<TopoDS_Shape>& aBlockLENew = aMBlocksSp(i);

    // Get all participating faces to get the bounds
    NCollection_List<TopoDS_Shape>           aLFaces;
    NCollection_List<TopoDS_Shape>::Iterator aItLE(aBlockLENew);
    for (; aItLE.More(); aItLE.Next())
    {
      const NCollection_List<TopoDS_Shape>* pLF = aMELF.Seek(aItLE.Value());
      if (!pLF)
        continue;
      NCollection_List<TopoDS_Shape>::Iterator aItLF(*pLF);
      for (; aItLF.More(); aItLF.Next())
        AppendToList(aLFaces, aItLF.Value());
    }

    // Localized bounds of the splits of the offset faces
    // to filter the new splits of the current block
    TopoDS_Shape aFilterBounds;
    GetBounds(aLFaces, aMEB, aFilterBounds);

    // Filter the splits by bounds
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInvLoc;
    GetInvalidEdgesByBounds(aCE,
                            aFilterBounds,
                            aMVOld,
                            aMENew,
                            aDMEOr,
                            aMELF,
                            theEImages,
                            theMECheckExt,
                            theMEInvOnArt,
                            theVertsToAvoid,
                            aMEInvLoc);

    // Keep only valid edges of the block
    TopoDS_Compound aCEVal;
    aBB.MakeCompound(aCEVal);

    bool bKept = false;

    TopExp_Explorer anExpE(aCE, TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
    {
      const TopoDS_Shape& aESp = anExpE.Current();
      if (!aMEInvLoc.Contains(aESp) && aMEVal.Add(aESp))
      {
        aBB.Add(aCEVal, aESp);
        bKept = true;
      }
    }

    if (bKept)
      aLValBlocks.Append(aCEVal);
  }

  // Filter the images of edges after the first filtering stage
  TopoDS_Shape aSplits1;
  FilterSplits(aLE, aMEVal, false, theEImages, aSplits1);

  if (aLValBlocks.IsEmpty())
  {
    // update intersection edges
    UpdateNewIntersectionEdges(aLE, aMELF, theEImages, theEETrim);
    return;
  }

  aPSOuter.Next();
  if (!aPSOuter.More())
  {
    return;
  }

  // SECOND STAGE - Filter the remaining splits together

  // Add for intersection already removed new edges using them
  // as markers for other invalid edges
  aNbB = aMBlocksSp.Extent();
  for (i = 1; i <= aNbB; ++i)
  {
    const TopoDS_Shape& aCE = aMBlocksSp.FindKey(i);
    for (TopExp_Explorer anExp(aCE, TopAbs_EDGE); anExp.More(); anExp.Next())
    {
      const TopoDS_Shape& aEIm = anExp.Current();
      if (aMENew.Contains(aEIm) && !aMEVal.Contains(aEIm))
        aLValBlocks.Append(aEIm);
    }
  }

  if (aLValBlocks.Extent() > 1)
    // intersect the new splits among themselves to avoid self-intersections
    IntersectEdges(aLValBlocks,
                   aLE,
                   theMVBounds,
                   theVertsToAvoid,
                   aMENew,
                   theMECheckExt,
                   theEImages,
                   aDMEOr,
                   aMELF,
                   aSplits1);
  else
    aSplits1 = aLValBlocks.First();

  aPSOuter.Next();
  if (!aPSOuter.More())
  {
    return;
  }

  // Get all faces to get the bounds from their splits
  NCollection_List<TopoDS_Shape> aLFaces;
  for (i = 1; i <= myOFImages.Extent(); ++i)
    aLFaces.Append(myOFImages.FindKey(i));

  // Bounds of the splits of the offset faces to filter the new splits
  TopoDS_Shape aFilterBounds;
  GetBounds(aLFaces, aMEB, aFilterBounds);

  // Filter the splits by intersection with bounds
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
  GetInvalidEdgesByBounds(aSplits1,
                          aFilterBounds,
                          aMVOld,
                          aMENew,
                          aDMEOr,
                          aMELF,
                          theEImages,
                          theMECheckExt,
                          theMEInvOnArt,
                          theVertsToAvoid,
                          aMEInv);

  // Filter the images of edges after the second filtering stage
  // and combine all valid edges into a single compound
  TopoDS_Shape aSplits;
  FilterSplits(aLE, aMEInv, true, theEImages, aSplits);

  aPSOuter.Next();
  if (!aPSOuter.More())
  {
    return;
  }

  // get bounds to update
  // we need to update the edges of all the affected faces
  NCollection_List<TopoDS_Shape> aLF;
  // prepare the vertices from new splits of edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMVSp;
  TopExp::MapShapes(aSplits, TopAbs_VERTEX, aMVSp);
  //
  int aNbF = myOFImages.Extent();
  for (i = 1; i <= aNbF; ++i)
  {
    const TopoDS_Shape& aF = myOFImages.FindKey(i);
    if (theFLE.Contains(aF))
    {
      aLF.Append(aF);
      continue;
    }
    //
    // check the splits of faces to have vertices from splits
    const NCollection_List<TopoDS_Shape>&    aLFIm = myOFImages(i);
    NCollection_List<TopoDS_Shape>::Iterator aItLFIm(aLFIm);
    for (; aItLFIm.More(); aItLFIm.Next())
    {
      const TopoDS_Shape& aFIm = aItLFIm.Value();
      //
      TopExp_Explorer aExpV(aFIm, TopAbs_VERTEX);
      for (; aExpV.More(); aExpV.Next())
      {
        const TopoDS_Shape& aV = aExpV.Current();
        if (aMVSp.Contains(aV))
        {
          break;
        }
      }
      //
      if (aExpV.More())
      {
        break;
      }
    }
    //
    if (aItLFIm.More())
    {
      aLF.Append(aF);
    }
  }
  //
  // get bounds from splits of faces of aLF
  TopoDS_Shape                   aBounds;
  NCollection_List<TopoDS_Shape> aLAValid, aLABounds;
  GetBoundsToUpdate(aLF, aMEB, aLABounds, aLAValid, aBounds);
  //
  // Intersect valid splits with bounds and update both
  BOPAlgo_Builder aGF;
  aGF.AddArgument(aBounds);
  aGF.AddArgument(aSplits);
  aGF.Perform(aPSOuter.Next(3));
  //
  // update splits
  UpdateImages(aLE, theEImages, aGF, myModifiedEdges);
  //
  // update new intersection edges
  UpdateNewIntersectionEdges(aLE, aMELF, theEImages, theEETrim);
  //
  // update bounds
  UpdateImages(aLAValid, myOEImages, aGF, myModifiedEdges);
  UpdateOrigins(aLABounds, myOEOrigins, aGF);
  UpdateOrigins(aLABounds, *myEdgesOrigins, aGF);
  UpdateIntersectedEdges(aLABounds, aGF);
  //
  // update the EdgesToAvoid with the splits
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aNewEdges;
  const NCollection_List<TopoDS_Shape>* pSplitsIm = aGF.Images().Seek(aSplits);
  if (pSplitsIm)
  {
    NCollection_List<TopoDS_Shape>::Iterator aItSpIm(*pSplitsIm);
    for (; aItSpIm.More(); aItSpIm.Next())
    {
      TopExp::MapShapes(aItSpIm.Value(), TopAbs_EDGE, aNewEdges);
    }
  }

  TopoDS_Compound anInsideEdges;
  BRep_Builder().MakeCompound(anInsideEdges);
  for (int iE = 1; iE <= myInsideEdges.Extent(); ++iE)
  {
    BRep_Builder().Add(anInsideEdges, myInsideEdges(iE));
  }

  //
  // Rebuild the map of edges to avoid, using the intersection results
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMEAvoid;
  TopoDS_Compound                                               aCEAvoid;
  BRep_Builder().MakeCompound(aCEAvoid);
  // GF's data structure
  const BOPDS_PDS& pDS = aGF.PDS();

  aNbE = myEdgesToAvoid.Extent();
  for (i = 1; i <= aNbE; ++i)
  {
    const TopoDS_Shape&                   aE    = myEdgesToAvoid(i);
    const NCollection_List<TopoDS_Shape>& aLEIm = aGF.Modified(aE);

    // Only untouched and fully coinciding edges should be kept in the avoid map
    bool bKeep = aLEIm.IsEmpty();
    if (aLEIm.Extent() == 1 && aE.IsSame(aLEIm.First()))
    {
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = pDS->PaveBlocks(pDS->Index(aE));
      if (aLPB.Extent() == 1)
      {
        const occ::handle<BOPDS_PaveBlock>&   aPB = aLPB.First();
        const occ::handle<BOPDS_CommonBlock>& aCB = pDS->CommonBlock(aPB);
        if (!aCB.IsNull())
        {
          const NCollection_List<occ::handle<BOPDS_PaveBlock>>&    aLPBCB = aCB->PaveBlocks();
          NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB(aLPBCB);
          for (; aItLPB.More(); aItLPB.Next())
          {
            if (pDS->PaveBlocks(aItLPB.Value()->OriginalEdge()).Extent() > 1)
              break;
          }
          bKeep = !aItLPB.More();
        }
      }
    }

    if (bKeep)
    {
      // keep the original edge
      AddToContainer(aE, aMEAvoid);
      continue;
    }

    NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEIm);
    for (; aItLEIm.More(); aItLEIm.Next())
    {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      if (!aNewEdges.Contains(aEIm))
      {
        AddToContainer(aEIm, aCEAvoid);
      }
    }
  }

  bool isCut = false;
  if (aCEAvoid.NbChildren() > 0)
  {
    // Perform intersection with the small subset of the edges to make
    // it possible to use the inside edges for building new splits.
    BOPAlgo_BOP aBOP;
    aBOP.AddArgument(aCEAvoid);
    aBOP.AddTool(anInsideEdges);
    aBOP.SetOperation(BOPAlgo_CUT);
    aBOP.Perform();
    isCut = !aBOP.HasErrors();

    if (isCut)
    {
      for (TopoDS_Iterator itCE(aCEAvoid); itCE.More(); itCE.Next())
      {
        if (!aBOP.IsDeleted(itCE.Value()))
        {
          aMEAvoid.Add(itCE.Value());
        }
      }
    }
  }

  if (!isCut)
  {
    TopExp::MapShapes(aCEAvoid, TopAbs_EDGE, aMEAvoid);
  }

  myEdgesToAvoid = aMEAvoid;
}

//=================================================================================================

void BRepOffset_BuildOffsetFaces::TrimNewIntersectionEdges(
  const NCollection_List<TopoDS_Shape>& theLE,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                theEETrim,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECheckExt,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                          theEImages,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEB,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVOld,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMENew,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMEOr,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMELF)
{
  NCollection_List<TopoDS_Shape>::Iterator aIt, aIt1;
  aIt.Initialize(theLE);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aE = aIt.Value();
    //
    bool bCheckExt = theMECheckExt.Remove(aE);
    //
    bool bOld = theEETrim.IsBound(aE);
    if (bOld)
    {
      const NCollection_List<TopoDS_Shape>& aLET = theEETrim.Find(aE);
      aIt1.Initialize(aLET);
      for (; aIt1.More(); aIt1.Next())
      {
        const TopoDS_Shape& aET = aIt1.Value();
        theMEB.Add(aET);
        TopExp_Explorer aExpV(aET, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next())
        {
          const TopoDS_Shape& aV = aExpV.Current();
          theMVOld.Add(aV);
        }
      }
    }
    //
    if (!theEImages.IsBound(aE))
    {
      continue;
    }
    //
    NCollection_List<TopoDS_Shape>& aLEIm = theEImages.ChangeFind(aE);
    if (aLEIm.IsEmpty())
    {
      theEImages.UnBind(aE);
      continue;
    }
    //
    TopoDS_Shape                                           aCEIm;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEVBounds;
    //
    if (aLEIm.Extent() > 1)
    {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMV;
      // fuse these parts
      BOPAlgo_Builder                          aGFE;
      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        aGFE.AddArgument(aEIm);
        TopExp::MapShapes(aEIm, TopAbs_VERTEX, aMV);
      }
      //
      // add two bounding vertices of this edge to the operation
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(TopoDS::Edge(aE), aV1, aV2);
      //
      aGFE.AddArgument(aV1);
      aGFE.AddArgument(aV2);
      aMV.Add(aV1);
      aMV.Add(aV2);
      //
      aGFE.Perform();
      if (!aGFE.HasErrors())
      {
        // get images of bounding vertices to remove splits containing them
        // in case some of the bounding edges has been interfered
        // during operation it is necessary to update their images as well
        int iV, aNbV = aMV.Extent();
        for (iV = 1; iV <= aNbV; ++iV)
        {
          const TopoDS_Shape& aV = aMV(iV);
          if (theMVBounds.Contains(aV) || aV.IsSame(aV1) || aV.IsSame(aV2))
          {
            const NCollection_List<TopoDS_Shape>& aLVIm = aGFE.Modified(aV);
            aMEVBounds.Add(aLVIm.IsEmpty() ? aV : aLVIm.First());
          }
        }
        //
        aCEIm = aGFE.Shape();
      }
    }
    else
    {
      aCEIm = aLEIm.First();
    }
    //
    aLEIm.Clear();
    //
    TopExp_Explorer aExp(aCEIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      // check the split not to contain bounding vertices
      TopoDS_Iterator aItV(aEIm);
      for (; aItV.More(); aItV.Next())
      {
        const TopoDS_Shape& aV = aItV.Value();
        if (aMEVBounds.Contains(aV) || theMVBounds.Contains(aV))
        {
          break;
        }
      }
      //
      if (!aItV.More())
      {
        aLEIm.Append(aEIm);
        //
        theDMEOr.Bound(aEIm, NCollection_List<TopoDS_Shape>())->Append(aE);
      }
    }
    //
    if (aLEIm.IsEmpty())
    {
      theEImages.UnBind(aE);
    }
    else
    {
      const NCollection_List<TopoDS_Shape>&    aLFE = theMELF.Find(aE);
      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        const TopoDS_Shape&             aEIm   = aItLEIm.Value();
        NCollection_List<TopoDS_Shape>* pLFEIm = theMELF.ChangeSeek(aEIm);
        if (!pLFEIm)
        {
          pLFEIm = theMELF.Bound(aEIm, NCollection_List<TopoDS_Shape>());
        }
        NCollection_List<TopoDS_Shape>::Iterator aItLF(aLFE);
        for (; aItLF.More(); aItLF.Next())
        {
          AppendToList(*pLFEIm, aItLF.Value());
        }
        //
        if (bCheckExt)
        {
          theMECheckExt.Add(aEIm);
        }
        else if (!bOld)
        {
          theMENew.Add(aEIm);
        }
      }
    }
  }
}

//=======================================================================
// function : IntersectEdges
// purpose  : Intersecting the trimmed edges to avoid self-intersections
//=======================================================================
void BRepOffset_BuildOffsetFaces::IntersectEdges(
  const NCollection_List<TopoDS_Shape>&                         theLA,
  const NCollection_List<TopoDS_Shape>&                         theLE,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVBounds,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsToAvoid,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMENew,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMECheckExt,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEImages,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMEOr,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                theMELF,
  TopoDS_Shape& theSplits)
{
  BOPAlgo_Builder aGFA;
  aGFA.SetArguments(theLA);
  aGFA.Perform();
  if (aGFA.HasErrors())
  {
    // just copy input to the result
    TopoDS_Compound aSp;
    BRep_Builder    aBB;
    aBB.MakeCompound(aSp);
    NCollection_List<TopoDS_Shape>::Iterator anIt(theLA);
    for (; anIt.More(); anIt.Next())
    {
      const TopoDS_Shape& aE = anIt.Value();
      aBB.Add(aSp, aE);
    }
    theSplits = aSp;
    return;
  }
  //
  UpdateImages(theLE, theEImages, aGFA, myModifiedEdges);
  //
  // compound of valid splits
  theSplits = aGFA.Shape();
  //
  NCollection_List<TopoDS_Shape>::Iterator aIt, aIt1;

  // prepare list of edges to update
  NCollection_List<TopoDS_Shape> aLEInput;
  for (aIt.Initialize(theLA); aIt.More(); aIt.Next())
  {
    TopExp_Explorer anExpE(aIt.Value(), TopAbs_EDGE);
    for (; anExpE.More(); anExpE.Next())
      aLEInput.Append(anExpE.Current());
  }

  // update new edges
  aIt.Initialize(aLEInput);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aE = aIt.Value();
    if (!theMENew.Contains(aE))
      continue;

    const NCollection_List<TopoDS_Shape>& aLEIm = aGFA.Modified(aE);
    if (aLEIm.IsEmpty())
      continue;

    theMENew.Remove(aE);
    aIt1.Initialize(aLEIm);
    for (; aIt1.More(); aIt1.Next())
      theMENew.Add(aIt1.Value());
  }
  //
  // update edges after intersection for extended checking
  aIt.Initialize(aLEInput);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape&                   aE    = aIt.Value();
    const NCollection_List<TopoDS_Shape>& aLEIm = aGFA.Modified(aE);
    if (aLEIm.IsEmpty())
    {
      continue;
    }
    //
    if (theMECheckExt.Contains(aE))
    {
      aIt1.Initialize(aLEIm);
      for (; aIt1.More(); aIt1.Next())
      {
        theMECheckExt.Add(aIt1.Value());
      }
      theMECheckExt.Remove(aE);
    }
    //
    const NCollection_List<TopoDS_Shape>& aLFE = theMELF.Find(aE);
    aIt1.Initialize(aLEIm);
    for (; aIt1.More(); aIt1.Next())
    {
      const TopoDS_Shape&             aEIm   = aIt1.Value();
      NCollection_List<TopoDS_Shape>* pLFEIm = theMELF.ChangeSeek(aEIm);
      if (!pLFEIm)
      {
        pLFEIm = theMELF.Bound(aEIm, NCollection_List<TopoDS_Shape>());
      }
      NCollection_List<TopoDS_Shape>::Iterator aItLF(aLFE);
      for (; aItLF.More(); aItLF.Next())
      {
        AppendToList(*pLFEIm, aItLF.Value());
      }
    }
  }
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMEInv;
  GetInvalidEdges(theVertsToAvoid, theMVBounds, aGFA, aMEInv);
  if (aMEInv.Extent())
  {
    // update shape
    TopoDS_Compound aSp;
    BRep_Builder    aBB;
    aBB.MakeCompound(aSp);
    TopExp_Explorer aExp(theSplits, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Shape& aE = aExp.Current();
      if (!aMEInv.Contains(aE))
      {
        aBB.Add(aSp, aE);
      }
    }
    theSplits = aSp;
  }
  //
  // update origins
  UpdateOrigins(aLEInput, theDMEOr, aGFA);
}

//=======================================================================
// function : GetBounds
// purpose  : Getting edges from the splits of offset faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::GetBounds(
  const NCollection_List<TopoDS_Shape>&                         theLFaces,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEB,
  TopoDS_Shape&                                                 theBounds)
{
  BRep_Builder aBB;
  // Make compound of edges contained in the splits of faces
  TopoDS_Compound aBounds;
  aBB.MakeCompound(aBounds);
  // Fence map
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;

  NCollection_List<TopoDS_Shape>::Iterator aItLF(theLFaces);
  for (; aItLF.More(); aItLF.Next())
  {
    const NCollection_List<TopoDS_Shape>* pLFIm = myOFImages.Seek(aItLF.Value());
    if (!pLFIm)
      continue;
    NCollection_List<TopoDS_Shape>::Iterator aIt(*pLFIm);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aFIm = aIt.Value();
      //
      TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next())
      {
        const TopoDS_Shape& aEIm = aExpE.Current();
        if (!theMEB.Contains(aEIm) && aMFence.Add(aEIm))
        {
          aBB.Add(aBounds, aEIm);
        }
      }
    }
  }
  theBounds = aBounds;
}

//=======================================================================
// function : GetBoundsToUpdate
// purpose  : Get bounding edges that should be updated
//=======================================================================
void BRepOffset_BuildOffsetFaces::GetBoundsToUpdate(
  const NCollection_List<TopoDS_Shape>&                         theLF,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEB,
  NCollection_List<TopoDS_Shape>&                               theLABounds,
  NCollection_List<TopoDS_Shape>&                               theLAValid,
  TopoDS_Shape&                                                 theBounds)
{
  // get all edges
  TopoDS_Compound aBounds;
  BRep_Builder    aBB;
  aBB.MakeCompound(aBounds);
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMAValid, aMFence;
  //
  NCollection_List<TopoDS_Shape>::Iterator aItLF(theLF);
  for (; aItLF.More(); aItLF.Next())
  {
    const TopoDS_Shape& aF = aItLF.Value();
    //
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMDE;
    const NCollection_List<TopoDS_Shape>&                         aLFDes = myAsDes->Descendant(aF);
    NCollection_List<TopoDS_Shape>::Iterator                      aItLFDes(aLFDes);
    for (; aItLFDes.More(); aItLFDes.Next())
    {
      const TopoDS_Shape&                   aED    = aItLFDes.Value();
      const NCollection_List<TopoDS_Shape>* pLEDIm = myOEImages.Seek(aED);
      if (!pLEDIm)
      {
        aMDE.Add(aED);
        continue;
      }
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLEDIm(*pLEDIm);
      for (; aItLEDIm.More(); aItLEDIm.Next())
      {
        const TopoDS_Shape& aEDIm = aItLEDIm.Value();
        aMDE.Add(aEDIm);
      }
    }
    //
    int j, aNbE = aMDE.Extent();
    for (j = 1; j <= aNbE; ++j)
    {
      const TopoDS_Edge& aEIm = TopoDS::Edge(aMDE(j));
      //
      if (!theMEB.Contains(aEIm) && aMFence.Add(aEIm))
      {
        aBB.Add(aBounds, aEIm);
        theLABounds.Append(aEIm);
      }
      //
      const NCollection_List<TopoDS_Shape>* pLO = myOEOrigins.Seek(aEIm);
      if (pLO)
      {
        NCollection_List<TopoDS_Shape>::Iterator aItLO(*pLO);
        for (; aItLO.More(); aItLO.Next())
        {
          const TopoDS_Shape& aEO = aItLO.Value();
          //
          if (aMAValid.Add(aEO))
          {
            theLAValid.Append(aEO);
          }
        }
      }
      else
      {
        if (aMAValid.Add(aEIm))
        {
          theLAValid.Append(aEIm);
        }
      }
    }
  }
  theBounds = aBounds;
}

//=======================================================================
// function : GetInvalidEdgesByBounds
// purpose  : Filter new splits by intersection with bounds
//=======================================================================
void BRepOffset_BuildOffsetFaces::GetInvalidEdgesByBounds(
  const TopoDS_Shape&                                           theSplits,
  const TopoDS_Shape&                                           theBounds,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMVOld,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMENew,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theDMEOr,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMELF,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                theEImages,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMECheckExt,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEInvOnArt,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theVertsToAvoid,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMEInv)
{
  // map splits to check the vertices of edges
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aDMVE;
  TopExp::MapShapesAndAncestors(theSplits, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  BOPAlgo_Section aSec;
  aSec.AddArgument(theSplits);
  aSec.AddArgument(theBounds);
  //
  aSec.Perform();
  //
  // invalid vertices
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMVInv;
  // vertices to check additionally by classification relatively to solid
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMVCheckAdd;
  // collect parts for removal
  const BOPDS_PDS& pDS = aSec.PDS();
  //
  // check edge/edge intersections
  const NCollection_Vector<BOPDS_InterfEE>& aEEs = pDS->InterfEE();
  int                                       i, aNb = aEEs.Length();
  for (i = 0; i < aNb; ++i)
  {
    const BOPDS_InterfEE& aEE = aEEs(i);
    //
    const TopoDS_Shape& aE1 = pDS->Shape(aEE.Index1());
    const TopoDS_Shape& aE2 = pDS->Shape(aEE.Index2());
    //
    if (!aEE.HasIndexNew())
    {
      if (theMECheckExt.Contains(aE1) && (aEE.CommonPart().Type() == TopAbs_EDGE))
      {
        theMEInv.Add(aE1);
      }
      continue;
    }
    //
    if (myInvalidEdges.Contains(aE2))
    {
      theMEInv.Add(aE1);
    }
    //
    if (theMEInvOnArt.Contains(aE2))
    {
      // avoid checking of the vertices of the split edge intersected by
      // the invalid edge from artificial face
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(TopoDS::Edge(aE2), aV1, aV2);
      if (aDMVE.Contains(aV1) && aDMVE.Contains(aV2))
      {
        continue;
      }
    }
    //
    // add vertices of all images of the edge from splits for checking
    const NCollection_List<TopoDS_Shape>&    aLEOr = theDMEOr.Find(aE1);
    NCollection_List<TopoDS_Shape>::Iterator aItLEOr(aLEOr);
    for (; aItLEOr.More(); aItLEOr.Next())
    {
      const TopoDS_Shape& aEOr = aItLEOr.Value();
      //
      const NCollection_List<TopoDS_Shape>* pLEIm = theEImages.Seek(aEOr);
      if (!pLEIm)
        continue;
      NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next())
      {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        //
        TopoDS_Iterator aItV(aEIm);
        for (; aItV.More(); aItV.Next())
        {
          const TopoDS_Shape& aV = aItV.Value();
          if (!theMVOld.Contains(aV))
          {
            aMVInv.Add(aV);
            aMVCheckAdd.Add(aV);
          }
        }
      }
    }
  }
  //
  // to avoid unnecessary filling of parts due to extra trim of the edges
  // process Edge/Edge interferences of type EDGE, i.e. common blocks and check
  // not the bounding vertices of the edges, but check the edge itself
  // to be lying on some face
  //
  // all common blocks are contained in the result of SECTION operation
  // between sets of edges
  const TopoDS_Shape& aSecR = aSec.Shape();
  //
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMSSec;
  TopExp::MapShapes(aSecR, aMSSec);
  //
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    anIm = aSec.Images();
  for (TopExp_Explorer aExp(theSplits, TopAbs_EDGE); aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aE = aExp.Current();
    if (aSec.IsDeleted(aE))
    {
      // no common blocks for this edge
      continue;
    }
    //
    const NCollection_List<TopoDS_Shape>* pLEIm = anIm.Seek(aE);
    if (!pLEIm)
    {
      // no splits, i.e. completely coincides with some edge from boundary
      continue;
    }
    //
    NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
    for (; aItLEIm.More(); aItLEIm.Next())
    {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      if (!aMSSec.Contains(aEIm))
      {
        // the edge included in section only partially.
        // the part not included in section may be excessive
        //
        // check vertices of this edge - if one of them is new
        // the edge might be removed
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(TopoDS::Edge(aEIm), aV1, aV2);
        if (!theMVOld.Contains(aV1) || !theMVOld.Contains(aV2))
        {
          // add this edge for checking by making new vertex in the middle of the edge
          TopoDS_Vertex                  aV;
          double                         f, l;
          const occ::handle<Geom_Curve>& aC = BRep_Tool::Curve(TopoDS::Edge(aEIm), f, l);
          BRep_Builder().MakeVertex(aV, aC->Value((f + l) * 0.5), Precision::Confusion());
          // and adding this vertex for checking
          aDMVE.ChangeFromIndex(aDMVE.Add(aV, NCollection_List<TopoDS_Shape>())).Append(aE);
          aMVInv.Add(aV);
          break;
        }
      }
    }
  }
  //
  // Add for check also the edges created from common between splits
  // of offset faces edges not connected to any invalidity.
  // These edges may also accidentally fill some part.
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItM(theMECheckExt);
  for (; aItM.More(); aItM.Next())
  {
    const TopoDS_Shape& aE = aItM.Value();
    //
    // make new vertex in the middle of the edge
    TopoDS_Vertex                  aV;
    double                         f, l;
    const occ::handle<Geom_Curve>& aC = BRep_Tool::Curve(TopoDS::Edge(aE), f, l);
    BRep_Builder().MakeVertex(aV, aC->Value((f + l) * 0.5), Precision::Confusion());
    // add this vertex for checking
    aDMVE.ChangeFromIndex(aDMVE.Add(aV, NCollection_List<TopoDS_Shape>())).Append(aE);
    aMVInv.Add(aV);
  }
  //
  // add for check also the vertices connected only to new or old edges
  aNb = aDMVE.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aV = aDMVE.FindKey(i);
    if (theMVOld.Contains(aV))
    {
      continue;
    }
    //
    bool                                     bNew = false, bOld = false;
    const NCollection_List<TopoDS_Shape>&    aLEx = aDMVE(i);
    NCollection_List<TopoDS_Shape>::Iterator aIt(aLEx);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Shape& aE = aIt.Value();
      if (theMECheckExt.Contains(aE))
      {
        continue;
      }
      //
      if (theMENew.Contains(aE))
      {
        bNew = true;
      }
      else
      {
        bOld = true;
      }
      //
      if (bNew && bOld)
      {
        break;
      }
    }
    //
    if (!bNew || !bOld)
    {
      aMVInv.Add(aV);
      aMVCheckAdd.Remove(aV);
    }
  }
  //
  // perform the checking of the vertices
  int iv, aNbIV = aMVInv.Extent();
  for (iv = 1; iv <= aNbIV; ++iv)
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aMVInv(iv));
    if (theMVOld.Contains(aV))
    {
      continue;
    }
    //
    const NCollection_List<TopoDS_Shape>* pLEInv = aDMVE.Seek(aV);
    if (!pLEInv)
    {
      continue;
    }
    // find faces by the edges to check the vertex
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMF;
    NCollection_List<TopoDS_Shape>::Iterator                      aItLE(*pLEInv);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Shape&                      aE  = aItLE.Value();
      const NCollection_List<TopoDS_Shape>&    aLF = theMELF.Find(aE);
      NCollection_List<TopoDS_Shape>::Iterator aItLF(aLF);
      for (; aItLF.More(); aItLF.Next())
      {
        aMF.Add(aItLF.Value());
      }
    }
    //
    // check the vertex to belong to some split of the faces
    bool bInvalid = true;
    //
    int aNbF = aMF.Extent();
    for (i = 1; i <= aNbF && bInvalid; ++i)
    {
      const TopoDS_Face&                    aF    = TopoDS::Face(aMF(i));
      const NCollection_List<TopoDS_Shape>& aLFIm = myOFImages.FindFromKey(aF);
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLF(aLFIm);
      for (; aItLF.More() && bInvalid; aItLF.Next())
      {
        const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
        TopExp_Explorer    aExp(aFIm, TopAbs_VERTEX);
        for (; aExp.More() && bInvalid; aExp.Next())
        {
          const TopoDS_Shape& aVF = aExp.Current();
          bInvalid                = !aVF.IsSame(aV);
        }
      }
      //
      if (bInvalid)
      {
        double U, V, aTol;
        int    iStatus = myContext->ComputeVF(aV, aF, U, V, aTol);
        if (!iStatus)
        {
          // classify the point relatively faces
          gp_Pnt2d aP2d(U, V);
          aItLF.Initialize(aLFIm);
          for (; aItLF.More() && bInvalid; aItLF.Next())
          {
            const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
            bInvalid                = !myContext->IsPointInOnFace(aFIm, aP2d);
          }
        }
      }
    }
    //
    if (bInvalid && aMVCheckAdd.Contains(aV))
    {
      // the vertex is invalid for all faces
      // check the same vertex for the solids
      const gp_Pnt& aP    = BRep_Tool::Pnt(aV);
      double        aTolV = BRep_Tool::Tolerance(aV);
      //
      TopExp_Explorer aExpS(mySolids, TopAbs_SOLID);
      for (; aExpS.More() && bInvalid; aExpS.Next())
      {
        const TopoDS_Solid&          aSol = TopoDS::Solid(aExpS.Current());
        BRepClass3d_SolidClassifier& aSC  = myContext->SolidClassifier(aSol);
        aSC.Perform(aP, aTolV);
        bInvalid = (aSC.State() == TopAbs_OUT);
      }
    }
    //
    if (bInvalid)
    {
      theVertsToAvoid.Add(aV);
      aItLE.Initialize(*pLEInv);
      for (; aItLE.More(); aItLE.Next())
      {
        theMEInv.Add(aItLE.Value());
      }
    }
  }
}

//=======================================================================
// function : FilterSplits
// purpose  : Filter the images of edges from the invalid edges
//=======================================================================
void BRepOffset_BuildOffsetFaces::FilterSplits(
  const NCollection_List<TopoDS_Shape>&                         theLE,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMEFilter,
  const bool                                                    theIsInv,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                theEImages,
  TopoDS_Shape& theSplits)
{
  TopoDS_Compound aSplits;
  BRep_Builder().MakeCompound(aSplits);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;

  NCollection_List<TopoDS_Shape>::Iterator aItLE(theLE);
  for (; aItLE.More(); aItLE.Next())
  {
    const TopoDS_Shape&             aE    = aItLE.Value();
    NCollection_List<TopoDS_Shape>* pLEIm = theEImages.ChangeSeek(aE);
    if (!pLEIm)
      continue;

    NCollection_List<TopoDS_Shape>::Iterator aItLEIm(*pLEIm);
    for (; aItLEIm.More();)
    {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      if (theMEFilter.Contains(aEIm) == theIsInv)
      {
        pLEIm->Remove(aItLEIm);
        continue;
      }

      if (aMFence.Add(aEIm))
        BRep_Builder().Add(aSplits, aEIm);
      aItLEIm.Next();
    }

    if (pLEIm->IsEmpty())
      theEImages.UnBind(aE);
  }
  theSplits = aSplits;
}

//=======================================================================
// function : UpdateNewIntersectionEdges
// purpose  : Updating the maps of images and origins of the offset edges
//=======================================================================
void BRepOffset_BuildOffsetFaces::UpdateNewIntersectionEdges(
  const NCollection_List<TopoDS_Shape>& theLE,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theMELF,
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEImages,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theEETrim)
{
  NCollection_List<TopoDS_Shape>           aLEImEmpty;
  NCollection_List<TopoDS_Shape>::Iterator aIt, aIt1;
  // update global maps of images and origins with new splits
  aIt.Initialize(theLE);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aE = aIt.Value();
    //
    if (!theEImages.IsBound(aE))
    {
      NCollection_List<TopoDS_Shape>* pLET = theEETrim.ChangeSeek(aE);
      if (!pLET)
      {
        continue;
      }
      //
      NCollection_List<TopoDS_Shape>::Iterator aItLET(*pLET);
      for (; aItLET.More();)
      {
        const TopoDS_Shape& aET = aItLET.Value();
        if (!myInvalidEdges.Contains(aET) && !myInvertedEdges.Contains(aET))
        {
          pLET->Remove(aItLET);
        }
        else
        {
          aItLET.Next();
        }
      }
      //
      if (pLET->IsEmpty())
      {
        continue;
      }
    }
    // new images
    const NCollection_List<TopoDS_Shape>& aLENew =
      theEImages.IsBound(aE) ? theEImages.Find(aE) : aLEImEmpty;
    //
    // save connection to untrimmed edge for the next steps
    aIt1.Initialize(aLENew);
    for (; aIt1.More(); aIt1.Next())
    {
      const TopoDS_Shape& aET = aIt1.Value();
      myETrimEInf->Bind(aET, aE);
      myModifiedEdges.Add(aET);
    }
    //
    // check if it is existing edge
    if (!theEETrim.IsBound(aE))
    {
      const NCollection_List<TopoDS_Shape>& aLF = theMELF.Find(aE);
      // the edge is new
      // add this edge to AsDes
      aIt1.Initialize(aLF);
      for (; aIt1.More(); aIt1.Next())
      {
        const TopoDS_Shape& aF = aIt1.Value();
        myAsDes->Add(aF, aE);
      }
      //
      // add aE to the images
      myOEImages.Bind(aE, aLENew);
      myModifiedEdges.Add(aE);
      //
      // add to origins
      NCollection_List<TopoDS_Shape>::Iterator aItNew(aLENew);
      for (; aItNew.More(); aItNew.Next())
      {
        const TopoDS_Shape& aENew = aItNew.Value();
        if (myOEOrigins.IsBound(aENew))
        {
          NCollection_List<TopoDS_Shape>& aEOrigins = myOEOrigins.ChangeFind(aENew);
          AppendToList(aEOrigins, aE);
        }
        else
        {
          NCollection_List<TopoDS_Shape> aEOrigins;
          aEOrigins.Append(aE);
          myOEOrigins.Bind(aENew, aEOrigins);
        }
      }
      //
      // update connection to initial origins
      if (myEdgesOrigins->IsBound(aE))
      {
        const NCollection_List<TopoDS_Shape>& aLEOrInit = myEdgesOrigins->Find(aE);
        aIt1.Initialize(aLENew);
        for (; aIt1.More(); aIt1.Next())
        {
          const TopoDS_Shape& aENew = aIt1.Value();
          if (myEdgesOrigins->IsBound(aENew))
          {
            NCollection_List<TopoDS_Shape>&          aLENewOr = myEdgesOrigins->ChangeFind(aENew);
            NCollection_List<TopoDS_Shape>::Iterator aItOrInit(aLEOrInit);
            for (; aItOrInit.More(); aItOrInit.Next())
            {
              const TopoDS_Shape& aEOr = aItOrInit.Value();
              AppendToList(aLENewOr, aEOr);
            }
          }
          else
          {
            myEdgesOrigins->Bind(aENew, aLEOrInit);
          }
        }
      }
      //
      continue;
    }
    //
    // old images
    const NCollection_List<TopoDS_Shape>& aLEOld = theEETrim.Find(aE);
    //
    // list of initial origins
    NCollection_List<TopoDS_Shape> anInitOrigins;
    //
    // it is necessary to replace the old edges with new ones
    aIt1.Initialize(aLEOld);
    for (; aIt1.More(); aIt1.Next())
    {
      const TopoDS_Shape& aEOld = aIt1.Value();
      //
      if (myOEOrigins.IsBound(aEOld))
      {
        // get its origins
        const NCollection_List<TopoDS_Shape>& aEOrigins = myOEOrigins.Find(aEOld);
        //
        NCollection_List<TopoDS_Shape>::Iterator aItOr(aEOrigins);
        for (; aItOr.More(); aItOr.Next())
        {
          const TopoDS_Shape& aEOr = aItOr.Value();
          //
          myModifiedEdges.Add(aEOr);
          //
          NCollection_List<TopoDS_Shape>& aEImages = myOEImages.ChangeFind(aEOr);
          //
          // remove old edge from images
          NCollection_List<TopoDS_Shape>::Iterator aItIm(aEImages);
          for (; aItIm.More();)
          {
            const TopoDS_Shape& aEIm = aItIm.Value();
            if (aEIm.IsSame(aEOld))
            {
              aEImages.Remove(aItIm);
            }
            else
            {
              aItIm.Next();
            }
          }
          //
          // add new images
          NCollection_List<TopoDS_Shape>::Iterator aItNew(aLENew);
          for (; aItNew.More(); aItNew.Next())
          {
            const TopoDS_Shape& aENew = aItNew.Value();
            AppendToList(aEImages, aENew);
            if (myOEOrigins.IsBound(aENew))
            {
              NCollection_List<TopoDS_Shape>& aENewOrigins = myOEOrigins.ChangeFind(aENew);
              AppendToList(aENewOrigins, aEOr);
            }
            else
            {
              NCollection_List<TopoDS_Shape> aENewOrigins;
              aENewOrigins.Append(aEOr);
              myOEOrigins.Bind(aENew, aENewOrigins);
            }
          }
        }
      }
      else
      {
        // add to images
        myOEImages.Bind(aEOld, aLENew);
        //
        myModifiedEdges.Add(aEOld);
        //
        // add to origins
        NCollection_List<TopoDS_Shape>::Iterator aItNew(aLENew);
        for (; aItNew.More(); aItNew.Next())
        {
          const TopoDS_Shape& aENew = aItNew.Value();
          if (myOEOrigins.IsBound(aENew))
          {
            NCollection_List<TopoDS_Shape>& aEOrigins = myOEOrigins.ChangeFind(aENew);
            AppendToList(aEOrigins, aEOld);
          }
          else
          {
            NCollection_List<TopoDS_Shape> aEOrigins;
            aEOrigins.Append(aEOld);
            myOEOrigins.Bind(aENew, aEOrigins);
          }
        }
      }
      //
      // update connection to initial shape
      if (myEdgesOrigins->IsBound(aEOld))
      {
        const NCollection_List<TopoDS_Shape>&    aLEOrInit = myEdgesOrigins->Find(aEOld);
        NCollection_List<TopoDS_Shape>::Iterator aItEOrInit(aLEOrInit);
        for (; aItEOrInit.More(); aItEOrInit.Next())
        {
          const TopoDS_Shape& aEOrInit = aItEOrInit.Value();
          AppendToList(anInitOrigins, aEOrInit);
        }
      }
    }
    //
    if (anInitOrigins.Extent())
    {
      NCollection_List<TopoDS_Shape>::Iterator aItNew(aLENew);
      for (; aItNew.More(); aItNew.Next())
      {
        const TopoDS_Shape& aENew = aItNew.Value();
        if (myEdgesOrigins->IsBound(aENew))
        {
          NCollection_List<TopoDS_Shape>&          aLENewOr = myEdgesOrigins->ChangeFind(aENew);
          NCollection_List<TopoDS_Shape>::Iterator aItOrInit(anInitOrigins);
          for (; aItOrInit.More(); aItOrInit.Next())
          {
            const TopoDS_Shape& aEOr = aItOrInit.Value();
            AppendToList(aLENewOr, aEOr);
          }
        }
        else
        {
          myEdgesOrigins->Bind(aENew, anInitOrigins);
        }
      }
    }
  }
}

//=======================================================================
// function : FillGaps
// purpose  : Fill possible gaps (holes) in the splits of the offset faces
//=======================================================================
void BRepOffset_BuildOffsetFaces::FillGaps(const Message_ProgressRange& theRange)
{
  int aNbF = myOFImages.Extent();
  if (!aNbF)
    return;

  Message_ProgressScope aPS(theRange, "Filling gaps", 2 * aNbF);

  // Check the splits of offset faces on the free edges and fill the gaps (holes)
  // in created splits, otherwise the closed volume will not be possible to create.

  // Map the splits of faces to find free edges
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    anEFMap;
  for (int i = 1; i <= aNbF; ++i, aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }

    NCollection_List<TopoDS_Shape>::Iterator itLF(myOFImages(i));
    for (; itLF.More(); itLF.Next())
      TopExp::MapShapesAndAncestors(itLF.Value(), TopAbs_EDGE, TopAbs_FACE, anEFMap);
  }

  // Analyze images of each offset face on the presence of free edges
  // and try to fill the holes
  for (int i = 1; i <= aNbF; ++i, aPS.Next())
  {
    if (!aPS.More())
    {
      return;
    }

    NCollection_List<TopoDS_Shape>& aLFImages = myOFImages(i);
    if (aLFImages.IsEmpty())
      continue;

    // Collect all edges from the splits
    TopoDS_Compound anEdges;
    BRep_Builder().MakeCompound(anEdges);

    // Collect all free edges into a map with reverted orientation
    NCollection_Map<TopoDS_Shape>            aFreeEdgesMap;
    NCollection_List<TopoDS_Shape>::Iterator itLF(aLFImages);
    for (; itLF.More(); itLF.Next())
    {
      const TopoDS_Shape& aFIm = itLF.Value();
      TopExp_Explorer     anExpE(aFIm, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        const TopoDS_Shape& aE = anExpE.Current();
        if (aE.Orientation() != TopAbs_FORWARD && aE.Orientation() != TopAbs_REVERSED)
          // Skip internals
          continue;

        const NCollection_List<TopoDS_Shape>& aLF = anEFMap.FindFromKey(aE);
        if (aLF.Extent() == 1)
          aFreeEdgesMap.Add(aE.Reversed());

        BRep_Builder().Add(anEdges, aE);
      }
    }

    if (aFreeEdgesMap.IsEmpty())
      // No free edges
      continue;

    // Free edges are found - fill the gaps by creating new splits
    // of the face using these free edges
    const TopoDS_Shape& aF = myOFImages.FindKey(i);

    // Build new splits using all kept edges and among new splits
    // find those containing free edges
    NCollection_List<TopoDS_Shape>                                           aLFNew;
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aDummy;

    BuildSplitsOfFace(TopoDS::Face(aF), anEdges, aDummy, aLFNew);

    // Find faces filling holes
    itLF.Initialize(aLFNew);
    for (; itLF.More(); itLF.Next())
    {
      const TopoDS_Shape& aFNew = itLF.Value();
      TopExp_Explorer     anExpE(aFNew, TopAbs_EDGE);
      for (; anExpE.More(); anExpE.Next())
      {
        const TopoDS_Shape& aE = anExpE.Current();
        if (aFreeEdgesMap.Contains(aE))
        {
          // Add face to splits
          aLFImages.Append(aFNew);
          break;
        }
      }
    }
  }
}

//=======================================================================
// function : FillHistory
// purpose  : Saving obtained results in history tools
//=======================================================================
void BRepOffset_BuildOffsetFaces::FillHistory()
{
  int aNbF = myOFImages.Extent();
  if (!aNbF)
  {
    return;
  }

#ifdef OFFSET_DEBUG
  // Build compound of faces to see preliminary result
  TopoDS_Compound aDFaces;
  BRep_Builder().MakeCompound(aDFaces);
#endif

  // Map of kept edges
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgesMap;

  // Fill history for faces
  for (int i = 1; i <= aNbF; ++i)
  {
    const NCollection_List<TopoDS_Shape>& aLFImages = myOFImages(i);
    if (aLFImages.IsEmpty())
    {
      continue;
    }

    // Add the splits to history map
    const TopoDS_Shape& aF = myOFImages.FindKey(i);
    if (myImage->HasImage(aF))
      myImage->Add(aF, aLFImages);
    else
      myImage->Bind(aF, aLFImages);

    // Collect edges from splits
    NCollection_List<TopoDS_Shape>::Iterator itLF(aLFImages);
    for (; itLF.More(); itLF.Next())
    {
      const TopoDS_Shape& aFIm = itLF.Value();
      TopExp::MapShapes(aFIm, TopAbs_EDGE, anEdgesMap);

#ifdef OFFSET_DEBUG
      BRep_Builder().Add(aDFaces, aFIm);
#endif
    }
  }

  // Fill history for edges (iteration by the map is safe because the
  // order is not important here)
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator aItEIm(myOEImages);
  for (; aItEIm.More(); aItEIm.Next())
  {
    const TopoDS_Shape&                   aE    = aItEIm.Key();
    const NCollection_List<TopoDS_Shape>& aLEIm = aItEIm.Value();

    bool                                     bHasImage = myImage->HasImage(aE);
    NCollection_List<TopoDS_Shape>::Iterator aItLE(aLEIm);
    for (; aItLE.More(); aItLE.Next())
    {
      const TopoDS_Shape& aEIm = aItLE.Value();
      if (anEdgesMap.Contains(aEIm))
      {
        if (bHasImage)
        {
          myImage->Add(aE, aEIm);
        }
        else
        {
          myImage->Bind(aE, aEIm);
          bHasImage = true;
        }
      }
    }
  }
}

//=======================================================================
// function : BuildSplitsOfTrimmedFaces
// purpose  : Building splits of already trimmed faces
//=======================================================================
void BRepOffset_MakeOffset::BuildSplitsOfTrimmedFaces(const NCollection_List<TopoDS_Shape>& theLF,
                                                      const occ::handle<BRepAlgo_AsDes>& theAsDes,
                                                      BRepAlgo_Image&                    theImage,
                                                      const Message_ProgressRange&       theRange)
{
  BRepOffset_BuildOffsetFaces aBFTool(theImage);
  aBFTool.SetFaces(theLF);
  aBFTool.SetAsDesInfo(theAsDes);
  aBFTool.BuildSplitsOfTrimmedFaces(theRange);
}

//=======================================================================
// function : BuildSplitsOfExtendedFaces
// purpose  : Building splits of not-trimmed offset faces.
//           For the cases in which invalidity will be found,
//           these invalidities will be rebuilt.
//=======================================================================
void BRepOffset_MakeOffset::BuildSplitsOfExtendedFaces(
  const NCollection_List<TopoDS_Shape>& theLF,
  const BRepOffset_Analyse&             theAnalyse,
  const occ::handle<BRepAlgo_AsDes>&    theAsDes,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                            theEdgesOrigins,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theFacesOrigins,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theETrimEInf,
  BRepAlgo_Image&                                                           theImage,
  const Message_ProgressRange&                                              theRange)
{
  BRepOffset_BuildOffsetFaces aBFTool(theImage);
  aBFTool.SetFaces(theLF);
  aBFTool.SetAsDesInfo(theAsDes);
  aBFTool.SetAnalysis(theAnalyse);
  aBFTool.SetEdgesOrigins(theEdgesOrigins);
  aBFTool.SetFacesOrigins(theFacesOrigins);
  aBFTool.SetInfEdges(theETrimEInf);
  aBFTool.BuildSplitsOfExtendedFaces(theRange);
}
