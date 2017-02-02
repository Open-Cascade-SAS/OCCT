// Created on: 1995-09-18
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepFill_ListIteratorOfListOfOffsetWire.hxx>
#include <BRepFill_OffsetWire.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Extrema_ExtPS.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG
#include <BRepTools.hxx>
static Standard_Boolean AffichSpine = Standard_False;
#endif

//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffset::BRepOffsetAPI_MakeOffset()
  : myIsInitialized( Standard_False),
    myJoin(GeomAbs_Arc),
    myIsOpenResult(Standard_False)
{
}


//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffset::BRepOffsetAPI_MakeOffset(const TopoDS_Face& Spine, 
                                                   const GeomAbs_JoinType Join,
                                                   const Standard_Boolean IsOpenResult)
{
  Init(Spine, Join, IsOpenResult);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Init(const TopoDS_Face&     Spine,
                                    const GeomAbs_JoinType Join,
                                    const Standard_Boolean IsOpenResult)
{
  myFace          = Spine;
  myIsInitialized = Standard_True;
  myJoin          = Join;
  myIsOpenResult  = IsOpenResult;
  TopExp_Explorer exp;
  for (exp.Init(myFace,TopAbs_WIRE); exp.More();exp.Next()) {
    myWires.Append(exp.Current());
  }
}

//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffset::BRepOffsetAPI_MakeOffset(const TopoDS_Wire& Spine, 
                                                   const GeomAbs_JoinType Join,
                                                   const Standard_Boolean IsOpenResult)
{
  myWires.Append(Spine);
  myIsInitialized = Standard_True;
  myJoin = Join;
  myIsOpenResult  = IsOpenResult;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Init(const GeomAbs_JoinType Join,
                                    const Standard_Boolean IsOpenResult)
{
  myJoin = Join;
  myIsOpenResult  = IsOpenResult;
}

//=======================================================================
//function : BRepOffsetAPI_MakeOffset
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::AddWire(const TopoDS_Wire& Spine)

{
  myIsInitialized = Standard_True;
  myWires.Append(Spine);
}

//=======================================================================
//function : BuildDomain
//purpose  : 
//=======================================================================

static void BuildDomains(TopoDS_Face&               myFace,
                         TopTools_ListOfShape&      WorkWires,
                         BRepFill_ListOfOffsetWire& myAlgos,
                         GeomAbs_JoinType           myJoin,
                         Standard_Boolean           myIsOpenResult,
                         Standard_Boolean           isPositive)
{
  BRepAlgo_FaceRestrictor  FR;
  TopoDS_Vertex            VF,VL;
  TopTools_ListOfShape     LOW;
  BRep_Builder             B;

  if (myFace.IsNull()) {
    myFace   = BRepBuilderAPI_MakeFace(TopoDS::Wire(WorkWires.First()),Standard_True);
    if (myFace.IsNull())
      throw StdFail_NotDone("BRepOffsetAPI_MakeOffset : the wire is not planar");
  }
//  Modified by Sergey KHROMOV - Thu Apr 26 16:04:43 2001 Begin
  TopExp_Explorer anExp(myFace, TopAbs_WIRE);
  TopoDS_Shape    aWire1 = WorkWires.First();
  TopoDS_Shape    aWire2;
  if (anExp.More()) {
    aWire2 = anExp.Current();
    if ((aWire1.Orientation() == aWire2.Orientation() && isPositive) ||
      (aWire1.Orientation() == TopAbs::Complement(aWire2.Orientation()) && !isPositive)) {
        TopTools_ListOfShape LWires;
        TopTools_ListIteratorOfListOfShape itl;
        for (itl.Initialize(WorkWires); itl.More(); itl.Next()) {
          const TopoDS_Shape& W = itl.Value();
          LWires.Append(W.Reversed());
        }
        WorkWires = LWires;
    }
  }
//  Modified by Sergey KHROMOV - Thu Apr 26 16:04:44 2001 End
  FR.Init(myFace,Standard_True);
  //====================================================
  // Construction of faces limited by closed wires.
  //====================================================
  TopTools_ListIteratorOfListOfShape itl(WorkWires);
  for (; itl.More(); itl.Next()) {
    TopoDS_Wire& W = TopoDS::Wire(itl.Value());
    if (W.Closed()){
      FR.Add(W);
      continue;
    }
    TopExp::Vertices (W,VF,VL);
    if (VF.IsSame(VL)) {
      FR.Add(W);
    }
    else {
      LOW.Append(W);
    }
  }
  FR.Perform();
  if (!FR.IsDone()) {
    throw StdFail_NotDone("BRepOffsetAPI_MakeOffset : Build Domains");
  }
  TopTools_ListOfShape Faces;
#ifdef OCCT_DEBUG
  Standard_Integer ns = 0;
#endif
  for (; FR.More(); FR.Next()) {
    Faces.Append(FR.Current());
#ifdef OCCT_DEBUG
    if(AffichSpine)
    {
      char name[32];
      ns++;
      sprintf(name, "FR%d",ns);
      BRepTools::Write(FR.Current(), name);
    }
#endif
  }

  //===========================================
  // No closed wire => only one domain
  //===========================================
  if (Faces.IsEmpty()) {
    TopoDS_Shape aLocalShape = myFace.EmptyCopied();
    TopoDS_Face F = TopoDS::Face(aLocalShape);
//    TopoDS_Face F = TopoDS::Face(myFace.EmptyCopied());
    TopTools_ListIteratorOfListOfShape itW(LOW);
    for ( ; itW.More(); itW.Next()) {
      B.Add(F,itW.Value());
    }
    BRepFill_OffsetWire Algo(F, myJoin, myIsOpenResult);
    myAlgos.Append(Algo);
    return;
  }

  //====================================================
  // Classification of open wires.
  //====================================================  
//  for (TopTools_ListIteratorOfListOfShape itF(Faces); itF.More(); itF.Next()) {
  TopTools_ListIteratorOfListOfShape itF;
  for (itF.Initialize(Faces) ; itF.More(); itF.Next()) {
    TopoDS_Face&          F = TopoDS::Face(itF.Value());
    BRepAdaptor_Surface   S(F,0);
    Standard_Real         Tol = BRep_Tool::Tolerance(F); 

    BRepTopAdaptor_FClass2d CL(F,Precision::Confusion());

    TopTools_ListIteratorOfListOfShape itW(LOW);
    while (itW.More()) {
      TopoDS_Wire& W = TopoDS::Wire(itW.Value());
      //=======================================================
      // Choice of a point on the wire. + projection on the face.
      //=======================================================
      TopExp_Explorer exp(W,TopAbs_VERTEX);
      TopoDS_Vertex   V  = TopoDS::Vertex(exp.Current());
      gp_Pnt2d        PV;
      gp_Pnt          P3d = BRep_Tool::Pnt(V);
      Extrema_ExtPS ExtPS (P3d,S,Tol,Tol);
      Standard_Real     Dist2Min = Precision::Infinite();
      Standard_Real     Found   = Standard_False;
      for (Standard_Integer ie = 1; ie <= ExtPS.NbExt(); ie++) {
        Standard_Real X,Y;
        if (ExtPS.SquareDistance(ie) < Dist2Min) {
          Dist2Min = ExtPS.SquareDistance(ie);
          Found   = Standard_True;
          ExtPS.Point(ie).Parameter(X,Y);
          PV.SetCoord(X,Y);
        }
      }
      if ( Found && (CL.Perform(PV) == TopAbs_IN)) {
        // The face that contains a wire is found and it is removed from the list
        B.Add(F,W);
        LOW.Remove(itW);
      }
      else {
        itW.Next();
      }
    }
  }
  //========================================
  // Creation of algorithms on each domain.
  //========================================
  for (itF.Initialize(Faces); itF.More(); itF.Next()) {
    BRepFill_OffsetWire Algo(TopoDS::Face(itF.Value()), myJoin, myIsOpenResult);
    myAlgos.Append(Algo);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Perform(const Standard_Real Offset,
                                       const Standard_Real Alt)
{
  StdFail_NotDone_Raise_if ( !myIsInitialized,
    "BRepOffsetAPI_MakeOffset : Perform without Init");

  try
  {
    Standard_Integer i = 1;
    BRepFill_ListIteratorOfListOfOffsetWire itOW;
    TopoDS_Compound Res;
    BRep_Builder    B;
    B.MakeCompound (Res);
    myLastIsLeft = (Offset <= 0);

    if( Offset <= 0. )
    {
      if( myLeft.IsEmpty() )
      {
        //  Modified by Sergey KHROMOV - Fri Apr 27 14:35:26 2001 Begin
        BuildDomains(myFace,myWires,myLeft,myJoin,myIsOpenResult, Standard_False);
        //  Modified by Sergey KHROMOV - Fri Apr 27 14:35:26 2001 End
      }

      for (itOW.Initialize(myLeft); itOW.More(); itOW.Next())
      {
        BRepFill_OffsetWire& Algo = itOW.Value();
        Algo.Perform(Abs(Offset),Alt);
        if (Algo.IsDone() && !Algo.Shape().IsNull())
        {
          B.Add(Res,Algo.Shape());
          if (i == 1)
            myShape = Algo.Shape();

          i++;
        }
      }
    }
    else
    {
      if (myRight.IsEmpty())
      {
        //  Modified by Sergey KHROMOV - Fri Apr 27 14:35:28 2001 Begin
        BuildDomains(myFace,myWires,myRight,myJoin,myIsOpenResult, Standard_True);
        //  Modified by Sergey KHROMOV - Fri Apr 27 14:35:35 2001 End
      }

      for(itOW.Initialize(myRight); itOW.More(); itOW.Next())
      {
        BRepFill_OffsetWire& Algo = itOW.Value();
        Algo.Perform(Offset,Alt);

        if (Algo.IsDone() && !Algo.Shape().IsNull())
        {
          B.Add(Res,Algo.Shape());

          if (i == 1)
            myShape = Algo.Shape();

          i++;
        }
      }
    }

    if( i > 2 )
      myShape = Res;

    if(myShape.IsNull())
      NotDone();
    else
      Done();
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout<<"An exception was caught in BRepOffsetAPI_MakeOffset::Perform : ";
    anException.Print(cout); 
    cout<<endl;
#endif
    (void)anException;
    NotDone();
    myShape.Nullify();
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffset::Build()
{
  Done();
}


//=======================================================================
//function : ShapesFromShape
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeOffset::Generated
  (const TopoDS_Shape& S)
{
  myGenerated.Clear();
  BRepFill_ListIteratorOfListOfOffsetWire itOW;
  BRepFill_ListOfOffsetWire* Algos;
  Algos= &myLeft;
  if (!myLastIsLeft) {
    Algos = &myRight;
  }
  for (itOW.Initialize(*Algos); itOW.More(); itOW.Next()) {
    BRepFill_OffsetWire& OW = itOW.Value();
    TopTools_ListOfShape L;
    L =  OW.GeneratedShapes(S.Oriented(TopAbs_FORWARD));
    myGenerated.Append(L);
    L =  OW.GeneratedShapes(S.Oriented(TopAbs_REVERSED));
    myGenerated.Append(L);
  }
  return myGenerated;
}
