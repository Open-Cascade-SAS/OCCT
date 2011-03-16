// File:	BOP_CompositeClassifier.cxx
// Created:	Fri Jan  5 15:26:38 1996
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <BOP_CompositeClassifier.ixx>

//=======================================================================
//function : BOP_CompositeClassifier::BOP_CompositeClassifier
//purpose  : 
//=======================================================================
  BOP_CompositeClassifier::BOP_CompositeClassifier (const BOP_BlockBuilder& BB)
 :
  myBlockBuilder((void*)&BB)
{}
//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================
  TopAbs_State BOP_CompositeClassifier::Compare(const Handle(BOP_Loop)& L1,
						const Handle(BOP_Loop)& L2)
{ 
  TopAbs_State state = TopAbs_UNKNOWN;

  Standard_Boolean isshape1, isshape2, yena1 ;

  isshape1 = L1->IsShape();
  isshape2 = L2->IsShape();

  if      ( isshape2 && isshape1 )  { 
    // L1 is Shape , L2 is Shape
    const TopoDS_Shape& s1 = L1->Shape();
    const TopoDS_Shape& s2 = L2->Shape();
    state = CompareShapes(s1,s2);
  }

  else if ( isshape2 && !isshape1 ) { 
    // L1 is Block , L2 is Shape
    BOP_BlockIterator Bit1 = L1->BlockIterator();
    Bit1.Initialize();
    yena1 = Bit1.More();
    while (yena1) {
      const TopoDS_Shape& s1 = 
	((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit1);
      const TopoDS_Shape& s2 = L2->Shape();
      
      state = CompareElementToShape(s1,s2);
      yena1 = Standard_False;
      if (state == TopAbs_UNKNOWN) { 
	if (Bit1.More()) Bit1.Next();
	yena1 = Bit1.More();
      }
    }
  }
  else if ( !isshape2 && isshape1 ) { 
    // L1 is Shape , L2 is Block
    const TopoDS_Shape& s1 = L1->Shape();
    
    ResetShape(s1);
    BOP_BlockIterator Bit2 = L2->BlockIterator();
    for (Bit2.Initialize(); Bit2.More(); Bit2.Next()) {
      const TopoDS_Shape& s2 = 
	((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit2);
      CompareElement(s2);
    }
    state = State();
  }
  
  else if ( !isshape2 && !isshape1 ) {
    // L1 is Block , L2 is Block
    BOP_BlockIterator Bit1 = L1->BlockIterator();
    Bit1.Initialize();
    yena1 = Bit1.More();
    while (yena1) {
      const TopoDS_Shape& s1 = 
	((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit1);
      
      ResetElement(s1);
      BOP_BlockIterator Bit2 = L2->BlockIterator();
      for (Bit2.Initialize(); Bit2.More(); Bit2.Next()) {
	const TopoDS_Shape& s2 = 
	  ((BOP_BlockBuilder*)myBlockBuilder)->Element(Bit2);
	CompareElement(s2);
      }
    
      state = State();
      yena1 = Standard_False;
      if (state == TopAbs_UNKNOWN) { 
	if (Bit1.More()) Bit1.Next();
	yena1 = Bit1.More();
      }
    }
  }
  return state;
}
