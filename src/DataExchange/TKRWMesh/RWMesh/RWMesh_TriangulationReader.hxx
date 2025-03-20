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

#ifndef _RWMesh_TriangulationReader_HeaderFile
#define _RWMesh_TriangulationReader_HeaderFile

#include <Poly_Triangulation.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <Standard_Mutex.hxx>

class OSD_FileSystem;
class RWMesh_TriangulationSource;

//! Interface for reading primitive array from the buffer.
class RWMesh_TriangulationReader : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWMesh_TriangulationReader, Standard_Transient)
public:
  struct LoadingStatistic
  {
    LoadingStatistic()
        : ExpectedNodesNb(0),
          LoadedNodesNb(0),
          ExpectedTrianglesNb(0),
          DegeneratedTrianglesNb(0),
          LoadedTrianglesNb(0)
    {
    }

    void Reset()
    {
      ExpectedNodesNb        = 0;
      LoadedNodesNb          = 0;
      ExpectedTrianglesNb    = 0;
      DegeneratedTrianglesNb = 0;
      LoadedTrianglesNb      = 0;
    }

    Standard_EXPORT void PrintStatistic(const TCollection_AsciiString& thePrefix = "") const;

    Standard_Integer ExpectedNodesNb;
    Standard_Integer LoadedNodesNb;
    Standard_Integer ExpectedTrianglesNb;
    Standard_Integer DegeneratedTrianglesNb;
    Standard_Integer LoadedTrianglesNb;
  };

  //! Constructor.
  Standard_EXPORT RWMesh_TriangulationReader();

  //! Destructor.
  Standard_EXPORT virtual ~RWMesh_TriangulationReader();

  //! Returns file name for reporting issues.
  const TCollection_AsciiString& FileName() const { return myFileName; }

  //! Sets file name for reporting issues.
  void SetFileName(const TCollection_AsciiString& theFileName) { myFileName = theFileName; }

  //! Returns coordinate system converter using for correct data loading.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const
  {
    return myCoordSysConverter;
  }

  //! Sets coordinate system converter.
  void SetCoordinateSystemConverter(const RWMesh_CoordinateSystemConverter& theConverter)
  {
    myCoordSysConverter = theConverter;
  }

  //! Returns flag to fill in triangulation using double or single precision; FALSE by default.
  bool IsDoublePrecision() const { return myIsDoublePrecision; }

  //! Sets flag to fill in triangulation using double or single precision.
  void SetDoublePrecision(bool theIsDouble) { myIsDoublePrecision = theIsDouble; }

  //! Returns TRUE if degenerated triangles should be skipped during mesh loading (only indexes will
  //! be checked).
  Standard_Boolean ToSkipDegenerates() const { return myToSkipDegenerateTris; }

  //! Sets flag to skip degenerated triangles during mesh loading (only indexes will be checked).
  void SetToSkipDegenerates(const Standard_Boolean theToSkip)
  {
    myToSkipDegenerateTris = theToSkip;
  }

  //! Returns TRUE if additional debug information should be print.
  Standard_Boolean ToPrintDebugMessages() const { return myToPrintDebugMessages; }

  //! Sets flag to print debug information.
  void SetToPrintDebugMessages(const Standard_Boolean theToPrint)
  {
    myToPrintDebugMessages = theToPrint;
  }

  //! Starts and reset internal object that accumulates nodes/triangles statistic during data
  //! reading.
  void StartStatistic()
  {
    if (myLoadingStatistic)
    {
      myLoadingStatistic->Reset();
    }
    else
    {
      myLoadingStatistic = new LoadingStatistic();
    }
  }

  //! Stops and nullify internal object that accumulates nodes/triangles statistic during data
  //! reading.
  void StopStatistic()
  {
    if (myLoadingStatistic)
    {
      delete myLoadingStatistic;
      myLoadingStatistic = NULL;
    }
  }

  //! Prints loading statistic.
  //! This method should be used between StartStatistic() and StopStatistic() calls
  //! for correct results.
  void PrintStatistic() const
  {
    if (myLoadingStatistic)
    {
      myLoadingStatistic->PrintStatistic(TCollection_AsciiString("[Mesh reader. File '")
                                         + myFileName + "']. ");
    }
  }

  //! Loads primitive array.
  Standard_EXPORT bool Load(const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                            const Handle(Poly_Triangulation)&         theDestMesh,
                            const Handle(OSD_FileSystem)&             theFileSystem) const;

protected:
  //! Loads primitive array.
  Standard_EXPORT virtual bool load(const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                    const Handle(Poly_Triangulation)&         theDestMesh,
                                    const Handle(OSD_FileSystem)& theFileSystem) const = 0;

  //! Performs additional actions to finalize data loading.
  Standard_EXPORT virtual bool finalizeLoading(
    const Handle(RWMesh_TriangulationSource)& theSourceMesh,
    const Handle(Poly_Triangulation)&         theDestMesh) const;

protected: //! @name interface for filling triangulation data
  //! Resizes array of position nodes to specified size.
  //! @param[in] theMesh  triangulation to be modified
  //! @param[in] theNbNodes  nodes number
  //! @param[in] theToCopyData  copy old nodes into new array
  //! @return TRUE in case of success operation
  virtual bool setNbPositionNodes(const Handle(Poly_Triangulation)& theMesh,
                                  Standard_Integer                  theNbNodes,
                                  Standard_Boolean                  theToCopyData = false) const
  {
    if (theNbNodes <= 0)
    {
      return false;
    }
    theMesh->ResizeNodes(theNbNodes, theToCopyData);
    return true;
  }

  //! Sets node position.
  //! @param[in] theMesh  triangulation to be modified
  //! @param[in] theIndex  node index starting from 1
  //! @param[in] thePnt  node position
  virtual void setNodePosition(const Handle(Poly_Triangulation)& theMesh,
                               Standard_Integer                  theIndex,
                               const gp_Pnt&                     thePnt) const
  {
    theMesh->SetNode(theIndex, thePnt);
  }

  //! Resizes array of UV nodes to specified size.
  //! @param[in] theMesh  triangulation to be modified
  //! @param[in] theNbNodes  nodes number
  //! @return TRUE in case of success operation
  virtual bool setNbUVNodes(const Handle(Poly_Triangulation)& theMesh,
                            Standard_Integer                  theNbNodes) const
  {
    if (theNbNodes <= 0 || theMesh->NbNodes() != theNbNodes)
    {
      return false;
    }
    theMesh->AddUVNodes();
    return true;
  }

  //! Sets node UV texture coordinates.
  //! @param[in] theMesh  triangulation to be modified
  //! @param[in] theIndex  node index starting from 1
  //! @param[in] theUV  node UV coordinates
  virtual void setNodeUV(const Handle(Poly_Triangulation)& theMesh,
                         Standard_Integer                  theIndex,
                         const gp_Pnt2d&                   theUV) const
  {
    theMesh->SetUVNode(theIndex, theUV);
  }

  //! Resizes array of nodes normals to specified size.
  //! @param[in] theMesh  triangulation to be modified
  //! @param[in] theNbNodes  nodes number
  //! @return TRUE in case of success operation
  virtual bool setNbNormalNodes(const Handle(Poly_Triangulation)& theMesh,
                                Standard_Integer                  theNbNodes) const
  {
    if (theNbNodes <= 0 || theMesh->NbNodes() != theNbNodes)
    {
      return false;
    }
    theMesh->AddNormals();
    return true;
  }

  //! Sets node normal.
  //! @param[in] theMesh  triangulation to be modified
  //! @param theIndex  node index starting from 1
  //! @param theNormal node normal vector
  virtual void setNodeNormal(const Handle(Poly_Triangulation)& theMesh,
                             Standard_Integer                  theIndex,
                             const gp_Vec3f&                   theNormal) const
  {
    theMesh->SetNormal(theIndex, theNormal);
  }

  //! Resizes array of triangles to specified size.
  //! @param[in] theMesh  triangulation to be modified
  //! @param[in] theNbTris  elements number
  //! @param[in] theToCopyData  copy old triangles into new array
  //! @return TRUE in case of success operation
  virtual bool setNbTriangles(const Handle(Poly_Triangulation)& theMesh,
                              Standard_Integer                  theNbTris,
                              Standard_Boolean                  theToCopyData = false) const
  {
    if (theNbTris >= 1)
    {
      theMesh->ResizeTriangles(theNbTris, theToCopyData);
      return true;
    }
    return false;
  }

  //! Adds triangle element.
  //! @param[in] theMesh  triangulation to be modified
  //! @param theIndex    triangle index starting from 1
  //! @param theTriangle triangle nodes starting from 1
  //! @return 0 if node indexes are out of range,
  //!        -1 if triangle is degenerated and should be skipped,
  //!         1 in case of success operation.
  virtual Standard_Integer setTriangle(const Handle(Poly_Triangulation)& theMesh,
                                       Standard_Integer                  theIndex,
                                       const Poly_Triangle&              theTriangle) const
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
    theMesh->SetTriangle(theIndex, theTriangle);
    return 1;
  }

  //! Resizes array of edges to specified size.
  //! @param[in] theMesh  triangulation source to be modified
  //! @param[in] theNbEdges  elements number
  //! @param[in] theToCopyData  copy old edges into new array
  //! @return TRUE in case of success operation
  Standard_EXPORT virtual bool setNbEdges(const Handle(Poly_Triangulation)& theMesh,
                                          const Standard_Integer            theNbEdges,
                                          const Standard_Boolean theToCopyData = false) const;

  //! Adds edge element.
  //! @param[in] theMesh  triangulation source to be modified
  //! @param theIndex     edge index starting from 1
  //! @param theEdge      edge nodes starting from 1
  //! @return 0 if node indexes are out of range,
  //!         1 in case of success operation.
  Standard_EXPORT virtual Standard_Integer setEdge(const Handle(Poly_Triangulation)& theMesh,
                                                   const Standard_Integer            theIndex,
                                                   const Standard_Integer            theEdge) const;

protected:
  RWMesh_CoordinateSystemConverter myCoordSysConverter; //!< coordinate system converter
  // clang-format off
  TCollection_AsciiString          myFileName;             //!< file name to use during message printing
  mutable Standard_Mutex           myMutex;                //!< internal mutex to collect nodes/triangles statistic
  mutable LoadingStatistic*        myLoadingStatistic;     //!< statistic of loaded triangulation
  Standard_Boolean                 myIsDoublePrecision;    //!< flag to fill in triangulation using single or double precision
  Standard_Boolean                 myToSkipDegenerateTris; //!< flag to skip degenerate triangles during loading, FALSE by default
  Standard_Boolean                 myToPrintDebugMessages; //!< flag to print additional debug information
  // clang-format on
};

#endif // _RWMesh_TriangulationReader_HeaderFile
