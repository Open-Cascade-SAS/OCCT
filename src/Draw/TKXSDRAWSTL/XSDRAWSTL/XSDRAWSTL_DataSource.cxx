// Created on: 2004-06-10
// Created by: Alexander SOLOVYOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <XSDRAWSTL_DataSource.hxx>

#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSDRAWSTL_DataSource, MeshVS_DataSource)

//=================================================================================================

XSDRAWSTL_DataSource::XSDRAWSTL_DataSource(const occ::handle<Poly_Triangulation>& aMesh)
{
  myMesh = aMesh;

  if (!myMesh.IsNull())
  {
    const int aNbNodes = myMesh->NbNodes();
    myNodeCoords       = new NCollection_HArray2<double>(1, aNbNodes, 1, 3);
    std::cout << "Nodes : " << aNbNodes << std::endl;

    for (int i = 1; i <= aNbNodes; i++)
    {
      myNodes.Add(i);
      gp_Pnt xyz = myMesh->Node(i);

      myNodeCoords->SetValue(i, 1, xyz.X());
      myNodeCoords->SetValue(i, 2, xyz.Y());
      myNodeCoords->SetValue(i, 3, xyz.Z());
    }

    const int aNbTris = myMesh->NbTriangles();
    myElemNormals     = new NCollection_HArray2<double>(1, aNbTris, 1, 3);
    myElemNodes       = new NCollection_HArray2<int>(1, aNbTris, 1, 3);

    std::cout << "Elements : " << aNbTris << std::endl;

    for (int i = 1; i <= aNbTris; i++)
    {
      myElements.Add(i);

      const Poly_Triangle aTri = myMesh->Triangle(i);

      int V[3];
      aTri.Get(V[0], V[1], V[2]);

      const gp_Pnt aP1 = myMesh->Node(V[0]);
      const gp_Pnt aP2 = myMesh->Node(V[1]);
      const gp_Pnt aP3 = myMesh->Node(V[2]);

      gp_Vec aV1(aP1, aP2);
      gp_Vec aV2(aP2, aP3);

      gp_Vec aN = aV1.Crossed(aV2);
      if (aN.SquareMagnitude() > Precision::SquareConfusion())
        aN.Normalize();
      else
        aN.SetCoord(0.0, 0.0, 0.0);

      for (int j = 0; j < 3; j++)
      {
        myElemNodes->SetValue(i, j + 1, V[j]);
      }

      myElemNormals->SetValue(i, 1, aN.X());
      myElemNormals->SetValue(i, 2, aN.Y());
      myElemNormals->SetValue(i, 3, aN.Z());
    }
  }
  std::cout << "Construction is finished" << std::endl;
}

//=================================================================================================

bool XSDRAWSTL_DataSource::GetGeom(const int                   ID,
                                   const bool                  IsElement,
                                   NCollection_Array1<double>& Coords,
                                   int&                        NbNodes,
                                   MeshVS_EntityType&          Type) const
{
  if (myMesh.IsNull())
    return false;

  if (IsElement)
  {
    if (ID >= 1 && ID <= myElements.Extent())
    {
      Type    = MeshVS_ET_Face;
      NbNodes = 3;

      for (int i = 1, k = 1; i <= 3; i++)
      {
        int IdxNode = myElemNodes->Value(ID, i);
        for (int j = 1; j <= 3; j++, k++)
          Coords(k) = myNodeCoords->Value(IdxNode, j);
      }

      return true;
    }
    else
      return false;
  }
  else if (ID >= 1 && ID <= myNodes.Extent())
  {
    Type    = MeshVS_ET_Node;
    NbNodes = 1;

    Coords(1) = myNodeCoords->Value(ID, 1);
    Coords(2) = myNodeCoords->Value(ID, 2);
    Coords(3) = myNodeCoords->Value(ID, 3);
    return true;
  }
  else
    return false;
}

//=================================================================================================

bool XSDRAWSTL_DataSource::GetGeomType(const int,
                                       const bool         IsElement,
                                       MeshVS_EntityType& Type) const
{
  if (IsElement)
  {
    Type = MeshVS_ET_Face;
    return true;
  }
  else
  {
    Type = MeshVS_ET_Node;
    return true;
  }
}

//=================================================================================================

void* XSDRAWSTL_DataSource::GetAddr(const int, const bool) const
{
  return NULL;
}

//=================================================================================================

bool XSDRAWSTL_DataSource::GetNodesByElement(const int                ID,
                                             NCollection_Array1<int>& theNodeIDs,
                                             int& /*theNbNodes*/) const
{
  if (myMesh.IsNull())
    return false;

  if (ID >= 1 && ID <= myElements.Extent() && theNodeIDs.Length() >= 3)
  {
    int aLow             = theNodeIDs.Lower();
    theNodeIDs(aLow)     = myElemNodes->Value(ID, 1);
    theNodeIDs(aLow + 1) = myElemNodes->Value(ID, 2);
    theNodeIDs(aLow + 2) = myElemNodes->Value(ID, 3);
    return true;
  }
  return false;
}

//=================================================================================================

const TColStd_PackedMapOfInteger& XSDRAWSTL_DataSource::GetAllNodes() const
{
  return myNodes;
}

//=================================================================================================

const TColStd_PackedMapOfInteger& XSDRAWSTL_DataSource::GetAllElements() const
{
  return myElements;
}

//=================================================================================================

bool XSDRAWSTL_DataSource::GetNormal(const int Id,
                                     const int Max,
                                     double&   nx,
                                     double&   ny,
                                     double&   nz) const
{
  if (myMesh.IsNull())
    return false;

  if (Id >= 1 && Id <= myElements.Extent() && Max >= 3)
  {
    nx = myElemNormals->Value(Id, 1);
    ny = myElemNormals->Value(Id, 2);
    nz = myElemNormals->Value(Id, 3);
    return true;
  }
  else
    return false;
}
