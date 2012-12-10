// Created on: 1995-12-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepCheck_Analyzer.ixx>

#include <BRepCheck_Vertex.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepCheck_Face.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepCheck_Analyzer::Init(const TopoDS_Shape& S,
			      const Standard_Boolean B)
{
  if (S.IsNull()) {
    Standard_NullObject::Raise();
  }
  myShape = S;
  myMap.Clear();
  Put(S,B);
  Perform(S);
}



//=======================================================================
//function : Put
//purpose  : 
//=======================================================================

void BRepCheck_Analyzer::Put(const TopoDS_Shape& S,
			     const Standard_Boolean B)
{
  if (!myMap.IsBound(S)) {
    Handle(BRepCheck_Result) HR;
    switch (S.ShapeType()) {
    case TopAbs_VERTEX:
      HR = new BRepCheck_Vertex(TopoDS::Vertex(S));
      break;
    case TopAbs_EDGE:
      HR = new BRepCheck_Edge(TopoDS::Edge(S));
      Handle(BRepCheck_Edge)::DownCast(HR)->GeometricControls(B);
      break;
    case TopAbs_WIRE:
      HR = new BRepCheck_Wire(TopoDS::Wire(S));
      Handle(BRepCheck_Wire)::DownCast(HR)->GeometricControls(B);
      break;
    case TopAbs_FACE:
      HR = new BRepCheck_Face(TopoDS::Face(S));
      Handle(BRepCheck_Face)::DownCast(HR)->GeometricControls(B);
      break;
    case TopAbs_SHELL:
      HR = new BRepCheck_Shell(TopoDS::Shell(S));
      break;
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID:
    case TopAbs_COMPOUND:
      break;
    default:
      break;
    }
    myMap.Bind(S,HR);
    for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) {
      Put(theIterator.Value(),B); // performs minimum on each shape
    }
  }
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepCheck_Analyzer::Perform(const TopoDS_Shape& S)
{
  for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) {
    Perform(theIterator.Value());
  }
  //
  TopAbs_ShapeEnum styp;
  TopExp_Explorer exp;
  //
  styp = S.ShapeType();
  switch (styp) {
  case TopAbs_VERTEX: 
    // modified by NIZHNY-MKK  Wed May 19 16:56:16 2004.BEGIN
    // There is no need to check anything.
    //       if (myShape.IsSame(S)) {
    // 	myMap(S)->Blind();
    //       }
    // modified by NIZHNY-MKK  Wed May 19 16:56:23 2004.END
  
    break;
  case TopAbs_EDGE:    {
    //  Modified by skv - Tue Apr 27 11:38:08 2004 Begin
    // There is no need to check anything except vertices on single edge.
    //       if (myShape.IsSame(S)) {
    // 	myMap(S)->Blind();
    //       }
    //  Modified by skv - Tue Apr 27 11:38:09 2004 End
    TopTools_MapOfShape MapS;
    
    for (exp.Init(S,TopAbs_VERTEX);exp.More(); exp.Next()) {
      const TopoDS_Shape& aVertex = exp.Current();
      try {
	OCC_CATCH_SIGNALS
	if (MapS.Add(aVertex)) {
	  myMap(aVertex)->InContext(S);
	}
      }
      catch(Standard_Failure) {
#ifdef DEB
	cout<<"BRepCheck_Analyzer : ";
	Standard_Failure::Caught()->Print(cout);  
	cout<<endl;
#endif
	if ( ! myMap(S).IsNull() ) {
	  myMap(S)->SetFailStatus(S);
	}
	Handle(BRepCheck_Result) aRes = myMap(aVertex);
	if ( ! aRes.IsNull() ) {
	  aRes->SetFailStatus(aVertex);
	  aRes->SetFailStatus(S);
	}
      }
    }
    }
    break;
  case TopAbs_WIRE:
    //  Modified by Sergey KHROMOV - Mon May  6 15:53:39 2002 Begin
    // There is no need to check the orientation of a single wire 
    // (not in context of face).
    //     {
    //       if (myShape.IsSame(S)) {
    // 	Handle(BRepCheck_Wire)::DownCast(myMap(S))->Orientation(TopoDS_Face(),
    // 								Standard_True);
    //       }
    //     }
    //  Modified by Sergey KHROMOV - Mon May  6 15:53:40 2002 End
    break;
  case TopAbs_FACE:
    {
      TopTools_MapOfShape MapS;
      for (exp.Init(S,TopAbs_VERTEX);exp.More(); exp.Next()) {
	try {
	  OCC_CATCH_SIGNALS
	  if (MapS.Add(exp.Current())) {
	    myMap(exp.Current())->InContext(S);
	  }
	}
	catch(Standard_Failure) {
#ifdef DEB
	  cout<<"BRepCheck_Analyzer : ";
	  Standard_Failure::Caught()->Print(cout);  
	  cout<<endl;
#endif
	  if ( ! myMap(S).IsNull() ) {
	    myMap(S)->SetFailStatus(S);
	  }
	  Handle(BRepCheck_Result) aRes = myMap(exp.Current());

	  if ( ! aRes.IsNull() ) {
	    aRes->SetFailStatus(exp.Current());
	    aRes->SetFailStatus(S);
	  }
	}
      }
      Standard_Boolean performwire = Standard_True;
      MapS.Clear();
      for (exp.Init(S,TopAbs_EDGE);exp.More(); exp.Next()) {
	try {
	  OCC_CATCH_SIGNALS
	  if (MapS.Add(exp.Current())) {
	    Handle(BRepCheck_Result)& res = myMap(exp.Current());
	    res->InContext(S);
	    if (performwire) {
	      for (res->InitContextIterator();
		   res->MoreShapeInContext();
		   res->NextShapeInContext()) {
		if(res->ContextualShape().IsSame(S)) {
		  break;
		}
	      }
	      BRepCheck_ListIteratorOfListOfStatus itl(res->StatusOnShape());
	      for (; itl.More(); itl.Next()) {
		BRepCheck_Status ste = itl.Value();
		if (ste == BRepCheck_NoCurveOnSurface  ||
		    ste == BRepCheck_InvalidCurveOnSurface ||
		    ste == BRepCheck_InvalidRange ||
		    ste == BRepCheck_InvalidCurveOnClosedSurface) {
		  performwire = Standard_False;
		  break;
		}
	      }
	    }
	  }
	}
	catch(Standard_Failure) {
#ifdef DEB
	  cout<<"BRepCheck_Analyzer : ";
	  Standard_Failure::Caught()->Print(cout);  
	  cout<<endl;
#endif

	  if ( ! myMap(S).IsNull() ) {
	    myMap(S)->SetFailStatus(S);
	  }
	  Handle(BRepCheck_Result) aRes = myMap(exp.Current());

	  if ( ! aRes.IsNull() ) {
	    aRes->SetFailStatus(exp.Current());
	    aRes->SetFailStatus(S);
	  }
	}
      }
      Standard_Boolean orientofwires = performwire;
      for (exp.Init(S,TopAbs_WIRE);exp.More(); exp.Next()) {
	try {
	  OCC_CATCH_SIGNALS
	  Handle(BRepCheck_Result)& res = myMap(exp.Current());
	  res->InContext(S);
	  if (orientofwires) {
	    for (res->InitContextIterator();
		 res->MoreShapeInContext();
		 res->NextShapeInContext()) {
	      if(res->ContextualShape().IsSame(S)) {
		break;
	      }
	    }
	    BRepCheck_ListIteratorOfListOfStatus itl(res->StatusOnShape());
	    for (; itl.More(); itl.Next()) {
	      BRepCheck_Status ste = itl.Value();
	      if (ste != BRepCheck_NoError) {
		orientofwires = Standard_False;
		break;
	      }
	    }
	  }	  
	}
	catch(Standard_Failure) {
#ifdef DEB
	  cout<<"BRepCheck_Analyzer : ";
	  Standard_Failure::Caught()->Print(cout);  
	  cout<<endl;
#endif
	  if ( ! myMap(S).IsNull() ) {
	    myMap(S)->SetFailStatus(S);
	  }
	  Handle(BRepCheck_Result) aRes = myMap(exp.Current());

	  if ( ! aRes.IsNull() ) {
	    aRes->SetFailStatus(exp.Current());
	    aRes->SetFailStatus(S);
	  }
	}
      }
      
      try {
        OCC_CATCH_SIGNALS
	if (performwire) {
	  if (orientofwires) {
	    Handle(BRepCheck_Face)::DownCast(myMap(S))->
	      OrientationOfWires(Standard_True);// on enregistre
	  }
	  //	else {
	  //	   Handle(BRepCheck_Face)::DownCast(myMap(S))->
	  //	      IntersectWires(Standard_True); // on enregistre
	  //      }
	  else {
	    Handle(BRepCheck_Face)::DownCast(myMap(S))->SetUnorientable();
	  }
	}
	else {
	  Handle(BRepCheck_Face)::DownCast(myMap(S))->SetUnorientable();
	}
      }
      catch(Standard_Failure) {
#ifdef DEB
	  cout<<"BRepCheck_Analyzer : ";
	  Standard_Failure::Caught()->Print(cout);  
	  cout<<endl;
#endif

	if ( ! myMap(S).IsNull() ) {
	  myMap(S)->SetFailStatus(S);
	}
	
	for (exp.Init(S,TopAbs_WIRE);exp.More(); exp.Next()) {
	  Handle(BRepCheck_Result) aRes = myMap(exp.Current());

	  if ( ! aRes.IsNull() ) {
	    aRes->SetFailStatus(exp.Current());
	    aRes->SetFailStatus(S);
	    myMap(S)->SetFailStatus(exp.Current());
	  }
	}
      }
    }
    break;
    
  case TopAbs_SHELL:   
    //modified by NIZNHY-PKV Mon Oct 13 14:23:53 2008f
    /* {
    Standard_Boolean VerifyOrientation, bFlag;
    //
    VerifyOrientation = Standard_True;
    //
    exp.Init(S,TopAbs_FACE);
    for (; exp.More(); exp.Next()) {
      const TopoDS_Shape& aF=exp.Current();
      try {
	OCC_CATCH_SIGNALS
	bFlag=  !(Handle(BRepCheck_Face)::DownCast(myMap(aF))->IsUnorientable());
	VerifyOrientation = (VerifyOrientation && bFlag);
      }
      catch(Standard_Failure) {
#ifdef DEB
	cout<<"BRepCheck_Analyzer : ";
	Standard_Failure::Caught()->Print(cout);  
	cout<<endl;
#endif
	
	if ( ! myMap(S).IsNull() ) {
	  myMap(S)->SetFailStatus(S);
	}
	Handle(BRepCheck_Result) aRes = myMap(exp.Current());
	
	if ( ! aRes.IsNull() ) {
	  aRes->SetFailStatus(exp.Current());
	  aRes->SetFailStatus(S);
	}
      }
    } // 
    try {
      OCC_CATCH_SIGNALS
      if (VerifyOrientation) {
	Handle(BRepCheck_Shell)::DownCast(myMap(S))->Orientation(Standard_True);
      }
      else {
	Handle(BRepCheck_Shell)::DownCast(myMap(S))->SetUnorientable();
      }
    }
    catch(Standard_Failure) {
#ifdef DEB
      cout<<"BRepCheck_Analyzer : ";
      Standard_Failure::Caught()->Print(cout);  
      cout<<endl;
#endif
      
      if ( ! myMap(S).IsNull() ) {
	myMap(S)->SetFailStatus(S);
      }
      exp.Init(S,TopAbs_FACE);
      for (; exp.More(); exp.Next()) {
	Handle(BRepCheck_Result) aRes = myMap(exp.Current());
	if ( ! aRes.IsNull() ) {
	  aRes->SetFailStatus(exp.Current());
	  aRes->SetFailStatus(S);
	  myMap(S)->SetFailStatus(exp.Current());
	}
      }
    }
  }
  */
    //modified by NIZNHY-PKV Mon Oct 13 14:24:04 2008t
    break;
    //
  case TopAbs_SOLID:    {
    exp.Init(S,TopAbs_SHELL);
    for (; exp.More(); exp.Next()) {
      const TopoDS_Shape& aShell=exp.Current();
      try {
	OCC_CATCH_SIGNALS
	myMap(aShell)->InContext(S);
      }
      catch(Standard_Failure) {
#ifdef DEB
	cout<<"BRepCheck_Analyzer : ";
	Standard_Failure::Caught()->Print(cout);  
	cout<<endl;
#endif
	if ( ! myMap(S).IsNull() ) {
	  myMap(S)->SetFailStatus(S);
	}
	//
	Handle(BRepCheck_Result) aRes = myMap(aShell);
	if (!aRes.IsNull() ) {
	  aRes->SetFailStatus(exp.Current());
	  aRes->SetFailStatus(S);
	}
      }
    }
  }
  break;//case TopAbs_SOLID
  default:
    break;
  }//switch (styp) {
}


//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Analyzer::IsValid(const TopoDS_Shape& S) const
{
  if (!myMap(S).IsNull()) {
    BRepCheck_ListIteratorOfListOfStatus itl;
    itl.Initialize(myMap(S)->Status());
    if (itl.Value() != BRepCheck_NoError) { // a voir
      return Standard_False;
    }
  }

  for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) {
    if (!IsValid(theIterator.Value())) {
      return Standard_False;
    }
  }

  switch (S.ShapeType()) {
  case TopAbs_EDGE:
    {
      return ValidSub(S,TopAbs_VERTEX);
    }
//    break;
  case TopAbs_FACE:
    {
      Standard_Boolean valid = ValidSub(S,TopAbs_WIRE);
      valid = valid && ValidSub(S,TopAbs_EDGE);
      valid = valid && ValidSub(S,TopAbs_VERTEX);
      return valid;
    }

//    break;
  case TopAbs_SHELL:
//    return ValidSub(S,TopAbs_FACE);
    break;
  case TopAbs_SOLID:
//    return ValidSub(S,TopAbs_EDGE);
//    break;
    return ValidSub(S,TopAbs_SHELL);
    break;
  default:
    break;
  }

  return Standard_True;
}

//=======================================================================
//function : ValidSub
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Analyzer::ValidSub
   (const TopoDS_Shape& S,
    const TopAbs_ShapeEnum SubType) const
{
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopExp_Explorer exp;
  for (exp.Init(S,SubType);exp.More(); exp.Next()) {
//  for (TopExp_Explorer exp(S,SubType);exp.More(); exp.Next()) {
    const Handle(BRepCheck_Result)& RV = myMap(exp.Current());
    for (RV->InitContextIterator();
	 RV->MoreShapeInContext(); 
	 RV->NextShapeInContext()) {
      if (RV->ContextualShape().IsSame(S)) {
	break;
      }
    }

    if(!RV->MoreShapeInContext()) break;

    for (itl.Initialize(RV->StatusOnShape()); itl.More(); itl.Next()) {
      if (itl.Value() != BRepCheck_NoError) {
	return Standard_False;
      }
    }
  }
  return Standard_True ;
}
