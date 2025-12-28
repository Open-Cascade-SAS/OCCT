// Created on : Thu Mar 24 18:30:11 2022
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include "RWStepVisual_RWComplexTriangulatedSurfaceSet.pxx"
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>
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
#include <Standard_Transient.hxx>

//=================================================================================================

RWStepVisual_RWComplexTriangulatedSurfaceSet::RWStepVisual_RWComplexTriangulatedSurfaceSet() = default;

//=================================================================================================

void RWStepVisual_RWComplexTriangulatedSurfaceSet::ReadStep(
  const occ::handle<StepData_StepReaderData>&                  theData,
  const int                                                    theNum,
  occ::handle<Interface_Check>&                                theCheck,
  const occ::handle<StepVisual_ComplexTriangulatedSurfaceSet>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 7, theCheck, "complex_triangulated_surface_set"))
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
  int                                      sub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellated_surface_set.normals", theCheck, sub4))
  {
    int nb0                        = theData->NbParams(sub4);
    int nbj0                       = theData->NbParams(theData->ParamNumber(sub4, 1));
    aTessellatedSurfaceSet_Normals = new NCollection_HArray2<double>(1, nb0, 1, nbj0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int subj4 = 0;
      if (theData
            ->ReadSubList(sub4, i0, "sub-part(tessellated_surface_set.normals)", theCheck, subj4))
      {
        int num4 = subj4;
        for (int j0 = 1; j0 <= nbj0; j0++)
        {
          double anIt0;
          theData->ReadReal(num4, j0, "real", theCheck, anIt0);
          aTessellatedSurfaceSet_Normals->SetValue(i0, j0, anIt0);
        }
      }
    }
  }

  // Own fields of ComplexTriangulatedSurfaceSet

  occ::handle<NCollection_HArray1<int>> aPnindex;
  int                                   sub5 = 0;
  if (theData->ReadSubList(theNum, 5, "pnindex", theCheck, sub5))
  {
    int nb0  = theData->NbParams(sub5);
    aPnindex = new NCollection_HArray1<int>(1, nb0);
    int num2 = sub5;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aPnindex->SetValue(i0, anIt0);
    }
  }

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriangleStrips;
  int                                                               sub6 = 0;
  if (theData->ReadSubList(theNum, 6, "triangle_strips", theCheck, sub6))
  {
    int nb0         = theData->NbParams(sub6);
    aTriangleStrips = new NCollection_HArray1<occ::handle<Standard_Transient>>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int nbj0 = theData->NbParams(theData->ParamNumber(sub6, i0));
      occ::handle<NCollection_HArray1<int>> aSingleTriangleStrip =
        new NCollection_HArray1<int>(1, nbj0);
      int subj6 = 0;
      if (theData->ReadSubList(sub6, i0, "sub-part(triangle_strips)", theCheck, subj6))
      {
        int num4 = subj6;
        for (int j0 = 1; j0 <= nbj0; j0++)
        {
          int anIt0;
          theData->ReadInteger(num4, j0, "integer", theCheck, anIt0);
          aSingleTriangleStrip->SetValue(j0, anIt0);
        }
        aTriangleStrips->SetValue(i0, aSingleTriangleStrip);
      }
    }
  }

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriangleFans;
  int                                                               sub7 = 0;
  if (theData->ReadSubList(theNum, 7, "triangle_fans", theCheck, sub7))
  {
    int nb0       = theData->NbParams(sub7);
    aTriangleFans = new NCollection_HArray1<occ::handle<Standard_Transient>>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int nbj0 = theData->NbParams(theData->ParamNumber(sub7, i0));
      occ::handle<NCollection_HArray1<int>> aSingleTriangleFan =
        new NCollection_HArray1<int>(1, nbj0);
      int subj7 = 0;
      if (theData->ReadSubList(sub7, i0, "sub-part(triangle_fans)", theCheck, subj7))
      {
        int num4 = subj7;
        for (int j0 = 1; j0 <= nbj0; j0++)
        {
          int anIt0;
          theData->ReadInteger(num4, j0, "integer", theCheck, anIt0);
          aSingleTriangleFan->SetValue(j0, anIt0);
        }
        aTriangleFans->SetValue(i0, aSingleTriangleFan);
      }
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aTessellatedSurfaceSet_Coordinates,
               aTessellatedSurfaceSet_Pnmax,
               aTessellatedSurfaceSet_Normals,
               aPnindex,
               aTriangleStrips,
               aTriangleFans);
}

//=================================================================================================

void RWStepVisual_RWComplexTriangulatedSurfaceSet::WriteStep(
  StepData_StepWriter&                                         theSW,
  const occ::handle<StepVisual_ComplexTriangulatedSurfaceSet>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedSurfaceSet

  theSW.Send(theEnt->Coordinates());

  theSW.Send(theEnt->Pnmax());

  theSW.OpenSub();
  // According to "Recommended Practices Recommended Practices for 3D Tessellated Geometry",
  // Release 1.1:
  // "...The size of the list of normals may be:
  //    0: no normals are defined..."
  // In OCC this situation is reflected by nullptr normals container.
  if (theEnt->NbNormals() != 0)
  {
    for (int i3 = 1; i3 <= theEnt->Normals()->NbRows(); i3++)
    {
      theSW.NewLine(false);
      theSW.OpenSub();
      for (int j3 = 1; j3 <= theEnt->Normals()->NbColumns(); j3++)
      {
        double Var0 = theEnt->Normals()->Value(i3, j3);
        theSW.Send(Var0);
      }
      theSW.CloseSub();
    }
  }
  theSW.CloseSub();

  // Own fields of ComplexTriangulatedSurfaceSet

  theSW.OpenSub();
  for (int i4 = 1; i4 <= theEnt->Pnindex()->Length(); i4++)
  {
    int Var0 = theEnt->Pnindex()->Value(i4);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (int i5 = 1; i5 <= theEnt->NbTriangleStrips(); i5++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    occ::handle<NCollection_HArray1<int>> aTriangleStrip =
      occ::down_cast<NCollection_HArray1<int>>(theEnt->TriangleStrips()->Value(i5));
    for (int j5 = 1; j5 <= aTriangleStrip->Length(); j5++)
    {
      int Var0 = aTriangleStrip->Value(j5);
      theSW.Send(Var0);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (int i6 = 1; i6 <= theEnt->NbTriangleFans(); i6++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    occ::handle<NCollection_HArray1<int>> aTriangleFan =
      occ::down_cast<NCollection_HArray1<int>>(theEnt->TriangleFans()->Value(i6));
    for (int j6 = 1; j6 <= aTriangleFan->Length(); j6++)
    {
      int Var0 = aTriangleFan->Value(j6);
      theSW.Send(Var0);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWComplexTriangulatedSurfaceSet::Share(
  const occ::handle<StepVisual_ComplexTriangulatedSurfaceSet>& theEnt,
  Interface_EntityIterator&                                    theIter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of TessellatedSurfaceSet

  theIter.AddItem(theEnt->StepVisual_TessellatedSurfaceSet::Coordinates());

  // Own fields of ComplexTriangulatedSurfaceSet
}
