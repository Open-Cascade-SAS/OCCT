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

#ifndef _RWMesh_TriangulationSource_HeaderFile
#define _RWMesh_TriangulationSource_HeaderFile

#include <Poly_Triangulation.hxx>

class RWMesh_TriangulationReader;

//! Mesh data wrapper for delayed triangulation loading.
//! Class inherits Poly_Triangulation so that it can be put temporarily into TopoDS_Face within assembly structure.
class RWMesh_TriangulationSource : public Poly_Triangulation
{
  DEFINE_STANDARD_RTTIEXT(RWMesh_TriangulationSource, Poly_Triangulation)
public:

  //! Constructor.
  Standard_EXPORT RWMesh_TriangulationSource();

  //! Destructor.
  Standard_EXPORT virtual ~RWMesh_TriangulationSource();

  //! Returns reader allowing to read data from the buffer.
  const Handle(RWMesh_TriangulationReader)& Reader() const { return myReader; }

  //! Sets reader allowing to read data from the buffer.
  void SetReader (const Handle(RWMesh_TriangulationReader)& theReader) { myReader = theReader; }

  //! Returns number of degenerated triangles collected during data reading.
  //! Used for debug statistic purpose.
  Standard_Integer DegeneratedTriNb() const { return myStatisticOfDegeneratedTriNb; }

  //! Gets access to number of degenerated triangles to collect them during data reading.
  Standard_Integer& ChangeDegeneratedTriNb() { return myStatisticOfDegeneratedTriNb; }

public: //! @name late-load deferred data interface

  //! Returns number of nodes for deferred loading.
  //! Note: this is estimated values defined in object header, which might be different from actually loaded values
  //! (due to broken header or extra mesh processing).
  //! Always check triangulation size of actually loaded data in code to avoid out-of-range issues.
  virtual Standard_Integer NbDeferredNodes() const Standard_OVERRIDE { return myNbDefNodes; }

  //! Sets number of nodes for deferred loading.
  void SetNbDeferredNodes (const Standard_Integer theNbNodes) { myNbDefNodes = theNbNodes; }

  //! Returns number of triangles for deferred loading.
  //! Note: this is estimated values defined in object header, which might be different from actually loaded values
  //! (due to broken header or extra mesh processing).
  //! Always check triangulation size of actually loaded data in code to avoid out-of-range issues.
  virtual Standard_Integer NbDeferredTriangles() const Standard_OVERRIDE { return myNbDefTriangles; }

  //! Sets number of triangles for deferred loading.
  void SetNbDeferredTriangles (const Standard_Integer theNbTris) { myNbDefTriangles = theNbTris; }

protected:

  //! Loads triangulation data from deferred storage using specified shared input file system.
  Standard_EXPORT virtual Standard_Boolean loadDeferredData (const Handle(OSD_FileSystem)& theFileSystem,
                                                             const Handle(Poly_Triangulation)& theDestTriangulation) const Standard_OVERRIDE;

protected:

  Handle(RWMesh_TriangulationReader) myReader;
  Standard_Integer                   myNbDefNodes;
  Standard_Integer                   myNbDefTriangles;
  mutable Standard_Integer           myStatisticOfDegeneratedTriNb;

};

#endif // _RWMesh_TriangulationSource_HeaderFile
