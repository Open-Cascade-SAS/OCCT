// File:	StepToTopoDS_TranslateShell.cxx
// Created:	Tue Jan  3 14:01:53 1995
// Author:	Frederic MAUPAS
//		<fma@stylox>
//:   gka 09.04.99: S4136: improving tolerance management

#include <StepToTopoDS_TranslateShell.ixx>

#include <StepToTopoDS_TranslateFace.hxx>

#include <StepShape_FaceSurface.hxx>

#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <Transfer_TransientProcess.hxx>

#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressSentry.hxx>

// ============================================================================
// Method  : StepToTopoDS_TranslateShell::StepToTopoDS_TranslateShell
// Purpose : Empty Constructor
// ============================================================================

StepToTopoDS_TranslateShell::StepToTopoDS_TranslateShell()
{
  done = Standard_False;
}

// ============================================================================
// Method  : StepToTopoDS_TranslateShell::StepToTopoDS_TranslateShell()
// Purpose : Constructor with a ConnectedFaceSet and a Tool
// ============================================================================

StepToTopoDS_TranslateShell::StepToTopoDS_TranslateShell
(const Handle(StepShape_ConnectedFaceSet)& CFS, StepToTopoDS_Tool& T, StepToTopoDS_NMTool& NMTool)
{
  Init(CFS, T, NMTool);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a ConnectedFaceSet and a Tool
// ============================================================================

void StepToTopoDS_TranslateShell::Init
(const Handle(StepShape_ConnectedFaceSet)& CFS, StepToTopoDS_Tool& aTool, StepToTopoDS_NMTool& NMTool)
{

  if (!aTool.IsBound(CFS)) {

    BRep_Builder B;
    Handle(Transfer_TransientProcess) TP = aTool.TransientProcess();

    Standard_Integer NbFc = CFS->NbCfsFaces();
    TopoDS_Shell Sh;
    B.MakeShell(Sh);
    TopoDS_Face F;
    TopoDS_Shape S;
    Handle(StepShape_Face) StepFace;

    StepToTopoDS_TranslateFace myTranFace;
    myTranFace.SetPrecision(Precision()); //gka
    myTranFace.SetMaxTol(MaxTol());

    Message_ProgressSentry PS ( TP->GetProgress(), "Face", 0, NbFc, 1 );
    for (Standard_Integer i=1; i<=NbFc && PS.More(); i++, PS.Next()) {
#ifdef DEBUG
      cout << "Processing Face : " << i << endl;
#endif
      StepFace = CFS->CfsFacesValue(i);
      Handle(StepShape_FaceSurface) theFS = 
	Handle(StepShape_FaceSurface)::DownCast(StepFace);
      if (!theFS.IsNull()) {
	myTranFace.Init(theFS, aTool, NMTool);
	if (myTranFace.IsDone()) {
	  S = myTranFace.Value();
	  F = TopoDS::Face(S);
	  B.Add(Sh, F);
	}
	else { // Warning only + add FaceSurface file Identifier
	  TP->AddWarning(theFS," a Face from Shell not mapped to TopoDS");
	}
      }
      else { // Warning : ajouter identifier
	TP->AddWarning(StepFace," Face is not of FaceSurface Type; not mapped to TopoDS");
      }
    }
    myResult = Sh;
    aTool.Bind(CFS, myResult);
    myError  = StepToTopoDS_TranslateShellDone;
    done     = Standard_True;
  }
  else {
    myResult = TopoDS::Shell(aTool.Find(CFS));
    myError  = StepToTopoDS_TranslateShellDone;
    done     = Standard_True;
  }
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslateShell::Value() const 
{
  StdFail_NotDone_Raise_if(!done,"");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Return the TranslateShell Error code
// ============================================================================

StepToTopoDS_TranslateShellError StepToTopoDS_TranslateShell::Error() const
{
  return myError;
}
