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

#include "RWStepVisual_RWComplexTriangulatedFace.pxx"
#include <StepVisual_ComplexTriangulatedFace.hxx>
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
#include <StepVisual_FaceOrSurface.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepVisual_RWComplexTriangulatedFace::RWStepVisual_RWComplexTriangulatedFace() {}

//=================================================================================================

void RWStepVisual_RWComplexTriangulatedFace::ReadStep(
  const occ::handle<StepData_StepReaderData>&            theData,
  const int                            theNum,
  occ::handle<Interface_Check>&                          theCheck,
  const occ::handle<StepVisual_ComplexTriangulatedFace>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 8, theCheck, "complex_triangulated_face"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Inherited fields of TessellatedFace

  occ::handle<StepVisual_CoordinatesList> aTessellatedFace_Coordinates;
  theData->ReadEntity(theNum,
                      2,
                      "tessellated_face.coordinates",
                      theCheck,
                      STANDARD_TYPE(StepVisual_CoordinatesList),
                      aTessellatedFace_Coordinates);

  int aTessellatedFace_Pnmax;
  theData->ReadInteger(theNum, 3, "tessellated_face.pnmax", theCheck, aTessellatedFace_Pnmax);

  occ::handle<NCollection_HArray2<double>> aTessellatedFace_Normals;
  int              sub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellated_face.normals", theCheck, sub4))
  {
    int nb0     = theData->NbParams(sub4);
    int nbj0    = theData->NbParams(theData->ParamNumber(sub4, 1));
    aTessellatedFace_Normals = new NCollection_HArray2<double>(1, nb0, 1, nbj0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int subj4 = 0;
      if (theData->ReadSubList(sub4, i0, "sub-part(tessellated_face.normals)", theCheck, subj4))
      {
        int num4 = subj4;
        for (int j0 = 1; j0 <= nbj0; j0++)
        {
          double anIt0;
          theData->ReadReal(num4, j0, "real", theCheck, anIt0);
          aTessellatedFace_Normals->SetValue(i0, j0, anIt0);
        }
      }
    }
  }

  StepVisual_FaceOrSurface aTessellatedFace_GeometricLink;
  bool         hasTessellatedFace_GeometricLink = true;
  if (theData->IsParamDefined(theNum, 5))
  {
    theData->ReadEntity(theNum,
                        5,
                        "tessellated_face.geometric_link",
                        theCheck,
                        aTessellatedFace_GeometricLink);
  }
  else
  {
    hasTessellatedFace_GeometricLink = false;
    aTessellatedFace_GeometricLink   = StepVisual_FaceOrSurface();
  }

  // Own fields of ComplexTriangulatedFace

  occ::handle<NCollection_HArray1<int>> aPnindex;
  int                 sub6 = 0;
  if (theData->ReadSubList(theNum, 6, "pnindex", theCheck, sub6))
  {
    int nb0  = theData->NbParams(sub6);
    aPnindex              = new NCollection_HArray1<int>(1, nb0);
    int num2 = sub6;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aPnindex->SetValue(i0, anIt0);
    }
  }

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> aTriangleStrips;
  int                   sub7 = 0;
  if (theData->ReadSubList(theNum, 7, "triangle_strips", theCheck, sub7))
  {
    int nb0 = theData->NbParams(sub7);
    aTriangleStrips      = new NCollection_HArray1<occ::handle<Standard_Transient>>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int                 nbj0 = theData->NbParams(theData->ParamNumber(sub7, i0));
      occ::handle<NCollection_HArray1<int>> aSingleTriangleStrip = new NCollection_HArray1<int>(1, nbj0);
      int                 subj7                = 0;
      if (theData->ReadSubList(sub7, i0, "sub-part(triangle_strips)", theCheck, subj7))
      {
        int num4 = subj7;
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
  int                   sub8 = 0;
  if (theData->ReadSubList(theNum, 8, "triangle_fans", theCheck, sub8))
  {
    int nb0 = theData->NbParams(sub8);
    aTriangleFans        = new NCollection_HArray1<occ::handle<Standard_Transient>>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int                 nbj0 = theData->NbParams(theData->ParamNumber(sub8, i0));
      occ::handle<NCollection_HArray1<int>> aSingleTriangleFan = new NCollection_HArray1<int>(1, nbj0);
      int                 subj8              = 0;
      if (theData->ReadSubList(sub8, i0, "sub-part(triangle_fans)", theCheck, subj8))
      {
        int num4 = subj8;
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
               aTessellatedFace_Coordinates,
               aTessellatedFace_Pnmax,
               aTessellatedFace_Normals,
               hasTessellatedFace_GeometricLink,
               aTessellatedFace_GeometricLink,
               aPnindex,
               aTriangleStrips,
               aTriangleFans);
}

//=================================================================================================

void RWStepVisual_RWComplexTriangulatedFace::WriteStep(
  StepData_StepWriter&                              theSW,
  const occ::handle<StepVisual_ComplexTriangulatedFace>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedFace

  theSW.Send(theEnt->Coordinates());

  theSW.Send(theEnt->Pnmax());

  theSW.OpenSub();
  for (int i3 = 1; i3 <= theEnt->Normals()->RowLength(); i3++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    for (int j3 = 1; j3 <= theEnt->Normals()->ColLength(); j3++)
    {
      double Var0 = theEnt->Normals()->Value(i3, j3);
      theSW.Send(Var0);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();

  if (theEnt->HasGeometricLink())
  {
    theSW.Send(theEnt->GeometricLink().Value());
  }
  else
  {
    theSW.SendUndef();
  }

  // Own fields of ComplexTriangulatedFace

  theSW.OpenSub();
  for (int i5 = 1; i5 <= theEnt->Pnindex()->Length(); i5++)
  {
    int Var0 = theEnt->Pnindex()->Value(i5);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (int i6 = 1; i6 <= theEnt->NbTriangleStrips(); i6++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    occ::handle<NCollection_HArray1<int>> aTriangleStrip =
      occ::down_cast<NCollection_HArray1<int>>(theEnt->TriangleStrips()->Value(i6));
    for (int j6 = 1; j6 <= aTriangleStrip->Length(); j6++)
    {
      int Var0 = aTriangleStrip->Value(j6);
      theSW.Send(Var0);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (int i7 = 1; i7 <= theEnt->NbTriangleFans(); i7++)
  {
    theSW.NewLine(false);
    theSW.OpenSub();
    occ::handle<NCollection_HArray1<int>> aTriangleFan =
      occ::down_cast<NCollection_HArray1<int>>(theEnt->TriangleFans()->Value(i7));
    for (int j7 = 1; j7 <= aTriangleFan->Length(); j7++)
    {
      int Var0 = aTriangleFan->Value(j7);
      theSW.Send(Var0);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWComplexTriangulatedFace::Share(
  const occ::handle<StepVisual_ComplexTriangulatedFace>& theEnt,
  Interface_EntityIterator&                         theIter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of TessellatedFace

  theIter.AddItem(theEnt->StepVisual_TessellatedFace::Coordinates());

  if (theEnt->StepVisual_TessellatedFace::HasGeometricLink())
  {
    theIter.AddItem(theEnt->StepVisual_TessellatedFace::GeometricLink().Value());
  }

  // Own fields of ComplexTriangulatedFace
}
