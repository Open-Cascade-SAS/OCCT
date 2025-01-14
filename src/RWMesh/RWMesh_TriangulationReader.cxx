// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <RWMesh_TriangulationReader.hxx>

#include <Message.hxx>
#include <RWMesh_TriangulationSource.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWMesh_TriangulationReader, Standard_Transient)

namespace
{
  //! Forms string with loading statistic.
  static TCollection_AsciiString loadingStatistic (const TCollection_AsciiString& thePrefix,
                                                   const Standard_Integer theExpectedNodesNb,
                                                   const Standard_Integer theLoadedNodesNb,
                                                   const Standard_Integer theExpectedTrianglesNb,
                                                   const Standard_Integer theDegeneratedTrianglesNb,
                                                   const Standard_Integer theLoadedTrianglesNb)
  {
    TCollection_AsciiString aNodesInfo;
    if (theExpectedNodesNb != theLoadedNodesNb)
    {
      aNodesInfo = TCollection_AsciiString("Nodes: ") + theExpectedNodesNb + " expected / ";
      aNodesInfo += TCollection_AsciiString(theLoadedNodesNb) + " loaded.";
    }
    TCollection_AsciiString aTrianglesInfo;
    if (theExpectedTrianglesNb != theLoadedTrianglesNb)
    {
      if (!aNodesInfo.IsEmpty())
      {
        aNodesInfo += " ";
      }
      aTrianglesInfo = TCollection_AsciiString("Triangles: ") + theExpectedTrianglesNb + " expected / ";
      if (theDegeneratedTrianglesNb != 0)
      {
        aTrianglesInfo += TCollection_AsciiString(theDegeneratedTrianglesNb) + " skipped degenerated / ";
      }
      aTrianglesInfo += TCollection_AsciiString(theLoadedTrianglesNb) + " loaded.";
    }
    if (aNodesInfo.IsEmpty() && aTrianglesInfo.IsEmpty())
    {
      return TCollection_AsciiString();
    }
    return thePrefix + ("Disconformity of the expected number of nodes/triangles for deferred mesh to the loaded amount. ")
           + aNodesInfo + aTrianglesInfo;
  }
}

// =======================================================================
// function : PrintStatistic
// purpose  :
// =======================================================================
void RWMesh_TriangulationReader::LoadingStatistic::PrintStatistic (const TCollection_AsciiString& thePrefix) const
{
  TCollection_AsciiString aStatisticInfo = loadingStatistic (thePrefix, ExpectedNodesNb, LoadedNodesNb,
                                                             ExpectedTrianglesNb, DegeneratedTrianglesNb, LoadedTrianglesNb);
  if (!aStatisticInfo.IsEmpty())
  {
    Message::SendWarning (aStatisticInfo);
  }
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
RWMesh_TriangulationReader::RWMesh_TriangulationReader()
: myLoadingStatistic(NULL),
  myIsDoublePrecision(false),
  myToSkipDegenerateTris(false),
  myToPrintDebugMessages(false)
{
}

// =======================================================================
// function : Destructor
// purpose  :
// =======================================================================
RWMesh_TriangulationReader::~RWMesh_TriangulationReader()
{
  delete myLoadingStatistic;
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::Load (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                       const Handle(RWMesh_TriangulationSource)& theDestMesh,
                                       const Handle(OSD_FileSystem)& theFileSystem) const
{
  Standard_ASSERT_RETURN (!theDestMesh.IsNull(), "The destination mesh should be initialized before loading data to it", false);
  theDestMesh->Clear();
  theDestMesh->SetDoublePrecision (myIsDoublePrecision);

  if (!load (theSourceMesh, theDestMesh, theFileSystem))
  {
    theDestMesh->Clear();
    return false;
  }
  if (!finalizeLoading (theSourceMesh, theDestMesh))
  {
    theDestMesh->Clear();
    return false;
  }
  return true;
}

// =======================================================================
// function : finalizeLoading
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::finalizeLoading (const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                                  const Handle(RWMesh_TriangulationSource)& theDestMesh) const
{
  if (!theSourceMesh->CachedMinMax().IsVoid())
  {
    theDestMesh->SetCachedMinMax (theSourceMesh->CachedMinMax());
  }
  if (myLoadingStatistic)
  {
    Standard_Mutex::Sentry aLock(myMutex);
    myLoadingStatistic->ExpectedNodesNb += theSourceMesh->NbDeferredNodes();
    myLoadingStatistic->ExpectedTrianglesNb += theSourceMesh->NbDeferredTriangles();
    myLoadingStatistic->DegeneratedTrianglesNb += theSourceMesh->DegeneratedTriNb();
    myLoadingStatistic->LoadedNodesNb += theDestMesh->NbNodes();
    myLoadingStatistic->LoadedTrianglesNb += theDestMesh->NbTriangles();
  }
  else if (myToPrintDebugMessages)
  {
    TCollection_AsciiString aStatisticInfo = loadingStatistic (TCollection_AsciiString("[Mesh reader. File '") + myFileName + "']. ",
                                                               theSourceMesh->NbDeferredNodes(), theDestMesh->NbNodes(),
                                                               theSourceMesh->NbDeferredTriangles(), theSourceMesh->DegeneratedTriNb(),
                                                               theDestMesh->NbTriangles());
    Message::SendTrace (aStatisticInfo);
  }
  return true;
}

// =======================================================================
// function : setNbPositionNodes
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::setNbPositionNodes (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                     Standard_Integer theNbNodes,
                                                     Standard_Boolean theToCopyData) const
{
  if (theNbNodes <= 0)
  {
    return false;
  }
  theMesh->ResizeNodes(theNbNodes, theToCopyData);
  return true;
}

// =======================================================================
// function : setNodePosition
// purpose  :
// =======================================================================
void RWMesh_TriangulationReader::setNodePosition (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                  Standard_Integer theIndex,
                                                  const gp_Pnt& thePnt) const
{
  theMesh->SetNode(theIndex, thePnt);
}

// =======================================================================
// function : setNbUVNodes
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::setNbUVNodes (const Handle(RWMesh_TriangulationSource)& theMesh,
                                               Standard_Integer theNbNodes) const
{
  if (theNbNodes <= 0
    || theMesh->NbNodes() != theNbNodes)
  {
    return false;
  }
  theMesh->AddUVNodes();
  return true;
}

// =======================================================================
// function : setNodeUV
// purpose  :
// =======================================================================
void RWMesh_TriangulationReader::setNodeUV (const Handle(RWMesh_TriangulationSource)& theMesh,
                                            Standard_Integer theIndex,
                                            const gp_Pnt2d& theUV) const
{
  theMesh->SetUVNode (theIndex, theUV);
}

// =======================================================================
// function : setNbNormalNodes
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::setNbNormalNodes (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                   Standard_Integer theNbNodes) const
{
  if (theNbNodes <= 0
    || theMesh->NbNodes() != theNbNodes)
  {
    return false;
  }
  theMesh->AddNormals();
  return true;
}

// =======================================================================
// function : setNodeNormal
// purpose  :
// =======================================================================
void RWMesh_TriangulationReader::setNodeNormal (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                Standard_Integer theIndex,
                                                const gp_Vec3f& theNormal) const
{
  theMesh->SetNormal(theIndex, theNormal);
}

// =======================================================================
// function : setNbTriangles
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::setNbTriangles (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                 Standard_Integer theNbTris,
                                                 Standard_Boolean theToCopyData) const
{
  if (theNbTris >= 1)
  {
    theMesh->ResizeTriangles (theNbTris, theToCopyData);
    return true;
  }
  return false;
}

// =======================================================================
// function : setTriangle
// purpose  :
// =======================================================================
Standard_Integer RWMesh_TriangulationReader::setTriangle (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                          Standard_Integer theIndex,
                                                          const Poly_Triangle& theTriangle) const
{
  if (theTriangle.Value(1) < 1 || theTriangle.Value(1) > theMesh->NbNodes()
    || theTriangle.Value(2) < 1 || theTriangle.Value(2) > theMesh->NbNodes()
    || theTriangle.Value(3) < 1 || theTriangle.Value(3) > theMesh->NbNodes())
  {
    return 0;
  }
  if (myToSkipDegenerateTris
    && (theTriangle.Value(1) == theTriangle.Value(2)
      || theTriangle.Value(1) == theTriangle.Value(3)
      || theTriangle.Value(2) == theTriangle.Value(3)))
  {
    return -1;
  }
  theMesh->SetTriangle (theIndex, theTriangle);
  return 1;
}

// =======================================================================
// function : setNbEdges
// purpose  :
// =======================================================================
bool RWMesh_TriangulationReader::setNbEdges (const Handle(RWMesh_TriangulationSource)& theMesh,
                                             Standard_Integer theNbTris,
                                             Standard_Boolean theToCopyData) const
{
  if (theNbTris >= 1)
  {
    theMesh->ResizeEdges (theNbTris, theToCopyData);
    return true;
  }
  return false;
}

// =======================================================================
// function : setEdge
// purpose  :
// =======================================================================
Standard_Integer RWMesh_TriangulationReader::setEdge (const Handle(RWMesh_TriangulationSource)& theMesh,
                                                      Standard_Integer theIndex,
                                                      const Standard_Integer theEdge) const
{
  if (theEdge < 1 || theEdge > theMesh->NbNodes())
  {
    return 0;
  }
  theMesh->SetEdge(theIndex, theEdge);
  return 1;
}
