// Created on: 1994-06-07
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepFill_Pipe.ixx>

#include <Standard_ErrorHandler.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTools_Substitution.hxx>

#include <GeomFill_CorrectedFrenet.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_DiscreteTrihedron.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>

#include <BRepFill_SectionPlacement.hxx>
#include <BRepFill_ShapeLaw.hxx>
#include <BRepFill_Edge3DLaw.hxx>
#include <BRepFill_Sweep.hxx>

#include <GeomAbs_Shape.hxx>
#include <TopExp.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>

#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#ifdef DRAW
#include <DBRep.hxx>
static Standard_Boolean Affich = 0;
#endif

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
		   M(3,1), M(3,2), M(3,3), V.Z(),
		   1.e-12, 1.e-14);

  fila.Multiply(myTrsf);
  TopLoc_Location LocFirst(fila);
  myFirst = myProfile;
  if ( ! LocFirst.IsIdentity()) {
    myFirst.Location( LocFirst.Multiplied(myProfile.Location()) );
  }

  myLoc->Law(myLoc->NbLaw())->GetDomain(first, last);
  myLoc->Law(myLoc->NbLaw())->D0(last,M, V);
//    try { // Not good, but there are no other means to test SetValues
  fila.SetValues(M(1,1), M(1,2), M(1,3), V.X(),
		 M(2,1), M(2,2), M(2,3), V.Y(),
		 M(3,1), M(3,2), M(3,3), V.Z(),
		 1.e-12, 1.e-14);
  fila.Multiply(myTrsf);
  TopLoc_Location LocLast(fila);
  if (! myLoc->IsClosed() || LocFirst != LocLast) {
    myLast = myProfile;
    if ( ! LocLast.IsIdentity()) {
      myLast.Location(LocLast.Multiplied(myProfile.Location()) );
    }
  }
  else {
    myLast = myFirst;
  }
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

  if (!iprof) Standard_DomainError::Raise(
              "BRepFill_Pipe::Face : Edge not in the Profile");


  // *************************************************
  // Search if ESpine  is an edge of mySpine and find 
  // the index of the corresponding Filler
  // *************************************************
 for (ii=1; ii<=myLoc->NbLaw() && (!ispin); ii++) 
    if  (ESpine.IsSame(myLoc->Edge(ii))) ispin = ii;

  if (!ispin) Standard_DomainError::Raise(
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
  if (!iprof) Standard_DomainError::Raise(
	"BRepFill_Pipe::Edge : Vertex not in the Profile");


  // *************************************************
  // Search if ESpine  is an edge of mySpine and find 
  // the index of the corresponding Filler
  // *************************************************
   
  for (ii=1; ii<=myLoc->NbLaw() && (!ispin); ii++) 
    if  (ESpine.IsSame(myLoc->Edge(ii))) ispin = ii;

  if (!ispin) Standard_DomainError::Raise(
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

  if (!ispin) Standard_DomainError::Raise(
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

TopoDS_Wire BRepFill_Pipe::PipeLine(const gp_Pnt& Point) const
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
 MkSw.Build( BRepFill_Modified, myContinuity, GeomFill_Location, myDegmax, mySegmax );
 TopoDS_Shape aLocalShape = MkSw.Shape();
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
      W.Closed(S.Closed());
      TheS = W;
      if (!FirstShape.IsNull()) {
	B.MakeWire(W);
	B.Add(W, FirstShape);
	W.Closed(FirstShape.Closed());
	TheFirst = W;
      }
      if (!LastShape.IsNull()) {
	B.MakeWire(W);
	B.Add(W, LastShape);
	W.Closed(LastShape.Closed());
	TheLast = W;
      }
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
    Standard_DomainError::Raise("BRepFill_Pipe::SOLID or COMPSOLID");
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
      MkSw.Build( BRepFill_Modified, myContinuity, GeomFill_Location, myDegmax, mySegmax );
      result = MkSw.Shape();
    }

    if (TheS.ShapeType() == TopAbs_WIRE ) {
      Handle(BRepFill_ShapeLaw) Section = 
	new (BRepFill_ShapeLaw) (TopoDS::Wire(TheS));
      BRepFill_Sweep MkSw(Section, myLoc, Standard_True);
      MkSw.SetBounds(TopoDS::Wire(TheFirst), 
		     TopoDS::Wire(TheLast));
      MkSw.SetForceApproxC1(myForceApproxC1);
      MkSw.Build( BRepFill_Modified, myContinuity, GeomFill_Location, myDegmax, mySegmax );
      result = MkSw.Shape();

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
        const Standard_Integer aNbFaces    = myFaces->ColLength();
        const Standard_Integer aNbEdges    = myEdges->ColLength();
        const Standard_Integer aNbSections = mySections->ColLength();

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

        // Perform sharing faces
        result = ShareFaces(result, aNbFaces, aNbEdges, aNbSections);
      }
    }
  }
      
  if ( TheS.ShapeType() == TopAbs_FACE ) {
    Standard_Integer ii, jj;
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
      Standard_Integer ii = InitialLength+1;
      Handle(BRepFill_ShapeLaw) Section = 
	new (BRepFill_ShapeLaw) (TopoDS::Wire(S), Standard_False);
      InitialLength += Section->NbLaw();
     
      for (; (ii<=InitialLength) && (!result); ii++) {
	if (E.IsSame(Section->Edge(ii)) ) result = ii;
      }
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
    Standard_DomainError::Raise("BRepFill_Pipe::SOLID or COMPSOLID");
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
    Standard_DomainError::Raise("BRepFill_Pipe::SOLID or COMPSOLID");
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
	    C = (*((Handle(Geom_TrimmedCurve)*)&C))->BasisCurve();
	  if (C->DynamicType() == STANDARD_TYPE(Geom_OffsetCurve))
	    C = (*((Handle(Geom_OffsetCurve)*)&C))->BasisCurve();
	}
      if (C->DynamicType() == STANDARD_TYPE(Geom_BSplineCurve))
	{
	  const Handle(Geom_BSplineCurve)& BC = *((Handle(Geom_BSplineCurve)*)&C);
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
//function : ShareFaces
//purpose  : 
//=======================================================================

TopoDS_Shape BRepFill_Pipe::ShareFaces
                (const TopoDS_Shape &theShape,
                 const Standard_Integer theInitialFacesLen,
                 const Standard_Integer theInitialEdgesLen,
                 const Standard_Integer theInitialSectionsLen)
{
  TopoDS_Shape aResult = theShape;
  // Check if there are shapes to be shared.
  TopTools_DataMapOfShapeInteger aMapBndEdgeIndex;
  TColStd_DataMapOfIntegerInteger aMapNewOldFIndex;
  TColStd_DataMapOfIntegerInteger aMapNewOldEIndex;
  TopTools_MapOfShape aMapUsedVtx;
  TopExp_Explorer anExp;
  Standard_Integer i;
  Standard_Integer ii;
  Standard_Integer jj;
  BRep_Builder aBuilder;

  // Check the first and last J index of myFaces.
  for (i = 1; i <= 2; i++) {
    // Compute jj index of faces.
    if (i == 1) {
      jj = 1;
    } else {
      jj = myFaces->RowLength();

      if (jj == 1) {
        break;
      }
    }

    // Fill the map of boundary edges on initial faces.
    for (ii = 1; ii <= theInitialFacesLen; ii++) {
      anExp.Init(myFaces->Value(ii, jj), TopAbs_EDGE);
  
      for (; anExp.More(); anExp.Next()) {
        aMapBndEdgeIndex.Bind(anExp.Current(), ii);
      }
    }

    // Check if edges of newly created faces are shared with old ones.
    for (ii = theInitialFacesLen + 1; ii <= myFaces->ColLength(); ii++) {
      anExp.Init(myFaces->Value(ii, jj), TopAbs_EDGE);
  
      for (; anExp.More(); anExp.Next()) {
        if (aMapBndEdgeIndex.IsBound(anExp.Current())) {
          // This row should be replaced.
          Standard_Integer anOldIndex = aMapBndEdgeIndex.Find(anExp.Current());

          aMapNewOldFIndex.Bind(ii, anOldIndex);

          // Find corresponding new and old edges indices.
          TopoDS_Vertex aV[2];
          TopExp::Vertices(TopoDS::Edge(anExp.Current()), aV[0], aV[1]);
          Standard_Integer ie;

          // Compute jj index of edges.
          Standard_Integer je = (i == 1 ? 1 : myEdges->RowLength());

          for (Standard_Integer j = 0; j < 2; j++) {
            if (aMapUsedVtx.Contains(aV[j])) {
              // This vertex is treated.
              continue;
            }
  
            // Find old index.
            Standard_Integer iEOld = -1;
            TopoDS_Vertex aVE[2];
       
            for (ie = 1; ie <= theInitialEdgesLen; ie++) {
              const TopoDS_Shape &anEdge = myEdges->Value(ie, je);
       
              TopExp::Vertices(TopoDS::Edge(anEdge), aVE[0], aVE[1]);
       
              if (aV[j].IsSame(aVE[0]) || aV[j].IsSame(aVE[1])) {
                iEOld = ie;
                break;
              }
            }
       
            if (iEOld > 0) {
              // Find new index.
              for (ie = theInitialEdgesLen+1; ie <= myEdges->ColLength(); ie++) {
                const TopoDS_Shape &anEdge = myEdges->Value(ie, je);
       
                TopExp::Vertices(TopoDS::Edge(anEdge), aVE[0], aVE[1]);
       
                if (aV[j].IsSame(aVE[0]) || aV[j].IsSame(aVE[1])) {
                  // This row should be replaced.
                  aMapNewOldEIndex.Bind(ie, iEOld);
                  aMapUsedVtx.Add(aV[j]);
                  break;
                }
              }
            }
          }

          break;
        }
      }
    }
  }

  if (!aMapNewOldFIndex.IsEmpty()) {
    TColStd_DataMapIteratorOfDataMapOfIntegerInteger anIter(aMapNewOldFIndex);
    TopTools_ListOfShape   aListShape;
    BRepTools_Substitution aSubstitute;

    for (; anIter.More(); anIter.Next()) {
      const Standard_Integer aNewIndex  = anIter.Key();
      const Standard_Integer anOldIndex = anIter.Value();

      // Change new faces by old ones.
      for (jj = 1; jj <= myFaces->RowLength(); jj++) {
        const TopoDS_Shape &aNewFace  = myFaces->Value(aNewIndex, jj);
        const TopoDS_Shape &anOldFace = myFaces->Value(anOldIndex, jj);

        if (!aSubstitute.IsCopied(aNewFace)) {
          aListShape.Append(anOldFace.Oriented(TopAbs_REVERSED));
          aSubstitute.Substitute(aNewFace, aListShape);
          aListShape.Clear();
        }
      }
    }

    // Change new edges by old ones.
    for (anIter.Initialize(aMapNewOldEIndex); anIter.More(); anIter.Next()) {
      const Standard_Integer aNewIndex  = anIter.Key();
      const Standard_Integer anOldIndex = anIter.Value();

      for (jj = 1; jj <= myEdges->RowLength(); jj++) {
        const TopoDS_Shape &aNewEdge  = myEdges->Value(aNewIndex, jj);
        const TopoDS_Shape &anOldEdge = myEdges->Value(anOldIndex, jj);

        if (!aSubstitute.IsCopied(aNewEdge)) {
          aListShape.Append(anOldEdge.Oriented(TopAbs_FORWARD));
          aSubstitute.Substitute(aNewEdge, aListShape);
          aListShape.Clear();

          // Change new vertices by old ones.
          TopoDS_Iterator aNewIt(aNewEdge);
          TopoDS_Iterator anOldIt(anOldEdge);

          for (; aNewIt.More() && anOldIt.More();
                                  aNewIt.Next(), anOldIt.Next()) {
            if (!aNewIt.Value().IsSame(anOldIt.Value())) {
              if (!aSubstitute.IsCopied(aNewIt.Value())) {
                aListShape.Append(anOldIt.Value().Oriented(TopAbs_FORWARD));
                aSubstitute.Substitute(aNewIt.Value(), aListShape);
                aListShape.Clear();
              }
            }
          }
        }
      }
    }

    // Perform substitution.
    aSubstitute.Build(aResult);

    if (aSubstitute.IsCopied(aResult)) {
      // Get copied shape.
      const TopTools_ListOfShape& listSh = aSubstitute.Copy(aResult);

      aResult = listSh.First();

      // Update original faces with copied ones.
      for (ii = theInitialFacesLen + 1; ii <= myFaces->ColLength(); ii++) {
        for (jj = 1; jj <= myFaces->RowLength(); jj++) {
          TopoDS_Shape anOldFace = myFaces->Value(ii, jj); // Copy

          if (aSubstitute.IsCopied(anOldFace)) {
            const TopTools_ListOfShape& aList = aSubstitute.Copy(anOldFace);

            if(!aList.IsEmpty()) {
              // Store copied face.
              const TopoDS_Shape &aCopyFace = aList.First();
              TopAbs_Orientation anOri = anOldFace.Orientation();
              const Standard_Boolean isShared = aMapNewOldFIndex.IsBound(ii);

              if (isShared) {
                // Reverse the orientation for shared face.
                anOri = TopAbs::Reverse(anOri);
              }

              myFaces->SetValue(ii, jj, aCopyFace.Oriented(anOri));

              // Check if it is necessary to update PCurves on this face.
              if (!isShared) {
                TopoDS_Face anOldF = TopoDS::Face(anOldFace);
                TopoDS_Face aCopyF = TopoDS::Face(aCopyFace);

                anOldF.Orientation(TopAbs_FORWARD);
                anExp.Init(anOldF, TopAbs_EDGE);

                for (; anExp.More(); anExp.Next()) {
                  const TopoDS_Shape &anOldEdge = anExp.Current();

                  if (aSubstitute.IsCopied(anOldEdge)) {
                    const TopTools_ListOfShape& aListE =
                      aSubstitute.Copy(anOldEdge);

                    if(!aListE.IsEmpty()) {
                      // This edge is copied. Check if there is a PCurve
                      // on the face.
                      TopoDS_Edge aCopyE = TopoDS::Edge(aListE.First());
                      Standard_Real aFirst;
                      Standard_Real aLast;
                      Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface
                          (aCopyE, aCopyF, aFirst, aLast);

                      if (aPCurve.IsNull()) {
                        // There is no pcurve copy it from the old edge.
                        TopoDS_Edge anOldE = TopoDS::Edge(anOldEdge);

                        aPCurve = BRep_Tool::CurveOnSurface
                          (anOldE, anOldF, aFirst, aLast);

                        if (aPCurve.IsNull() == Standard_False) {
                          // Update the shared edge with PCurve from new Face.
                          Standard_Real aTol = Max(BRep_Tool::Tolerance(anOldE),
                               BRep_Tool::Tolerance(aCopyE));

                          aBuilder.UpdateEdge(aCopyE, aPCurve, aCopyF, aTol);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
	}
      }

      // Update new edges with shared ones.
      for (ii = theInitialEdgesLen + 1; ii <= myEdges->ColLength(); ii++) {
        for (jj = 1; jj <= myEdges->RowLength(); jj++) {
          const TopoDS_Shape &aLocalShape = myEdges->Value(ii, jj);

          if (aSubstitute.IsCopied(aLocalShape)) {
            const TopTools_ListOfShape& aList = aSubstitute.Copy(aLocalShape);

            if(!aList.IsEmpty()) {
              const TopAbs_Orientation anOri = TopAbs_FORWARD;

              myEdges->SetValue(ii, jj, aList.First().Oriented(anOri));
            }
          }
        }
      }

      // Update new sections with shared ones.
      for (ii = theInitialSectionsLen+1; ii <= mySections->ColLength(); ii++) {
        for (jj = 1; jj <= mySections->RowLength(); jj++) {
          const TopoDS_Shape &aLocalShape = mySections->Value(ii, jj);

          if (aSubstitute.IsCopied(aLocalShape)) {
            const TopTools_ListOfShape& aList = aSubstitute.Copy(aLocalShape);

            if(!aList.IsEmpty()) {
              const TopAbs_Orientation anOri = TopAbs_FORWARD;

              mySections->SetValue(ii, jj, aList.First().Oriented(anOri));
            }
          }
        }
      }
    }
  }

  return aResult;
}
