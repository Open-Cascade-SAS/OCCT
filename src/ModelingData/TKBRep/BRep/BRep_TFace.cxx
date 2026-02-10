// Created on: 1992-08-25
// Created by: Remi Lequette
// Copyright (c) 1992-1999 Matra Datavision
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

#include <BRep_TFace.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_TFace, TopoDS_TFace)

//=================================================================================================

BRep_TFace::BRep_TFace()
    : myTolerance(Precision::Confusion()),
      myNaturalRestriction(false),
      myIsPlane(false)
{
}

//=================================================================================================

//! Checks if the given surface is a plane (directly, or through trimming/offset).
static bool computeIsPlane(const occ::handle<Geom_Surface>& theSurface)
{
  if (theSurface.IsNull())
  {
    return false;
  }
  occ::handle<Geom_RectangularTrimmedSurface> aGRTS =
    occ::down_cast<Geom_RectangularTrimmedSurface>(theSurface);
  occ::handle<Geom_OffsetSurface> aGOFS = occ::down_cast<Geom_OffsetSurface>(theSurface);
  occ::handle<Geom_Plane>         aGP;
  if (!aGOFS.IsNull())
  {
    aGP = occ::down_cast<Geom_Plane>(aGOFS->BasisSurface());
  }
  else if (!aGRTS.IsNull())
  {
    aGP = occ::down_cast<Geom_Plane>(aGRTS->BasisSurface());
  }
  else
  {
    aGP = occ::down_cast<Geom_Plane>(theSurface);
  }
  return !aGP.IsNull();
}

//=================================================================================================

void BRep_TFace::Surface(const occ::handle<Geom_Surface>& theSurface)
{
  mySurface = theSurface;
  myIsPlane = computeIsPlane(theSurface);
}

//=================================================================================================

void BRep_TFace::Tolerance(const double theTolerance)
{
  myTolerance = std::max(theTolerance, Precision::Confusion());
}

//=================================================================================================

occ::handle<TopoDS_TShape> BRep_TFace::EmptyCopy() const
{
  occ::handle<BRep_TFace> TF = new BRep_TFace();
  // Assign surface and cached plane flag directly to avoid
  // redundant RTTI downcasts in computeIsPlane().
  TF->mySurface = mySurface;
  TF->myIsPlane = myIsPlane;
  TF->myLocation = myLocation;
  TF->Tolerance(myTolerance);
  return TF;
}

//=================================================================================================

const occ::handle<Poly_Triangulation>& BRep_TFace::Triangulation(
  const Poly_MeshPurpose thePurpose) const
{
  if (thePurpose == Poly_MeshPurpose_NONE)
  {
    return ActiveTriangulation();
  }
  for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(myTriangulations);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Poly_Triangulation>& aTriangulation = anIter.Value();
    if ((aTriangulation->MeshPurpose() & thePurpose) != 0)
    {
      return aTriangulation;
    }
  }
  if ((thePurpose & Poly_MeshPurpose_AnyFallback) != 0 && !myTriangulations.IsEmpty())
  {
    // if none matching other criteria was found return the first defined triangulation
    return myTriangulations.First();
  }
  static const occ::handle<Poly_Triangulation> anEmptyTriangulation;
  return anEmptyTriangulation;
}

//=================================================================================================

void BRep_TFace::Triangulation(const occ::handle<Poly_Triangulation>& theTriangulation,
                               const bool                             theToReset)
{
  if (theToReset || theTriangulation.IsNull())
  {
    if (!myActiveTriangulation.IsNull())
    {
      // Reset Active bit
      myActiveTriangulation->SetMeshPurpose(myActiveTriangulation->MeshPurpose()
                                            & ~Poly_MeshPurpose_Active);
      myActiveTriangulation.Nullify();
    }
    myTriangulations.Clear();
    if (!theTriangulation.IsNull())
    {
      // Reset list of triangulations to new list with only one input triangulation that will be
      // active
      myTriangulations.Append(theTriangulation);
      myActiveTriangulation = theTriangulation;
      // Set Active bit
      theTriangulation->SetMeshPurpose(theTriangulation->MeshPurpose() | Poly_MeshPurpose_Active);
    }
    return;
  }
  for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(myTriangulations);
       anIter.More();
       anIter.Next())
  {
    // Make input triangulation active if it is already contained in list of triangulations
    if (anIter.Value() == theTriangulation)
    {
      if (!myActiveTriangulation.IsNull())
      {
        // Reset Active bit
        myActiveTriangulation->SetMeshPurpose(myActiveTriangulation->MeshPurpose()
                                              & ~Poly_MeshPurpose_Active);
      }
      myActiveTriangulation = theTriangulation;
      // Set Active bit
      theTriangulation->SetMeshPurpose(theTriangulation->MeshPurpose() | Poly_MeshPurpose_Active);
      return;
    }
  }
  for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(myTriangulations);
       anIter.More();
       anIter.Next())
  {
    // Replace active triangulation to input one
    if (anIter.Value() == myActiveTriangulation)
    {
      // Reset Active bit
      myActiveTriangulation->SetMeshPurpose(myActiveTriangulation->MeshPurpose()
                                            & ~Poly_MeshPurpose_Active);
      anIter.ChangeValue()  = theTriangulation;
      myActiveTriangulation = theTriangulation;
      // Set Active bit
      theTriangulation->SetMeshPurpose(theTriangulation->MeshPurpose() | Poly_MeshPurpose_Active);
      return;
    }
  }
}

//=================================================================================================

void BRep_TFace::Triangulations(
  const NCollection_List<occ::handle<Poly_Triangulation>>& theTriangulations,
  const occ::handle<Poly_Triangulation>&                   theActiveTriangulation)
{
  if (theTriangulations.IsEmpty())
  {
    myActiveTriangulation.Nullify();
    myTriangulations.Clear();
    return;
  }
  bool anActiveInList = false;
  for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(theTriangulations);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Poly_Triangulation>& aTriangulation = anIter.Value();
    Standard_ASSERT_RAISE(!aTriangulation.IsNull(),
                          "Try to set list with NULL triangulation to the face");
    if (aTriangulation == theActiveTriangulation)
    {
      anActiveInList = true;
    }
    // Reset Active bit
    aTriangulation->SetMeshPurpose(aTriangulation->MeshPurpose() & ~Poly_MeshPurpose_Active);
  }
  Standard_ASSERT_RAISE(theActiveTriangulation.IsNull() || anActiveInList,
                        "Active triangulation isn't part of triangulations list");
  myTriangulations = theTriangulations;
  if (theActiveTriangulation.IsNull())
  {
    // Save the first one as active
    myActiveTriangulation = myTriangulations.First();
  }
  else
  {
    myActiveTriangulation = theActiveTriangulation;
  }
  myActiveTriangulation->SetMeshPurpose(myActiveTriangulation->MeshPurpose()
                                        | Poly_MeshPurpose_Active);
}

//=================================================================================================

void BRep_TFace::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TopoDS_TFace)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myActiveTriangulation.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, mySurface.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myLocation)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTolerance)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNaturalRestriction)

  for (NCollection_List<occ::handle<Poly_Triangulation>>::Iterator anIter(myTriangulations);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Poly_Triangulation>& aTriangulation = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aTriangulation.get())
  }
}
