// Created on: 1993-05-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_FaceEdgeIntersector.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_ShapeIntersector.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopOpeBRepTool_HBoxTool.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRep_GettraceSI(); 
extern Standard_Boolean TopOpeBRep_GetcontextFFOR();
extern Standard_Integer SAVFFi1; // FacesIntersector
extern Standard_Integer SAVFFi2; // FacesIntersector
extern void TopOpeBRep_SettraceEEFF(const Standard_Boolean b);
extern Standard_Boolean TopOpeBRep_GettraceEEFF(const Standard_Integer e1,const Standard_Integer e2,const Standard_Integer f1,const Standard_Integer f2);
void seteeff(const Standard_Boolean b,const Standard_Integer e1,const Standard_Integer e2, const Standard_Integer f1,const Standard_Integer f2)
{cout<<"b,e1,e2,f1,f2 : "<<b<<" "<<e1<<","<<e2<<","<<f1<<","<<f2<<endl;TopOpeBRep_SettraceEEFF(b);}
void seteefft(const Standard_Integer e1,const Standard_Integer e2, const Standard_Integer f1,const Standard_Integer f2) {seteeff(Standard_True,e1,e2,f1,f2);}
void seteefff(const Standard_Integer e1,const Standard_Integer e2, const Standard_Integer f1,const Standard_Integer f2) {seteeff(Standard_False,e1,e2,f1,f2);}
#endif

// modified by NIZHNY-OFV  Thu Apr 18 17:15:38 2002 (S)
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
static Standard_Integer OneShapeIsHalfSpace(const TopoDS_Shape& S1,const TopoDS_Shape& S2);
static TopoDS_Solid GetNewSolid(const TopoDS_Shape& S, TopoDS_Face& F);
// modified by NIZHNY-OFV  Thu Apr 18 17:16:45 2002 (F)

//=======================================================================
//function : TopOpeBRep_ShapeIntersector
//purpose  : 
//=======================================================================

TopOpeBRep_ShapeIntersector::TopOpeBRep_ShapeIntersector()
{
  Reset();
  myFFIntersector.GetTolerances(myTol1,myTol2);
  myHBoxTool = FBOX_GetHBoxTool();
  myFaceScanner.ChangeBoxSort().SetHBoxTool(myHBoxTool);
  myEdgeScanner.ChangeBoxSort().SetHBoxTool(myHBoxTool);
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::Reset()
{
  myIntersectionDone = Standard_False;

  myFFDone = Standard_False;
  myFFSameDomain = Standard_False;
  myEEFFDone = Standard_False;
  myEFDone = Standard_False;
  myFEDone = Standard_False;
  myEEDone = Standard_False;

  myFFInit = Standard_False;
  myEEFFInit = Standard_False;
  myEFInit = Standard_False;
  myFEInit = Standard_False;
  myEEInit = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::Init
(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  Reset();
  myShape1 = S1;
  myShape2 = S2;
}

//=======================================================================
//function : SetIntersectionDone
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::SetIntersectionDone()
{
  myIntersectionDone = (myFFDone || 
			myEEFFDone || 
			myFEDone || 
			myEFDone || 
			myEEDone);
}


//=======================================================================
//function : CurrentGeomShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRep_ShapeIntersector::CurrentGeomShape
  (const Standard_Integer Index) const
{
  if ( myIntersectionDone ) {
    if      (myFFDone) {
      if      ( Index == 1 ) return myFaceScanner.Current();
      else if ( Index == 2 ) return myFaceExplorer.Current();
    }
    else if (myEEFFDone) {
      if      ( Index == 1 ) return myEdgeScanner.Current();
      else if ( Index == 2 ) return myEdgeExplorer.Current();
    }
    else if (myFEDone) {
      if      ( Index == 1 ) return myFaceScanner.Current();
      else if ( Index == 2 ) return myEdgeExplorer.Current();
    }
    else if (myEFDone) {
      if      ( Index == 1 ) return myEdgeScanner.Current();
      else if ( Index == 2 ) return myFaceExplorer.Current();
    }
    else if (myEEDone) {
      if      ( Index == 1 ) return myEdgeScanner.Current();
      else if ( Index == 2 ) return myEdgeExplorer.Current();
    }
  }

  throw Standard_Failure("CurrentGeomShape : no intersection");
}
//modified by NIZNHY-PKV Fri Sep 24 11:02:59 1999 from
//=======================================================================
//function : RejectedFaces
//purpose  : 
//=======================================================================
void TopOpeBRep_ShapeIntersector::RejectedFaces (const TopoDS_Shape& anObj,
						 const TopoDS_Shape& aReference,
						 TopTools_ListOfShape& aListOfShape)
{

  Standard_Integer isHalfSpace = OneShapeIsHalfSpace( anObj, aReference );
  if( isHalfSpace != 0 )
    {
      TopoDS_Face newRejectFace;
      TopoDS_Solid newSolid;
      aListOfShape.Clear();
      
      if( isHalfSpace == 1 )
	{
	  newSolid = GetNewSolid( anObj, newRejectFace );
	  Init( newSolid, aReference );

	  TopAbs_ShapeEnum tscann = TopAbs_SOLID;
	  TopAbs_ShapeEnum texplo = TopAbs_FACE;
	  myFaceScanner.Clear();
	  myFaceScanner.AddBoxesMakeCOB( aReference, tscann );
	  myFaceExplorer.Init( newSolid, texplo );
  
	  for(; myFaceExplorer.More(); myFaceExplorer.Next())
	    {
	      TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
	      if(!aBS.Compare(myFaceExplorer.Current()).More())
		{
		  const TopoDS_Shape& aS=myFaceExplorer.Current();
		  aListOfShape.Append (aS);
		}
	    }

	  texplo = TopAbs_EDGE;
	  myFaceScanner.Clear();
	  myFaceScanner.AddBoxesMakeCOB( aReference, tscann );
	  myFaceExplorer.Init( newSolid, texplo );

	  for(; myFaceExplorer.More(); myFaceExplorer.Next())
	    {
	      TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
	      if(!aBS.Compare(myFaceExplorer.Current()).More())
		{
		  const TopoDS_Shape& aS=myFaceExplorer.Current();
		  aListOfShape.Append (aS);
		}
	    }
	}
      else
	{
	  newSolid = GetNewSolid( aReference, newRejectFace );
	  Init( anObj, newSolid );

	  TopAbs_ShapeEnum tscann = TopAbs_SOLID;
	  TopAbs_ShapeEnum texplo = TopAbs_FACE;
	  myFaceScanner.Clear();
	  myFaceScanner.AddBoxesMakeCOB( newSolid, tscann );
	  myFaceExplorer.Init( anObj, texplo);
  
	  for(; myFaceExplorer.More(); myFaceExplorer.Next())
	    {
	      TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
	      if(!aBS.Compare(myFaceExplorer.Current()).More())
		{
		  const TopoDS_Shape& aS=myFaceExplorer.Current();
		  aListOfShape.Append (aS);
		}
	    }

	  texplo = TopAbs_EDGE;
	  myFaceScanner.Clear();
	  myFaceScanner.AddBoxesMakeCOB( newSolid, tscann );
	  myFaceExplorer.Init( anObj, texplo );

	  for(; myFaceExplorer.More(); myFaceExplorer.Next())
	    {
	      TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
	      if(!aBS.Compare(myFaceExplorer.Current()).More())
		{
		  const TopoDS_Shape& aS=myFaceExplorer.Current();
		  aListOfShape.Append (aS);
		}
	    }
	}
      // remove all shapes of < newRejectFace > from list
      TopExp_Explorer ExpRF(newRejectFace, TopAbs_EDGE);
      for(; ExpRF.More(); ExpRF.Next() )
	{
	  const TopoDS_Edge& edgef = TopoDS::Edge( ExpRF.Current() );
	  TopTools_ListIteratorOfListOfShape it( aListOfShape );
	  for(; it.More(); it.Next() )
	    {
	      const TopoDS_Shape& shape = it.Value();

	      if( shape.ShapeType() != TopAbs_EDGE )
		continue;

	      const TopoDS_Edge& edgel = TopoDS::Edge( shape );
	      if( edgef.IsSame( edgel ) )
		{
		  aListOfShape.Remove(it);
		  break;
		}
	    }
	}
      TopTools_ListIteratorOfListOfShape it( aListOfShape );
      for(; it.More(); it.Next() )
	{
	  const TopoDS_Shape& shape = it.Value();

	  if( shape.ShapeType() != TopAbs_FACE )
	    continue;

	  const TopoDS_Face& facel = TopoDS::Face( shape );
	  if( facel.IsSame( newRejectFace ) )
	    {
	      aListOfShape.Remove(it);
	      break;
	    }
	}
      
      Init(anObj, aReference);
      return;
    }

  Init(anObj, aReference);

  aListOfShape.Clear(); 
  //find faces to reject
  
  TopAbs_ShapeEnum tscann = TopAbs_SOLID;
  TopAbs_ShapeEnum texplo = TopAbs_FACE;
  myFaceScanner.Clear();
  myFaceScanner.AddBoxesMakeCOB(aReference,tscann);
  myFaceExplorer.Init(anObj,texplo);
  
  for(; myFaceExplorer.More(); myFaceExplorer.Next()) {
    TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
    if(!aBS.Compare(myFaceExplorer.Current()).More()) {
      const TopoDS_Shape& aS=myFaceExplorer.Current();
      aListOfShape.Append (aS);
    }
  }

  //modified by NIZHNY-MZV  Wed Apr  5 09:45:17 2000
  texplo = TopAbs_EDGE;
  myFaceScanner.Clear();
  myFaceScanner.AddBoxesMakeCOB(aReference,tscann);
  myFaceExplorer.Init(anObj,texplo);

  for(; myFaceExplorer.More(); myFaceExplorer.Next()) {
    TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
    if(!aBS.Compare(myFaceExplorer.Current()).More()) {
      const TopoDS_Shape& aS=myFaceExplorer.Current();
      aListOfShape.Append (aS);
    }
  }

  //modified by NIZHNY-MZV  Wed Apr  5 09:45:17 2000
/*  texplo = TopAbs_VERTEX;
  myFaceScanner.Clear();
  myFaceScanner.AddBoxesMakeCOB(aReference,tscann);
  myFaceExplorer.Init(anObj,texplo);

  for(; myFaceExplorer.More(); myFaceExplorer.Next()) {
    TopOpeBRepTool_BoxSort& aBS = myFaceScanner.ChangeBoxSort();
    if(!aBS.Compare(myFaceExplorer.Current()).More()) {
      const TopoDS_Shape& aS=myFaceExplorer.Current();
      aListOfShape.Append (aS);
    }
  }
*/
}
//modified by NIZNHY-PKV Fri Sep 24 11:03:02 1999 to




//=======================================================================
//function : InitIntersection
//purpose  : 
//=======================================================================

  void TopOpeBRep_ShapeIntersector::InitIntersection (const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  Init(S1,S2);

  InitFFIntersection();
  if ( MoreFFCouple() ) return;

  InitFEIntersection();
  if ( MoreFECouple() ) return;

  InitEFIntersection();
  if ( MoreEFCouple() ) return;
}


//=======================================================================
//function : InitIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::InitIntersection
  (const TopoDS_Shape& S1, const TopoDS_Shape& S2,
   const TopoDS_Face&  F1, const TopoDS_Face&  F2)
{
  Init(S1,S2);

  myEEFace1 = F1;
  myEEFace2 = F2;

  InitEEIntersection();
}


//=======================================================================
//function : MoreIntersection
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector::MoreIntersection() const
{
  Standard_Boolean res = myIntersectionDone;

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceSI() && res) {
    if      ( myFFDone )   cout<<"FF : ";
    else if ( myEEFFDone ) cout<<"    EE : ";
    DumpCurrent(1);
    DumpCurrent(2);
    if      ( myFFDone && myFFSameDomain ) cout<<"(FF SameDomain)";
    else if ( myEEFFDone )                 cout<<"(EE of FF SameDomain)";
    else if ( myEEDone )                   cout<<"EE : ";
    cout<<endl;
    if (myEEFFDone) {
      Standard_Integer ie1 = myEdgeScanner.Index();
      Standard_Integer ie2 = myEdgeExplorer.Index();
      Standard_Integer if1 = myFaceScanner.Index();
      Standard_Integer if2 = myFaceExplorer.Index();
      cout<<"    trc teeff 1 "<<ie1<<" "<<ie2<<" "<<if1<<" "<<if2<<"; # ie1 ie2 if1 if2"<<endl;
      Standard_Boolean b = TopOpeBRep_GettraceEEFF(ie1,ie2,if1,if2);
      if (b) seteefft(ie1,ie2,if1,if2);
      else   seteefff(ie1,ie2,if1,if2);
    }
  }    
#endif

  return res;
}


//=======================================================================
//function : DumpCurrent
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
void TopOpeBRep_ShapeIntersector::DumpCurrent(const Standard_Integer K) const
{
  if      ( myFFDone ) {
    if      ( K == 1 ) myFaceScanner.DumpCurrent(cout);
    else if ( K == 2 ) myFaceExplorer.DumpCurrent(cout);
  }
  else if ( myEEFFDone ) {
    if      ( K == 1 ) myEdgeScanner.DumpCurrent(cout);
    else if ( K == 2 ) myEdgeExplorer.DumpCurrent(cout);
  }
  else if ( myFEDone ) {
    if      ( K == 1 ) myFaceScanner.DumpCurrent(cout);
    else if ( K == 2 ) myEdgeExplorer.DumpCurrent(cout);
  }
  else if ( myEFDone ) {
    if      ( K == 1 ) myEdgeScanner.DumpCurrent(cout);
    else if ( K == 2 ) myFaceExplorer.DumpCurrent(cout);
  }
  else if ( myEEDone ) {
    if      ( K == 1 ) myEdgeScanner.DumpCurrent(cout);
    else if ( K == 2 ) myEdgeExplorer.DumpCurrent(cout);
  }
}
#else
void TopOpeBRep_ShapeIntersector::DumpCurrent(const Standard_Integer) const {}
#endif

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
Standard_Integer TopOpeBRep_ShapeIntersector::Index
(const Standard_Integer K)const
{
  Standard_Integer i = 0;

  if      ( myFFDone ) {
    if      ( K == 1 ) i = myFaceScanner.Index();
    else if ( K == 2 ) i = myFaceExplorer.Index();
  }
  else if ( myEEFFDone ) {
    if      ( K == 1 ) i = myEdgeScanner.Index();
    else if ( K == 2 ) i = myEdgeExplorer.Index();
  }
  else if ( myFEDone ) {
    if      ( K == 1 ) i = myFaceScanner.Index();
    else if ( K == 2 ) i = myEdgeExplorer.Index();
  }
  else if ( myEFDone ) {
    if      ( K == 1 ) i = myEdgeScanner.Index();
    else if ( K == 2 ) i = myFaceExplorer.Index();
  }
  else if ( myEEDone ) {
    if      ( K == 1 ) i = myEdgeScanner.Index();
    else if ( K == 2 ) i = myEdgeExplorer.Index();
  }
  return i;
}
#else
Standard_Integer TopOpeBRep_ShapeIntersector::Index (const Standard_Integer)const
{
  return 0;
}
#endif


//=======================================================================
//function : NextIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::NextIntersection()
{
  myIntersectionDone = Standard_False;

  if (myFFSameDomain) {
    // precedant etat du More() : 2 faces samedomain
    myFFDone = Standard_False;
    myFFSameDomain = Standard_False;
    InitEEFFIntersection();
    FindEEFFIntersection();
    if ( !myIntersectionDone ) {
      NextFFCouple();
      FindFFIntersection();
    }
  }
  else if (myFFDone) {
    NextFFCouple();
    FindFFIntersection();
  }
  else if ( myEEFFDone ) {
    NextEEFFCouple();
    FindEEFFIntersection();
    if ( !myIntersectionDone ) {
      NextFFCouple();
      FindFFIntersection();
    }
  }
  else if ( myFEDone ) {
    NextFECouple();
    FindFEIntersection();
  }
  else if ( myEFDone ) {
    NextEFCouple();
    FindEFIntersection();
  }
   else if ( myEEDone ) {
    NextEECouple();
    FindEEIntersection();
  }

  if ( !myIntersectionDone ) {
    InitFFIntersection();
  }
    
  if ( !myIntersectionDone ) {
    InitFEIntersection();
  }
    
  if ( !myIntersectionDone ) {
    InitEFIntersection();
  }

  if ( !myIntersectionDone ) {
    if ( !myEEFace1.IsNull() && !myEEFace2.IsNull() ) {
      InitEEIntersection();
    }
  }

}


// ========
// FFFFFFFF
// ========


//=======================================================================
//function : InitFFIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::InitFFIntersection()
{
 if ( !myFFInit) { 
   TopAbs_ShapeEnum tscann = TopAbs_FACE;
   TopAbs_ShapeEnum texplo = TopAbs_FACE;
   myFaceScanner.Clear();
   myFaceScanner.AddBoxesMakeCOB(myShape1,tscann);
   myFaceExplorer.Init(myShape2,texplo);
   myFaceScanner.Init(myFaceExplorer);
   FindFFIntersection();
 }
 myFFInit = Standard_True;
}


//=======================================================================
//function : FindFFIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::FindFFIntersection()
{
  myFFDone = Standard_False;
  myFFSameDomain = Standard_False ;

  while ( MoreFFCouple() ) {

    // The two candidate intersecting GeomShapes GS1,GS2 and their types t1,t2
    const TopoDS_Shape& GS1 = myFaceScanner.Current();
    const TopoDS_Shape& GS2 = myFaceExplorer.Current();

#ifdef OCCT_DEBUG
    SAVFFi1 = myFaceScanner.Index(); SAVFFi2 = myFaceExplorer.Index(); 
    if (TopOpeBRep_GettraceSI()) {
      cout<<"?? FF : ";
      myFaceScanner.DumpCurrent(cout); myFaceExplorer.DumpCurrent(cout);
      cout<<endl;
    }    
#endif

    const TopOpeBRepTool_BoxSort& BS = myFaceScanner.BoxSort();
    const Bnd_Box& B1 = BS.Box(GS1);
    const Bnd_Box& B2 = BS.Box(GS2);
    myFFIntersector.Perform(GS1,GS2,B1,B2);
    Standard_Boolean ok = myFFIntersector.IsDone(); //xpu210998
    if (!ok) {
      NextFFCouple();
      continue;
    }

    myFFSameDomain = myFFIntersector.SameDomain();

    if (myFFSameDomain) {
      myFFDone = Standard_True;
      break;
    }
    else {
      myFFDone = ! (myFFIntersector.IsEmpty());
      
      // update face/face intersection tolerances
      if (myFFDone) {
	Standard_Real tol1,tol2;
	myFFIntersector.GetTolerances(tol1,tol2);
	myTol1 = Max(myTol1,tol1); myTol2 = Max(myTol2,tol2);
      }
      
      if ( myFFDone ) {
	break;
      }
    }

    NextFFCouple();
  }

  SetIntersectionDone();
}


//=======================================================================
//function : MoreFFCouple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector::MoreFFCouple() const
{
  Standard_Boolean more1 = myFaceScanner.More();
  Standard_Boolean more2 = myFaceExplorer.More();
  return (more1 && more2);
}


//=======================================================================
//function : NextFFCouple
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::NextFFCouple()
{
  myFaceScanner.Next();
  Standard_Boolean b1,b2;

  b1 = (!myFaceScanner.More());
  b2 = (myFaceExplorer.More());
  while ( b1 && b2 ) {
    myFaceExplorer.Next();
    myFaceScanner.Init(myFaceExplorer);
    b1 = (!myFaceScanner.More());
    b2 = (myFaceExplorer.More());
  }

}


// ========
// EEFFEEFF
// ========


//=======================================================================
//function : InitEEFFIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::InitEEFFIntersection()
{
  // prepare exploration of the edges of the two current SameDomain faces 
  TopoDS_Shape face1 = myFaceScanner.Current(); // -26-08-96
  TopoDS_Shape face2 = myFaceExplorer.Current(); // -26-08-96

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GetcontextFFOR()) {
    face1.Orientation(TopAbs_FORWARD); //-05/07
    face2.Orientation(TopAbs_FORWARD); //-05/07
    cout<<"ctx : InitEEFFIntersection : faces FORWARD"<<endl;
  }
#endif

  const TopOpeBRepTool_BoxSort& BS = myFaceScanner.BoxSort();
  const Bnd_Box& B1 = BS.Box(face1);
  const Bnd_Box& B2 = BS.Box(face2);
  myEEIntersector.SetFaces(face1,face2,B1,B2);
  
  TopAbs_ShapeEnum tscann = TopAbs_EDGE;
  TopAbs_ShapeEnum texplo = TopAbs_EDGE;
  myEdgeScanner.Clear();
  myEdgeScanner.AddBoxesMakeCOB(face1,tscann);
  myEdgeExplorer.Init(face2,texplo);
  myEdgeScanner.Init(myEdgeExplorer);
  
  myEEFFInit = Standard_True;
}


//=======================================================================
//function : FindEEFFIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::FindEEFFIntersection()
{
  myEEFFDone = Standard_False;
  while ( MoreEEFFCouple() ) {
    const TopoDS_Shape& GS1 = myEdgeScanner.Current();
    const TopoDS_Shape& GS2 = myEdgeExplorer.Current();
    myEEIntersector.Perform(GS1,GS2);

#ifdef OCCT_DEBUG
    if (TopOpeBRep_GettraceSI() && myEEIntersector.IsEmpty()) {
      cout<<"    EE : ";
      myEdgeScanner.DumpCurrent(cout);
      myEdgeExplorer.DumpCurrent(cout);
      cout<<"(EE of FF SameDomain)";
      cout<<" : EMPTY INTERSECTION";
      cout<<endl;
    }    
#endif

    myEEFFDone = ! (myEEIntersector.IsEmpty());
    if (myEEFFDone) break;
    else NextEEFFCouple();
  }
  SetIntersectionDone();
}


//=======================================================================
//function : MoreEEFFCouple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector::MoreEEFFCouple() const
{
  Standard_Boolean more1 = myEdgeScanner.More();
  Standard_Boolean more2 = myEdgeExplorer.More();
  return (more1 && more2);
}


//=======================================================================
//function : NextEEFFCouple
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::NextEEFFCouple()
{
  myEdgeScanner.Next();
  while ( ! myEdgeScanner.More() && myEdgeExplorer.More() ) {
    myEdgeExplorer.Next();
    myEdgeScanner.Init(myEdgeExplorer);
  }
}


// ========
// FEFEFEFE
// ========


//=======================================================================
//function : InitFEIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::InitFEIntersection()
{
 if ( !myFEInit) { 
   TopAbs_ShapeEnum tscann = TopAbs_FACE;
   TopAbs_ShapeEnum texplo = TopAbs_EDGE;
   myFaceScanner.Clear(); 
   myFaceScanner.AddBoxesMakeCOB(myShape1,tscann);
   myEdgeExplorer.Init(myShape2,texplo,TopAbs_FACE); // NYI defaut de spec
   myFaceScanner.Init(myEdgeExplorer);
   FindFEIntersection();
 }
 myFEInit = Standard_True;
}


//=======================================================================
//function : FindFEIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::FindFEIntersection()
{
  myFEDone = Standard_False;
  while ( MoreFECouple() ) {
    const TopoDS_Shape& GS1 = myFaceScanner.Current();
    const TopoDS_Shape& GS2 = myEdgeExplorer.Current();
    myFEIntersector.Perform(GS1,GS2);
    myFEDone = ! (myFEIntersector.IsEmpty());
    if (myFEDone) break;
    else NextFECouple();
  }
  SetIntersectionDone();
}


//=======================================================================
//function : MoreFECouple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector::MoreFECouple() const
{
  Standard_Boolean more1 = myFaceScanner.More();
  Standard_Boolean more2 = myEdgeExplorer.More();
  return (more1 && more2);
}


//=======================================================================
//function : NextFECouple
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::NextFECouple()
{
  myFaceScanner.Next();
  while ( ! myFaceScanner.More() && myEdgeExplorer.More() ) {
    myEdgeExplorer.Next();
    myFaceScanner.Init(myEdgeExplorer);
  }
}


// ========
// EFEFEFEF
// ========


//=======================================================================
//function : InitEFIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::InitEFIntersection()
{
 if ( !myEFInit) { 
   TopAbs_ShapeEnum tscann = TopAbs_EDGE;
   TopAbs_ShapeEnum texplo = TopAbs_FACE;
   myEdgeScanner.Clear(); 
   myEdgeScanner.AddBoxesMakeCOB(myShape1,tscann, TopAbs_FACE); // NYI defaut de spec
   myFaceExplorer.Init(myShape2,texplo);
   myEdgeScanner.Init(myFaceExplorer);
   FindEFIntersection();
 }
 myEFInit = Standard_True;
} 
 
//=======================================================================
//function : FindEFIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::FindEFIntersection()
{
  myEFDone = Standard_False;
  while ( MoreEFCouple() ) {
    const TopoDS_Shape& GS1 = myEdgeScanner.Current();
    const TopoDS_Shape& GS2 = myFaceExplorer.Current();
    myFEIntersector.Perform(GS2,GS1);
    myEFDone = ! (myFEIntersector.IsEmpty());
    if (myEFDone) break;
    else NextEFCouple();
  }
  SetIntersectionDone();
}


//=======================================================================
//function : MoreEFCouple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector::MoreEFCouple() const
{
  Standard_Boolean more1 = myEdgeScanner.More();
  Standard_Boolean more2 = myFaceExplorer.More();
  return (more1 && more2);
}


//=======================================================================
//function : NextEFCouple
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::NextEFCouple()
{
  myEdgeScanner.Next();
  while ( ! myEdgeScanner.More() && myFaceExplorer.More() ) {
    myFaceExplorer.Next();
    myEdgeScanner.Init(myFaceExplorer);
  }
}

// ========
// EEEEEEEE
// ========

//=======================================================================
//function : InitEEIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::InitEEIntersection()
{
  if ( ! myEEInit ) {
    TopoDS_Shape face1 = myEEFace1.Oriented(TopAbs_FORWARD);
    TopoDS_Shape face2 = myEEFace2.Oriented(TopAbs_FORWARD);
    const TopOpeBRepTool_BoxSort& BS = myFaceScanner.BoxSort();
    const Bnd_Box& B1 = BS.Box(face1);
    const Bnd_Box& B2 = BS.Box(face2);
    myEEIntersector.SetFaces(face1,face2,B1,B2);

    TopAbs_ShapeEnum tscann = TopAbs_EDGE;
    TopAbs_ShapeEnum texplo = TopAbs_EDGE;
    myEdgeScanner.Clear();
    myEdgeScanner.AddBoxesMakeCOB(myShape1,tscann);
    myEdgeExplorer.Init(myShape2,texplo);
    myEdgeScanner.Init(myEdgeExplorer);
    FindEEIntersection();
  }
  myEEInit = Standard_True;
}


//=======================================================================
//function : FindEEIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::FindEEIntersection()
{
  myEEDone = Standard_False;
  while ( MoreEECouple() ) {
    const TopoDS_Shape& GS1 = myEdgeScanner.Current();
    const TopoDS_Shape& GS2 = myEdgeExplorer.Current();
    myEEIntersector.Perform(GS1,GS2);
    myEEDone = ! (myEEIntersector.IsEmpty());
    if (myEEDone) break;
    else NextEECouple();
  }
  SetIntersectionDone();
}


//=======================================================================
//function : MoreEECouple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_ShapeIntersector::MoreEECouple() const
{
  Standard_Boolean more1 = myEdgeScanner.More();
  Standard_Boolean more2 = myEdgeExplorer.More();
  return (more1 && more2);
}


//=======================================================================
//function : NextEECouple
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::NextEECouple()
{
  myEdgeScanner.Next();
  while ( ! myEdgeScanner.More() && myEdgeExplorer.More() ) {
    myEdgeExplorer.Next();
    myEdgeScanner.Init(myEdgeExplorer);
  }
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRep_ShapeIntersector::Shape
  ( const Standard_Integer Index )const
{
  if      ( Index == 1 ) return myShape1;
  else if ( Index == 2 ) return myShape2;

  throw Standard_Failure("ShapeIntersector : no shape");
}


//=======================================================================
//function : ChangeFacesIntersector
//purpose  : 
//=======================================================================

TopOpeBRep_FacesIntersector& 
TopOpeBRep_ShapeIntersector::ChangeFacesIntersector()
{ return myFFIntersector; }


//=======================================================================
//function : ChangeEdgesIntersector
//purpose  : 
//=======================================================================

TopOpeBRep_EdgesIntersector& 
TopOpeBRep_ShapeIntersector::ChangeEdgesIntersector()
{ return myEEIntersector; }


//=======================================================================
//function : ChangeFaceEdgeIntersector
//purpose  : 
//=======================================================================

TopOpeBRep_FaceEdgeIntersector& 
TopOpeBRep_ShapeIntersector::ChangeFaceEdgeIntersector()
{ return myFEIntersector; }


//=======================================================================
//function : GetTolerances
//purpose  : 
//=======================================================================

void TopOpeBRep_ShapeIntersector::GetTolerances(Standard_Real& tol1,
						Standard_Real& tol2)const
{ tol1 = myTol1; tol2 = myTol2; }




//=======================================================================
//function : IsHalfSpaceShape
//purpose  : tries to define if one of solids is a half space object
//           returns:
//                    0 - no half spaces ( default )
//                    1 - half space is S1
//                    2 - half space is S2
//=======================================================================
static Standard_Integer OneShapeIsHalfSpace(const TopoDS_Shape& S1,const TopoDS_Shape& S2)
{
  Standard_Integer result = 0;

  if( S1.ShapeType() == TopAbs_SOLID && S2.ShapeType() == TopAbs_SOLID )
    {
      TopExp_Explorer ExpSol1(S1, TopAbs_FACE);
      TopExp_Explorer ExpSol2(S2, TopAbs_FACE);
      Standard_Integer NbFacesSol1 = 0;
      Standard_Integer NbFacesSol2 = 0;

      for(; ExpSol1.More(); ExpSol1.Next() )
	NbFacesSol1++;

      for(; ExpSol2.More(); ExpSol2.Next() )
	NbFacesSol2++;

      if( NbFacesSol1 == 0 || NbFacesSol2 == 0 )  // strange solids!!!
	return result;
      if( NbFacesSol1 == 1 && NbFacesSol2 == 1 )  // both shapes are half spaces ???
	return result;

      if( (NbFacesSol1 == 1 && NbFacesSol2 >= 2) || (NbFacesSol2 == 1 && NbFacesSol1 >= 2) )
	{
	  // if one solid has shell consisted of only a face but other one has valid closed
	  // shell we can detect current boolean operation as operation with half space object.
	  // if shell of second solid is not valid too we cann't detect what kind of objects
	  // we try to perform. in this case we do nothing and just return. 

	  // but before we must avoid shperes, toruses and solids with a face biult on spherical surfaces
	  // of revolution (SSRFS) - solids with shell of one face:
	  // sphere (U: 0, 2PI) (V: -PI/2, PI/2),
	  // torus  (U: 0, 2PI) (V: 0, 2PI).
	  // SSRFS  (U period = (PI), 2PI) (V period = (PI), 2PI)
	  // these solids are not halfspaces.

	  TopExp_Explorer SolidExplorer;
	  TopoDS_Face testFace;

	  if( NbFacesSol1 == 1 )
	    {
	      for( SolidExplorer.Init(S1, TopAbs_FACE); SolidExplorer.More(); SolidExplorer.Next() )
		testFace = TopoDS::Face( SolidExplorer.Current() );
	    }
	  else
	    {
	      for( SolidExplorer.Init(S2, TopAbs_FACE); SolidExplorer.More(); SolidExplorer.Next() )
		testFace = TopoDS::Face( SolidExplorer.Current() );
	    }

	  BRepAdaptor_Surface FSurf( testFace );
	  Standard_Boolean SolidIsSphereOrTorus = Standard_False;
	  
	  if( FSurf.GetType() == GeomAbs_Sphere ||  FSurf.GetType() == GeomAbs_Torus )
	    {
	      Standard_Real minU = FSurf.FirstUParameter();
	      Standard_Real maxU = FSurf.LastUParameter();
	      Standard_Real minV = FSurf.FirstVParameter();
	      Standard_Real maxV = FSurf.LastVParameter();
	      Standard_Boolean yesU = ( Abs(minU - 0.) < 1.e-9 && Abs(maxU - 2*M_PI) < 1.e-9 );
	      Standard_Boolean yesV = ( FSurf.GetType() == GeomAbs_Sphere ) ?
		( Abs(minV - (-M_PI/2.)) < 1.e-9 && Abs(maxV - M_PI/2.) < 1.e-9 ) :
		( Abs(minV - 0.) < 1.e-9 && Abs(maxV - 2*M_PI) < 1.e-9 );
	      SolidIsSphereOrTorus = ( yesU && yesV );
	    }

	  if( FSurf.GetType() == GeomAbs_SurfaceOfRevolution )
	    {
	      Standard_Boolean areBothPeriodic = ( FSurf.IsUPeriodic() && FSurf.IsVPeriodic() );
	      if( areBothPeriodic )
		{
		  Standard_Boolean yesU = ( Abs(FSurf.UPeriod() - M_PI) < 1.e-9 || Abs(FSurf.UPeriod() - 2*M_PI) < 1.e-9 );
		  Standard_Boolean yesV = ( Abs(FSurf.VPeriod() - M_PI) < 1.e-9 || Abs(FSurf.VPeriod() - 2*M_PI) < 1.e-9 );
		  SolidIsSphereOrTorus = ( yesU && yesV );
		}
	    }

	  if( SolidIsSphereOrTorus )
	      return result;

	  Standard_Boolean SecondShellOk = Standard_True;
	  TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
	  aMapEF.Clear();
	  Standard_Integer NbEdges = 0, NbFaces = 0, iE = 0;

	  if( NbFacesSol1 == 1 )
	    TopExp::MapShapesAndAncestors(S2, TopAbs_EDGE, TopAbs_FACE, aMapEF);
	  else
	    TopExp::MapShapesAndAncestors(S1, TopAbs_EDGE, TopAbs_FACE, aMapEF);

	  NbEdges = aMapEF.Extent();
	  for( iE = 1; iE <= NbEdges; iE++)
	    {
	      const TopTools_ListOfShape& listFaces = aMapEF.FindFromIndex( iE );
	      NbFaces = listFaces.Extent();
	      if( NbFaces != 2 )
		{
		  SecondShellOk = Standard_False;
		  break;
		}
	    }
	  aMapEF.Clear();

	  if( SecondShellOk )
	    result = (NbFacesSol1 == 1) ? 1 : 2;
	}
      else
	{
	  // ************************** IMPORTANT !!! ************************************
	  // both shells are of more than 2 faces. if both solids have invalid shells
	  // we do nothing and just return. on the other hand if only one shell is valid
	  // currently we should suppose that solid with invalid shell is a half space too,
	  // but this is not always true.
	  //
	  // so this suggestion must be developed carefully. while we don't classify it!
	  // *****************************************************************************
	}
#ifdef OCCT_DEBUG
      if( result != 0 )
	cout << "# one of the SOLIDs probably is a HALF SPACE" << endl;
#endif
    }

  return result;
}

//=======================================================================
//function : GetNewSolid
//purpose  : rebuild halfspace solid adding new "face on infinity"
//           to build correct bounding box to classify carefully
//           "rejected shapes".
//=======================================================================
static TopoDS_Solid GetNewSolid(const TopoDS_Shape& S, TopoDS_Face& F)
{
  // "new solid" is a new halfspace solid consists of two faces now: the first face is a face
  // used to build halfspace solid and the second face is a new "face on infinity" specially
  // created to constuct correct bounding box around halfspace solid with bounds more wide than
  // previous one.

  // the following algorithm is used:
  // 1. get face used to build halfspace solid and calculate its normal, Min/Max (U,V) parameters,
  //    four points lying on the surface of the face inside its restrictions, surface, tolerance
  //    and location.
  // 2. define the direction into the halfspace solid and project four points along this direction
  //    on infinite distance. then use those points to create "face on infinity".
  // 3. build new shell with two new faces and new "halfspace solid".
  // 4. return this solid and "face on infinity" to remove it and all its subshapes from the list
  //    of rejected shapes.

  TopExp_Explorer ShapeExplorer;

  TopoDS_Face hsFace;
  
  for( ShapeExplorer.Init(S, TopAbs_FACE); ShapeExplorer.More(); ShapeExplorer.Next() )
    hsFace = TopoDS::Face( ShapeExplorer.Current() );

  BRepAdaptor_Surface ASurf( hsFace );
  
  Standard_Real MinU = ASurf.FirstUParameter();
  Standard_Real MaxU = ASurf.LastUParameter();
  Standard_Real MinV = ASurf.FirstVParameter();
  Standard_Real MaxV = ASurf.LastUParameter();

  Standard_Real MidU = (MaxU + MinU) * 0.5;
  Standard_Real MidV = (MaxV + MinV) * 0.5;

  gp_Pnt MidP;
  gp_Vec SurfDU, SurfDV;
  ASurf.D1( MidU, MidV, MidP, SurfDU, SurfDV );
  
  gp_Vec Normal = SurfDU.Crossed( SurfDV );

  if( hsFace.Orientation() == TopAbs_FORWARD )
    Normal *= -1.e+10;
  else
    Normal *= 1.e+10;

  Standard_Real Pu1 = MinU + Abs( (MaxU - MinU) / 4. );
  Standard_Real Pu2 = MinU + Abs( (MaxU - MinU) / 4. * 3. );
  Standard_Real Pv1 = MinV + Abs( (MaxV - MinV) / 4. );
  Standard_Real Pv2 = MinV + Abs( (MaxV - MinV) / 4. * 3. );

  gp_Pnt P1, P2, P3, P4;
  ASurf.D0( Pu1, Pv1, P1 );
  ASurf.D0( Pu1, Pv2, P2 );
  ASurf.D0( Pu2, Pv1, P3 );
  ASurf.D0( Pu2, Pv2, P4 ); 

  P1.Translate( Normal );
  P2.Translate( Normal );
  P3.Translate( Normal );
  P4.Translate( Normal );

  BRepLib_MakeEdge mke1( P1, P2 );
  BRepLib_MakeEdge mke2( P2, P4 );
  BRepLib_MakeEdge mke3( P4, P3 );
  BRepLib_MakeEdge mke4( P3, P1 );

  TopoDS_Edge e1 = mke1.Edge();
  TopoDS_Edge e2 = mke2.Edge();
  TopoDS_Edge e3 = mke3.Edge();
  TopoDS_Edge e4 = mke4.Edge();

  BRepLib_MakeWire mkw( e1, e2, e3, e4 );
  TopoDS_Wire w = mkw.Wire();

  BRepLib_MakeFace mkf( w );
  TopoDS_Face infFace = mkf.Face();
  
  TopoDS_Shell newShell;
  TopoDS_Solid newSolid;

  BRep_Builder newShellBuilder;
  newShellBuilder.MakeShell( newShell );
  newShellBuilder.Add( newShell, hsFace );
  newShellBuilder.Add( newShell, infFace );
  newShell.Closed (BRep_Tool::IsClosed (newShell));

  BRep_Builder newSolidBuilder;
  newSolidBuilder.MakeSolid( newSolid );
  newSolidBuilder.Add( newSolid, newShell );

  F = infFace;
  return newSolid;
}
