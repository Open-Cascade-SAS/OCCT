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

IMPLEMENT_STANDARD_RTTIEXT (Poly_Triangulation, Standard_Transient)

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================
Poly_Triangulation::Poly_Triangulation()
: myCachedMinMax (NULL),
  myDeflection   (0),
  myPurpose      (Poly_MeshPurpose_NONE)
{
  //
}

//=======================================================================
//function : Poly_Triangulation
//purpose  :
//=======================================================================
Poly_Triangulation::Poly_Triangulation (const Standard_Integer theNbNodes,
                                        const Standard_Integer theNbTriangles,
                                        const Standard_Boolean theHasUVNodes,
                                        const Standard_Boolean theHasNormals)
: myCachedMinMax (NULL),
  myDeflection(0),
  myNodes     (theNbNodes),
  myTriangles (1, theNbTriangles),
  myPurpose   (Poly_MeshPurpose_NONE)
{
  if (theHasUVNodes)
  {
    myUVNodes.Resize (theNbNodes, false);
  }
  if (theHasNormals)
  {
    myNormals.Resize (0, theNbNodes - 1, false);
  }
}

//=======================================================================
//function : Poly_Triangulation
//purpose  :
//=======================================================================
Poly_Triangulation::Poly_Triangulation (const TColgp_Array1OfPnt&    theNodes,
                                        const Poly_Array1OfTriangle& theTriangles)
: myCachedMinMax (NULL),
  myDeflection   (0),
  myNodes        (theNodes.Length()),
  myTriangles    (1, theTriangles.Length()),
  myPurpose      (Poly_MeshPurpose_NONE)
{
  const Poly_ArrayOfNodes aNodeWrapper (theNodes.First(), theNodes.Length());
  myNodes = aNodeWrapper;
  myTriangles = theTriangles;
}

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation (const TColgp_Array1OfPnt&    theNodes,
                                        const TColgp_Array1OfPnt2d&  theUVNodes,
                                        const Poly_Array1OfTriangle& theTriangles)
: myCachedMinMax (NULL),
  myDeflection   (0),
  myNodes        (theNodes.Length()),
  myTriangles    (1, theTriangles.Length()),
  myUVNodes      (theNodes.Length()),
  myPurpose      (Poly_MeshPurpose_NONE)
{
  const Poly_ArrayOfNodes aNodeWrapper (theNodes.First(), theNodes.Length());
  myNodes = aNodeWrapper;
  myTriangles = theTriangles;
  const Poly_ArrayOfUVNodes aUVNodeWrapper (theUVNodes.First(), theUVNodes.Length());
  myUVNodes = aUVNodeWrapper;
}

//=======================================================================
//function : ~Poly_Triangulation
//purpose  :
//=======================================================================
Poly_Triangulation::~Poly_Triangulation()
{
  delete myCachedMinMax;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) Poly_Triangulation::Copy() const
{
  return new Poly_Triangulation (this);
}

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation (const Handle(Poly_Triangulation)& theTriangulation)
: myCachedMinMax (NULL),
  myDeflection(theTriangulation->myDeflection),
  myNodes     (theTriangulation->myNodes),
  myTriangles (theTriangulation->myTriangles),
  myUVNodes   (theTriangulation->myUVNodes),
  myNormals   (theTriangulation->myNormals),
  myPurpose   (theTriangulation->myPurpose)
{
  SetCachedMinMax (theTriangulation->CachedMinMax());
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void Poly_Triangulation::Clear()
{
  if (!myNodes.IsEmpty())
  {
    Poly_ArrayOfNodes anEmptyNodes;
    anEmptyNodes.SetDoublePrecision (myNodes.IsDoublePrecision());
    myNodes.Move (anEmptyNodes);
  }
  if (!myTriangles.IsEmpty())
  {
    Poly_Array1OfTriangle anEmptyTriangles;
    myTriangles.Move(anEmptyTriangles);
  }
  RemoveUVNodes();
  RemoveNormals();
}

//=======================================================================
//function : RemoveUVNodes
//purpose  :
//=======================================================================
void Poly_Triangulation::RemoveUVNodes()
{
  if (!myUVNodes.IsEmpty())
  {
    Poly_ArrayOfUVNodes anEmpty;
    anEmpty.SetDoublePrecision (myUVNodes.IsDoublePrecision());
    myUVNodes.Move (anEmpty);
  }
}

//=======================================================================
//function : RemoveNormals
//purpose  :
//=======================================================================
void Poly_Triangulation::RemoveNormals()
{
  if (!myNormals.IsEmpty())
  {
    NCollection_Array1<gp_Vec3f> anEmpty;
    myNormals.Move (anEmpty);
  }
}

//=======================================================================
//function : MapNodeArray
//purpose  :
//=======================================================================
Handle(TColgp_HArray1OfPnt) Poly_Triangulation::MapNodeArray() const
{
  if (myNodes.IsEmpty())
  {
    return Handle(TColgp_HArray1OfPnt)();
  }

  if (myNodes.IsDoublePrecision())
  {
    // wrap array
    const gp_Pnt* aPntArr = &myNodes.First<gp_Pnt>();
    Handle(TColgp_HArray1OfPnt) anHArray = new TColgp_HArray1OfPnt();
    TColgp_Array1OfPnt anArray (*aPntArr, 1, NbNodes());
    anHArray->Move (anArray);
    return anHArray;
  }

  // deep copy
  Handle(TColgp_HArray1OfPnt) anArray = new TColgp_HArray1OfPnt (1, NbNodes());
  for (Standard_Integer aNodeIter = 0; aNodeIter < NbNodes(); ++aNodeIter)
  {
    const gp_Pnt aPnt = myNodes.Value (aNodeIter);
    anArray->SetValue (aNodeIter + 1, aPnt);
  }
  return anArray;
}

//=======================================================================
//function : MapTriangleArray
//purpose  :
//=======================================================================
Handle(Poly_HArray1OfTriangle) Poly_Triangulation::MapTriangleArray() const
{
  if (myTriangles.IsEmpty())
  {
    return Handle(Poly_HArray1OfTriangle)();
  }

  Handle(Poly_HArray1OfTriangle) anHArray = new Poly_HArray1OfTriangle();
  Poly_Array1OfTriangle anArray (myTriangles.First(), 1, NbTriangles());
  anHArray->Move (anArray);
  return anHArray;
}

//=======================================================================
//function : MapUVNodeArray
//purpose  :
//=======================================================================
Handle(TColgp_HArray1OfPnt2d) Poly_Triangulation::MapUVNodeArray() const
{
  if (myUVNodes.IsEmpty())
  {
    return Handle(TColgp_HArray1OfPnt2d)();
  }

  if (myUVNodes.IsDoublePrecision())
  {
    // wrap array
    const gp_Pnt2d* aPntArr = &myUVNodes.First<gp_Pnt2d>();
    Handle(TColgp_HArray1OfPnt2d) anHArray = new TColgp_HArray1OfPnt2d();
    TColgp_Array1OfPnt2d anArray (*aPntArr, 1, NbNodes());
    anHArray->Move (anArray);
    return anHArray;
  }

  // deep copy
  Handle(TColgp_HArray1OfPnt2d) anArray = new TColgp_HArray1OfPnt2d (1, NbNodes());
  for (Standard_Integer aNodeIter = 0; aNodeIter < NbNodes(); ++aNodeIter)
  {
    const gp_Pnt2d aPnt = myUVNodes.Value (aNodeIter);
    anArray->SetValue (aNodeIter + 1, aPnt);
  }
  return anArray;
}

//=======================================================================
//function : MapNormalArray
//purpose  :
//=======================================================================
Handle(TShort_HArray1OfShortReal) Poly_Triangulation::MapNormalArray() const
{
  if (myNormals.IsEmpty())
  {
    return Handle(TShort_HArray1OfShortReal)();
  }

  Handle(TShort_HArray1OfShortReal) anHArray = new TShort_HArray1OfShortReal();
  TShort_Array1OfShortReal anArray (*myNormals.First().GetData(), 1, 3 * NbNodes());
  anHArray->Move (anArray);
  return anHArray;
}

//=======================================================================
//function : SetNormals
//purpose  :
//=======================================================================
void Poly_Triangulation::SetNormals (const Handle(TShort_HArray1OfShortReal)& theNormals)
{
  if (theNormals.IsNull() || theNormals->Length() != 3 * NbNodes())
  {
    throw Standard_DomainError("Poly_Triangulation::SetNormals : wrong length");
  }

  AddNormals();
  const Standard_Integer anArrayLower = theNormals->Lower();
  for (Standard_Integer aNodeIter = 1; aNodeIter <= NbNodes(); ++aNodeIter)
  {
    Standard_Integer anArrayInd = anArrayLower + (aNodeIter - 1) * 3;
    gp_Vec3f aNorm (theNormals->Value (anArrayInd + 0),
                    theNormals->Value (anArrayInd + 1),
                    theNormals->Value (anArrayInd + 2));
    SetNormal (aNodeIter, aNorm);
  }
}

// =======================================================================
// function : SetDoublePrecision
// purpose  :
// =======================================================================
void Poly_Triangulation::SetDoublePrecision (bool theIsDouble)
{
  myNodes  .SetDoublePrecision (theIsDouble);
  myUVNodes.SetDoublePrecision (theIsDouble);
}

// =======================================================================
// function : ResizeNodes
// purpose  :
// =======================================================================
void Poly_Triangulation::ResizeNodes (Standard_Integer theNbNodes,
                                      Standard_Boolean theToCopyOld)
{
  myNodes.Resize (theNbNodes, theToCopyOld);
  if (!myUVNodes.IsEmpty())
  {
    myUVNodes.Resize (theNbNodes, theToCopyOld);
  }
  if (!myNormals.IsEmpty())
  {
    myNormals.Resize (0, theNbNodes - 1, theToCopyOld);
  }
}

// =======================================================================
// function : ResizeTriangles
// purpose  :
// =======================================================================
void Poly_Triangulation::ResizeTriangles (Standard_Integer theNbTriangles,
                                          Standard_Boolean theToCopyOld)
{
  myTriangles.Resize (1, theNbTriangles, theToCopyOld);
}

// =======================================================================
// function : AddUVNodes
// purpose  :
// =======================================================================
void Poly_Triangulation::AddUVNodes()
{
  if (myUVNodes.IsEmpty() || myUVNodes.Size() != myNodes.Size())
  {
    myUVNodes.Resize (myNodes.Size(), false);
  }
}

// =======================================================================
// function : AddNormals
// purpose  :
// =======================================================================
void Poly_Triangulation::AddNormals()
{
  if (myNormals.IsEmpty() || myNormals.Size() != myNodes.Size())
  {
    myNormals.Resize (0, myNodes.Size() - 1, false);
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================

void Poly_Triangulation::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDeflection)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myNodes.Size())
  if (!myUVNodes.IsEmpty())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUVNodes.Size())
  if (!myNormals.IsEmpty())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myNormals.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTriangles.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myPurpose)

}

// =======================================================================
// function : CachedMinMax
// purpose  :
// =======================================================================
const Bnd_Box& Poly_Triangulation::CachedMinMax() const
{
  static const Bnd_Box anEmptyBox;
  return (myCachedMinMax == NULL) ? anEmptyBox : *myCachedMinMax;
}

// =======================================================================
// function : SetCachedMinMax
// purpose  :
// =======================================================================
void Poly_Triangulation::SetCachedMinMax (const Bnd_Box& theBox)
{
  if (theBox.IsVoid())
  {
    unsetCachedMinMax();
    return;
  }
  if (myCachedMinMax == NULL)
  {
    myCachedMinMax = new Bnd_Box();
  }
  *myCachedMinMax = theBox;
}

// =======================================================================
// function : unsetCachedMinMax
// purpose  :
// =======================================================================
void Poly_Triangulation::unsetCachedMinMax()
{
  if (myCachedMinMax != NULL)
  {
    delete myCachedMinMax;
    myCachedMinMax = NULL;
  }
}

// =======================================================================
// function : MinMax
// purpose  :
// =======================================================================
Standard_Boolean Poly_Triangulation::MinMax (Bnd_Box& theBox, const gp_Trsf& theTrsf, const bool theIsAccurate) const
{
  Bnd_Box aBox;
  if (HasCachedMinMax() &&
      (!HasGeometry() || !theIsAccurate ||
       theTrsf.Form() == gp_Identity || theTrsf.Form() == gp_Translation ||
       theTrsf.Form() == gp_PntMirror || theTrsf.Form() == gp_Scale))
  {
    aBox = myCachedMinMax->Transformed (theTrsf);
  }
  else
  {
    aBox = computeBoundingBox (theTrsf);
  }
  if (aBox.IsVoid())
  {
    return Standard_False;
  }
  theBox.Add (aBox);
  return Standard_True;
}

// =======================================================================
// function : computeBoundingBox
// purpose  :
// =======================================================================
Bnd_Box Poly_Triangulation::computeBoundingBox (const gp_Trsf& theTrsf) const
{
  Bnd_Box aBox;
  if (theTrsf.Form() == gp_Identity)
  {
    for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes(); aNodeIdx++)
    {
      aBox.Add (myNodes.Value (aNodeIdx));
    }
  }
  else
  {
    for (Standard_Integer aNodeIdx = 0; aNodeIdx < NbNodes(); aNodeIdx++)
    {
      aBox.Add (myNodes.Value (aNodeIdx).Transformed (theTrsf));
    }
  }
  return aBox;
}

//=======================================================================
//function : ComputeNormals
//purpose  :
//=======================================================================
void Poly_Triangulation::ComputeNormals()
{
  // zero values
  AddNormals();
  myNormals.Init (gp_Vec3f (0.0f));

  Standard_Integer anElem[3] = {0, 0, 0};
  for (Poly_Array1OfTriangle::Iterator aTriIter (myTriangles); aTriIter.More(); aTriIter.Next())
  {
    aTriIter.Value().Get (anElem[0], anElem[1], anElem[2]);
    const gp_Pnt aNode0 = myNodes.Value (anElem[0] - 1);
    const gp_Pnt aNode1 = myNodes.Value (anElem[1] - 1);
    const gp_Pnt aNode2 = myNodes.Value (anElem[2] - 1);

    const gp_XYZ aVec01 = aNode1.XYZ() - aNode0.XYZ();
    const gp_XYZ aVec02 = aNode2.XYZ() - aNode0.XYZ();
    const gp_XYZ aTriNorm = aVec01 ^ aVec02;
    const gp_Vec3f aNorm3f = gp_Vec3f (float(aTriNorm.X()), float(aTriNorm.Y()), float(aTriNorm.Z()));
    for (Standard_Integer aNodeIter = 0; aNodeIter < 3; ++aNodeIter)
    {
      myNormals.ChangeValue (anElem[aNodeIter] - 1) += aNorm3f;
    }
  }

  // Normalize all vectors
  for (NCollection_Array1<gp_Vec3f>::Iterator aNodeIter (myNormals); aNodeIter.More(); aNodeIter.Next())
  {
    gp_Vec3f& aNorm3f = aNodeIter.ChangeValue();
    const float aMod = aNorm3f.Modulus();
    aNorm3f = aMod == 0.0f ? gp_Vec3f (0.0f, 0.0f, 1.0f) : (aNorm3f / aMod);
  }
}

//=======================================================================
//function : LoadDeferredData
//purpose  :
//=======================================================================
Standard_Boolean Poly_Triangulation::LoadDeferredData (const Handle(OSD_FileSystem)& theFileSystem)
{
  if (!HasDeferredData())
  {
    return false;
  }
  if (!loadDeferredData (theFileSystem, this))
  {
    return false;
  }
  SetMeshPurpose (myPurpose | Poly_MeshPurpose_Loaded);
  return true;
}

//=======================================================================
//function : DetachedLoadDeferredData
//purpose  :
//=======================================================================
Handle(Poly_Triangulation) Poly_Triangulation::DetachedLoadDeferredData (const Handle(OSD_FileSystem)& theFileSystem) const
{
  if (!HasDeferredData())
  {
    return Handle(Poly_Triangulation)();
  }
  Handle(Poly_Triangulation) aResult = createNewEntity();
  if (!loadDeferredData (theFileSystem, aResult))
  {
    return Handle(Poly_Triangulation)();
  }
  aResult->SetMeshPurpose (aResult->MeshPurpose() | Poly_MeshPurpose_Loaded);
  return aResult;
}

//=======================================================================
//function : UnloadDeferredData
//purpose  :
//=======================================================================
Standard_Boolean Poly_Triangulation::UnloadDeferredData()
{
  if (HasDeferredData())
  {
    Clear();
    SetMeshPurpose (myPurpose & ~Poly_MeshPurpose_Loaded);
    return true;
  }
  return false;
}
