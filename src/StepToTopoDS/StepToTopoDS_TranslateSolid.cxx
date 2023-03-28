// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <BRep_Builder.hxx>
#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_FaceSurface.hxx>
#include <StepToTopoDS_NMTool.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslateFace.hxx>
#include <StepToTopoDS_TranslateSolid.hxx>
#include <StepVisual_TessellatedFace.hxx>
#include <StepVisual_TessellatedSolid.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep_ShapeBinder.hxx>

// ============================================================================
// Method  : StepToTopoDS_TranslateSolid::StepToTopoDS_TranslateSolid
// Purpose : Empty Constructor
// ============================================================================
StepToTopoDS_TranslateSolid::StepToTopoDS_TranslateSolid()
  : myError(StepToTopoDS_TranslateSolidOther)
{
  done = Standard_False;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a TessellatedSolid and a Tool
// ============================================================================

void StepToTopoDS_TranslateSolid::Init(const Handle(StepVisual_TessellatedSolid)& theTSo,
                                       const Handle(Transfer_TransientProcess)& theTP,
                                       StepToTopoDS_Tool& theTool,
                                       StepToTopoDS_NMTool& theNMTool,
                                       const Standard_Boolean theReadTessellatedWhenNoBRepOnly,
                                       Standard_Boolean& theHasGeom,
                                       const StepData_Factors& theLocalFactors,
                                       const Message_ProgressRange& theProgress)
{
  if (theTSo.IsNull())
    return;

  BRep_Builder aB;
  TopoDS_Shell aSh;
  TopoDS_Solid aSo;

  Standard_Integer aNb = theTSo->NbItems();
  Message_ProgressScope aPS(theProgress, "Face", aNb);

  if (theTSo->HasGeometricLink() && theTP->IsBound(theTSo->GeometricLink())) 
  {
    Handle(TransferBRep_ShapeBinder) aBinder 
      = Handle(TransferBRep_ShapeBinder)::DownCast(theTP->Find(theTSo->GeometricLink()));
    if (aBinder) 
      aSo = aBinder->Solid();
  }

  Standard_Boolean aNewSolid = Standard_False;
  if (aSo.IsNull()) 
  {
    aB.MakeShell(aSh);
    aB.MakeSolid(aSo);
    aNewSolid = Standard_True;
    theHasGeom = Standard_False;
  }

  Handle(Transfer_TransientProcess) aTP = theTool.TransientProcess();

  StepToTopoDS_TranslateFace aTranTF;
  aTranTF.SetPrecision(Precision());
  aTranTF.SetMaxTol(MaxTol());

  for (Standard_Integer i = 1; i <= aNb && aPS.More(); i++, aPS.Next()) 
  {
#ifdef OCCT_DEBUG
    std::cout << "Processing Face : " << i << std::endl;
#endif
    Handle(StepVisual_TessellatedStructuredItem) anItem = theTSo->ItemsValue(i);
    if (anItem->IsKind(STANDARD_TYPE(StepVisual_TessellatedFace))) 
    {
      Handle(StepVisual_TessellatedFace) aTFace = Handle(StepVisual_TessellatedFace)::DownCast(anItem);
      Standard_Boolean aHasFaceGeom = Standard_False;
      aTranTF.Init(aTFace, theTool, theNMTool, theReadTessellatedWhenNoBRepOnly, aHasFaceGeom, theLocalFactors);
      if (aTranTF.IsDone()) 
      {
        if (aNewSolid) 
        {
          aB.Add(aSh, TopoDS::Face(aTranTF.Value()));
        }
        theHasGeom &= aHasFaceGeom;
      }
      else 
      {
        aTP->AddWarning(anItem, " Tessellated face if not mapped to TopoDS");
      }
    }
    else 
    {
      aTP->AddWarning(anItem, " Face is not of TessellatedFace Type; not mapped to TopoDS");
    }
  }

  if (aNewSolid) 
  {
    aB.Add(aSo, aSh);
  }

  myResult = aSo;
  myError = StepToTopoDS_TranslateSolidDone;
  done = Standard_True;
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslateSolid::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "StepToTopoDS_TranslateSolid::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Return the TranslateShell Error code
// ============================================================================

StepToTopoDS_TranslateSolidError StepToTopoDS_TranslateSolid::Error() const
{
  return myError;
}

