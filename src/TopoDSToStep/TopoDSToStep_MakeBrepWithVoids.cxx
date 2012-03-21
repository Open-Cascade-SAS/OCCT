// Created on: 1993-07-23
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <TopoDSToStep_MakeBrepWithVoids.ixx>
#include <TopoDSToStep.hxx>
#include <TopoDSToStep_Builder.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_OpenShell.hxx> //:d7
#include <StepShape_ClosedShell.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <BRepTools.hxx>
#include <StdFail_NotDone.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <TCollection_HAsciiString.hxx>

#ifdef DEB
#define DEBUG
#endif

//=============================================================================
// Create a BrepWithVoids of StepShape from a Solid of TopoDS containing
// more than one closed shell 
//=============================================================================

TopoDSToStep_MakeBrepWithVoids::
  TopoDSToStep_MakeBrepWithVoids(const TopoDS_Solid& aSolid,
				    const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False ;
  TopoDS_Iterator              It;
  MoniTool_DataMapOfShapeTransient   aMap;
  TColStd_SequenceOfTransient  S;
  TopoDS_Shell                 aOutShell;

  Handle(StepShape_TopologicalRepresentationItem) aItem;
  Handle(StepShape_ClosedShell)                   aOuter, aCShell;
  Handle(StepShape_OrientedClosedShell)           aOCShell;
  Handle(StepShape_HArray1OfOrientedClosedShell)  aVoids;

  aOutShell = BRepTools::OuterShell(aSolid);

  TopoDSToStep_Builder StepB;
  TopoDSToStep_Tool    aTool;
  
  if (!aOutShell.IsNull()) {
    It.Initialize(aSolid);
    for ( ; It.More(); It.Next() ) {
      if (It.Value().ShapeType() == TopAbs_SHELL) {
	TopoDS_Shell CurrentShell = TopoDS::Shell(It.Value());
	if ( ! aOutShell.IsEqual(CurrentShell) ) //:e0 abv 25 Mar 98: voids should be reversed according to EXPRESS for ABSR
	  CurrentShell.Reverse(); 
	//:d7 abv 16 Mar 98: try to treat 'open' shells as closed since flag 
	// IsClosed() is often incorrect (taken from MakeManifoldSolid(Solid))
	aTool.Init(aMap, Standard_False);
	StepB.Init(CurrentShell, aTool, FP);
	TopoDSToStep::AddResult ( FP, aTool );
	if (StepB.IsDone()) {
	  aCShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
	  // si OPEN on le force a CLOSED mais que c est une honte !
	  if (aCShell.IsNull()) {
	    Handle(StepShape_OpenShell) aOShell = Handle(StepShape_OpenShell)::DownCast(StepB.Value());
	    if (!aOShell.IsNull()) {
	      aCShell = new StepShape_ClosedShell;
	      aCShell->Init (aOShell->Name(),aOShell->CfsFaces());
#ifdef DEBUG
	      cout<<"Warning: MakeBRepWithVoids: Open shell in the solid; treated as closed"<<endl;
#endif
	    }
	  }
	  if ( ! aCShell.IsNull() ) {
	    if ( aOutShell.IsEqual(It.Value()) ) 
	      aOuter = aCShell;
	    else 
	      S.Append(aCShell);
	  }
#ifdef DEBUG
	  else cout<<"*** MakeBRepWithVoids : Shell not mapped"<<endl;
#endif
	}
#ifdef DEBUG
	else cout << "Shell not mapped" << endl;
#endif
/* //:d7 
	if (It.Value().Closed()) {
	  aTool.Init(aMap, Standard_False);
	  StepB.Init(CurrentShell, aTool, FP);
	  TopoDSToStep::AddResult ( FP, aTool );
	  if (StepB.IsDone()) {
	    aCShell = Handle(StepShape_ClosedShell)::DownCast(StepB.Value());
	    if ( aOutShell.IsEqual(It.Value()) ) 
	      aOuter = aCShell;
	    else 
	      S.Append(aCShell);
	  }
	  else {
#ifdef DEBUG
	    cout << "Shell not mapped" << endl;
#endif
	    done = Standard_False;
	  }
	}
	else {
	  // Error Handling : the Shape is not closed
	  done = Standard_False;
	}
*/	
      }
    }
    Standard_Integer N = S.Length();
    if ( N>=1 ) {
      Handle(TCollection_HAsciiString) aName = 
	new TCollection_HAsciiString("");
      aVoids = new StepShape_HArray1OfOrientedClosedShell(1,N);
      for ( Standard_Integer i=1; i<=N; i++ ) {
	aOCShell = new StepShape_OrientedClosedShell;
	// Warning : the Oriented Shell Orientation is not always
	//           TRUE.
	//           Shall check the TopoDS_Shell orientation.
	// => if the Shell is reversed, shall create an OrientedShell.
	aOCShell -> Init(aName,Handle(StepShape_ClosedShell)::DownCast(S.Value(i)),
			 Standard_False); //:e0
//:e0			 Standard_True);
	aVoids->SetValue(i, aOCShell);
      }
      theBrepWithVoids = new StepShape_BrepWithVoids();
      theBrepWithVoids->Init(aName, aOuter, aVoids);
      done = Standard_True;
    }
    else done = Standard_False;
  }
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepShape_BrepWithVoids) &
      TopoDSToStep_MakeBrepWithVoids::Value() const
{
  StdFail_NotDone_Raise_if(!done,"");
  return theBrepWithVoids;
}
