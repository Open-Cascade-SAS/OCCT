// Created on: 1995-03-06
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Poly_Triangulation_HeaderFile
#define _Poly_Triangulation_HeaderFile

#include <Bnd_Box.hxx>
#include <Poly_HArray1OfTriangle.hxx>
#include <Poly_ArrayOfNodes.hxx>
#include <Poly_ArrayOfUVNodes.hxx>
#include <Poly_MeshPurpose.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TShort_HArray1OfShortReal.hxx>

class OSD_FileSystem;
class Poly_Triangulation;
class Poly_TriangulationParameters;

DEFINE_STANDARD_HANDLE(Poly_Triangulation, Standard_Transient)

//! Provides a triangulation for a surface, a set of surfaces, or more generally a shape.
//!
//! A triangulation consists of an approximate representation of the actual shape,
//! using a collection of points and triangles.
//! The points are located on the surface.
//! The edges of the triangles connect adjacent points with a straight line that approximates the true curve on the surface.
//!
//! A triangulation comprises:
//! - A table of 3D nodes (3D points on the surface).
//! - A table of triangles.
//!   Each triangle (Poly_Triangle object) comprises a triplet of indices in the table of 3D nodes specific to the triangulation.
//! - An optional table of 2D nodes (2D points), parallel to the table of 3D nodes.
//!   2D point are the (u, v) parameters of the corresponding 3D point on the surface approximated by the triangulation.
//! - An optional table of 3D vectors, parallel to the table of 3D nodes, defining normals to the surface at specified 3D point.
//! - An optional deflection, which maximizes the distance from a point on the surface to the corresponding point on its approximate triangulation.
//!
//! In many cases, algorithms do not need to work with the exact representation of a surface.
//! A triangular representation induces simpler and more robust adjusting, faster performances, and the results are as good.
class Poly_Triangulation : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Poly_Triangulation, Standard_Transient)
public:

  //! Constructs an empty triangulation.
  Standard_EXPORT Poly_Triangulation();

  //! Constructs a triangulation from a set of triangles.
  //! The triangulation is initialized without a triangle or a node,
  //! but capable of containing specified number of nodes and triangles.
  //! @param theNbNodes     [in] number of nodes to allocate
  //! @param theNbTriangles [in] number of triangles to allocate
  //! @param theHasUVNodes  [in] indicates whether 2D nodes will be associated with 3D ones,
  //!                            (i.e. to enable a 2D representation)
  //! @param theHasNormals  [in] indicates whether normals will be given and associated with nodes
  Standard_EXPORT Poly_Triangulation (const Standard_Integer theNbNodes,
                                      const Standard_Integer theNbTriangles,
                                      const Standard_Boolean theHasUVNodes,
                                      const Standard_Boolean theHasNormals = false);

  //! Constructs a triangulation from a set of triangles. The
  //! triangulation is initialized with 3D points from Nodes and triangles
  //! from Triangles.
  Standard_EXPORT Poly_Triangulation(const TColgp_Array1OfPnt& Nodes, const Poly_Array1OfTriangle& Triangles);

  //! Constructs a triangulation from a set of triangles. The
  //! triangulation is initialized with 3D points from Nodes, 2D points from
  //! UVNodes and triangles from Triangles, where
  //! coordinates of a 2D point from UVNodes are the
  //! (u, v) parameters of the corresponding 3D point
  //! from Nodes on the surface approximated by the
  //! constructed triangulation.
  Standard_EXPORT Poly_Triangulation(const TColgp_Array1OfPnt& Nodes, const TColgp_Array1OfPnt2d& UVNodes, const Poly_Array1OfTriangle& Triangles);

  //! Destructor
  Standard_EXPORT virtual ~Poly_Triangulation();

  //! Creates full copy of current triangulation
  Standard_EXPORT virtual Handle(Poly_Triangulation) Copy() const;

  //! Copy constructor for triangulation.
  Standard_EXPORT Poly_Triangulation (const Handle(Poly_Triangulation)& theTriangulation);

  //! Returns the deflection of this triangulation.
  Standard_Real Deflection() const { return myDeflection; }

  //! Sets the deflection of this triangulation to theDeflection.
  //! See more on deflection in Polygon2D
  void Deflection (const Standard_Real theDeflection) { myDeflection = theDeflection; }

  //! Returns initial set of parameters used to generate this triangulation.
  const Handle(Poly_TriangulationParameters)& Parameters() const { return myParams; }

  //! Updates initial set of parameters used to generate this triangulation.
  void Parameters (const Handle(Poly_TriangulationParameters)& theParams) { myParams = theParams; }

  //! Clears internal arrays of nodes and all attributes.
  Standard_EXPORT virtual void Clear();

  //! Returns TRUE if triangulation has some geometry.
  virtual Standard_Boolean HasGeometry() const { return !myNodes.IsEmpty() && !myTriangles.IsEmpty(); }

  //! Returns the number of nodes for this triangulation.
  Standard_Integer NbNodes() const { return myNodes.Length(); }

  //! Returns the number of triangles for this triangulation.
  Standard_Integer NbTriangles() const { return myTriangles.Length(); }

  //! Returns Standard_True if 2D nodes are associated with 3D nodes for this triangulation.
  Standard_Boolean HasUVNodes() const { return !myUVNodes.IsEmpty(); }

  //! Returns Standard_True if nodal normals are defined.
  Standard_Boolean HasNormals() const { return !myNormals.IsEmpty(); }

  //! Returns a node at the given index.
  //! @param[in] theIndex node index within [1, NbNodes()] range
  //! @return 3D point coordinates
  gp_Pnt Node (Standard_Integer theIndex) const { return myNodes.Value (theIndex - 1); }

  //! Sets a node coordinates.
  //! @param[in] theIndex node index within [1, NbNodes()] range
  //! @param[in] thePnt   3D point coordinates
  void SetNode (Standard_Integer theIndex,
                const gp_Pnt& thePnt)
  {
    myNodes.SetValue (theIndex - 1, thePnt);
  }

  //! Returns UV-node at the given index.
  //! @param[in] theIndex node index within [1, NbNodes()] range
  //! @return 2D point defining UV coordinates
  gp_Pnt2d UVNode (Standard_Integer theIndex) const { return myUVNodes.Value (theIndex - 1); }

  //! Sets an UV-node coordinates.
  //! @param[in] theIndex node index within [1, NbNodes()] range
  //! @param[in] thePnt   UV coordinates
  void SetUVNode (Standard_Integer theIndex,
                  const gp_Pnt2d&  thePnt)
  {
    myUVNodes.SetValue (theIndex - 1, thePnt);
  }

  //! Returns triangle at the given index.
  //! @param[in] theIndex triangle index within [1, NbTriangles()] range
  //! @return triangle node indices, with each node defined within [1, NbNodes()] range
  const Poly_Triangle& Triangle (Standard_Integer theIndex) const { return myTriangles.Value (theIndex); }

  //! Sets a triangle.
  //! @param[in] theIndex triangle index within [1, NbTriangles()] range
  //! @param[in] theTriangle triangle node indices, with each node defined within [1, NbNodes()] range
  void SetTriangle (Standard_Integer theIndex,
                    const Poly_Triangle& theTriangle)
  {
    myTriangles.SetValue (theIndex, theTriangle);
  }

  //! Returns normal at the given index.
  //! @param[in] theIndex node index within [1, NbNodes()] range
  //! @return normalized 3D vector defining a surface normal
  gp_Dir Normal (Standard_Integer theIndex) const
  {
    const gp_Vec3f& aNorm = myNormals.Value (theIndex - 1);
    return gp_Dir (aNorm.x(), aNorm.y(), aNorm.z());
  }

  //! Returns normal at the given index.
  //! @param[in]  theIndex node index within [1, NbNodes()] range
  //! @param[out] theVec3  3D vector defining a surface normal
  void Normal (Standard_Integer theIndex,
               gp_Vec3f& theVec3) const
  {
    theVec3 = myNormals.Value (theIndex - 1);
  }

  //! Changes normal at the given index.
  //! @param[in] theIndex node index within [1, NbNodes()] range
  //! @param[in] theVec3  normalized 3D vector defining a surface normal
  void SetNormal (const Standard_Integer theIndex,
                  const gp_Vec3f& theNormal)
  {
    myNormals.SetValue (theIndex - 1, theNormal);
  }

  //! Changes normal at the given index.
  //! @param[in] theIndex  node index within [1, NbNodes()] range
  //! @param[in] theNormal normalized 3D vector defining a surface normal
  void SetNormal (const Standard_Integer theIndex,
                  const gp_Dir& theNormal)
  {
    SetNormal (theIndex, gp_Vec3f (float(theNormal.X()),
                                   float(theNormal.Y()),
                                   float(theNormal.Z())));
  }

  //! Returns mesh purpose bits.
  Poly_MeshPurpose MeshPurpose() const { return myPurpose; }

  //! Sets mesh purpose bits.
  void SetMeshPurpose (const Poly_MeshPurpose thePurpose) { myPurpose = thePurpose; }

  //! Returns cached min - max range of triangulation data,
  //! which is VOID by default (e.g, no cached information).
  Standard_EXPORT const Bnd_Box& CachedMinMax() const;

  //! Sets a cached min - max range of this triangulation.
  //! The bounding box should exactly match actual range of triangulation data
  //! without a gap or transformation, or otherwise undefined behavior will be observed.
  //! Passing a VOID range invalidates the cache.
  Standard_EXPORT void SetCachedMinMax (const Bnd_Box& theBox);

  //! Returns TRUE if there is some cached min - max range of this triangulation.
  Standard_EXPORT Standard_Boolean HasCachedMinMax() const { return myCachedMinMax != NULL; }

  //! Updates cached min - max range of this triangulation with bounding box of nodal data.
  void UpdateCachedMinMax()
  {
    Bnd_Box aBox;
    MinMax (aBox, gp_Trsf(), true);
    SetCachedMinMax (aBox);
  }

  //! Extends the passed box with bounding box of this triangulation.
  //! Uses cached min - max range when available and:
  //! - input transformation theTrsf has no rotation part;
  //! - theIsAccurate is set to FALSE;
  //! - no triangulation data available (e.g. it is deferred and not loaded).
  //! @param theBox [in] [out] bounding box to extend by this triangulation
  //! @param theTrsf [in] optional transformation
  //! @param theIsAccurate [in] when FALSE, allows using a cached min - max range of this triangulation
  //!                           even for non-identity transformation.
  //! @return FALSE if there is no any data to extend the passed box (no both triangulation and cached min - max range).
  Standard_EXPORT Standard_Boolean MinMax (Bnd_Box& theBox, const gp_Trsf& theTrsf, const bool theIsAccurate = false) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

public:

  //! Returns TRUE if node positions are defined with double precision; TRUE by default.
  bool IsDoublePrecision() const { return myNodes.IsDoublePrecision(); }

  //! Set if node positions should be defined with double or single precision for 3D and UV nodes.
  //! Raises exception if data was already allocated.
  Standard_EXPORT void SetDoublePrecision (bool theIsDouble);

  //! Method resizing internal arrays of nodes (synchronously for all attributes).
  //! @param theNbNodes   [in] new number of nodes
  //! @param theToCopyOld [in] copy old nodes into the new array
  Standard_EXPORT void ResizeNodes (Standard_Integer theNbNodes,
                                    Standard_Boolean theToCopyOld);

  //! Method resizing an internal array of triangles.
  //! @param theNbTriangles [in] new number of triangles
  //! @param theToCopyOld   [in] copy old triangles into the new array
  Standard_EXPORT void ResizeTriangles (Standard_Integer theNbTriangles,
                                        Standard_Boolean theToCopyOld);

  //! If an array for UV coordinates is not allocated yet, do it now.
  Standard_EXPORT void AddUVNodes();

  //! Deallocates the UV nodes array.
  Standard_EXPORT void RemoveUVNodes();

  //! If an array for normals is not allocated yet, do it now.
  Standard_EXPORT void AddNormals();

  //! Deallocates the normals array.
  Standard_EXPORT void RemoveNormals();

  //! Compute smooth normals by averaging triangle normals.
  Standard_EXPORT void ComputeNormals();

public:

  //! Returns the table of 3D points for read-only access or NULL if nodes array is undefined.
  //! Poly_Triangulation::Node() should be used instead when possible.
  //! Returned object should not be used after Poly_Triangulation destruction.
  Standard_EXPORT Handle(TColgp_HArray1OfPnt) MapNodeArray() const;

  //! Returns the triangle array for read-only access or NULL if triangle array is undefined.
  //! Poly_Triangulation::Triangle() should be used instead when possible.
  //! Returned object should not be used after Poly_Triangulation destruction.
  Standard_EXPORT Handle(Poly_HArray1OfTriangle) MapTriangleArray() const;

  //! Returns the table of 2D nodes for read-only access or NULL if UV nodes array is undefined.
  //! Poly_Triangulation::UVNode() should be used instead when possible.
  //! Returned object should not be used after Poly_Triangulation destruction.
  Standard_EXPORT Handle(TColgp_HArray1OfPnt2d) MapUVNodeArray() const;

  //! Returns the table of per-vertex normals for read-only access or NULL if normals array is undefined.
  //! Poly_Triangulation::Normal() should be used instead when possible.
  //! Returned object should not be used after Poly_Triangulation destruction.
  Standard_EXPORT Handle(TShort_HArray1OfShortReal) MapNormalArray() const;

public:

  //! Returns an internal array of triangles.
  //! Triangle()/SetTriangle() should be used instead in portable code.
  Poly_Array1OfTriangle& InternalTriangles() { return myTriangles; }

  //! Returns an internal array of nodes.
  //! Node()/SetNode() should be used instead in portable code.
  Poly_ArrayOfNodes& InternalNodes() { return myNodes; }

  //! Returns an internal array of UV nodes.
  //! UBNode()/SetUVNode() should be used instead in portable code.
  Poly_ArrayOfUVNodes& InternalUVNodes() { return myUVNodes; }

  //! Return an internal array of normals.
  //! Normal()/SetNormal() should be used instead in portable code.
  NCollection_Array1<gp_Vec3f>& InternalNormals() { return myNormals; }

  Standard_DEPRECATED("Deprecated method, SetNormal() should be used instead")
  Standard_EXPORT void SetNormals (const Handle(TShort_HArray1OfShortReal)& theNormals);

  Standard_DEPRECATED("Deprecated method, Triangle() should be used instead")
  const Poly_Array1OfTriangle& Triangles() const { return myTriangles; }

  Standard_DEPRECATED("Deprecated method, SetTriangle() should be used instead")
  Poly_Array1OfTriangle& ChangeTriangles() { return myTriangles; }

  Standard_DEPRECATED("Deprecated method, SetTriangle() should be used instead")
  Poly_Triangle& ChangeTriangle (const Standard_Integer theIndex) { return myTriangles.ChangeValue (theIndex); }

public: //! @name late-load deferred data interface

  //! Returns number of deferred nodes that can be loaded using LoadDeferredData().
  //! Note: this is estimated values, which might be different from actually loaded values.
  //! Always check triangulation size of actually loaded data in code to avoid out-of-range issues.
  virtual Standard_Integer NbDeferredNodes() const { return 0; }

  //! Returns number of deferred triangles that can be loaded using LoadDeferredData().
  //! Note: this is estimated values, which might be different from actually loaded values
  //! Always check triangulation size of actually loaded data in code to avoid out-of-range issues.
  virtual Standard_Integer NbDeferredTriangles() const { return 0; }

  //! Returns TRUE if there is some triangulation data that can be loaded using LoadDeferredData().
  virtual Standard_Boolean HasDeferredData() const { return NbDeferredTriangles() > 0; }

  //! Loads triangulation data into itself
  //! from some deferred storage using specified shared input file system.
  Standard_EXPORT virtual Standard_Boolean LoadDeferredData (const Handle(OSD_FileSystem)& theFileSystem = Handle(OSD_FileSystem)());

  //! Loads triangulation data into new Poly_Triangulation object
  //! from some deferred storage using specified shared input file system.
  Standard_EXPORT virtual Handle(Poly_Triangulation) DetachedLoadDeferredData
    (const Handle(OSD_FileSystem)& theFileSystem = Handle(OSD_FileSystem)()) const;

  //! Releases triangulation data if it has connected deferred storage.
  Standard_EXPORT virtual Standard_Boolean UnloadDeferredData();

protected:

  //! Creates new triangulation object (can be inheritor of Poly_Triangulation).
  virtual Handle(Poly_Triangulation) createNewEntity() const
  {
    return new Poly_Triangulation();
  }

  //! Load triangulation data from deferred storage using specified shared input file system.
  virtual Standard_Boolean loadDeferredData (const Handle(OSD_FileSystem)& theFileSystem,
                                             const Handle(Poly_Triangulation)& theDestTriangulation) const
  {
    (void )theFileSystem;
    (void )theDestTriangulation;
    return false;
  }

protected:

  //! Clears cached min - max range saved previously.
  Standard_EXPORT void unsetCachedMinMax();

  //! Calculates bounding box of nodal data.
  //! @param theTrsf [in] optional transformation.
  Standard_EXPORT virtual Bnd_Box computeBoundingBox (const gp_Trsf& theTrsf) const;

protected:

  Bnd_Box*                     myCachedMinMax;
  Standard_Real                myDeflection;
  Poly_ArrayOfNodes            myNodes;
  Poly_Array1OfTriangle        myTriangles;
  Poly_ArrayOfUVNodes          myUVNodes;
  NCollection_Array1<gp_Vec3f> myNormals;
  Poly_MeshPurpose             myPurpose;

  Handle(Poly_TriangulationParameters) myParams;
};

#endif // _Poly_Triangulation_HeaderFile
