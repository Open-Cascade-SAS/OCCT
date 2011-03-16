// File:	BOP_EmptyBuilder.cxx
// Created:	Fri Feb  1 12:58:48 2002
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <BOP_EmptyBuilder.ixx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <TopoDS_Compound.hxx>

#include <BRep_Builder.hxx>

#include <BOPTColStd_Dump.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_Tools3D.hxx>




//=======================================================================
// function: BOP_EmptyBuilder::BOP_EmptyBuilder
// purpose: 
//=======================================================================
   BOP_EmptyBuilder::BOP_EmptyBuilder()
{
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void BOP_EmptyBuilder::Destroy() 
{
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_EmptyBuilder::Do() 
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
  void BOP_EmptyBuilder::DoWithFiller(const BOPTools_DSFiller& aDSFiller) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  myResultMap.Clear();
  myModifiedMap.Clear();
  //
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
  //
  // 
  try {
    OCC_CATCH_SIGNALS
    Standard_Boolean bIsNewFiller;
    bIsNewFiller=aDSFiller.IsNewFiller();
    
    if (bIsNewFiller) {
      aDSFiller.SetNewFiller(!bIsNewFiller);
    }
    //
    BuildResult();
    //
    //
    //FillModified();
    myIsDone=Standard_True;
  }

  catch ( Standard_Failure ) {
    myErrorStatus = 1;
    BOPTColStd_Dump::PrintMessage("Can not build result\n");
  }
  //
}

//=======================================================================
// function: BuildResult
// purpose: 
//=======================================================================
  void BOP_EmptyBuilder::BuildResult()
{
  Standard_Boolean bIsEmptyShape1, bIsEmptyShape2;
  BRep_Builder aBB;
  TopoDS_Compound aCompound;

  aBB.MakeCompound(aCompound);
  //
  bIsEmptyShape1=BOPTools_Tools3D::IsEmptyShape(myShape1);
  bIsEmptyShape2=BOPTools_Tools3D::IsEmptyShape(myShape2);
  //
  if (!bIsEmptyShape1 && bIsEmptyShape2) {
    if (myOperation==BOP_FUSE || myOperation==BOP_CUT) {
      aBB.Add(aCompound, myShape1);
    }
  }
  //
  else if (bIsEmptyShape1 && !bIsEmptyShape2) {
    if (myOperation==BOP_FUSE || myOperation==BOP_CUT21) {
      aBB.Add(aCompound, myShape2);
    }
  }
  //
  myResult=aCompound;
}

