// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWCurveElementEndOffset.hxx>
#include <Standard_Real.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_CurveElementEndOffset.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : RWStepFEA_RWCurveElementEndOffset
//purpose  : 
//=======================================================================
RWStepFEA_RWCurveElementEndOffset::RWStepFEA_RWCurveElementEndOffset ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementEndOffset::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepFEA_CurveElementEndOffset) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"curve_element_end_offset") ) return;

  // Own fields of CurveElementEndOffset

  StepFEA_CurveElementEndCoordinateSystem aCoordinateSystem;
  data->ReadEntity (num, 1, "coordinate_system", ach, aCoordinateSystem);

  Handle(TColStd_HArray1OfReal) aOffsetVector;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "offset_vector", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aOffsetVector = new TColStd_HArray1OfReal (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Standard_Real anIt0;
      data->ReadReal (num2, i0, "real", ach, anIt0);
      aOffsetVector->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aCoordinateSystem,
            aOffsetVector);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementEndOffset::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepFEA_CurveElementEndOffset) &ent) const
{

  // Own fields of CurveElementEndOffset

  SW.Send (ent->CoordinateSystem().Value());

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->OffsetVector()->Length(); i1++ ) {
    Standard_Real Var0 = ent->OffsetVector()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementEndOffset::Share (const Handle(StepFEA_CurveElementEndOffset) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementEndOffset

  iter.AddItem (ent->CoordinateSystem().Value());
}
