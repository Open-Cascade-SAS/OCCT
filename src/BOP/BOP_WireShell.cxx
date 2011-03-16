// File:	BOP_WireShell.cxx
// Created:	Mon Feb  4 11:01:01 2002
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOP_WireShell.ixx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <BOPTColStd_Dump.hxx>

#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_WireStateFiller.hxx>

#include <BOP_CorrectTolerances.hxx>

//=======================================================================
// function: BOP_WireShell::BOP_WireShell
// purpose: 
//=======================================================================
  BOP_WireShell::BOP_WireShell()
{
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void BOP_WireShell::Destroy() 
{
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_WireShell::Do() 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  // Filling the DS
  BOPTools_DSFiller aDSFiller;
  aDSFiller.SetShapes (myShape1, myShape2);
  //
  aDSFiller.Perform ();
  //
  DoWithFiller(aDSFiller);
}

//=======================================================================
// function: DoWithFiller
// purpose: 
//=======================================================================
  void BOP_WireShell::DoWithFiller(const BOPTools_DSFiller& aDSFiller) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  myResultMap.Clear();
  myModifiedMap.Clear();
  //
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
  //
  try {
    OCC_CATCH_SIGNALS

    if(!myDSFiller->IsDone()) {
      myErrorStatus = 1;
      BOPTColStd_Dump::PrintMessage("DSFiller is invalid: Can not build result\n");
      return;
    }

    Standard_Boolean bCheckTypes;
    //
    bCheckTypes=CheckArgTypes();
    if (!bCheckTypes) {
      myErrorStatus=10;
      return;
    }
    //
    Standard_Boolean bIsNewFiller;
    bIsNewFiller=aDSFiller.IsNewFiller();
    
    if (bIsNewFiller) {
      //
      // Preparing the States
      const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
      BOPTools_WireStateFiller aStateFiller(aPaveFiller);
      aStateFiller.Do();
      
      aDSFiller.SetNewFiller(!bIsNewFiller);
    }
    //
    BuildResult();
    //
    BOP_CorrectTolerances::CorrectTolerances(myResult, 0.01);
    //
    FillModified();
    myIsDone=Standard_True;
  }
  catch ( Standard_Failure ) {
    myErrorStatus = 1;
    BOPTColStd_Dump::PrintMessage("Can not build result\n");
  }
}

//=======================================================================
// function: BuildResult
// purpose: 
//=======================================================================
  void BOP_WireShell::BuildResult()
{
  AddSplitPartsINOUT();
  AddSplitPartsON();
  //
  MakeResult();
}

//=======================================================================
// function: CheckArgTypes
// purpose: 
//=======================================================================
Standard_Boolean BOP_WireShell::CheckArgTypes(const TopAbs_ShapeEnum theType1,
					      const TopAbs_ShapeEnum theType2,
					      const BOP_Operation theOperation) 
{
  Standard_Boolean bFlag=Standard_False;
  
  //
  if (theType1==TopAbs_WIRE && theType2==TopAbs_SHELL) {
    if (theOperation==BOP_FUSE || theOperation==BOP_CUT21) {
      return bFlag;
    }
  }
  //
  if (theType1==TopAbs_SHELL && theType2==TopAbs_WIRE) {
    if (theOperation==BOP_FUSE || theOperation==BOP_CUT) {
      return bFlag;
    }
  }
  //
  return !bFlag;
}


//=======================================================================
// function: CheckArgTypes
// purpose: 
//=======================================================================
  Standard_Boolean BOP_WireShell::CheckArgTypes() const
{
//   Standard_Boolean bFlag=Standard_False;
  
  TopAbs_ShapeEnum aT1, aT2;
  const BooleanOperations_ShapesDataStructure& aDS=myDSFiller->DS();

  aT1=aDS.Object().ShapeType();
  aT2=aDS.Tool().ShapeType();
  //
//   if (aT1==TopAbs_WIRE && aT2==TopAbs_SHELL) {
//     if (myOperation==BOP_FUSE || myOperation==BOP_CUT21) {
//       return bFlag;
//     }
//   }
//   //
//   if (aT1==TopAbs_SHELL && aT2==TopAbs_WIRE) {
//     if (myOperation==BOP_FUSE || myOperation==BOP_CUT) {
//       return bFlag;
//     }
//   }
//   //
//   return !bFlag;
  return CheckArgTypes(aT1, aT2, myOperation);
}
