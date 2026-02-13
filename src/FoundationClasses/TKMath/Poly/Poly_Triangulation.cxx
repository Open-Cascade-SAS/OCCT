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

#include <Poly_Triangulation.hxx>

#include <gp_Pnt.hxx>
#include <OSD_FileSystem.hxx>
#include <Poly_Triangle.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Poly_Triangulation, Standard_Transient)

//=================================================================================================

Poly_Triangulation::Poly_Triangulation()
    : myCachedMinMax(nullptr),
      myDeflection(0),
      myPurpose(Poly_MeshPurpose_NONE)
{
}

//=================================================================================================

Poly_Triangulation::Poly_Triangulation(const int  theNbNodes,
                                       const int  theNbTriangles,
                                       const bool theHasUVNodes,
                                       const bool theHasNormals)
    : myCachedMinMax(nullptr),
      myDeflection(0),
      myNodes(theNbNodes),
      myTriangles(1, theNbTriangles),
      myPurpose(Poly_MeshPurpose_NONE)
{
  if (theHasUVNodes)
  {
    myUVNodes.Resize(theNbNodes, false);
  }
  if (theHasNormals)
  {
    myNormals.Resize(0, theNbNodes - 1, false);
  }
}

//=================================================================================================

Poly_Triangulation::Poly_Triangulation(const NCollection_Array1<gp_Pnt>&        theNodes,
                                       const NCollection_Array1<Poly_Triangle>& theTriangles)
    : myCachedMinMax(nullptr),
      myDeflection(0),
      myNodes(theNodes.Length()),
      myTriangles(1, theTriangles.Length()),
      myPurpose(Poly_MeshPurpose_NONE)
{
  const Poly_ArrayOfNodes aNodeWrapper(theNodes.First(), theNodes.Length());
  myNodes     = aNodeWrapper;
  myTriangles = theTriangles;
}

//=================================================================================================

Poly_Triangulation::Poly_Triangulation(const NCollection_Array1<gp_Pnt>&        theNodes,
                                       const NCollection_Array1<gp_Pnt2d>&      theUVNodes,
                                       const NCollection_Array1<Poly_Triangle>& theTriangles)
    : myCachedMinMax(nullptr),
      myDeflection(0),
      myNodes(theNodes.Length()),
      myTriangles(1, theTriangles.Length()),
      myUVNodes(theNodes.Length()),
      myPurpose(Poly_MeshPurpose_NONE)
{
  const Poly_ArrayOfNodes aNodeWrapper(theNodes.First(), theNodes.Length());
  myNodes     = aNodeWrapper;
  myTriangles = theTriangles;
  const Poly_ArrayOfUVNodes aUVNodeWrapper(theUVNodes.First(), theUVNodes.Length());
  myUVNodes = aUVNodeWrapper;
}

//=================================================================================================

Poly_Triangulation::~Poly_Triangulation()
{
  delete myCachedMinMax;
}

//=================================================================================================

occ::handle<Poly_Triangulation> Poly_Triangulation::Copy() const
{
  return new Poly_Triangulation(this);
}

//=================================================================================================

Poly_Triangulation::Poly_Triangulation(const occ::handle<Poly_Triangulation>& theTriangulation)
    : myCachedMinMax(nullptr),
      myDeflection(theTriangulation->myDeflection),
      myNodes(theTriangulation->myNodes),
      myTriangles(theTriangulation->myTriangles),
      myUVNodes(theTriangulation->myUVNodes),
      myNormals(theTriangulation->myNormals),
      myPurpose(theTriangulation->myPurpose)
{
  SetCachedMinMax(theTriangulation->CachedMinMax());
}

//=================================================================================================

void Poly_Triangulation::Clear()
{
  if (!myNodes.IsEmpty())
  {
    Poly_ArrayOfNodes anEmptyNodes;
    anEmptyNodes.SetDoublePrecision(myNodes.IsDoublePrecision());
    myNodes.Move(anEmptyNodes);
  }
  if (!myTriangles.IsEmpty())
  {
    NCollection_Array1<Poly_Triangle> anEmptyTriangles;
    myTriangles.Move(anEmptyTriangles);
  }
  RemoveUVNodes();
  RemoveNormals();
}

//=================================================================================================

void Poly_Triangulation::RemoveUVNodes()
{
  if (!myUVNodes.IsEmpty())
  {
    Poly_ArrayOfUVNodes anEmpty;
    anEmpty.SetDoublePrecision(myUVNodes.IsDoublePrecision());
    myUVNodes.Move(anEmpty);
  }
}

//=================================================================================================

void Poly_Triangulation::RemoveNormals()
{
  if (!myNormals.IsEmpty())
  {
    NCollection_Array1<NCollection_Vec3<float>> anEmpty;
    myNormals.Move(anEmpty);
  }
}

//=================================================================================================

occ::handle<NCollection_HArray1<gp_Pnt>> Poly_Triangulation::MapNodeArray() const
{
  if (myNodes.IsEmpty())
  {
    return occ::handle<NCollection_HArray1<gp_Pnt>>();
  }

  if (myNodes.IsDoublePrecision())
  {
    // wrap array
    const gp_Pnt*                            aPntArr  = &myNodes.First<gp_Pnt>();
    occ::handle<NCollection_HArray1<gp_Pnt>> anHArray = new NCollection_HArray1<gp_Pnt>();
    NCollection_Array1<gp_Pnt>               anArray(*aPntArr, 1, NbNodes());
    anHArray->Move(anArray);
    return anHArray;
  }

  // deep copy
  occ::handle<NCollection_HArray1<gp_Pnt>> anArray = new NCollection_HArray1<gp_Pnt>(1, NbNodes());
  for (int aNodeIter = 0; aNodeIter < NbNodes(); ++aNodeIter)
  {
    const gp_Pnt aPnt = myNodes.Value(aNodeIter);
    anArray->SetValue(aNodeIter + 1, aPnt);
  }
  return anArray;
}

//=================================================================================================

occ::handle<NCollection_HArray1<Poly_Triangle>> Poly_Triangulation::MapTriangleArray() const
{
  if (myTriangles.IsEmpty())
  {
    return occ::handle<NCollection_HArray1<Poly_Triangle>>();
  }

  occ::handle<NCollection_HArray1<Poly_Triangle>> anHArray =
    new NCollection_HArray1<Poly_Triangle>();
  NCollection_Array1<Poly_Triangle> anArray(myTriangles.First(), 1, NbTriangles());
  anHArray->Move(anArray);
  return anHArray;
}

//=================================================================================================

occ::handle<NCollection_HArray1<gp_Pnt2d>> Poly_Triangulation::MapUVNodeArray() const
{
  if (myUVNodes.IsEmpty())
  {
    return occ::handle<NCollection_HArray1<gp_Pnt2d>>();
  }

  if (myUVNodes.IsDoublePrecision())
  {
    // wrap array
    const gp_Pnt2d*                            aPntArr  = &myUVNodes.First<gp_Pnt2d>();
    occ::handle<NCollection_HArray1<gp_Pnt2d>> anHArray = new NCollection_HArray1<gp_Pnt2d>();
    NCollection_Array1<gp_Pnt2d>               anArray(*aPntArr, 1, NbNodes());
    anHArray->Move(anArray);
    return anHArray;
  }

  // deep copy
  occ::handle<NCollection_HArray1<gp_Pnt2d>> anArray =
    new NCollection_HArray1<gp_Pnt2d>(1, NbNodes());
  for (int aNodeIter = 0; aNodeIter < NbNodes(); ++aNodeIter)
  {
    const gp_Pnt2d aPnt = myUVNodes.Value(aNodeIter);
    anArray->SetValue(aNodeIter + 1, aPnt);
  }
  return anArray;
}

//=================================================================================================

occ::handle<NCollection_HArray1<float>> Poly_Triangulation::MapNormalArray() const
{
  if (myNormals.IsEmpty())
  {
    return occ::handle<NCollection_HArray1<float>>();
  }

  occ::handle<NCollection_HArray1<float>> anHArray = new NCollection_HArray1<float>();
  NCollection_Array1<float>               anArray(*myNormals.First().GetData(), 1, 3 * NbNodes());
  anHArray->Move(anArray);
  return anHArray;
}

//=================================================================================================

void Poly_Triangulation::SetNormals(const occ::handle<NCollection_HArray1<float>>& theNormals)
{
  if (theNormals.IsNull() || theNormals->Length() != 3 * NbNodes())
  {
    throw Standard_DomainError("Poly_Triangulation::SetNormals : wrong length");
  }

  AddNormals();
  const int anArrayLower = theNormals->Lower();
  for (int aNodeIter = 1; aNodeIter <= NbNodes(); ++aNodeIter)
  {
    int                     anArrayInd = anArrayLower + (aNodeIter - 1) * 3;
    NCollection_Vec3<float> aNorm(theNormals->Value(anArrayInd + 0),
                                  theNormals->Value(anArrayInd + 1),
                                  theNormals->Value(anArrayInd + 2));
    SetNormal(aNodeIter, aNorm);
  }
}

//=================================================================================================

void Poly_Triangulation::SetDoublePrecision(bool theIsDouble)
{
  myNodes.SetDoublePrecision(theIsDouble);
  myUVNodes.SetDoublePrecision(theIsDouble);
}

//=================================================================================================

void Poly_Triangulation::ResizeNodes(int theNbNodes, bool theToCopyOld)
{
  myNodes.Resize(theNbNodes, theToCopyOld);
  if (!myUVNodes.IsEmpty())
  {
    myUVNodes.Resize(theNbNodes, theToCopyOld);
  }
  if (!myNormals.IsEmpty())
  {
    myNormals.Resize(0, theNbNodes - 1, theToCopyOld);
  }
}

//=================================================================================================

void Poly_Triangulation::ResizeTriangles(int theNbTriangles, bool theToCopyOld)
{
  myTriangles.Resize(1, theNbTriangles, theToCopyOld);
}

//=================================================================================================

void Poly_Triangulation::AddUVNodes()
{
  if (myUVNodes.IsEmpty() || myUVNodes.Size() != myNodes.Size())
  {
    myUVNodes.Resize(myNodes.Size(), false);
  }
}

//=================================================================================================

void Poly_Triangulation::AddNormals()
{
  if (myNormals.IsEmpty() || myNormals.Size() != myNodes.Size())
  {
    myNormals.Resize(0, myNodes.Size() - 1, false);
  }
}

//=================================================================================================

void Poly_Triangulation::DumpJson(Standard_OStream& theOStream, int) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeflection)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNodes.Size())
  if (!myUVNodes.IsEmpty())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUVNodes.Size())
  if (!myNormals.IsEmpty())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNormals.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTriangles.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPurpose)
}

//=================================================================================================

const Bnd_Box& Poly_Triangulation::CachedMinMax() const
{
  static const Bnd_Box anEmptyBox;
  return (myCachedMinMax == nullptr) ? anEmptyBox : *myCachedMinMax;
}

//=================================================================================================

void Poly_Triangulation::SetCachedMinMax(const Bnd_Box& theBox)
{
  if (theBox.IsVoid())
  {
    unsetCachedMinMax();
    return;
  }
  if (myCachedMinMax == nullptr)
  {
    myCachedMinMax = new Bnd_Box();
  }
  *myCachedMinMax = theBox;
}

//=================================================================================================

void Poly_Triangulation::unsetCachedMinMax()
{
  delete myCachedMinMax;
  myCachedMinMax = nullptr;
}

//=================================================================================================

bool Poly_Triangulation::MinMax(Bnd_Box&       theBox,
                                const gp_Trsf& theTrsf,
                                const bool     theIsAccurate) const
{
  Bnd_Box aBox;
  if (HasCachedMinMax()
      && (!HasGeometry() || !theIsAccurate || theTrsf.Form() == gp_Identity
          || theTrsf.Form() == gp_Translation || theTrsf.Form() == gp_PntMirror
          || theTrsf.Form() == gp_Scale))
  {
    aBox = myCachedMinMax->Transformed(theTrsf);
  }
  else
  {
    aBox = computeBoundingBox(theTrsf);
  }
  if (aBox.IsVoid())
  {
    return false;
  }
  theBox.Add(aBox);
  return true;
}

//=================================================================================================

Bnd_Box Poly_Triangulation::computeBoundingBox(const gp_Trsf& theTrsf) const
{
  Bnd_Box aBox;
  if (theTrsf.Form() == gp_Identity)
  {
    for (int aNodeIdx = 0; aNodeIdx < NbNodes(); aNodeIdx++)
    {
      aBox.Add(myNodes.Value(aNodeIdx));
    }
  }
  else
  {
    for (int aNodeIdx = 0; aNodeIdx < NbNodes(); aNodeIdx++)
    {
      aBox.Add(myNodes.Value(aNodeIdx).Transformed(theTrsf));
    }
  }
  return aBox;
}

//=================================================================================================

void Poly_Triangulation::ComputeNormals()
{
  // zero values
  AddNormals();
  myNormals.Init(NCollection_Vec3<float>(0.0f));

  int anElem[3] = {0, 0, 0};
  for (NCollection_Array1<Poly_Triangle>::Iterator aTriIter(myTriangles); aTriIter.More();
       aTriIter.Next())
  {
    aTriIter.Value().Get(anElem[0], anElem[1], anElem[2]);
    const gp_Pnt aNode0 = myNodes.Value(anElem[0] - 1);
    const gp_Pnt aNode1 = myNodes.Value(anElem[1] - 1);
    const gp_Pnt aNode2 = myNodes.Value(anElem[2] - 1);

    const gp_XYZ                  aVec01   = aNode1.XYZ() - aNode0.XYZ();
    const gp_XYZ                  aVec02   = aNode2.XYZ() - aNode0.XYZ();
    const gp_XYZ                  aTriNorm = aVec01 ^ aVec02;
    const NCollection_Vec3<float> aNorm3f =
      NCollection_Vec3<float>(float(aTriNorm.X()), float(aTriNorm.Y()), float(aTriNorm.Z()));
    for (int aNodeIter = 0; aNodeIter < 3; ++aNodeIter)
    {
      myNormals.ChangeValue(anElem[aNodeIter] - 1) += aNorm3f;
    }
  }

  // Normalize all vectors
  for (NCollection_Array1<NCollection_Vec3<float>>::Iterator aNodeIter(myNormals); aNodeIter.More();
       aNodeIter.Next())
  {
    NCollection_Vec3<float>& aNorm3f = aNodeIter.ChangeValue();
    const float              aMod    = aNorm3f.Modulus();
    aNorm3f = aMod == 0.0f ? NCollection_Vec3<float>(0.0f, 0.0f, 1.0f) : (aNorm3f / aMod);
  }
}

//=================================================================================================

bool Poly_Triangulation::LoadDeferredData(const occ::handle<OSD_FileSystem>& theFileSystem)
{
  if (!HasDeferredData())
  {
    return false;
  }
  if (!loadDeferredData(theFileSystem, this))
  {
    return false;
  }
  SetMeshPurpose(myPurpose | Poly_MeshPurpose_Loaded);
  return true;
}

//=================================================================================================

occ::handle<Poly_Triangulation> Poly_Triangulation::DetachedLoadDeferredData(
  const occ::handle<OSD_FileSystem>& theFileSystem) const
{
  if (!HasDeferredData())
  {
    return occ::handle<Poly_Triangulation>();
  }
  occ::handle<Poly_Triangulation> aResult = createNewEntity();
  if (!loadDeferredData(theFileSystem, aResult))
  {
    return occ::handle<Poly_Triangulation>();
  }
  aResult->SetMeshPurpose(aResult->MeshPurpose() | Poly_MeshPurpose_Loaded);
  return aResult;
}

//=================================================================================================

bool Poly_Triangulation::UnloadDeferredData()
{
  if (HasDeferredData())
  {
    Clear();
    SetMeshPurpose(myPurpose & ~Poly_MeshPurpose_Loaded);
    return true;
  }
  return false;
}
