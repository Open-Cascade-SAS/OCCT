// Author: Kirill Gavrilov
// Copyright (c) 2015-2019 OPEN CASCADE SAS
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

#ifndef _RWObj_Reader_HeaderFile
#define _RWObj_Reader_HeaderFile

#include <gp_XYZ.hxx>
#include <Graphic3d_Vec2.hxx>
#include <Graphic3d_Vec4.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Shared.hxx>

#include <RWMesh_CoordinateSystemConverter.hxx>
#include <RWObj_Material.hxx>
#include <RWObj_SubMesh.hxx>
#include <RWObj_SubMeshReason.hxx>
#include <RWObj_Tools.hxx>

#include <vector>

//! An abstract class implementing procedure to read OBJ file.
//!
//! This class is not bound to particular data structure
//! and can be used to read the file directly into arbitrary data model.
//! To use it, create descendant class and implement interface methods.
//!
//! Call method Read() to read the file.
class RWObj_Reader : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWObj_Reader, Standard_Transient)
public:

  //! Empty constructor.
  Standard_EXPORT RWObj_Reader();

  //! Reads data from OBJ file.
  //! Unicode paths can be given in UTF-8 encoding.
  //! Returns true if success, false on error or user break.
  Standard_Boolean Read (const TCollection_AsciiString& theFile,
                         const Message_ProgressRange& theProgress)
  {
    return read (theFile, theProgress, Standard_False);
  }

  //! Probe data from OBJ file (comments, external references) without actually reading mesh data.
  //! Although mesh data will not be collected, the full file content will be parsed, due to OBJ format limitations.
  //! @param theFile     path to the file
  //! @param theProgress progress indicator
  //! @return TRUE if success, FALSE on error or user break.
  //! @sa FileComments(), ExternalFiles(), NbProbeNodes(), NbProbeElems().
  Standard_Boolean Probe (const TCollection_AsciiString& theFile,
                          const Message_ProgressRange& theProgress)
  {
    return read (theFile, theProgress, Standard_True);
  }

  //! Returns file comments (lines starting with # at the beginning of file).
  const TCollection_AsciiString& FileComments() const { return myFileComments; }

  //! Return the list of external file references.
  const NCollection_IndexedMap<TCollection_AsciiString>& ExternalFiles() const { return myExternalFiles; }

  //! Number of probed nodes.
  Standard_Integer NbProbeNodes() const { return myNbProbeNodes; }

  //!< number of probed polygon elements (of unknown size).
  Standard_Integer NbProbeElems() const { return myNbProbeElems; }

  //! Returns memory limit in bytes; -1 (no limit) by default.
  Standard_Size MemoryLimit() const { return myMemLimitBytes; }

  //! Specify memory limit in bytes, so that import will be aborted
  //! by specified limit before memory allocation error occurs.
  void SetMemoryLimit (Standard_Size theMemLimit) { myMemLimitBytes = theMemLimit; }

  //! Return transformation from one coordinate system to another; no transformation by default.
  const RWMesh_CoordinateSystemConverter& Transformation() const { return myCSTrsf; }

  //! Setup transformation from one coordinate system to another.
  //! OBJ file might be exported following various coordinate system conventions,
  //! so that it might be useful automatically transform data during file reading.
  void SetTransformation (const RWMesh_CoordinateSystemConverter& theCSConverter) { myCSTrsf = theCSConverter; }

  //! Return single precision flag for reading vertex data (coordinates); FALSE by default.
  Standard_Boolean IsSinglePrecision() const { return myObjVerts.IsSinglePrecision(); }

  //! Setup single/double precision flag for reading vertex data (coordinates).
  void SetSinglePrecision (Standard_Boolean theIsSinglePrecision) { myObjVerts.SetSinglePrecision (theIsSinglePrecision); }

protected:

  //! Reads data from OBJ file.
  //! Unicode paths can be given in UTF-8 encoding.
  //! Returns true if success, false on error or user break.
  Standard_EXPORT Standard_Boolean read (const TCollection_AsciiString& theFile,
                                         const Message_ProgressRange& theProgress,
                                         const Standard_Boolean theToProbe);

//! @name interface methods which should be implemented by sub-class
protected:

  //! Add new sub-mesh.
  //! Basically, this method will be called multiple times for the same group with different reason,
  //! so that implementation should decide if previously allocated sub-mesh should be used or new one to be allocated.
  //! Sub-mesh command can be skipped if previous sub-mesh is empty,
  //! or if the reason is out of interest for particular reader
  //! (e.g. if materials are ignored, reader may ignore RWObj_SubMeshReason_NewMaterial reason).
  //! @param theMesh   mesh definition
  //! @param theReason reason to create new sub-mesh
  //! @return TRUE if new sub-mesh should be started since this point
  virtual Standard_Boolean addMesh (const RWObj_SubMesh& theMesh,
                                    const RWObj_SubMeshReason theReason) = 0;

  //! Retrieve sub-mesh node position, added by addNode().
  virtual gp_Pnt getNode (Standard_Integer theIndex) const = 0;

  //! Callback function to be implemented in descendant.
  //! Should create new node with specified coordinates in the target model, and return its ID as integer.
  virtual Standard_Integer addNode (const gp_Pnt& thePnt) = 0;

  //! Callback function to be implemented in descendant.
  //! Should set normal coordinates for specified node.
  //! @param theIndex node ID as returned by addNode()
  //! @param theNorm  normal vector
  virtual void setNodeNormal (const Standard_Integer theIndex,
                              const Graphic3d_Vec3& theNorm) = 0;

  //! Callback function to be implemented in descendant.
  //! Should set texture coordinates for specified node.
  //! @param theIndex node ID as returned by addNode()
  //! @param theUV    UV texture coordinates
  virtual void setNodeUV (const Standard_Integer theIndex,
                          const Graphic3d_Vec2& theUV) = 0;

  //! Callback function to be implemented in descendant.
  //! Should create new element (triangle or quad if 4th index is != -1) built on specified nodes in the target model.
  virtual void addElement (Standard_Integer theN1,
                           Standard_Integer theN2,
                           Standard_Integer theN3,
                           Standard_Integer theN4) = 0;

//! @name implementation details
private:

  //! Handle "v X Y Z".
  void pushVertex (const char* theXYZ)
  {
    char* aNext = NULL;
    gp_Pnt anXYZ;
    RWObj_Tools::ReadVec3 (theXYZ, aNext, anXYZ.ChangeCoord());
    myCSTrsf.TransformPosition (anXYZ.ChangeCoord());

    myMemEstim += myObjVerts.IsSinglePrecision() ? sizeof(Graphic3d_Vec3) : sizeof(gp_Pnt);
    myObjVerts.Append (anXYZ);
  }

  //! Handle "vn NX NY NZ".
  void pushNormal (const char* theXYZ)
  {
    char* aNext = NULL;
    Graphic3d_Vec3 aNorm;
    RWObj_Tools::ReadVec3 (theXYZ, aNext, aNorm);
    myCSTrsf.TransformNormal (aNorm);

    myMemEstim += sizeof(Graphic3d_Vec3);
    myObjNorms.Append (aNorm);
  }

  //! Handle "vt U V".
  void pushTexel (const char* theUV)
  {
    char* aNext = NULL;
    Graphic3d_Vec2 anUV;
    anUV.x() = (float )Strtod (theUV, &aNext);
    theUV = aNext;
    anUV.y() = (float )Strtod (theUV, &aNext);

    myMemEstim += sizeof(Graphic3d_Vec2);
    myObjVertsUV.Append (anUV);
  }

  //! Handle "f indices".
  void pushIndices (const char* thePos);

  //! Compute the center of planar polygon.
  //! @param theIndices polygon indices
  //! @return center of polygon
  gp_XYZ polygonCenter (const NCollection_Array1<Standard_Integer>& theIndices);

  //! Compute the normal to planar polygon.
  //! The logic is similar to ShapeAnalysis_Curve::IsPlanar().
  //! @param theIndices polygon indices
  //! @return polygon normal
  gp_XYZ polygonNormal (const NCollection_Array1<Standard_Integer>& theIndices);

  //! Create triangle fan from specified polygon.
  //! @param theIndices polygon nodes
  //! @return number of added triangles
  Standard_Integer triangulatePolygonFan (const NCollection_Array1<Standard_Integer>& theIndices);

  //! Triangulate specified polygon.
  //! @param theIndices polygon nodes
  //! @return number of added triangles
  Standard_Integer triangulatePolygon (const NCollection_Array1<Standard_Integer>& theIndices);

  //! Handle "o ObjectName".
  void pushObject (const char* theObjectName);

  //! Handle "g GroupName".
  void pushGroup (const char* theGroupName);

  //! Handle "s SmoothGroupIndex".
  void pushSmoothGroup (const char* theSmoothGroupIndex);

  //! Handle "usemtl MaterialName".
  void pushMaterial (const char* theMaterialName);

  //! Handle "mtllib FileName".
  void readMaterialLib (const char* theFileName);

  //! Check memory limits.
  //! @return FALSE on out of memory
  bool checkMemory();

protected:

  //! Hasher for 3 ordered integers.
  struct ObjVec3iHasher
  {
    static Standard_Integer HashCode (const Graphic3d_Vec3i& theKey,
                                      const Standard_Integer theUpper)
    {
      return ::HashCode (::HashCodes ((Standard_CString )&theKey, sizeof(Graphic3d_Vec3i)), theUpper);
    }

    static Standard_Boolean IsEqual (const Graphic3d_Vec3i& theKey1,
                                     const Graphic3d_Vec3i& theKey2)
    {
      return theKey1[0] == theKey2[0]
          && theKey1[1] == theKey2[1]
          && theKey1[2] == theKey2[2];
    }
  };

  //! Auxiliary structure holding vertex data either with single or double floating point precision.
  class VectorOfVertices
  {
  public:
    //! Empty constructor.
    VectorOfVertices() : myIsSinglePrecision (Standard_False) {}

    //! Return single precision flag; FALSE by default.
    bool IsSinglePrecision() const { return myIsSinglePrecision; }

    //! Setup single/double precision flag.
    void SetSinglePrecision (Standard_Boolean theIsSinglePrecision)
    {
      myIsSinglePrecision = theIsSinglePrecision;
      myPntVec.Nullify();
      myVec3Vec.Nullify();
    }

    //! Reset and (re)allocate buffer.
    void Reset()
    {
      if (myIsSinglePrecision)
      {
        myVec3Vec = new NCollection_Shared<NCollection_Vector<Graphic3d_Vec3> >();
      }
      else
      {
        myPntVec = new NCollection_Shared<NCollection_Vector<gp_Pnt> >();
      }
    }

    //! Return vector lower index.
    Standard_Integer Lower() const { return 0; }

    //! Return vector upper index.
    Standard_Integer Upper() const { return myIsSinglePrecision ? myVec3Vec->Upper() : myPntVec->Upper(); }

    //! Return point with the given index.
    gp_Pnt Value (Standard_Integer theIndex) const
    {
      if (myIsSinglePrecision)
      {
        const Graphic3d_Vec3& aPnt = myVec3Vec->Value (theIndex);
        return gp_Pnt (aPnt.x(), aPnt.y(), aPnt.z());
      }
      else
      {
        return myPntVec->Value (theIndex);
      }
    }

    //! Append new point.
    void Append (const gp_Pnt& thePnt)
    {
      if (myIsSinglePrecision)
      {
        myVec3Vec->Append (Graphic3d_Vec3 ((float )thePnt.X(), (float )thePnt.Y(), (float )thePnt.Z()));
      }
      else
      {
        myPntVec->Append (thePnt);
      }
    }
  private:
    Handle(NCollection_Shared<NCollection_Vector<gp_Pnt> >)         myPntVec;
    Handle(NCollection_Shared<NCollection_Vector<Graphic3d_Vec3> >) myVec3Vec;
    Standard_Boolean myIsSinglePrecision;
  };

protected:

  NCollection_IndexedMap<TCollection_AsciiString>
                                     myExternalFiles; //!< list of external file references
  TCollection_AsciiString            myFileComments;  //!< file header comments
  TCollection_AsciiString            myFolder;        //!< folder containing the OBJ file
  RWMesh_CoordinateSystemConverter   myCSTrsf;        //!< coordinate system flipper
  Standard_Size                      myMemLimitBytes; //!< memory limit in bytes
  Standard_Size                      myMemEstim;      //!< estimated memory occupation in bytes
  Standard_Integer                   myNbLines;       //!< number of parsed lines (e.g. current line)
  Standard_Integer                   myNbProbeNodes;  //!< number of probed nodes
  Standard_Integer                   myNbProbeElems;  //!< number of probed elements
  Standard_Integer                   myNbElemsBig;    //!< number of big elements (polygons with 5+ nodes)
  Standard_Boolean                   myToAbort;       //!< flag indicating abort state (e.g. syntax error)

  // Each node in the Element specifies independent indices of Vertex position, Texture coordinates and Normal.
  // This scheme does not match natural definition of Primitive Array
  // where each unique set of nodal properties defines Vertex
  // (thus node at the same location but with different normal should be duplicated).
  // The following code converts OBJ definition of nodal properties to Primitive Array definition.
  VectorOfVertices                   myObjVerts;      //!< temporary vector of vertices
  NCollection_Vector<Graphic3d_Vec2> myObjVertsUV;    //!< temporary vector of UV parameters
  NCollection_Vector<Graphic3d_Vec3> myObjNorms;      //!< temporary vector of normals
  NCollection_DataMap<Graphic3d_Vec3i, Standard_Integer, ObjVec3iHasher>
                                     myPackedIndices;
  NCollection_DataMap<TCollection_AsciiString, RWObj_Material>
                                     myMaterials;     //!< map of known materials

  RWObj_SubMesh                      myActiveSubMesh; //!< active sub-mesh definition
  std::vector<Standard_Integer>      myCurrElem;      //!< indices for the current element

};

#endif // _RWObj_Reader_HeaderFile
