// Created on: 1994-11-25
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Message_ProgressScope.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_FaceSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_MakeStepFace.hxx>
#include <TopoDSToStep_MakeTessellatedItem.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>

//=================================================================================================

TopoDSToStep_Builder::TopoDSToStep_Builder()
    : myError(TopoDSToStep_BuilderOther)
{
  done = false;
}

//=================================================================================================

TopoDSToStep_Builder::TopoDSToStep_Builder(const TopoDS_Shape&                        aShape,
                                           TopoDSToStep_Tool&                         aTool,
                                           const occ::handle<Transfer_FinderProcess>& FP,
                                           const int                    theTessellatedGeomParam,
                                           const StepData_Factors&      theLocalFactors,
                                           const Message_ProgressRange& theProgress)
{
  done = false;
  Init(aShape, aTool, FP, theTessellatedGeomParam, theLocalFactors, theProgress);
}

//=================================================================================================

void TopoDSToStep_Builder::Init(const TopoDS_Shape&                        aShape,
                                TopoDSToStep_Tool&                         myTool,
                                const occ::handle<Transfer_FinderProcess>& FP,
                                const int                                  theTessellatedGeomParam,
                                const StepData_Factors&                    theLocalFactors,
                                const Message_ProgressRange&               theProgress)
{

  if (myTool.IsBound(aShape))
  {
    myError  = TopoDSToStep_BuilderDone;
    done     = true;
    myResult = myTool.Find(aShape);
    return;
  }

  switch (aShape.ShapeType())
  {
    case TopAbs_SHELL: {
      TopoDS_Shell myShell = TopoDS::Shell(aShape);
      myTool.SetCurrentShell(myShell);

      occ::handle<StepShape_FaceSurface>                    FS;
      occ::handle<StepShape_TopologicalRepresentationItem>  Fpms;
      NCollection_Sequence<occ::handle<Standard_Transient>> mySeq;

      //	const TopoDS_Shell ForwardShell =
      //	  TopoDS::Shell(myShell.Oriented(TopAbs_FORWARD));

      //	TopExp_Explorer myExp(ForwardShell, TopAbs_FACE);
      //  CKY  9-DEC-1997 (PRO9824 et consorts)
      //   Pour passer les orientations : ELLES SONT DONNEES EN RELATIF
      //   Donc, pour SHELL, on doit l ecrire en direct en STEP (pas le choix)
      //   -> il faut repercuter en dessous, donc explorer le Shell TEL QUEL
      //   Pour FACE WIRE, d une part on ECRIT SON ORIENTATION relative au contenant
      //     (puisqu on peut), d autre part on EXPLORE EN FORWARD : ainsi les
      //     orientations des sous-shapes sont relatives a leur contenant immediat
      //     et la recombinaison en lecture est sans malice
      //  Il reste ici et la du code relatif a "en Faceted on combine differemment"
      //  -> reste encore du menage a faire

      TopExp_Explorer anExp;

      TopoDSToStep_MakeStepFace MkFace;

      Message_ProgressScope aPS(theProgress, nullptr, (theTessellatedGeomParam != 0) ? 2 : 1);

      int nbshapes = 0;
      for (anExp.Init(myShell, TopAbs_FACE); anExp.More(); anExp.Next())
        nbshapes++;
      Message_ProgressScope aPS1(aPS.Next(), nullptr, nbshapes);
      for (anExp.Init(myShell, TopAbs_FACE); anExp.More() && aPS1.More(); anExp.Next(), aPS1.Next())
      {
        const TopoDS_Face Face = TopoDS::Face(anExp.Current());

        MkFace.Init(Face, myTool, FP, theLocalFactors);

        if (MkFace.IsDone())
        {
          FS   = occ::down_cast<StepShape_FaceSurface>(MkFace.Value());
          Fpms = FS;
          mySeq.Append(Fpms);
        }
        else
        {
          // MakeFace Error Handling : warning only
          //	    std::cout << "Warning : one Face has not been mapped" << std::endl;
          //	  occ::handle<TransferBRep_ShapeMapper> errShape =
          //	    new TransferBRep_ShapeMapper(Face);
          //	    FP->AddWarning(errShape, " a Face from a Shell has not been mapped");
        }
      }
      if (!aPS1.More())
        return;

      int nbFaces = mySeq.Length();
      if (nbFaces >= 1)
      {
        occ::handle<NCollection_HArray1<occ::handle<StepShape_Face>>> aSet =
          new NCollection_HArray1<occ::handle<StepShape_Face>>(1, nbFaces);
        for (int i = 1; i <= nbFaces; i++)
        {
          aSet->SetValue(i, occ::down_cast<StepShape_Face>(mySeq.Value(i)));
        }
        occ::handle<StepShape_ConnectedFaceSet> CFSpms;
        if (myShell.Closed())
          CFSpms = new StepShape_ClosedShell();
        else
          CFSpms = new StepShape_OpenShell();
        occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
        CFSpms->Init(aName, aSet);

        // --------------------------------------------------------------
        // To add later : if not facetted context & shell is reversed
        //                then shall create an oriented_shell with
        //                orientation flag to false.
        // --------------------------------------------------------------

        myTool.Bind(aShape, CFSpms);
        myResult = CFSpms;
        done     = true;
      }
      else
      {
        // Builder Error handling;
        myError = TopoDSToStep_NoFaceMapped;
        done    = false;
      }

      if (theTessellatedGeomParam == 1 || (theTessellatedGeomParam == 2 && myResult.IsNull()))
      {
        TopoDSToStep_MakeTessellatedItem MkTessShell(myShell,
                                                     myTool,
                                                     FP,
                                                     theLocalFactors,
                                                     aPS.Next());
        if (MkTessShell.IsDone())
        {
          myTessellatedResult = MkTessShell.Value();
          myError             = TopoDSToStep_BuilderDone;
          done                = true;
        }
      }
      break;
    }

    case TopAbs_FACE: {
      const TopoDS_Face Face = TopoDS::Face(aShape);

      occ::handle<StepShape_FaceSurface>                   FS;
      occ::handle<StepShape_TopologicalRepresentationItem> Fpms;

      TopoDSToStep_MakeStepFace MkFace(Face, myTool, FP, theLocalFactors);

      TopoDSToStep_MakeTessellatedItem MkTessFace;

      if (theTessellatedGeomParam == 1 || (theTessellatedGeomParam == 2 && !MkFace.IsDone()))
      {
        Message_ProgressScope aPS(theProgress, nullptr, 1);
        // fourth parameter is true in order to create a tessellated_surface_set entity
        // or put false to create a triangulated_face instead
        MkTessFace.Init(Face, myTool, FP, true, theLocalFactors, aPS.Next());
      }

      if (MkFace.IsDone() || MkTessFace.IsDone())
      {
        if (MkFace.IsDone())
        {
          FS       = occ::down_cast<StepShape_FaceSurface>(MkFace.Value());
          Fpms     = FS;
          myResult = Fpms;
        }
        if (MkTessFace.IsDone())
        {
          myTessellatedResult = MkTessFace.Value();
        }
        myError = TopoDSToStep_BuilderDone;
        done    = true;
      }
      else
      {
        // MakeFace Error Handling : Face not Mapped
        myError = TopoDSToStep_BuilderOther;
        //	  occ::handle<TransferBRep_ShapeMapper> errShape =
        //	    new TransferBRep_ShapeMapper(Face);
        //	  FP->AddWarning(errShape, " the Face has not been mapped");
        done = false;
      }
      break;
    }
    default:
      break;
  }
}

// ============================================================================
// Method  : Value
// Purpose : Returns TopologicalRepresentationItem as the result
// ============================================================================
const occ::handle<StepShape_TopologicalRepresentationItem>& TopoDSToStep_Builder::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_Builder::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  : TessellatedValue
// Purpose : Returns TopologicalRepresentationItem as the optional result
// ============================================================================
const occ::handle<StepVisual_TessellatedItem>& TopoDSToStep_Builder::TessellatedValue() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_Builder::TessellatedValue() - no result");
  return myTessellatedResult;
}

// ============================================================================
// Method  : Error
// Purpose : Returns builder error if the process is not done
// ============================================================================
TopoDSToStep_BuilderError TopoDSToStep_Builder::Error() const
{
  return myError;
}
