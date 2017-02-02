// Created on: 1994-06-07
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepFill_Edge3DLaw.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepFill_Pipe.hxx>
#include <BRepFill_SectionPlacement.hxx>
#include <BRepFill_ShapeLaw.hxx>
#include <BRepFill_Sweep.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTools_Substitution.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_CorrectedFrenet.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>
#include <GeomFill_DiscreteTrihedron.hxx>
#include <GeomFill_Frenet.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeUpgrade_RemoveLocations.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#ifdef DRAW
#include <DBRep.hxx>
static Standard_Boolean Affich = 0;
#endif

// ---------------------------------------------------------------------------------
// static function: UpdateMap
// purpose:
// ---------------------------------------------------------------------------------
static Standard_Boolean UpdateMap(const TopoDS_Shape&                 theKey,
                                  const TopoDS_Shape&                 theValue,
                                  TopTools_DataMapOfShapeListOfShape& theMap)
{
  if(!theMap.IsBound(theKey))
  {
    TopTools_ListOfShape thelist;
    theMap.Bind(theKey, thelist);
  }
  TopTools_ListOfShape& aList = theMap.ChangeFind(theKey);
  TopTools_ListIteratorOfListOfShape anIt(aList);
  Standard_Boolean found = Standard_False;

  for(; anIt.More(); anIt.Next())
  {
    if(theValue.IsSame(anIt.Value()))
    {
      found = Standard_True;
      break;
    }
  }

  if(!found)
    aList.Append(theValue);
  
  return !found;
}

static void ReverseModifiedEdges(TopoDS_Shape& aShape,
                                 TopTools_MapOfShape& Emap)
{
  TopExp_Explorer Explo(aShape, TopAbs_FACE);
  BRep_Builder BB;
  
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Shape aFace = Explo.Current();
    TopoDS_Iterator itf(aFace);
    for (; itf.More(); itf.Next())
    {
      TopoDS_Shape aWire = itf.Value();
      TopTools_ListOfShape Ledges;
      TopoDS_Iterator itw(aWire);
      for (; itw.More(); itw.Next())
        Ledges.Append(itw.Value());

      aWire.Free(Standard_True);
      TopTools_ListIteratorOfListOfShape itl(Ledges);
      for (; itl.More(); itl.Next())
        BB.Remove(aWire, itl.Value());
        
      for (itl.Initialize(Ledges); itl.More(); itl.Next())
      {
        TopoDS_Shape anEdge = itl.Value();
        if (Emap.Contains(anEdge))
          anEdge.Reverse();
        BB.Add(aWire, anEdge);
      }
    }
  }
}

static void UpdateTolFromTopOrBottomPCurve(const TopoDS_Face& aFace,
                                           TopoDS_Edge& anEdge)
{
  Standard_Real fpar, lpar;
  Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, fpar, lpar);
  if (aPCurve.IsNull())
    return;

  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
  if (aCurve.IsNull())
    return;

  Handle(Geom2dAdaptor_HCurve) GAHC2d = new Geom2dAdaptor_HCurve(aPCurve, fpar, lpar);
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(aSurf);
  Adaptor3d_CurveOnSurface ConS(GAHC2d, GAHS);

  Standard_Real Tol = BRep_Tool::Tolerance(anEdge);
  Standard_Real InitTol = Tol;
  Standard_Real TolTol = Tol*Tol;
  const Standard_Integer NCONTROL = 22;
  Standard_Real delta = (lpar - fpar)/NCONTROL;
  for (Standard_Integer i = 0; i <= NCONTROL; i++)
  {
    Standard_Real par = fpar + i*delta;
    gp_Pnt pnt = aCurve->Value(par);
    gp_Pnt prj = ConS.Value(par);
    Standard_Real sqdist = pnt.SquareDistance(prj);
    if (sqdist > TolTol)
      TolTol = sqdist;
  }
  Tol = 1.00005 * Sqrt(TolTol);
  if (Tol >= InitTol)
  {
    BRep_Builder BB;
    BB.UpdateEdge(anEdge, Tol);
    TopoDS_Iterator itv(anEdge);
    for (; itv.More(); itv.Next())
    {
      TopoDS_Vertex aVertex = TopoDS::Vertex(itv.Value());
      BB.UpdateVertex(aVertex, Tol);
    }
  }
}

//=======================================================================
//function : BRepFill_Pipe
//purpose  : 
//=======================================================================

BRepFill_Pipe::BRepFill_Pipe()
{
  myDegmax = 11;
  mySegmax = 100;
  myContinuity = GeomAbs_C2;
  myMode = GeomFill_IsCorrectedFrenet;
  myForceApproxC1 = Standard_False;

  myCurIndexOfSectionEdge = 1;
}


//=======================================================================
//function : BRepFill_Pipe
//purpose  : 
//=======================================================================

BRepFill_Pipe::BRepFill_Pipe(const TopoDS_Wire&  Spine,
			     const TopoDS_Shape& Profile,
                             const GeomFill_Trihedron aMode,
                             const Standard_Boolean ForceApproxC1,
			     const Standard_Boolean KPart)
                             
{
  myDegmax = 11;
  mySegmax = 100;
  
  myMode = GeomFill_IsCorrectedFrenet;
  if (aMode == GeomFill_IsFrenet ||
      aMode == GeomFill_IsCorrectedFrenet ||
      aMode == GeomFill_IsDiscreteTrihedron)
    myMode = aMode;

  myContinuity = GeomAbs_C2;
  if (myMode == GeomFill_IsDiscreteTrihedron)
    myContinuity = GeomAbs_C0;
  
  myForceApproxC1 = ForceApproxC1;

  myCurIndexOfSectionEdge = 1;
  
  Perform(Spine, Profile, KPart);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_Pipe::Perform(const TopoDS_Wire&  Spine,
			    const TopoDS_Shape& Profile,
			    const Standard_Boolean /*KPart*/)

{
  mySections.Nullify();
  myFaces.Nullify();
  myEdges.Nullify();

  mySpine   = Spine;
  myProfile = Profile;

  DefineRealSegmax();

  BRepTools_WireExplorer wexp;
  TopoDS_Shape TheProf; 

  Handle(GeomFill_TrihedronLaw) TLaw;
  switch (myMode)
  {
  case GeomFill_IsFrenet:
    TLaw = new GeomFill_Frenet();
    break;
  case GeomFill_IsCorrectedFrenet:
    TLaw = new GeomFill_CorrectedFrenet();
    break;
  case GeomFill_IsDiscreteTrihedron:
    TLaw = new GeomFill_DiscreteTrihedron();
    break;
  default:
    break;
  }
  Handle(GeomFill_CurveAndTrihedron) Loc = 
    new (GeomFill_CurveAndTrihedron) (TLaw);
  myLoc = new (BRepFill_Edge3DLaw) (mySpine, Loc);
  if (myLoc->NbLaw() == 0) {
    return; // Degenerated case
  }
  myLoc->TransformInG0Law(); // Set into continuity
    
  BRepFill_SectionPlacement Place(myLoc, Profile);
  myTrsf = Place.Transformation();

  TopLoc_Location Loc2(myTrsf), Loc1;
  Loc1 = Profile.Location();
  TopoDS_Shape aux;
  TheProf =  myProfile;
  TheProf.Location(Loc2.Multiplied(Loc1));
 
  // Construct First && Last Shape
  Handle(GeomFill_LocationLaw) law;

  gp_Mat M;
  gp_Vec V;
  gp_Trsf fila;
  Standard_Real first, last;
  myLoc->Law(1)->GetDomain(first, last);
  myLoc->Law(1)->D0(first, M, V);
    fila.SetValues(M(1,1), M(1,2), M(1,3), V.X(),
		   M(2,1), M(2,2), M(2,3), V.Y(),
		   M(3,1), M(3,2), M(3,3), V.Z());

  fila.Multiply(myTrsf);
  TopLoc_Location LocFirst(fila);
  myFirst = myProfile;
  if ( ! LocFirst.IsIdentity()) {
    //myFirst.Location( LocFirst.Multiplied(myProfile.Location()) );
    myFirst = BRepBuilderAPI_Transform(myProfile, fila, Standard_True); //copy
  }

  ShapeUpgrade_RemoveLocations RemLoc;
  RemLoc.SetRemoveLevel(TopAbs_COMPOUND);
  RemLoc.Remove(myFirst);
  myFirst = RemLoc.GetResult();
  
  myLoc->Law(myLoc->NbLaw())->GetDomain(first, last);
  myLoc->Law(myLoc->NbLaw())->D0(last,M, V);
//    try { // Not good, but there are no other means to test SetValues
  fila.SetValues(M(1,1), M(1,2), M(1,3), V.X(),
		 M(2,1), M(2,2), M(2,3), V.Y(),
		 M(3,1), M(3,2), M(3,3), V.Z());
  fila.Multiply(myTrsf);
  TopLoc_Location LocLast(fila);
  if (! myLoc->IsClosed() || LocFirst != LocLast) {
    myLast = myProfile;
    if ( ! LocLast.IsIdentity()) {
      //myLast.Location(LocLast.Multiplied(myProfile.Location()) );
      myLast = BRepBuilderAPI_Transform(myProfile, fila, Standard_True); //copy
    }
  }
  else {
    myLast = myFirst;
  }

  RemLoc.Remove(myLast);
  myLast = RemLoc.GetResult();
  
#if DRAW
  if (Affich) {
    DBRep::Set("theprof",  TheProf);
    DBRep::Set("thefirst", myFirst);
    DBRep::Set("thelast" , myLast);
 }
#endif
 
  myShape = MakeShape(TheProf, myFirst, myLast);
}


//=======================================================================
//function : Spine
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_Pipe::Spine() const 
{
  return mySpine;
}

//=======================================================================
//function : Profile
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_Pipe::Profile() const 
{
  return myProfile;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_Pipe::Shape() const 
{
  return myShape;
}

//=======================================================================
//function : ErrorOnSurface
//purpose  : 
//=======================================================================

Standard_Real BRepFill_Pipe::ErrorOnSurface() const 
{
  return myErrorOnSurf;
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_Pipe::FirstShape() const 
{
  return myFirst;
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_Pipe::LastShape() const 
{
  return myLast;
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
void BRepFill_Pipe::Generated(const TopoDS_Shape&   theShape,
                              TopTools_ListOfShape& theList) 
{
  theList.Clear();

  if (theShape.IsSame(myProfile))
    theList.Append(myShape);
  else
  {
    if (theShape.ShapeType() == TopAbs_FACE ||
        theShape.ShapeType() == TopAbs_WIRE)
    {
      if(myGenMap.IsBound(theShape))
        theList = myGenMap.Find(theShape);
    }
    else if (theShape.ShapeType() == TopAbs_EDGE)
    {
      TopoDS_Iterator itw(mySpine);
      for (; itw.More(); itw.Next())
      {
        const TopoDS_Edge& aSpineEdge = TopoDS::Edge(itw.Value());
        const TopoDS_Shape& aFace = Face(aSpineEdge, TopoDS::Edge(theShape));
        theList.Append(aFace);
      }
    }
    else if (theShape.ShapeType() == TopAbs_VERTEX)
    {
      TopoDS_Iterator itw(mySpine);
      for (; itw.More(); itw.Next())
      {
        const TopoDS_Edge& aSpineEdge = TopoDS::Edge(itw.Value());
        const TopoDS_Shape& anEdge = Edge(aSpineEdge, TopoDS::Vertex(theShape));
        theList.Append(anEdge);
      }
    }
  }
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

TopoDS_Face BRepFill_Pipe::Face(const TopoDS_Edge& ESpine,
				const TopoDS_Edge& EProfile)
{
  TopoDS_Face theFace;

  if ( BRep_Tool::Degenerated(EProfile))
    return theFace;

  Standard_Integer ii, ispin = 0, iprof = 0, count = 0;

  // *************************************************
  // Search if EProfile is an edge of myProfile
  // *************************************************
  iprof = FindEdge(myProfile, EProfile, count);

  if (!iprof) throw Standard_DomainError(
              "BRepFill_Pipe::Face : Edge not in the Profile");


  // *************************************************
  // Search if ESpine  is an edge of mySpine and find 
  // the index of the corresponding Filler
  // *************************************************
 for (ii=1; ii<=myLoc->NbLaw() && (!ispin); ii++) 
    if  (ESpine.IsSame(myLoc->Edge(ii))) ispin = ii;

  if (!ispin) throw Standard_DomainError(
    "BRepFill_Pipe::Edge  : Edge not in the Spine");

  theFace = TopoDS::Face(myFaces->Value(iprof, ispin));
  return theFace;

}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
TopoDS_Edge BRepFill_Pipe::Edge(const TopoDS_Edge&   ESpine,
				const TopoDS_Vertex& VProfile)
{
  Standard_Integer ii, ispin = 0, iprof = 0, count = 0;;

  // *************************************************
  // Search if VProfile is a Vertex of myProfile
  // *************************************************
  iprof = FindVertex(myProfile, VProfile, count);
  if (!iprof) throw Standard_DomainError(
	"BRepFill_Pipe::Edge : Vertex not in the Profile");


  // *************************************************
  // Search if ESpine  is an edge of mySpine and find 
  // the index of the corresponding Filler
  // *************************************************
   
  for (ii=1; ii<=myLoc->NbLaw() && (!ispin); ii++) 
    if  (ESpine.IsSame(myLoc->Edge(ii))) ispin = ii;

  if (!ispin) throw Standard_DomainError(
    "BRepFill_Pipe::Edge  : Edge not in the Spine");


  // *************************************************
  // Generate the corresponding Shape
  // *************************************************  
  TopoDS_Edge theEdge;
  theEdge = TopoDS::Edge(myEdges->Value(iprof, ispin));

  return theEdge;

}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

TopoDS_Shape BRepFill_Pipe::Section(const TopoDS_Vertex& VSpine) const 
{
  TopoDS_Iterator it, itv;

  Standard_Integer ii, ispin = 0;

  TopoDS_Shape curSect = myProfile;

  // *************************************************
  // Search if ESpine  is an edge of mySpine and find 
  // the index of the corresponding Filler
  // *************************************************

  // iterate on all the edges of mySpine
 for (ii=1; ii<=myLoc->NbLaw()+1 && (!ispin); ii++) 
    if  (VSpine.IsSame(myLoc->Vertex(ii))) ispin = ii;

  if (!ispin) throw Standard_DomainError(
    "BRepFill_Pipe::Section  : Vertex not in the Spine");

  BRep_Builder B;
  TopoDS_Compound Comp; 
  B.MakeCompound(Comp);
  for (ii=1; ii<=mySections->ColLength(); ii++)
    B.Add(Comp, mySections->Value(ii, ispin));
  
  return Comp;
}

//=======================================================================
//function : PipeLine
//purpose  : Construct a wire by sweeping of a point
//=======================================================================

TopoDS_Wire BRepFill_Pipe::PipeLine(const gp_Pnt& Point)
{
 // Postioning 
 gp_Pnt P;
 P = Point;
 P.Transform(myTrsf);

 BRepLib_MakeVertex MkV(P); 
 Handle(BRepFill_ShapeLaw) Section = 
	new (BRepFill_ShapeLaw) (MkV.Vertex());

 // Sweeping
 BRepFill_Sweep MkSw(Section, myLoc, Standard_True);
 MkSw.SetForceApproxC1(myForceApproxC1);
 MkSw.Build( myReversedEdges, myTapes, myRails,
             BRepFill_Modified, myContinuity, GeomFill_Location, myDegmax, mySegmax );
 TopoDS_Shape aLocalShape = MkSw.Shape();
 myErrorOnSurf = MkSw.ErrorOnSurface();
 return TopoDS::Wire(aLocalShape);
// return TopoDS::Wire(MkSw.Shape());
}

//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepFill_Pipe::MakeShape(const TopoDS_Shape& S,
				      const TopoDS_Shape& FirstShape,
				      const TopoDS_Shape& LastShape)
{
  TopoDS_Shape result;
  BRep_Builder B;
  Standard_Boolean explode = Standard_False; 
  TopoDS_Shape TheS, TheFirst, TheLast;
  Standard_Integer InitialLength = 0;
  TheS = S;
  TheFirst = FirstShape;
  TheLast = LastShape;
  if (! myFaces.IsNull()) InitialLength = myFaces->ColLength();

  // there are two kinds of generation
  //  1. generate with S from each Filler (Vertex, Edge)
  //  2. call MakeShape recursively on the subshapes of S
  //
  // explode is True in the second case

  // create the result empty

  switch (S.ShapeType()) {

  case TopAbs_VERTEX :
    {
      B.MakeWire(TopoDS::Wire(result));
      break;
    }
	  
  case TopAbs_EDGE :
    {
      TopoDS_Wire W;
      B.MakeShell(TopoDS::Shell(result));
      B.MakeWire(W);
      B.Add(W, S);
      W.Closed(BRep_Tool::IsClosed(S));
      TheS = W;
      if (!FirstShape.IsNull()) {
	B.MakeWire(W);
	B.Add(W, FirstShape);
	W.Closed(BRep_Tool::IsClosed(FirstShape));
	TheFirst = W;
      }
      if (!LastShape.IsNull()) {
	B.MakeWire(W);
	B.Add(W, LastShape);
	W.Closed(BRep_Tool::IsClosed(LastShape));
	TheLast = W;
      }
      result.Closed (BRep_Tool::IsClosed (result));
      break;
    }
	  
  case TopAbs_WIRE :
    B.MakeShell(TopoDS::Shell(result));
    break;

  case TopAbs_FACE :
    {
      B.MakeShell(TopoDS::Shell(result));
      explode = Standard_True;
      if ( !mySpine.Closed() && !TheFirst.IsNull()) {
	 B.Add(result, TheFirst.Reversed());
      }
      result.Closed (BRep_Tool::IsClosed (result));
      break;
    }

  case TopAbs_SHELL :
    {
      B.MakeCompSolid(TopoDS::CompSolid(result));
      explode = Standard_True;
      break;
    }

  case TopAbs_SOLID :
  case TopAbs_COMPSOLID :
    throw Standard_DomainError("BRepFill_Pipe::profile contains solids");
    break;

  case TopAbs_COMPOUND :
    {
      B.MakeCompound(TopoDS::Compound(result));
      explode = Standard_True;
      break;
    }
  default:
    break;
  }

  if (explode) {
    // add the subshapes
    TopoDS_Iterator itFirst, itLast;
    TopoDS_Shape first, last;
    if (!TheFirst.IsNull()) itFirst.Initialize(TheFirst);
    if (!TheLast.IsNull()) itLast.Initialize(TheLast);

    for (TopoDS_Iterator it(S); it.More(); it.Next()) {
      if (!TheFirst.IsNull()) first = itFirst.Value();
      if (!TheLast.IsNull())  last = itLast.Value();
      if (TheS.ShapeType() == TopAbs_FACE ) 
	MakeShape(it.Value(), first, last);
      else
	B.Add(result,MakeShape(it.Value(), first, last));

      if (!TheFirst.IsNull()) itFirst.Next();
      if (!TheLast.IsNull())  itLast.Next();
    }
  }
  
  else {
    if (TheS.ShapeType() == TopAbs_VERTEX ) {
     Handle(BRepFill_ShapeLaw) Section = 
	new (BRepFill_ShapeLaw) (TopoDS::Vertex(TheS));
      BRepFill_Sweep MkSw(Section, myLoc, Standard_True);
      MkSw.SetForceApproxC1(myForceApproxC1);
      MkSw.Build( myReversedEdges, myTapes, myRails,
                  BRepFill_Modified, myContinuity, GeomFill_Location, myDegmax, mySegmax );
      result = MkSw.Shape();
      UpdateMap(TheS.Located(myProfile.Location()), result, myGenMap);
      myErrorOnSurf = MkSw.ErrorOnSurface();

      Handle(TopTools_HArray2OfShape) aSections = MkSw.Sections();

      if (aSections.IsNull() == Standard_False) {
        const Standard_Integer aVLast = aSections->UpperCol();

        myFirst = aSections->Value(1, 1);
        myLast  = aSections->Value(1, aVLast);
      }
    }

    if (TheS.ShapeType() == TopAbs_WIRE ) {
      Handle(BRepFill_ShapeLaw) Section = 
	new (BRepFill_ShapeLaw) (TopoDS::Wire(TheS));
      BRepFill_Sweep MkSw(Section, myLoc, Standard_True);
      MkSw.SetBounds(TopoDS::Wire(TheFirst), 
		     TopoDS::Wire(TheLast));
      MkSw.SetForceApproxC1(myForceApproxC1);
      MkSw.Build( myReversedEdges, myTapes, myRails,
                  BRepFill_Modified, myContinuity, GeomFill_Location, myDegmax, mySegmax );
      result = MkSw.Shape();
      UpdateMap(TheS.Located(myProfile.Location()), result, myGenMap);
      myErrorOnSurf = MkSw.ErrorOnSurface();
      //Correct <myFirst> and <myLast>
      ReverseModifiedEdges(myFirst, myReversedEdges);
      ReverseModifiedEdges(myLast, myReversedEdges);

      // Labeling of elements
      if (mySections.IsNull()) {
	myFaces    = MkSw.SubShape();
	mySections = MkSw.Sections();
	myEdges    = MkSw.InterFaces();
      }
      else {
	Handle(TopTools_HArray2OfShape) Aux, Somme;
	Standard_Integer length;
        Standard_Integer ii, jj, kk;

	Aux = MkSw.SubShape();
	length = Aux->ColLength() + myFaces->ColLength(); 
	Somme = new (TopTools_HArray2OfShape) (1, length, 1, 
					       Aux->RowLength());
	for (jj=1; jj<=myFaces->RowLength(); jj++) {
	  for (ii=1; ii<=myFaces->ColLength(); ii++)
	    Somme->SetValue(ii, jj, myFaces->Value(ii, jj));
 
	  for (kk=1, ii=myFaces->ColLength()+1; 
	       kk <=Aux->ColLength(); kk++, ii++)
	    Somme->SetValue(ii, jj, Aux->Value(kk, jj));
	}
	myFaces = Somme;    

	Aux = MkSw.Sections();
	length = Aux->ColLength() + mySections->ColLength(); 
	Somme = new (TopTools_HArray2OfShape) (1, length, 1, 
					       Aux->RowLength());
	for (jj=1; jj<=mySections->RowLength(); jj++) {
	  for (ii=1; ii<=mySections->ColLength(); ii++)
	    Somme->SetValue(ii, jj, mySections->Value(ii, jj));

          myCurIndexOfSectionEdge = mySections->ColLength()+1;
          
	  for (kk=1, ii=mySections->ColLength()+1; 
	       kk <=Aux->ColLength(); kk++, ii++)
	    Somme->SetValue(ii, jj, Aux->Value(kk, jj));   
	}
	mySections = Somme;

	Aux = MkSw.InterFaces();
	length = Aux->ColLength() + myEdges->ColLength(); 
	Somme = new (TopTools_HArray2OfShape) (1, length, 1, 
					       Aux->RowLength());
	for (jj=1; jj<=myEdges->RowLength(); jj++) {
	  for (ii=1; ii<=myEdges->ColLength(); ii++)
	    Somme->SetValue(ii, jj, myEdges->Value(ii, jj));
 
	  for (kk=1, ii=myEdges->ColLength()+1; 
	       kk <=Aux->ColLength(); kk++, ii++)
	    Somme->SetValue(ii, jj, Aux->Value(kk, jj));   
	}

	myEdges = Somme;
      }
    }
  }
      
  if ( TheS.ShapeType() == TopAbs_FACE ) {
    Standard_Integer ii, jj;
    //jgv
    TopExp_Explorer Explo(result, TopAbs_FACE);
    for (; Explo.More(); Explo.Next())
    {
      TopoDS_Shape aFace = Explo.Current();
      RebuildTopOrBottomFace(aFace.Reversed(), Standard_True); //top face was reversed
    }
    /////
    TopoDS_Face F;
    for (ii=InitialLength+1; ii<=myFaces->ColLength(); ii++) {
      for (jj=1; jj<=myFaces->RowLength(); jj++) {
	F = TopoDS::Face(myFaces->Value(ii, jj));
	if (!F.IsNull()) B.Add(result, F);
      }
    }

    if ( !mySpine.Closed()) {
      // if Spine is not closed 
      // add the last face of the solid

      //jgv
      RebuildTopOrBottomFace(TheLast, Standard_False); //bottom face
      /////
      B.Add(result, TopoDS::Face(TheLast));
    }

    TopoDS_Solid solid;
    BRep_Builder BS;
    BS.MakeSolid(solid);

    result.Closed(Standard_True);
    BS.Add(solid,TopoDS::Shell(result));

    BRepClass3d_SolidClassifier SC(solid);
    SC.PerformInfinitePoint(Precision::Confusion());
    if ( SC.State() == TopAbs_IN) {
      BS.MakeSolid(solid);
      TopoDS_Shape aLocalShape = result.Reversed();
      BS.Add(solid,TopoDS::Shell(aLocalShape));
//      BS.Add(solid,TopoDS::Shell(result.Reversed()));
    }
    UpdateMap(TheS.Located(myProfile.Location()), solid, myGenMap);
    return solid;
  }
  else {
    return result;
  }
}

//============================================================================
//function : FindEdge
//purpose  : Find the number of edge corresponding to the edge of the profile.
//============================================================================

Standard_Integer BRepFill_Pipe::FindEdge(const TopoDS_Shape& S,
					 const TopoDS_Edge& E,
					 Standard_Integer& InitialLength) const
{
  Standard_Integer result = 0;

  switch (S.ShapeType()) {
	  
  case TopAbs_EDGE :
    {
      InitialLength++;
      if (S.IsSame(E)) result = InitialLength;
      break;
    }
	  
  case TopAbs_WIRE :
    {
      Handle(BRepFill_ShapeLaw) Section = 
        new (BRepFill_ShapeLaw) (TopoDS::Wire(S), Standard_False);
      Standard_Integer NbLaw = Section->NbLaw();

      for (Standard_Integer ii = 1; (ii<=NbLaw) && (!result); ii++) {
        if (E.IsSame(Section->Edge(ii)) ) result = InitialLength + ii;
      }
      InitialLength += NbLaw;
      break;
    }

  case TopAbs_FACE :
  case TopAbs_SHELL :
  case TopAbs_COMPOUND :
    {
     for (TopoDS_Iterator it(S); it.More() && (!result); it.Next())
       result = FindEdge(it.Value(), E, InitialLength );
     break;
    }

  case TopAbs_SOLID :
  case TopAbs_COMPSOLID :
    throw Standard_DomainError("BRepFill_Pipe::SOLID or COMPSOLID");
    break;
  default:
    break;
  }

  return result; 
}

//=======================================================================
//function : FindVertex
//purpose  : Find the number of edge corresponding to an edge of the profile.
//=======================================================================

Standard_Integer BRepFill_Pipe::FindVertex(const TopoDS_Shape& S,
					   const TopoDS_Vertex& V,
					   Standard_Integer& InitialLength) const
{
  Standard_Integer result = 0;

  switch (S.ShapeType()) {
  case TopAbs_VERTEX :
    {
      InitialLength++;
      if (S.IsSame(V)) result = InitialLength;
      break;
    }
	  
  case TopAbs_EDGE :
    {
      TopoDS_Vertex VF, VL;
      TopExp::Vertices(TopoDS::Edge(S), VF, VL);
      if (S.Orientation() == TopAbs_REVERSED) {
	TopoDS_Vertex aux;
	aux = VF; VF = VL; VL = aux;
      }
      if (VF.IsSame(V)) result = InitialLength+1;
      else if (VL.IsSame(V)) result = InitialLength+2;
      InitialLength += 2;
      break;
    }
	  
  case TopAbs_WIRE :
    {
      Standard_Integer ii = InitialLength+1;
      Handle(BRepFill_ShapeLaw) Section = 
	new (BRepFill_ShapeLaw) (TopoDS::Wire(S), Standard_False);
      InitialLength += Section->NbLaw()+1;
     
      for (; (ii<=InitialLength) && (!result); ii++) {
	if (V.IsSame(Section->Vertex(ii, 0.)) ) result = ii;
      }
      break;
    }

  case TopAbs_FACE :
  case TopAbs_SHELL :
  case TopAbs_COMPOUND :
    {
     for (TopoDS_Iterator it(S); it.More() && (!result); it.Next())
       result = FindVertex(it.Value(), V, InitialLength);
     break;
    }

  case TopAbs_SOLID :
  case TopAbs_COMPSOLID :
    throw Standard_DomainError("BRepFill_Pipe::SOLID or COMPSOLID");
    break;
  default:
    break;
  }

  return result; 
}

//=======================================================================
//function : DefineRealSegmax
//purpose  : Defines the real number of segments
//           required in the case of bspline spine
//=======================================================================

void BRepFill_Pipe::DefineRealSegmax()
{
  Standard_Integer RealSegmax = 0;

  TopoDS_Iterator iter(mySpine);
  for (; iter.More(); iter.Next())
    {
      TopoDS_Edge E = TopoDS::Edge(iter.Value());
      Standard_Real first, last;
      Handle(Geom_Curve) C = BRep_Tool::Curve( E, first, last );
      if (C.IsNull())
	continue;
      while (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve) ||
	     C->DynamicType() == STANDARD_TYPE(Geom_OffsetCurve))
	{
	  if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
	    C = Handle(Geom_TrimmedCurve)::DownCast (C)->BasisCurve();
	  if (C->DynamicType() == STANDARD_TYPE(Geom_OffsetCurve))
	    C = Handle(Geom_OffsetCurve)::DownCast (C)->BasisCurve();
	}
      if (C->DynamicType() == STANDARD_TYPE(Geom_BSplineCurve))
	{
	  Handle(Geom_BSplineCurve) BC (Handle(Geom_BSplineCurve)::DownCast (C));
	  Standard_Integer NbKnots = BC->NbKnots();
	  Standard_Integer RealNbKnots = NbKnots;
	  if (first > BC->FirstParameter())
	    {
	      Standard_Integer I1, I2;
	      BC->LocateU( first, Precision::PConfusion(), I1, I2 );
	      RealNbKnots -= I1-1;
	    }
	  if (last < BC->LastParameter())
	    {
	      Standard_Integer I1, I2;
	      BC->LocateU( last, Precision::PConfusion(), I1, I2 );
	      RealNbKnots -= NbKnots-I2;
	    }
	  RealSegmax += RealNbKnots-1;
	}
    }

  if (mySegmax < RealSegmax)
    mySegmax = RealSegmax;
}

//=======================================================================
//function : RebuildTopOrBottomFace
//purpose  : Correct orientation of v-iso edges
//           according to new 3d and 2d curves taken from swept surfaces
//=======================================================================

void BRepFill_Pipe::RebuildTopOrBottomFace(const TopoDS_Shape& aFace,
                                           const Standard_Boolean IsTop) const
{
  Standard_Integer IndexOfSection =
    (IsTop)? 1 : mySections->RowLength();

  Standard_Integer ii;
  BRep_Builder BB;
  TopoDS_Iterator itf(aFace);
  for (; itf.More(); itf.Next())
  {
    TopoDS_Shape aWire = itf.Value();
    TopTools_SequenceOfShape InitEdges;
    TopTools_SequenceOfShape ResEdges;
    TopoDS_Iterator itw(aWire);
    for (; itw.More(); itw.Next())
    {
      TopoDS_Shape anEdge = itw.Value();
      for (ii = myCurIndexOfSectionEdge; ii <= mySections->ColLength(); ii++)
      {
        TopoDS_Shape aVisoEdge = mySections->Value(ii, IndexOfSection);
        if (anEdge.IsSame(aVisoEdge))
        {
          InitEdges.Append(anEdge);
          ResEdges.Append(aVisoEdge);
          break;
        }
      }
    }
    aWire.Free(Standard_True);
    for (ii = 1; ii <= InitEdges.Length(); ii++)
    {
      BB.Remove(aWire, InitEdges(ii));
      UpdateTolFromTopOrBottomPCurve(TopoDS::Face(aFace), TopoDS::Edge(ResEdges(ii)));
      BB.Add(aWire, ResEdges(ii));
    }
  }
}
