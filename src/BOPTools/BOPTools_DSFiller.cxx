// File:	BOPTools_PoolsFiller.cxx
// Created:	Tue Feb 20 12:44:28 2001
// Author:	Peter KURNEV
//		<pkv@irinox>
//

#include <BOPTools_DSFiller.ixx>
#include <BOPTools_PaveFiller.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Edge.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BOPTools_Tools3D.hxx>

//
static
	void FillList(const TopoDS_Shape& aS,
		      Standard_Boolean& bCompSolidFound,
		      TopTools_ListOfShape& aResultList);
static
	void BuildSolid(const TopTools_ListOfShape& aRL, TopoDS_Shape& aS);
static
	void BuildShell(const TopTools_ListOfShape& aRL, TopoDS_Shape& aS);
static
	void BuildWire (const TopTools_ListOfShape& aRL, TopoDS_Shape& aS);
static
  TopAbs_ShapeEnum GetTypeCurrent(const TopAbs_ShapeEnum aTypeX);

// static
//        Standard_Integer TreatCompound(const TopoDS_Shape& aS,
// 				      TopoDS_Shape& aShapeResult);

// Peter KURNEV
// p-kurnev@opencascade.com
//=======================================================================
// function: BOPTools_PoolsFiller
// purpose: 
//=======================================================================
  BOPTools_DSFiller::BOPTools_DSFiller() 
{
  myPaveFiller=NULL;
  myInterfPool=NULL;
  myDS=NULL;
  myNewFiller=Standard_True;
  myIsDone=Standard_False;
}
//=======================================================================
// function: Clear
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::Clear()
{
  if (myPaveFiller!=NULL) {
    delete myPaveFiller;
  }
  if (myInterfPool!=NULL) {
    delete myInterfPool;
  }
  if (myDS!=NULL) {
    delete myDS;
  }
  myPaveFiller=NULL;
  myInterfPool=NULL;
  myDS=NULL;
}

//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::Perform()
{
  myIsDone=Standard_False;

  if (myShape1.IsNull() || myShape2.IsNull()) {
    return;
  }
  //
  // Clear contents of the DS if it was before
  Clear();
  //
  myNewFiller=Standard_True;
  //
  myDS        = new BooleanOperations_ShapesDataStructure (myShape1, myShape2);
  myInterfPool= new BOPTools_InterferencePool (*myDS);
  //
  // Build Vertices, Paves, splits, 
  myPaveFiller = new BOPTools_PaveFiller(*myInterfPool);
  myPaveFiller->Perform();
  myIsDone=myPaveFiller->IsDone();
}

//=======================================================================
// function: InitFillersAndPools
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::InitFillersAndPools()
{
  myIsDone=Standard_False;

  if (myShape1.IsNull() || myShape2.IsNull()) {
    return;
  }
  //
  // Clear contents of the DS if it was before
  Clear();
  //
  myNewFiller=Standard_True;
  //
  myDS        = new BooleanOperations_ShapesDataStructure (myShape1, myShape2);
  myInterfPool= new BOPTools_InterferencePool (*myDS);
  //
  myPaveFiller = new BOPTools_PaveFiller(*myInterfPool);
}

//=======================================================================
// function: PartialPerform
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::PartialPerform(const TColStd_SetOfInteger& anObjSubSet,
					 const TColStd_SetOfInteger& aToolSubSet)
{
  myIsDone=Standard_False;

  //
  // Build Vertices, Paves, splits, 
  myPaveFiller->PartialPerform(anObjSubSet, aToolSubSet);
  myIsDone=myPaveFiller->IsDone();

}

//=======================================================================
// function: ToCompletePerform
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::ToCompletePerform()
{
  if(myIsDone) {
    myIsDone=Standard_False;
    //
    myPaveFiller->ToCompletePerform();
    myIsDone=myPaveFiller->IsDone();
    if(myIsDone) {
      // Check if any interference exists
      myIsDone=Standard_False;
    
      Standard_Integer nbs = myDS->NumberOfNewShapes();
      if(nbs > 0) {
	myIsDone=Standard_True;
	return;
      }
      
    }
  }
}

//=======================================================================
// function: Perform
// purpose:
//=======================================================================
  void BOPTools_DSFiller::Perform(const BOPTools_SSIntersectionAttribute& theSectionAttribute) 
{
  myIsDone=Standard_False;
    
  if (myShape1.IsNull() || myShape2.IsNull()) {
    return;
  }
  //
  // Clear contents of the DS if it was before
  Clear();
  //
  myNewFiller=Standard_True;
  //
  myDS        = new BooleanOperations_ShapesDataStructure (myShape1, myShape2);
  myInterfPool= new BOPTools_InterferencePool (*myDS);
  //
  // Build Vertices, Paves, splits, 
  myPaveFiller = new BOPTools_PaveFiller(*myInterfPool, theSectionAttribute);
  myPaveFiller->Perform();
  myIsDone=myPaveFiller->IsDone();
}

//=======================================================================
// function:Destroy
// purpose:
//=======================================================================
  void BOPTools_DSFiller::Destroy() 
{
  Clear();
}

//=======================================================================
// function: SetShapes
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::SetShapes (const TopoDS_Shape& aS1,
				     const TopoDS_Shape& aS2)
{
  myShape1=aS1;
  myShape2=aS2;
  //
  Standard_Integer aNbShapes;
  TopAbs_ShapeEnum aT1, aT2;
  aT1=aS1.ShapeType();
  aT2=aS2.ShapeType();
  //
  if (aT1==TopAbs_COMPOUND || aT2==TopAbs_COMPOUND) {
    Standard_Boolean bIsEmptyShape1, bIsEmptyShape2;
   
    bIsEmptyShape1=BOPTools_Tools3D::IsEmptyShape(myShape1);
    bIsEmptyShape2=BOPTools_Tools3D::IsEmptyShape(myShape2);
    if (bIsEmptyShape1 || bIsEmptyShape2) {
      myIsDone=Standard_True;
      return;
    }
  }
  //
  // 1.1
  TopoDS_Iterator anIt;
  TopoDS_Shape aSTmp;

  if (aT1==TopAbs_COMPOUND || aT1==TopAbs_COMPSOLID) {
    aNbShapes=0;
    anIt.Initialize(aS1);
    for (; anIt.More(); anIt.Next()) {
      if (!aNbShapes) {
	aSTmp=anIt.Value();
      }
      aNbShapes++;
      if (aNbShapes>1) {
	break;
      }
    }
    //
    if (aT1==TopAbs_COMPOUND) {
      if (aNbShapes==1) {
	TreatCompound(aS1, aSTmp);
	myShape1=aSTmp;
	aT1=myShape1.ShapeType();
      }
      else if (aNbShapes>1) {
	TreatCompound(aS1, aSTmp);
	myShape1=aSTmp;
	aT1=myShape1.ShapeType();
      }
    }
  }
  //

  if (aT1==TopAbs_COMPOUND || aT1==TopAbs_COMPSOLID) {
    myIsDone=Standard_False; 
    return;
  }
  //
  // 1.2 aS2
  if (aT2==TopAbs_COMPOUND || aT2==TopAbs_COMPSOLID) {
    aNbShapes=0;
    anIt.Initialize(aS2);
    for (; anIt.More(); anIt.Next()) {
      if (!aNbShapes) {
	aSTmp=anIt.Value();
      }
      aNbShapes++;
      if (aNbShapes>1) {
	break;
      }
    }
    //
    if (aT2==TopAbs_COMPOUND) {
      if (aNbShapes==1) {
	TreatCompound(aS2, aSTmp);
	myShape2=aSTmp;
	aT2=myShape2.ShapeType();
      }
      else if (aNbShapes>1) {
	TreatCompound(aS2, aSTmp);
	myShape2=aSTmp;
	aT2=myShape2.ShapeType();
      }
    }
  }
  //
  
  if (aT2==TopAbs_COMPOUND || aT2==TopAbs_COMPSOLID) {
    myIsDone=Standard_False; 
    return;
  }
  //
  // 2.1
  BRep_Builder BB;
  TopoDS_Shell aSh1, aSh2;
  TopoDS_Wire  aW1, aW2;
  
  if (aT1==TopAbs_FACE) {
    if (aT2==TopAbs_SOLID || 
	aT2==TopAbs_SHELL || 
	aT2==TopAbs_FACE  ||
	aT2==TopAbs_WIRE  ||
	aT2==TopAbs_EDGE) {
      BB.MakeShell(aSh1);
      BB.Add(aSh1, myShape1);
      myShape1=aSh1;
      aT1=TopAbs_SHELL;
    }
  }
  //
  // 2.2
  if (aT2==TopAbs_FACE) {
    if (aT1==TopAbs_SOLID ||
	aT1==TopAbs_SHELL ||
	aT1==TopAbs_WIRE  ||
	aT1==TopAbs_EDGE) {
      BB.MakeShell(aSh2);
      BB.Add(aSh2, myShape2);
      myShape2=aSh2;
      aT2=TopAbs_SHELL;
    }
  }
  // 3.1
  if (aT1==TopAbs_EDGE) {
    if (aT2==TopAbs_SOLID || 
	aT2==TopAbs_SHELL ||
	aT2==TopAbs_WIRE  ||
	aT2==TopAbs_EDGE) {
      BB.MakeWire (aW1);
      BB.Add(aW1, myShape1);
      myShape1=aW1;
      aT1=TopAbs_WIRE;
    }
  }
  // 3.2
  if (aT2==TopAbs_EDGE) {
    if (aT1==TopAbs_SOLID || 
	aT1==TopAbs_SHELL ||
	aT1==TopAbs_WIRE) {
      BB.MakeWire (aW2);
      BB.Add(aW2, myShape2);
      myShape2=aW2;
      aT2=TopAbs_WIRE;
    }
  }

  myIsDone=Standard_True; 
}

//=======================================================================
// function: Shape1
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOPTools_DSFiller::Shape1 () const 
{
  return myShape1;
}

//=======================================================================
// function: Shape2
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOPTools_DSFiller::Shape2 () const 
{
  return myShape2;
}
//=======================================================================
// function: DS
// purpose: 
//=======================================================================
  const BooleanOperations_ShapesDataStructure& BOPTools_DSFiller::DS () const 
{
  return *myDS;
}
//=======================================================================
// function: InterfPool
// purpose: 
//=======================================================================
  const BOPTools_InterferencePool& BOPTools_DSFiller::InterfPool () const
{
  return *myInterfPool;
}
//=======================================================================
// function: PavePool
// purpose: 
//=======================================================================
  const BOPTools_PavePool& BOPTools_DSFiller::PavePool () const 
{
  return myPaveFiller->PavePool();
}

//=======================================================================
// function: CommonBlockPool
// purpose: 
//=======================================================================
  const BOPTools_CommonBlockPool& BOPTools_DSFiller::CommonBlockPool () const 
{
  return myPaveFiller->CommonBlockPool();
}

//=======================================================================
// function:  SplitShapesPool
// purpose: 
//=======================================================================
  const BOPTools_SplitShapesPool& BOPTools_DSFiller::SplitShapesPool() const
{
  return myPaveFiller->SplitShapesPool();
}

//=======================================================================
// function:  PaveFiller
// purpose: 
//=======================================================================
  const BOPTools_PaveFiller& BOPTools_DSFiller::PaveFiller() const
{
  return *myPaveFiller;
}

//=======================================================================
// function: IsNewFiller 
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_DSFiller::IsNewFiller() const
{
  return myNewFiller;
}


//=======================================================================
// function:  SetNewFiller
// purpose: 
//=======================================================================
  void BOPTools_DSFiller::SetNewFiller(const Standard_Boolean aFlag) const
{
  BOPTools_DSFiller* pDSFiller=(BOPTools_DSFiller*)this;
  pDSFiller->myNewFiller=aFlag;
}
//=======================================================================
// function:  IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_DSFiller::IsDone()const 
{
  return myIsDone;
}
//
//=======================================================================
//function :TreatCompound
//purpose  : 
//=======================================================================
// Standard_Integer TreatCompound(const TopoDS_Shape& aS,
// 			       TopoDS_Shape& aShapeResult)
Standard_Integer BOPTools_DSFiller::TreatCompound(const TopoDS_Shape& aS,
						  TopoDS_Shape&       aShapeResult) 
{
  //
  TopAbs_ShapeEnum aType, aTypeX;
  //
  aType=aS.ShapeType();
  //
  if (aType==TopAbs_COMPSOLID) {
    aShapeResult=aS;
    return 1;
  }
  //
  if (aType!=TopAbs_COMPOUND) {
    // Nothing to do 
    aShapeResult=aS;
    return 2;
  }
  // 
  // 1 Fill the list by real shapes (solids, shells, wires, edges)
  Standard_Boolean bCompSolidFound=Standard_False;
  TopTools_ListOfShape aRL1, aRL;
  FillList (aS, bCompSolidFound, aRL1);
  if (bCompSolidFound) {
    aShapeResult=aS;
    return 3;
  }
  // 2
  // Analyse the list	and find the type
  Standard_Integer i=0;
  TopExp_Explorer anExp;
  TopAbs_ShapeEnum aTypeCurrent=TopAbs_SHAPE,
                   aType0=TopAbs_SHAPE;
  
  //aTypeCurrent=TopAbs_SHAPE;
  TopTools_ListIteratorOfListOfShape anItL(aRL1);
  for (; anItL.More(); anItL.Next(), i++) {
    const TopoDS_Shape& aSX=anItL.Value();
    aTypeX=aSX.ShapeType();
    
    aTypeCurrent=GetTypeCurrent(aTypeX);
    
    if (i==0) {
      aType0=aTypeCurrent;
      if (aType0==TopAbs_SHAPE) {
	// Unknown shape type;
	aShapeResult=aS;
	return 4;
      }
    }

    else {
      if (aTypeCurrent!=aType0) {
	// Heterogenous types occured;
	aShapeResult=aS;
	return 5;
      }
    }
    // 
    // Fill the RL
    if (aTypeCurrent==TopAbs_SOLID) {
      anExp.Init(aSX, TopAbs_SHELL);
      for (; anExp.More(); anExp.Next()) {
	const TopoDS_Shape& aSY=anExp.Current();
	aRL.Append(aSY);
      }
    }
    //
    else if (aTypeCurrent==TopAbs_SHELL) {
      if (aTypeX==TopAbs_FACE) {
	aRL.Append(aSX);
      }
      else {
	anExp.Init(aSX, TopAbs_FACE);
	for (; anExp.More(); anExp.Next()) {
	  const TopoDS_Shape& aSY=anExp.Current();
	  aRL.Append(aSY);
	}
      }
    }
    //
    else if (aTypeCurrent==TopAbs_WIRE) {
      if (aTypeX==TopAbs_EDGE) {
	aRL.Append(aSX);
      }
      else {
	anExp.Init(aSX, TopAbs_EDGE);
	for (; anExp.More(); anExp.Next()) {
	  const TopoDS_Shape& aSY=anExp.Current();
	  aRL.Append(aSY);
	}
      }
    }
  }
  //
  // Make Composite shape
  //BRep_Builder aBB;
  
  if (aType0==TopAbs_SOLID) {
    BuildSolid(aRL, aShapeResult);
  }
  if (aType0==TopAbs_SHELL) {
    BuildShell(aRL, aShapeResult);
  }
  if (aType0==TopAbs_WIRE) {
    BuildWire(aRL, aShapeResult);
  }
  //OK
  return 0;
}
//=======================================================================
//function : BuildSolid
//purpose  : 
//=======================================================================
void BuildSolid(const TopTools_ListOfShape& aRL,
		TopoDS_Shape& aS)
{
  BRep_Builder aBB;
  TopoDS_Solid aSolid;
  aBB.MakeSolid(aSolid);	
  TopTools_ListIteratorOfListOfShape anItL;
  anItL.Initialize(aRL);
  for (; anItL.More(); anItL.Next()) {
    const TopoDS_Shape& aSX=anItL.Value();
    aBB.Add(aSolid, TopoDS::Shell(aSX));
  }
  aS=aSolid;
}
//=======================================================================
//function : BuildShell
//purpose  : 
//=======================================================================
void BuildShell(const TopTools_ListOfShape& aRL,
		TopoDS_Shape& aS)
{
  BRep_Builder aBB;
  TopoDS_Shell aShell;
  aBB.MakeShell(aShell);	
  TopTools_ListIteratorOfListOfShape anItL;
  anItL.Initialize(aRL);
  for (; anItL.More(); anItL.Next()) {
    const TopoDS_Shape& aSX=anItL.Value();
    aBB.Add(aShell, aSX);
  }
  aS=aShell;
}
//=======================================================================
//function : BuildWire
//purpose  : 
//=======================================================================
void BuildWire(const TopTools_ListOfShape& aRL,
	       TopoDS_Shape& aS)
{
  BRep_Builder aBB;
  TopoDS_Wire aWire;
  aBB.MakeWire(aWire);	
  TopTools_ListIteratorOfListOfShape anItL;
  anItL.Initialize(aRL);
  for (; anItL.More(); anItL.Next()) {
    const TopoDS_Shape& aSX=anItL.Value();
    aBB.Add(aWire, aSX);
  }
  aS=aWire;
}
//=======================================================================
//function :FillList
//purpose  : 
//=======================================================================
void FillList(const TopoDS_Shape& aS,
	      Standard_Boolean& bCompSolidFound,
	      TopTools_ListOfShape& aResultList)
{
  if (bCompSolidFound) {
    return;
  }
  TopAbs_ShapeEnum aTypeX;
  TopoDS_Iterator anIt;
  anIt.Initialize(aS);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSX=anIt.Value();
    aTypeX=aSX.ShapeType();
    //
    if (aTypeX==TopAbs_COMPSOLID) {
      bCompSolidFound=Standard_True;
      return;
    }
    //
    if (aTypeX==TopAbs_COMPOUND) {
      FillList(aSX, bCompSolidFound, aResultList);
      if (bCompSolidFound) {
	return;
      }
    }
    if (aTypeX!=TopAbs_COMPOUND) {
      aResultList.Append(aSX);
    }
    
  }	
}
//=======================================================================
//function :GetTypeCurrent
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum  GetTypeCurrent(const TopAbs_ShapeEnum aTypeX)
{
  TopAbs_ShapeEnum aTypeCurrent=TopAbs_SHAPE;

  if (aTypeX==TopAbs_SOLID) {
    aTypeCurrent=TopAbs_SOLID;
  }
  
  if (aTypeX==TopAbs_SHELL || aTypeX==TopAbs_FACE ) {
    aTypeCurrent=TopAbs_SHELL;
  }

  if (aTypeX==TopAbs_WIRE || aTypeX==TopAbs_EDGE ) {
    aTypeCurrent=TopAbs_WIRE;
  }
  return aTypeCurrent;
}
//
