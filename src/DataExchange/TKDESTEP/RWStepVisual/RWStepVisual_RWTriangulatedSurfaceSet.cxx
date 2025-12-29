// Copyright (c) Open CASCADE 2023
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

#include "RWStepVisual_RWTriangulatedSurfaceSet.pxx"

#include <StepVisual_TriangulatedSurfaceSet.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepVisual_RWTriangulatedSurfaceSet::RWStepVisual_RWTriangulatedSurfaceSet() = default;

//=================================================================================================

void RWStepVisual_RWTriangulatedSurfaceSet::ReadStep(
  const occ::handle<StepData_StepReaderData>&           theData,
  const int                                             theNum,
  occ::handle<Interface_Check>&                         theCheck,
  const occ::handle<StepVisual_TriangulatedSurfaceSet>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 6, theCheck, "triangulated_surface_set"))
  {
    return;
  }

  // Inherited fields of RepresentationItem
  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Inherited fields of TessellatedSurfaceSet
  occ::handle<StepVisual_CoordinatesList> aTessellatedSurfaceSet_Coordinates;
  theData->ReadEntity(theNum,
                      2,
                      "tessellated_surface_set.coordinates",
                      theCheck,
                      STANDARD_TYPE(StepVisual_CoordinatesList),
                      aTessellatedSurfaceSet_Coordinates);

  int aTessellatedSurfaceSet_Pnmax;
  theData->ReadInteger(theNum,
                       3,
                       "tessellated_surface_set.pnmax",
                       theCheck,
                       aTessellatedSurfaceSet_Pnmax);

  occ::handle<NCollection_HArray2<double>> aTessellatedSurfaceSet_Normals;
  int                                      aNormSub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellated_surface_set.normals", theCheck, aNormSub4))
  {
    const int aNormNb              = theData->NbParams(aNormSub4);
    const int anAxesNb             = theData->NbParams(theData->ParamNumber(aNormSub4, 1));
    aTessellatedSurfaceSet_Normals = new NCollection_HArray2<double>(1, aNormNb, 1, anAxesNb);
    for (int aNormIndex = 1; aNormIndex <= aNormNb; aNormIndex++)
    {
      int anAxesSub4 = 0;
      if (theData->ReadSubList(aNormSub4,
                               aNormIndex,
                               "sub-part(tessellated_surface_set.normals)",
                               theCheck,
                               anAxesSub4))
      {
        const int anAxisNum4 = anAxesSub4;
        for (int anAxisIndex = 1; anAxisIndex <= anAxesNb; anAxisIndex++)
        {
          double aValue;
          theData->ReadReal(anAxisNum4, anAxisIndex, "real", theCheck, aValue);
          aTessellatedSurfaceSet_Normals->SetValue(aNormIndex, anAxisIndex, aValue);
        }
      }
    }
  }

  // Own fields of TriangulatedSurfaceSet
  occ::handle<NCollection_HArray1<int>> aPnindex;
  int                                   aPnSub5 = 0;
  if (theData->ReadSubList(theNum, 5, "pnindex", theCheck, aPnSub5))
  {
    const int aPnNb   = theData->NbParams(aPnSub5);
    aPnindex          = new NCollection_HArray1<int>(1, aPnNb);
    const int aPnNum2 = aPnSub5;
    for (int aPnIndex = 1; aPnIndex <= aPnNb; aPnIndex++)
    {
      int aValue;
      theData->ReadInteger(aPnNum2, aPnIndex, "integer", theCheck, aValue);
      aPnindex->SetValue(aPnIndex, aValue);
    }
  }

  occ::handle<NCollection_HArray2<int>> aTriangles;
  int                                   aTrSub6 = 0;
  if (theData->ReadSubList(theNum, 6, "triangles", theCheck, aTrSub6))
  {
    const int aTriaNb  = theData->NbParams(aTrSub6);
    const int anAxesNb = theData->NbParams(theData->ParamNumber(aTrSub6, 1));
    aTriangles         = new NCollection_HArray2<int>(1, aTriaNb, 1, anAxesNb);
    for (int aTriaIndex = 1; aTriaIndex <= aTriaNb; aTriaIndex++)
    {
      int anAxesSubj6 = 0;
      if (theData->ReadSubList(aTrSub6, aTriaIndex, "sub-part(triangles)", theCheck, anAxesSubj6))
      {
        const int anAxisNum4 = anAxesSubj6;
        for (int anAxisIndex = 1; anAxisIndex <= anAxesNb; anAxisIndex++)
        {
          int aValue;
          theData->ReadInteger(anAxisNum4, anAxisIndex, "integer", theCheck, aValue);
          aTriangles->SetValue(aTriaIndex, anAxisIndex, aValue);
        }
      }
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aTessellatedSurfaceSet_Coordinates,
               aTessellatedSurfaceSet_Pnmax,
               aTessellatedSurfaceSet_Normals,
               aPnindex,
               aTriangles);
}

//=================================================================================================

void RWStepVisual_RWTriangulatedSurfaceSet::WriteStep(
  StepData_StepWriter&                                  theSW,
  const occ::handle<StepVisual_TriangulatedSurfaceSet>& theEnt) const
{
  // Own fields of RepresentationItem
  theSW.Send(theEnt->Name());

  // Own fields of TessellatedSurfaceSet
  theSW.Send(theEnt->Coordinates());
  theSW.Send(theEnt->Pnmax());

  theSW.OpenSub();
  for (int aNormIndex = 1; aNormIndex <= theEnt->Normals()->NbRows(); aNormIndex++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    for (int anAxisIndex = 1; anAxisIndex <= theEnt->Normals()->NbColumns(); anAxisIndex++)
    {
      const double aValue = theEnt->Normals()->Value(aNormIndex, anAxisIndex);
      theSW.Send(aValue);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();

  // Own fields of TriangulatedSurfaceSet
  theSW.OpenSub();
  NCollection_HArray1<int>::Iterator aPnIndexIt(theEnt->Pnindex()->Array1());
  for (; aPnIndexIt.More(); aPnIndexIt.Next())
  {
    const int aValue = aPnIndexIt.Value();
    theSW.Send(aValue);
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (int aTriaIndex = 1; aTriaIndex <= theEnt->Triangles()->NbRows(); aTriaIndex++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    for (int anAxisIndex = 1; anAxisIndex <= theEnt->Triangles()->NbColumns(); anAxisIndex++)
    {
      const int aValue = theEnt->Triangles()->Value(aTriaIndex, anAxisIndex);
      theSW.Send(aValue);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWTriangulatedSurfaceSet::Share(
  const occ::handle<StepVisual_TriangulatedSurfaceSet>& theEnt,
  Interface_EntityIterator&                             theIter) const
{
  // Inherited fields of TessellatedSurfaceSet
  theIter.AddItem(theEnt->StepVisual_TessellatedSurfaceSet::Coordinates());
}
