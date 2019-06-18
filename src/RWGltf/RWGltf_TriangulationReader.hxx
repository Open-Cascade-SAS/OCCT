// Author: Kirill Gavrilov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_TriangulationReader_HeaderFile
#define _RWGltf_TriangulationReader_HeaderFile

#include <RWGltf_PrimitiveArrayReader.hxx>

//! RWGltf_PrimitiveArrayReader implementation creating Poly_Triangulation.
class RWGltf_TriangulationReader : public RWGltf_PrimitiveArrayReader
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_TriangulationReader, RWGltf_PrimitiveArrayReader)
public:

  //! Empty constructor.
  Standard_EXPORT RWGltf_TriangulationReader();

protected:

  //! Create Poly_Triangulation from collected data
  Standard_EXPORT virtual Handle(Poly_Triangulation) result() Standard_OVERRIDE;

  //! Reset cache before loading primitive array.
  Standard_EXPORT virtual void reset() Standard_OVERRIDE;

  //! Fill triangulation data and ignore non-triangulation primitives.
  //! @param theStream   input stream to read from
  //! @param theName     entity name for logging errors
  //! @param theAccessor buffer accessor
  //! @param theType     array type
  //! @param theMode     primitive mode
  //! @return FALSE on error
  Standard_EXPORT virtual bool readBuffer (std::istream& theStream,
                                           const TCollection_AsciiString& theName,
                                           const RWGltf_GltfAccessor& theAccessor,
                                           RWGltf_GltfArrayType theType,
                                           RWGltf_GltfPrimitiveMode theMode) Standard_OVERRIDE;

protected: //! @name interface for filling triangulation data

  //! Resize array of position nodes to specified size.
  virtual bool setNbPositionNodes (Standard_Integer theNbNodes)
  {
    if (theNbNodes <= 0)
    {
      return false;
    }
    myTriangulation->ChangeNodes().Resize (1, theNbNodes, false);
    return true;
  }

  //! Set node position.
  //! @param theIndex node index starting from 1
  //! @param thePnt   node position
  virtual void setNodePosition (Standard_Integer theIndex,
                                const gp_Pnt& thePnt)
  {
    myTriangulation->ChangeNode (theIndex) = thePnt;
  }

  //! Resize array of UV nodes to specified size.
  virtual bool setNbUVNodes (Standard_Integer theNbNodes)
  {
    if (theNbNodes <= 0
     || myTriangulation->NbNodes() != theNbNodes)
    {
      return false;
    }
    myTriangulation->ChangeUVNodes().Resize (1, theNbNodes, false);
    return true;
  }

  //! Set node UV texture coordinates.
  //! @param theIndex node index starting from 1
  //! @param theUV    node UV coordinates
  virtual void setNodeUV (Standard_Integer theIndex,
                          const gp_Pnt2d& theUV)
  {
    myTriangulation->ChangeUVNode (theIndex) = theUV;
  }

  //! Resize array of nodes normals to specified size.
  virtual bool setNbNormalNodes (Standard_Integer theNbNodes)
  {
    if (theNbNodes <= 0
     || myTriangulation->NbNodes() != theNbNodes)
    {
      return false;
    }
    myTriangulation->SetNormals (new TShort_HArray1OfShortReal (1, theNbNodes * 3));
    return true;
  }

  //! Set node normal.
  //! @param theIndex  node index starting from 1
  //! @param theNormal node normal
  virtual void setNodeNormal (Standard_Integer theIndex,
                              const gp_Dir& theNormal)
  {
    myTriangulation->SetNormal (theIndex, theNormal);
  }

  //! Resize array of triangles to specified size.
  virtual bool setNbTriangles (Standard_Integer theNbTris)
  {
    if (theNbTris >= 1)
    {
      myTriangulation->ChangeTriangles().Resize (1, theNbTris, false);
      return true;
    }
    return false;
  }

  //! Add triangle element.
  //! @param theIndex    triangle index starting from 1
  //! @param theTriangle triangle nodes starting from 1
  //! @return FALSE if node indexes are out of range
  virtual bool setTriangle (Standard_Integer theIndex,
                            const Poly_Triangle& theTriangle)
  {
    if (theTriangle.Value (1) < myTriangulation->Nodes().Lower() || theTriangle.Value (1) > myTriangulation->Nodes().Upper()
     || theTriangle.Value (2) < myTriangulation->Nodes().Lower() || theTriangle.Value (2) > myTriangulation->Nodes().Upper()
     || theTriangle.Value (3) < myTriangulation->Nodes().Lower() || theTriangle.Value (3) > myTriangulation->Nodes().Upper())
    {
      return false;
    }
    myTriangulation->ChangeTriangle (theIndex) = theTriangle;
    return true;
  }

protected:

  Handle(Poly_Triangulation) myTriangulation;

};

#endif // _RWGltf_TriangulationReader_HeaderFile
