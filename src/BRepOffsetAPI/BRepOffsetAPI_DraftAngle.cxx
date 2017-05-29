// Created on: 1995-02-22
// Created by: Jacques GOUSSARD
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
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepFill_DataMapIteratorOfDataMapOfShapeSequenceOfReal.hxx>
#include <BRepFill_DataMapOfShapeSequenceOfReal.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <BRepOffsetAPI_SequenceOfSequenceOfReal.hxx>
#include <BRepOffsetAPI_SequenceOfSequenceOfShape.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Substitution.hxx>
#include <Draft_Modification.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>

//=======================================================================
//function : BRepOffsetAPI_DraftAngle
//purpose  : 
//=======================================================================
BRepOffsetAPI_DraftAngle::BRepOffsetAPI_DraftAngle () {}


//=======================================================================
//function : BRepOffsetAPI_DraftAngle
//purpose  : 
//=======================================================================

BRepOffsetAPI_DraftAngle::BRepOffsetAPI_DraftAngle (const TopoDS_Shape& S)
{
  myInitialShape = S;
  myModification = new Draft_Modification(S);
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepOffsetAPI_DraftAngle::Clear ()
{
  if (!myModification.IsNull()) {
    Handle(Draft_Modification)::DownCast (myModification)->Clear();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepOffsetAPI_DraftAngle::Init (const TopoDS_Shape& S)
{
  myInitialShape = S;
  NotDone();
  if (!myModification.IsNull()) {
    Handle(Draft_Modification)::DownCast (myModification)->Init(S);   
  }
  else {
    myModification = new Draft_Modification(S);
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepOffsetAPI_DraftAngle::Add(const TopoDS_Face& F,
			     const gp_Dir& D,
			     const Standard_Real Angle,
			     const gp_Pln& Plane,
                             const Standard_Boolean Flag)
{
// POP-DPF : protection
  if ( Abs(Angle) <= 1.e-04 ) 
    return;
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::Add() - initial shape is not set");
  Handle(Draft_Modification)::DownCast (myModification)->Add(F,D,Angle,Plane, Flag);
}


//=======================================================================
//function : AddDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffsetAPI_DraftAngle::AddDone () const
{
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::AddDone() - initial shape is not set");
  return Handle(Draft_Modification)::DownCast (myModification)
    ->ProblematicShape().IsNull();
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void BRepOffsetAPI_DraftAngle::Remove(const TopoDS_Face& F)
{
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::Remove() - initial shape is not set");
  Handle(Draft_Modification)::DownCast (myModification)->Remove(F);
}


//=======================================================================
//function : ProblematicShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepOffsetAPI_DraftAngle::ProblematicShape () const
{
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::ProblematicShape() - initial shape is not set");
  return Handle(Draft_Modification)::DownCast (myModification)->ProblematicShape();
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Draft_ErrorStatus BRepOffsetAPI_DraftAngle::Status () const
{
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::Status() - initial shape is not set");
  return Handle(Draft_Modification)::DownCast (myModification)->Error();
}


//=======================================================================
//function : ConnectedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_DraftAngle::ConnectedFaces
   (const TopoDS_Face& F) const
{
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::ConnectedFaces() - initial shape is not set");
  return Handle(Draft_Modification)::DownCast (myModification)->ConnectedFaces(F);
}


//=======================================================================
//function : ModifiedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_DraftAngle::ModifiedFaces() const
{
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::ModifiedFaces() - initial shape is not set");
  return Handle(Draft_Modification)::DownCast (myModification)->ModifiedFaces();
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_DraftAngle::Generated(const TopoDS_Shape& S) 
{
  myGenerated.Clear();
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::Generated() - initial shape is not set");
  Handle(Draft_Modification) DMod = Handle(Draft_Modification)::DownCast (myModification);

  if (S.ShapeType() == TopAbs_FACE) {
    Handle(Geom_Surface) Surf;
    TopLoc_Location      L;
    Standard_Real        Tol;
    Standard_Boolean     RW,RF;
    if (DMod->NewSurface(TopoDS::Face(S), Surf, L, Tol, RW, RF)) {
      if(myVtxToReplace.IsEmpty())
      {
        myGenerated.Append(ModifiedShape (S));
      }
      else
      {
        myGenerated.Append(mySubs.Value(ModifiedShape (S)));
      }
    }
  }
  return myGenerated;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_DraftAngle::Modified(const TopoDS_Shape& S) 
{
  myGenerated.Clear();
  Standard_NullObject_Raise_if (myInitialShape.IsNull(),
                                "BRepOffsetAPI_DraftAngle::Modified() - initial shape is not set");
  Handle(Draft_Modification) DMod = Handle(Draft_Modification)::DownCast (myModification);

  if (S.ShapeType() == TopAbs_FACE) {
    Handle(Geom_Surface) Surf;
    TopLoc_Location      L;
    Standard_Real        Tol;
    Standard_Boolean     RW,RF;
    
    if (!DMod->NewSurface(TopoDS::Face(S), Surf, L, Tol, RW, RF)) {
      // Ce n est pas une generation => peut etre une  modif
      if(myVtxToReplace.IsEmpty())
      {
        myGenerated.Append(ModifiedShape (S));
      }
      else
      {
        myGenerated.Append(mySubs.Value(ModifiedShape (S)));
      }
      if (myGenerated.Extent() == 1 && myGenerated.First().IsSame(S)) {
	      myGenerated.Clear();
      }
    }
  }
  return myGenerated;
}

//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffsetAPI_DraftAngle::ModifiedShape
  (const TopoDS_Shape& S) const
{
  if(S.ShapeType() == TopAbs_VERTEX)
  {
    if(myVtxToReplace.IsBound(S))
    {
      return myVtxToReplace(S);
    }
  }
  if(myVtxToReplace.IsEmpty())
  {
    return myModifier.ModifiedShape(S);
  }
  else
  {
    const TopoDS_Shape& aNS = myModifier.ModifiedShape(S);
    return mySubs.Value(aNS);
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_DraftAngle::Build()
{
  Handle(Draft_Modification)::DownCast (myModification)->Perform();
  if (!Handle(Draft_Modification)::DownCast (myModification)->IsDone()) {
    NotDone();
  }
  else {
    DoModif(myInitialShape);
    CorrectWires();
    CorrectVertexTol();
  }
}

//=======================================================================
//function : CorrectWires
//purpose  :
//=======================================================================

void BRepOffsetAPI_DraftAngle::CorrectWires()
{
  Standard_Real TolInter = 1.e-7;
  Standard_Integer i, j, k;

  TopTools_SequenceOfShape Eseq;
  TopTools_SequenceOfShape Wseq;
  TopTools_SequenceOfShape Fseq;
  TopoDS_Shape CurEdge, CurWire, CurFace;
  TopoDS_Iterator wit, eit;

  TopExp_Explorer fexp( myShape, TopAbs_FACE );
  for (; fexp.More(); fexp.Next())
  {
    CurFace = fexp.Current();
    wit.Initialize( CurFace );
    for (; wit.More(); wit.Next())
    {
      CurWire = wit.Value();
      TopTools_MapOfShape emap;
      eit.Initialize( CurWire );
      for (; eit.More(); eit.Next())
        emap.Add( eit.Value() );
      TopTools_MapIteratorOfMapOfShape mapit( emap );
      for (; mapit.More(); mapit.Next())
      {
        CurEdge = mapit.Key();
        if (BRepTools::IsReallyClosed( TopoDS::Edge(CurEdge), TopoDS::Face(CurFace) ))
        {
          Eseq.Append( CurEdge );
          Wseq.Append( CurWire );
          Fseq.Append( CurFace );
        }
      }
    }
  }

  BRepFill_DataMapOfShapeSequenceOfReal Emap;

  TopTools_SequenceOfShape NonSeam;
  TopTools_SequenceOfShape NonSeamWires;
  BRepOffsetAPI_SequenceOfSequenceOfReal ParsNonSeam;
  BRepOffsetAPI_SequenceOfSequenceOfShape Seam;
  BRepOffsetAPI_SequenceOfSequenceOfReal ParsSeam;

  TopTools_DataMapOfShapeShape WFmap;
  TopTools_DataMapOfShapeListOfShape WWmap;
  for (i = 1; i <= Eseq.Length(); i++)
  {
    CurEdge = Eseq(i);
    CurWire = Wseq(i);
    CurFace = Fseq(i);
    //
    const TopoDS_Face& aFace = TopoDS::Face(CurFace);
    //
    // Prepare 2D adaptors for intersection.
    // The seam edge has two 2D curve, thus we have to create 2 adaptors
    BRepAdaptor_Curve2d aBAC2D1(TopoDS::Edge(CurEdge), aFace);
    BRepAdaptor_Curve2d aBAC2D1R(TopoDS::Edge(CurEdge.Reversed()), aFace);
    // Get surface of the face to get 3D intersection point
    TopLoc_Location aLoc;
    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aLoc);
    // Get the tolerance of the current edge to compare intersection points
    Standard_Real aTolCurE = BRep_Tool::Tolerance(TopoDS::Edge(CurEdge));
    //
    wit.Initialize( CurFace );
    for (; wit.More(); wit.Next())
    {
      TopoDS_Shape aWire = wit.Value();
      if (! aWire.IsSame( CurWire ))
      {
        TColgp_SequenceOfPnt pts;
        Standard_Boolean Wadd = Standard_False;
        eit.Initialize( aWire );
        for (; eit.More(); eit.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(eit.Value());
          //
          // Prepare 2D adaptor for intersection
          BRepAdaptor_Curve2d aBAC2D2(anEdge, aFace);
          // Perform intersection
          Geom2dInt_GInter aGInter;
          aGInter.Perform(aBAC2D1, aBAC2D2, TolInter, TolInter);
          if (!aGInter.IsDone() || aGInter.IsEmpty()) {
            // If first intersection is empty try intersection with reversed edge
            aGInter.Perform(aBAC2D1R, aBAC2D2, TolInter, TolInter);
            if (!aGInter.IsDone() || aGInter.IsEmpty()) {
              continue;
            }
          }
          //
          Wadd = Standard_True;
          if (! WFmap.IsBound( aWire ))
            WFmap.Bind( aWire, CurFace );
          Standard_Integer ind = 0;
          for (j = 1; j <= NonSeam.Length(); j++) {
            if (anEdge.IsSame(NonSeam(j)))
            {
              ind = j;
              break;
            }
          }
          if (ind == 0)
          {
            NonSeam.Append(anEdge);
            NonSeamWires.Append(aWire);
            ind = NonSeam.Length();
            TColStd_SequenceOfReal emptyseq1, emptyseq2;
            TopTools_SequenceOfShape emptyedgeseq;
            ParsNonSeam.Append(emptyseq1);
            Seam.Append(emptyedgeseq);
            ParsSeam.Append(emptyseq2);
          }
          if (!Emap.IsBound(CurEdge))
          {
            TColStd_SequenceOfReal emptyseq;
            Emap.Bind(CurEdge, emptyseq);
          }
          //
          // Get the tolerance of edge to compare intersection points
          Standard_Real aTolE = BRep_Tool::Tolerance(anEdge);
          // Tolerance to compare the intersection points is the maximal
          // tolerance of intersecting edges
          Standard_Real aTolCmp = Max(aTolCurE, aTolE);
          //
          Standard_Integer aNbIntPnt = aGInter.NbPoints();
          for (k = 1; k <= aNbIntPnt; ++k) {
            const IntRes2d_IntersectionPoint& aP2DInt = aGInter.Point(k);
            const gp_Pnt2d& aP2D = aP2DInt.Value();
            gp_Pnt aP3D = aSurf->Value(aP2D.X(), aP2D.Y());
            //
            // Check if the intersection point is new
            Standard_Integer ied = 0;
            for (j = 1; j <= pts.Length(); j++) {
              if (aP3D.IsEqual(pts(j), aTolCmp))
              {
                ied = j;
                break;
              }
            }
            if (ied == 0)
            {
              pts.Append(aP3D);
              Emap(CurEdge).Append(aP2DInt.ParamOnFirst());
              ParsNonSeam(ind).Append(aP2DInt.ParamOnSecond());
              Seam(ind).Append(CurEdge);
              ParsSeam(ind).Append(aP2DInt.ParamOnFirst());
            }
          }
        } //for (; eit.More(); eit.Next())
        if (Wadd)
        {
          if (! WWmap.IsBound( CurWire ))
          {
            TopTools_ListOfShape emptylist;
            WWmap.Bind( CurWire, emptylist );
          }
          WWmap(CurWire).Append( aWire );
        }
      } //if (! aWire.IsSame( CurWire ))
    } //for (; wit.More(); wit.Next())
  } //for (i = 1; i <= Eseq.Length(); i++)

  //Sorting
  for (i = 1; i <= NonSeam.Length(); i++)
  {
    for (j = 1; j < ParsNonSeam(i).Length(); j++)
    {
      for (k = j+1; k <= ParsNonSeam(i).Length(); k++)
      {
        if (ParsNonSeam(i)(k) < ParsNonSeam(i)(j))
        {
          Standard_Real temp = ParsNonSeam(i)(j);
          ParsNonSeam(i)(j) = ParsNonSeam(i)(k);
          ParsNonSeam(i)(k) = temp;
          TopoDS_Shape tmp = Seam(i)(j);
          Seam(i)(j) = Seam(i)(k);
          Seam(i)(k) = tmp;
          temp = ParsSeam(i)(j);
          ParsSeam(i)(j) = ParsSeam(i)(k);
          ParsSeam(i)(k) = temp;
        }
      }
    }
  }
  BRepFill_DataMapIteratorOfDataMapOfShapeSequenceOfReal iter (Emap);
  for (; iter.More (); iter.Next ())
  {
    TColStd_SequenceOfReal Seq = iter.Value ();
    for (i = 1; i < Seq.Length (); i++)
    {
      for (j = i + 1; j <= Seq.Length (); j++)
      {
        if (Seq (j) < Seq (i))
        {
          Standard_Real temp = Seq (i);
          Seq (i) = Seq (j);
          Seq (j) = temp;
        }
      }
    }
    Emap (iter.Key ()) = Seq;
  }
  BRepFill_DataMapOfShapeSequenceOfReal EPmap;
  TopTools_DataMapOfShapeSequenceOfShape EVmap; //Seam
  TopTools_DataMapOfShapeSequenceOfShape EWmap; //Seam and wires intersecting it
  iter.Initialize (Emap);
  for (; iter.More (); iter.Next ())
  {
    TColStd_SequenceOfReal parseq;
    EPmap.Bind (iter.Key (), parseq);
    TopTools_SequenceOfShape shapeseq;
    EVmap.Bind (iter.Key (), shapeseq);
    TopTools_SequenceOfShape shapeseq2;
    EWmap.Bind (iter.Key (), shapeseq2);
  }

  //Reconstruction of non-seam edges
  BRepTools_Substitution aSub;
  BRep_Builder BB;
  for (i = 1; i <= NonSeam.Length (); i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge (NonSeam (i));
    TopTools_ListOfShape NewEdges;
    TopoDS_Edge NewE;
    TopoDS_Vertex Vfirst, Vlast;
    TopExp::Vertices (anEdge, Vfirst, Vlast);
    Standard_Real par, FirstPar, LastPar;
    BRep_Tool::Range (anEdge, FirstPar, LastPar);
    Standard_Integer firstind = 1;
    par = ParsNonSeam (i)(1);
    TopoDS_Edge SeamEdge = TopoDS::Edge (Seam (i)(1));
    //Find the face
    for (j = 1; j <= Eseq.Length (); j++)
      if (SeamEdge.IsSame (Eseq (j)))
        break;
    TopoDS_Face theFace = TopoDS::Face (Fseq (j));
    TopLoc_Location L;
    Handle (Geom_Surface) theSurf = BRep_Tool::Surface (theFace, L);
    if (Abs (par - FirstPar) <= Precision::Confusion ())
    {
      BB.UpdateVertex (Vfirst, ParsSeam (i)(1), SeamEdge, BRep_Tool::Tolerance (Vfirst));
      EPmap (SeamEdge).Append (ParsSeam (i)(1));
      EVmap (SeamEdge).Append (Vfirst);
      EWmap (SeamEdge).Append (NonSeamWires (i));
      firstind = 2;
    }
    Standard_Real prevpar = FirstPar;
    TopoDS_Vertex PrevV = Vfirst;
    for (j = firstind; j <= ParsNonSeam (i).Length (); j++)
    {
      TopoDS_Shape aLocalShape = anEdge.EmptyCopied ();
      NewE = TopoDS::Edge (aLocalShape);
      //NewE = TopoDS::Edge( anEdge.EmptyCopied() );
      TopoDS_Vertex NewV;
      par = ParsNonSeam (i)(j);
      BB.Range (NewE, prevpar, par);
      SeamEdge = TopoDS::Edge (Seam (i)(j));
      if (j == ParsNonSeam (i).Length () && Abs (par - LastPar) <= Precision::Confusion ())
      {
        NewV = Vlast;
        if (firstind == 2 && j == 2)
        {
          BB.UpdateVertex (Vlast, ParsSeam (i)(j), SeamEdge, BRep_Tool::Tolerance (Vlast));
          EPmap (SeamEdge).Append (ParsSeam (i)(j));
          EVmap (SeamEdge).Append (Vlast);
          EWmap (SeamEdge).Append (NonSeamWires (i));
          break;
        }
      }
      else
      {
        BRepAdaptor_Curve bcur (NewE);
        gp_Pnt Point = bcur.Value (par);
        NewV = BRepLib_MakeVertex (Point);
        BB.UpdateVertex (NewV, par, NewE, 10.*Precision::Confusion ());
      }
      BB.UpdateVertex (NewV, ParsSeam (i)(j), SeamEdge, 10.*Precision::Confusion ());
      NewE.Orientation (TopAbs_FORWARD);
      BB.Add (NewE, PrevV.Oriented (TopAbs_FORWARD));
      BB.Add (NewE, NewV.Oriented (TopAbs_REVERSED));

      NewEdges.Append (NewE);
      EPmap (SeamEdge).Append (ParsSeam (i)(j));
      EVmap (SeamEdge).Append (NewV);
      EWmap (SeamEdge).Append (NonSeamWires (i));

      prevpar = par;
      PrevV = NewV;
    }
    //The last edge
    TopoDS_Shape aLocalShape = anEdge.EmptyCopied ();
    NewE = TopoDS::Edge (aLocalShape);
    //NewE = TopoDS::Edge( anEdge.EmptyCopied() );
    par = LastPar;
    if (Abs (prevpar - par) > Precision::Confusion ())
    {
      BB.Range (NewE, prevpar, par);
      NewE.Orientation (TopAbs_FORWARD);
      BB.Add (NewE, PrevV.Oriented (TopAbs_FORWARD));
      BB.Add (NewE, Vlast.Oriented (TopAbs_REVERSED));
      NewEdges.Append (NewE);
    }

    //Substitute anEdge by NewEdges
    aSub.Substitute (anEdge, NewEdges);
  }

  //Sorting of EPmap and EVmap and removing repeating points from them
  iter.Initialize (EPmap);
  for (; iter.More (); iter.Next ())
  {
    TColStd_SequenceOfReal Seq;
    Seq = iter.Value ();
    TopTools_SequenceOfShape SeqShape;
    SeqShape = EVmap (iter.Key ());
    TopTools_SequenceOfShape SeqShape2;
    SeqShape2 = EWmap (iter.Key ());
    for (i = 1; i < Seq.Length (); i++)
    {
      for (j = i + 1; j <= Seq.Length (); j++)
      {
        if (Seq (j) < Seq (i))
        {
          Standard_Real temp = Seq (i);
          Seq (i) = Seq (j);
          Seq (j) = temp;
          TopoDS_Shape tmp = SeqShape (i);
          SeqShape (i) = SeqShape (j);
          SeqShape (j) = tmp;
          tmp = SeqShape2 (i);
          SeqShape2 (i) = SeqShape2 (j);
          SeqShape2 (j) = tmp;
        }
      }
    }

    EPmap (iter.Key ()) = Seq;
    EVmap (iter.Key ()) = SeqShape;
    EWmap (iter.Key ()) = SeqShape2;
  }
  iter.Initialize (EPmap);
  for (; iter.More (); iter.Next ())
  {
    TColStd_SequenceOfReal Seq;
    Seq = iter.Value ();
    TopTools_SequenceOfShape SeqShape;
    SeqShape = EVmap (iter.Key ());
    TopTools_SequenceOfShape SeqShape2;
    SeqShape2 = EWmap (iter.Key ());
    Standard_Boolean remove = Standard_True;
    while (remove)
    {
      remove = Standard_False;
      for (i = 1; i < Seq.Length (); i++)
      {
        if (Abs (Seq (i) - Seq (i + 1)) <= Precision::Confusion ())
        {
          Seq.Remove (i + 1);
          SeqShape.Remove (i + 1);
          SeqShape2.Remove (i + 1);
          remove = Standard_True;
        }
      }
    }
    EPmap (iter.Key ()) = Seq;
    EVmap (iter.Key ()) = SeqShape;
    EWmap (iter.Key ()) = SeqShape2;
  }

  //Reconstruction of seam edges
  TopTools_DataMapOfShapeShape VEmap;
  iter.Initialize (Emap);
  for (; iter.More (); iter.Next ())
  {
    TopoDS_Edge anEdge = TopoDS::Edge (iter.Key ());
    Standard_Boolean onepoint = Standard_False;
    TopTools_ListOfShape NewEdges;
    TColStd_SequenceOfReal Seq;
    Seq = iter.Value ();
    TColStd_SequenceOfReal Seq2;
    Seq2 = EPmap (anEdge);
    TopTools_SequenceOfShape SeqVer;
    SeqVer = EVmap (anEdge);
    TopTools_SequenceOfShape SeqWire;
    SeqWire = EWmap (anEdge);
    TopoDS_Vertex Vfirst, Vlast;
    TopExp::Vertices (anEdge, Vfirst, Vlast);
    Standard_Real fpar, lpar, FirstPar, LastPar;
    BRep_Tool::Range (anEdge, FirstPar, LastPar);
    fpar = FirstPar;
    lpar = Seq (1);
    TopoDS_Edge NewE;
    Standard_Integer firstind = 1;
    if (Abs (fpar - lpar) <= Precision::Confusion ())
    {
      firstind = 2;
      fpar = Seq (1);
      lpar = Seq (2);
    }
    else
    {
      if (Seq.Length () % 2 != 0)
      {
        VEmap.Bind (Vfirst, anEdge);
        firstind = 2;
        fpar = Seq (1);
        if (Seq.Length () > 2)
          lpar = Seq (2);
        else
          onepoint = Standard_True;
      }
    }
    if (!onepoint)
    {
      TopoDS_Shape aLocalShape = anEdge.EmptyCopied ();
      NewE = TopoDS::Edge (aLocalShape);
      //NewE = TopoDS::Edge( anEdge.EmptyCopied() );
      BB.Range (NewE, fpar, lpar);
      NewE.Orientation (TopAbs_FORWARD);
      if (firstind == 1)
      {
        BB.Add (NewE, Vfirst.Oriented (TopAbs_FORWARD));
        aLocalShape = SeqVer (1).Oriented (TopAbs_REVERSED);
        BB.Add (NewE, TopoDS::Vertex (aLocalShape));
        //BB.Add( NewE, TopoDS::Vertex( SeqVer(1).Oriented(TopAbs_REVERSED) ) );
      }
      else
      {
        aLocalShape = SeqVer (1).Oriented (TopAbs_FORWARD);
        BB.Add (NewE, TopoDS::Vertex (aLocalShape));
        aLocalShape = SeqVer (2).Oriented (TopAbs_REVERSED);
        BB.Add (NewE, TopoDS::Vertex (aLocalShape));
        //BB.Add( NewE, TopoDS::Vertex( SeqVer(1).Oriented(TopAbs_FORWARD) ) );
        //BB.Add( NewE, TopoDS::Vertex( SeqVer(2).Oriented(TopAbs_REVERSED) ) );
      }
      NewEdges.Append (NewE);

      firstind++;
      for (i = firstind; i < Seq.Length (); i += 2)
      {
        aLocalShape = anEdge.EmptyCopied ();
        NewE = TopoDS::Edge (aLocalShape);
        //NewE = TopoDS::Edge( anEdge.EmptyCopied() );
        fpar = Seq (i);
        lpar = Seq (i + 1);
        BB.Range (NewE, fpar, lpar);
        //Find vertices
        for (j = 1; j <= Seq2.Length (); j++)
        {
          if (Abs (fpar - Seq2 (j)) <= Precision::Confusion ())
          {
            break;
          }
        }
        NewE.Orientation (TopAbs_FORWARD);
        TopoDS_Shape aLocalShapeCur = SeqVer (j).Oriented (TopAbs_FORWARD);
        BB.Add (NewE, TopoDS::Vertex (aLocalShapeCur));
        aLocalShapeCur = SeqVer (j + 1).Oriented (TopAbs_REVERSED);
        BB.Add (NewE, TopoDS::Vertex (aLocalShapeCur));
        //BB.Add( NewE, TopoDS::Vertex( SeqVer(j).Oriented(TopAbs_FORWARD) ) );
        //BB.Add( NewE, TopoDS::Vertex( SeqVer(j+1).Oriented(TopAbs_REVERSED) ) );
        NewEdges.Append (NewE);
      }
    }

    i = Seq.Length ();
    fpar = Seq (i);
    lpar = LastPar;
    if (Abs (fpar - lpar) <= Precision::Confusion ())
      continue;
    TopoDS_Shape aLocalShape = anEdge.EmptyCopied ();
    NewE = TopoDS::Edge (aLocalShape);
    //NewE = TopoDS::Edge( anEdge.EmptyCopied() );
    BB.Range (NewE, fpar, lpar);
    NewE.Orientation (TopAbs_FORWARD);
    aLocalShape = SeqVer (SeqVer.Length ()).Oriented (TopAbs_FORWARD);
    BB.Add (NewE, TopoDS::Vertex (aLocalShape));
    //BB.Add( NewE, TopoDS::Vertex( SeqVer(SeqVer.Length()).Oriented(TopAbs_FORWARD) ) );
    BB.Add (NewE, Vlast.Oriented (TopAbs_REVERSED));
    NewEdges.Append (NewE);

    //Substitute anEdge by NewEdges
    aSub.Substitute (anEdge, NewEdges);
  }

  //Removing edges connected with missing extremities of seam edges
  TopTools_DataMapIteratorOfDataMapOfShapeShape itve (VEmap);
  for (; itve.More (); itve.Next ())
  {
    TopoDS_Shape V = itve.Key ();
    TopoDS_Shape E = itve.Value ();
    TopoDS_Shape W;
    for (i = 1; i <= Eseq.Length (); i++)
    {
      if (E.IsSame (Eseq (i)))
      {
        W = Wseq (i);
        break;
      }
    }
    TopoDS_Shape Etoremove;
    eit.Initialize (W);
    for (; eit.More (); eit.Next ())
    {
      TopoDS_Edge CurE = TopoDS::Edge (eit.Value ());
      if (CurE.IsSame (E))
        continue;
      TopoDS_Vertex Vfirst, Vlast;
      TopExp::Vertices (CurE, Vfirst, Vlast);
      if (Vfirst.IsSame (V) || Vlast.IsSame (V))
      {
        Etoremove = CurE;
        break;
      }
    }
    if (!Etoremove.IsNull ())
    {
      W.Free (Standard_True);
      BB.Remove (W, Etoremove);
    }
  }

  aSub.Build (myShape);
  if (aSub.IsCopied (myShape))
  {
    const TopTools_ListOfShape& listSh = aSub.Copy (myShape);
    if (!listSh.IsEmpty ())
      myShape = listSh.First ();
  }

  //Reconstruction of wires
  TopTools_ListOfShape theCopy;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itww (WWmap);
  for (; itww.More (); itww.Next ())
  {
    CurWire = itww.Key ();
    theCopy = aSub.Copy (CurWire);
    CurWire = theCopy.First ();
    CurWire.Free (Standard_True);
    TopTools_ListIteratorOfListOfShape itl (itww.Value ());
    for (; itl.More (); itl.Next ())
    {
      TopoDS_Shape aWire = itl.Value ();
      CurFace = WFmap (aWire);
      theCopy = aSub.Copy (aWire);
      aWire = theCopy.First ();
      //Adjusting period
      TopLoc_Location L;
      Handle (Geom_Surface) theSurf = BRep_Tool::Surface (TopoDS::Face (CurFace), L);
      eit.Initialize (aWire);
      for (; eit.More (); eit.Next ())
      {
        TopoDS_Edge anEdge = TopoDS::Edge (eit.Value ());
        gp_Pnt2d Pfirst, Plast, Pmid;
        BRep_Tool::UVPoints (anEdge, TopoDS::Face (CurFace), Pfirst, Plast);
        BRepAdaptor_Curve2d bc2d (anEdge, TopoDS::Face (CurFace));
        Pmid = bc2d.Value ((bc2d.FirstParameter () + bc2d.LastParameter ()) / 2.);
        gp_Vec2d offset;
        Standard_Boolean translate = Standard_False;
        if (Pfirst.X () - 2.*M_PI > Precision::Confusion () ||
          Plast.X () - 2.*M_PI > Precision::Confusion () ||
          Pmid.X () - 2.*M_PI > Precision::Confusion ())
        {
          offset.SetCoord (-2.*M_PI, 0);
          translate = Standard_True;
        }
        if (Pfirst.X () < -Precision::Confusion () ||
          Plast.X () < -Precision::Confusion () ||
          Pmid.X () < -Precision::Confusion ())
        {
          offset.SetCoord (2.*M_PI, 0);
          translate = Standard_True;
        }
        if (translate)
        {
          const Handle (BRep_TEdge)& TE = *((Handle (BRep_TEdge)*) &anEdge.TShape ());
          BRep_ListIteratorOfListOfCurveRepresentation itcr (TE->ChangeCurves ());
          Handle (BRep_GCurve) GC;

          for (; itcr.More (); itcr.Next ())
          {
            GC = Handle (BRep_GCurve)::DownCast (itcr.Value ());
            if (!GC.IsNull () && GC->IsCurveOnSurface (theSurf, L))
            {
              Handle (Geom2d_Curve) PC = GC->PCurve ();
              PC = Handle (Geom2d_Curve)::DownCast (PC->Translated (offset));
              GC->PCurve (PC);
              TE->ChangeCurves ().Remove (itcr);
              TE->ChangeCurves ().Append (GC);
              break;
            }
          }
        }
      }
      ///////////////////
      eit.Initialize (aWire, Standard_False);
      for (; eit.More (); eit.Next ())
      {
        TopoDS_Shape anEdge = eit.Value ();
        BB.Add (CurWire, anEdge);
      }
      if (aSub.IsCopied (CurFace))
      {
        theCopy = aSub.Copy (CurFace);
        CurFace = theCopy.First ();
      }
      CurFace.Free (Standard_True);
      BB.Remove (CurFace, aWire);
    }
  }
}
//=======================================================================
//function : CorrectVertexTol
//purpose  : 
//=======================================================================

void BRepOffsetAPI_DraftAngle::CorrectVertexTol()
{
  TopTools_MapOfShape anInitVertices, anInitEdges, aNewEdges;
  TopExp_Explorer anExp(myInitialShape, TopAbs_EDGE);
  for(; anExp.More(); anExp.Next())
  {
    anInitEdges.Add(anExp.Current());
    TopoDS_Iterator anIter(anExp.Current());
    for(; anIter.More(); anIter.Next())
    {
      anInitVertices.Add(anIter.Value());
    }
  }
  //

  BRep_Builder aBB;
  myVtxToReplace.Clear();
  anExp.Init(myShape, TopAbs_EDGE);
  for(; anExp.More(); anExp.Next())
  {
    const TopoDS_Shape& anE = anExp.Current();
    //Skip old (not modified) edges
    if(anInitEdges.Contains(anE))
      continue;
    //
    //Skip processed edges
    if(aNewEdges.Contains(anE))
      continue;
    //
    aNewEdges.Add(anE);
    //
    Standard_Real anETol = BRep_Tool::Tolerance(TopoDS::Edge(anE));
    TopoDS_Iterator anIter(anE);
    for(; anIter.More(); anIter.Next())
    {
      const TopoDS_Vertex& aVtx = TopoDS::Vertex(anIter.Value());
      if(anInitVertices.Contains(aVtx))
      {
        if(myVtxToReplace.IsBound(aVtx))
        {
          aBB.UpdateVertex(TopoDS::Vertex(myVtxToReplace(aVtx)), anETol + Epsilon(anETol));
        }
        else
        {
          Standard_Real aVTol = BRep_Tool::Tolerance(aVtx);
          if(aVTol < anETol)
          {
            TopoDS_Vertex aNewVtx;
            gp_Pnt aVPnt = BRep_Tool::Pnt(aVtx);
            aBB.MakeVertex(aNewVtx, aVPnt,anETol + Epsilon(anETol));
            aNewVtx.Orientation(aVtx.Orientation());
            myVtxToReplace.Bind(aVtx, aNewVtx);
          }
        }
      }
      else
      {
        aBB.UpdateVertex(aVtx, anETol + Epsilon(anETol));
      }
    }
  }
  //
  if(myVtxToReplace.IsEmpty())
  {
    return;
  }
  //
  mySubs.Clear();
  TopTools_DataMapIteratorOfDataMapOfShapeShape anIter(myVtxToReplace);
  for(; anIter.More(); anIter.Next())
  {
    mySubs.Replace(anIter.Key(), anIter.Value());
  }
  mySubs.Apply( myShape );
  myShape = mySubs.Value(myShape);
  //
}
