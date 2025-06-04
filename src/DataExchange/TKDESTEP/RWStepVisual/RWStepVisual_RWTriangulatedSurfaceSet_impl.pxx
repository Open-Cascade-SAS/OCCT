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
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfInteger.hxx>

//=================================================================================================

RWStepVisual_RWTriangulatedSurfaceSet::RWStepVisual_RWTriangulatedSurfaceSet() {}

//=================================================================================================

void RWStepVisual_RWTriangulatedSurfaceSet::ReadStep(
  const Handle(StepData_StepReaderData)&           theData,
  const Standard_Integer                           theNum,
  Handle(Interface_Check)&                         theCheck,
  const Handle(StepVisual_TriangulatedSurfaceSet)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 6, theCheck, "triangulated_surface_set"))
  {
    return;
  }

  // Inherited fields of RepresentationItem
  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Inherited fields of TessellatedSurfaceSet
  Handle(StepVisual_CoordinatesList) aTessellatedSurfaceSet_Coordinates;
  theData->ReadEntity(theNum,
                      2,
                      "tessellated_surface_set.coordinates",
                      theCheck,
                      STANDARD_TYPE(StepVisual_CoordinatesList),
                      aTessellatedSurfaceSet_Coordinates);

  Standard_Integer aTessellatedSurfaceSet_Pnmax;
  theData->ReadInteger(theNum,
                       3,
                       "tessellated_surface_set.pnmax",
                       theCheck,
                       aTessellatedSurfaceSet_Pnmax);

  Handle(TColStd_HArray2OfReal) aTessellatedSurfaceSet_Normals;
  Standard_Integer              aNormSub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellated_surface_set.normals", theCheck, aNormSub4))
  {
    const Standard_Integer aNormNb  = theData->NbParams(aNormSub4);
    const Standard_Integer anAxesNb = theData->NbParams(theData->ParamNumber(aNormSub4, 1));
    aTessellatedSurfaceSet_Normals  = new TColStd_HArray2OfReal(1, aNormNb, 1, anAxesNb);
    for (Standard_Integer aNormIndex = 1; aNormIndex <= aNormNb; aNormIndex++)
    {
      Standard_Integer anAxesSub4 = 0;
      if (theData->ReadSubList(aNormSub4,
                               aNormIndex,
                               "sub-part(tessellated_surface_set.normals)",
                               theCheck,
                               anAxesSub4))
      {
        const Standard_Integer anAxisNum4 = anAxesSub4;
        for (Standard_Integer anAxisIndex = 1; anAxisIndex <= anAxesNb; anAxisIndex++)
        {
          Standard_Real aValue;
          theData->ReadReal(anAxisNum4, anAxisIndex, "real", theCheck, aValue);
          aTessellatedSurfaceSet_Normals->SetValue(aNormIndex, anAxisIndex, aValue);
        }
      }
    }
  }

  // Own fields of TriangulatedSurfaceSet
  Handle(TColStd_HArray1OfInteger) aPnindex;
  Standard_Integer                 aPnSub5 = 0;
  if (theData->ReadSubList(theNum, 5, "pnindex", theCheck, aPnSub5))
  {
    const Standard_Integer aPnNb   = theData->NbParams(aPnSub5);
    aPnindex                       = new TColStd_HArray1OfInteger(1, aPnNb);
    const Standard_Integer aPnNum2 = aPnSub5;
    for (Standard_Integer aPnIndex = 1; aPnIndex <= aPnNb; aPnIndex++)
    {
      Standard_Integer aValue;
      theData->ReadInteger(aPnNum2, aPnIndex, "integer", theCheck, aValue);
      aPnindex->SetValue(aPnIndex, aValue);
    }
  }

  Handle(TColStd_HArray2OfInteger) aTriangles;
  Standard_Integer                 aTrSub6 = 0;
  if (theData->ReadSubList(theNum, 6, "triangles", theCheck, aTrSub6))
  {
    const Standard_Integer aTriaNb  = theData->NbParams(aTrSub6);
    const Standard_Integer anAxesNb = theData->NbParams(theData->ParamNumber(aTrSub6, 1));
    aTriangles                      = new TColStd_HArray2OfInteger(1, aTriaNb, 1, anAxesNb);
    for (Standard_Integer aTriaIndex = 1; aTriaIndex <= aTriaNb; aTriaIndex++)
    {
      Standard_Integer anAxesSubj6 = 0;
      if (theData->ReadSubList(aTrSub6, aTriaIndex, "sub-part(triangles)", theCheck, anAxesSubj6))
      {
        const Standard_Integer anAxisNum4 = anAxesSubj6;
        for (Standard_Integer anAxisIndex = 1; anAxisIndex <= anAxesNb; anAxisIndex++)
        {
          Standard_Integer aValue;
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
  StepData_StepWriter&                             theSW,
  const Handle(StepVisual_TriangulatedSurfaceSet)& theEnt) const
{
  // Own fields of RepresentationItem
  theSW.Send(theEnt->Name());

  // Own fields of TessellatedSurfaceSet
  theSW.Send(theEnt->Coordinates());
  theSW.Send(theEnt->Pnmax());

  theSW.OpenSub();
  for (Standard_Integer aNormIndex = 1; aNormIndex <= theEnt->Normals()->NbRows(); aNormIndex++)
  {
    theSW.NewLine(Standard_False);
    theSW.OpenSub();
    for (Standard_Integer anAxisIndex = 1; anAxisIndex <= theEnt->Normals()->NbColumns();
         anAxisIndex++)
    {
      const Standard_Real aValue = theEnt->Normals()->Value(aNormIndex, anAxisIndex);
      theSW.Send(aValue);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();

  // Own fields of TriangulatedSurfaceSet
  theSW.OpenSub();
  TColStd_HArray1OfInteger::Iterator aPnIndexIt(theEnt->Pnindex()->Array1());
  for (; aPnIndexIt.More(); aPnIndexIt.Next())
  {
    const Standard_Integer aValue = aPnIndexIt.Value();
    theSW.Send(aValue);
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (Standard_Integer aTriaIndex = 1; aTriaIndex <= theEnt->Triangles()->NbRows(); aTriaIndex++)
  {
    theSW.NewLine(Standard_False);
    theSW.OpenSub();
    for (Standard_Integer anAxisIndex = 1; anAxisIndex <= theEnt->Triangles()->NbColumns();
         anAxisIndex++)
    {
      const Standard_Integer aValue = theEnt->Triangles()->Value(aTriaIndex, anAxisIndex);
      theSW.Send(aValue);
    }
    theSW.CloseSub();
  }
  theSW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWTriangulatedSurfaceSet::Share(
  const Handle(StepVisual_TriangulatedSurfaceSet)& theEnt,
  Interface_EntityIterator&                        theIter) const
{
  // Inherited fields of TessellatedSurfaceSet
  theIter.AddItem(theEnt->StepVisual_TessellatedSurfaceSet::Coordinates());
}
