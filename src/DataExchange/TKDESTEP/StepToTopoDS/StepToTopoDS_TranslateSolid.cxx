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

//=================================================================================================

StepToTopoDS_TranslateSolid::StepToTopoDS_TranslateSolid()
    : myError(StepToTopoDS_TranslateSolidOther)
{
  done = false;
}

// ============================================================================
// Method  : Init
// Purpose : Init with a TessellatedSolid and a Tool
// ============================================================================

void StepToTopoDS_TranslateSolid::Init(const occ::handle<StepVisual_TessellatedSolid>& theTSo,
                                       const occ::handle<Transfer_TransientProcess>&   theTP,
                                       StepToTopoDS_Tool&                         theTool,
                                       StepToTopoDS_NMTool&                       theNMTool,
                                       const bool  theReadTessellatedWhenNoBRepOnly,
                                       bool&       theHasGeom,
                                       const StepData_Factors& theLocalFactors,
                                       const Message_ProgressRange& theProgress)
{
  if (theTSo.IsNull())
    return;

  BRep_Builder aB;
  TopoDS_Shell aSh;
  TopoDS_Solid aSo;

  int      aNb = theTSo->NbItems();
  Message_ProgressScope aPS(theProgress, "Face", aNb);

  if (theTSo->HasGeometricLink() && theTP->IsBound(theTSo->GeometricLink()))
  {
    occ::handle<TransferBRep_ShapeBinder> aBinder =
      occ::down_cast<TransferBRep_ShapeBinder>(theTP->Find(theTSo->GeometricLink()));
    if (aBinder)
      aSo = aBinder->Solid();
  }

  bool aNewSolid = false;
  if (aSo.IsNull())
  {
    aB.MakeShell(aSh);
    aB.MakeSolid(aSo);
    aNewSolid  = true;
    theHasGeom = false;
  }

  occ::handle<Transfer_TransientProcess> aTP = theTool.TransientProcess();

  StepToTopoDS_TranslateFace aTranTF;
  aTranTF.SetPrecision(Precision());
  aTranTF.SetMaxTol(MaxTol());

  for (int i = 1; i <= aNb && aPS.More(); i++, aPS.Next())
  {
#ifdef OCCT_DEBUG
    std::cout << "Processing Face : " << i << std::endl;
#endif
    occ::handle<StepVisual_TessellatedStructuredItem> anItem = theTSo->ItemsValue(i);
    if (anItem->IsKind(STANDARD_TYPE(StepVisual_TessellatedFace)))
    {
      occ::handle<StepVisual_TessellatedFace> aTFace =
        occ::down_cast<StepVisual_TessellatedFace>(anItem);
      bool aHasFaceGeom = false;
      aTranTF.Init(aTFace,
                   theTool,
                   theNMTool,
                   theReadTessellatedWhenNoBRepOnly,
                   aHasFaceGeom,
                   theLocalFactors);
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
  myError  = StepToTopoDS_TranslateSolidDone;
  done     = true;
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslateSolid::Value() const
{
  StdFail_NotDone_Raise_if(!done, "StepToTopoDS_TranslateSolid::Value() - no result");
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
