// Created on: 1994-10-04
// Created by: Yves FRICAUD
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepLib.hxx>
#include <BRepMAT2d_Explorer.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCE2d_MakeArcOfCircle.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAPI.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>
#include <MAT2d_SequenceOfSequenceOfCurve.hxx>
#include <Precision.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>

//
//  Modified by Sergey KHROMOV - Thu Dec  5 10:38:14 2002 Begin
static TopoDS_Edge MakeEdge(const Handle(Geom2d_Curve) &theCurve,
  const TopoDS_Face          &theFace,
  const TopoDS_Vertex        &theVFirst,
  const TopoDS_Vertex        &theVLast);
//  Modified by Sergey KHROMOV - Thu Dec  5 10:38:16 2002 End
//
static GeomAbs_CurveType GetCurveType(const Handle(Geom2d_Curve)& theC2d);
static Handle(Geom2d_TrimmedCurve) AdjustCurveEnd (const Handle(Geom2d_BoundedCurve)& theC2d, 
                                                   const gp_Pnt2d theP, const Standard_Boolean isFirst);
//
//=======================================================================
//function : BRepMAT2d_Explorer
//purpose  : 
//=======================================================================

BRepMAT2d_Explorer::BRepMAT2d_Explorer()
{
  Clear();
}

//=======================================================================
//function : BRepMAT2d_Explorer
//purpose  : 
//=======================================================================

BRepMAT2d_Explorer::BRepMAT2d_Explorer(const TopoDS_Face& aFace)
{
  Perform (aFace);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::Perform(const TopoDS_Face& aFace)
{
  Clear();
  myShape        = aFace;
  TopoDS_Face  F = TopoDS::Face(aFace);
  F.Orientation(TopAbs_FORWARD);
  TopExp_Explorer Exp (F,TopAbs_WIRE);
  //  Modified by Sergey KHROMOV - Tue Nov 26 16:10:37 2002 Begin
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);
  TopoDS_Face          aNewF = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());

  while (Exp.More()) {
    Add (TopoDS::Wire (Exp.Current()),F, aNewF);
    Exp.Next();
  }

  BRepLib::BuildCurves3d(aNewF);

  myModifShapes.Add(aFace, aNewF);
  //   CheckConnection();
  //  Modified by Sergey KHROMOV - Tue Nov 26 16:10:38 2002 End
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::Add(const TopoDS_Wire& Spine,
  const TopoDS_Face& aFace,
  TopoDS_Face& aNewFace)
{  
  //  Modified by Sergey KHROMOV - Tue Nov 26 14:25:46 2002 Begin
  // This method is totally rewroted to include check
  // of connection and creation of a new spine.
  NewContour();
  myIsClosed(currentContour) = (Spine.Closed()) ? Standard_True : Standard_False;

  //  Modified by skv - Wed Jun 23 12:23:01 2004 Integration Begin
  //  Taking into account side of bisecting loci construction.
  //   TopoDS_Wire                         aWFwd = TopoDS::Wire(Spine.Oriented(TopAbs_FORWARD));
  //   BRepTools_WireExplorer              anExp(aWFwd, aFace);
  BRepTools_WireExplorer              anExp(Spine, aFace);
  //  Modified by skv - Wed Jun 23 12:23:02 2004 Integration End
  TopTools_IndexedDataMapOfShapeShape anOldNewE;

  if (!anExp.More())
    return;

  TopoDS_Edge                 aFirstEdge = anExp.Current();
  TopoDS_Edge                 aPrevEdge = aFirstEdge;
  Standard_Real               UFirst,ULast, aD;
  Handle(Geom2d_Curve)        C2d;
  Handle(Geom2d_TrimmedCurve) CT2d;
  Handle(Geom2d_TrimmedCurve) aFirstCurve;
  gp_Pnt2d                    aPFirst;
  gp_Pnt2d                    aPLast;
  gp_Pnt2d                    aPCurFirst;
  //  Modified by skv - Mon Jul 11 19:00:25 2005 Integration Begin
  //  Set the confusion tolerance in accordance with the further algo
  //   Standard_Real               aTolConf   = Precision::Confusion();
  Standard_Real               aTolConf   = 1.e-8;
  //  Modified by skv - Mon Jul 11 19:00:25 2005 Integration End
  Standard_Boolean            isModif    = Standard_False;

  // Treatment of the first edge of a wire.
  anOldNewE.Add(aFirstEdge, aFirstEdge);
  C2d  = BRep_Tool::CurveOnSurface (aFirstEdge, aFace, UFirst, ULast);
  CT2d = new Geom2d_TrimmedCurve(C2d,UFirst,ULast);

  if (aFirstEdge.Orientation() == TopAbs_REVERSED)
    CT2d->Reverse();

  aPFirst = CT2d->Value(CT2d->FirstParameter());
  aPLast  = CT2d->Value(CT2d->LastParameter());

  Add(CT2d);
  aFirstCurve = CT2d;
  anExp.Next();

  // Treatment of the next edges:
  for (; anExp.More(); anExp.Next()) {
    TopoDS_Edge  anEdge = anExp.Current();

    anOldNewE.Add(anEdge, anEdge);
    C2d  = BRep_Tool::CurveOnSurface (anEdge, aFace, UFirst, ULast);
    CT2d = new Geom2d_TrimmedCurve(C2d,UFirst,ULast);

    if (anEdge.Orientation() == TopAbs_REVERSED)
      CT2d->Reverse();

    aPCurFirst = CT2d->Value(CT2d->FirstParameter());
    //
    aD=aPLast.Distance(aPCurFirst);
    if (aD > aTolConf) {
      // There are two ways how to fill holes:
      //     First,  to create a line between these two points.
      //     Second, create a BSpline curve and to add the last point of the previous
      //             curve as the first pole of the current one. Second method which
      //             is worse was performed before and leaved here. Otherwise too much
      //             code should be rewritten.
      isModif = Standard_True;
      //
      Standard_Integer aNbC = theCurves.Value(currentContour).Length();
      Handle(Geom2d_BoundedCurve) CPrev = 
        Handle(Geom2d_BoundedCurve)::DownCast(theCurves.ChangeValue(currentContour).ChangeValue(aNbC));
      //
      GeomAbs_CurveType TCPrev = GetCurveType(CPrev);
      GeomAbs_CurveType TCCurr = GetCurveType(CT2d);
      //
      if(TCCurr <= TCPrev)
      {
        CT2d = AdjustCurveEnd (CT2d, aPLast, Standard_True);
        // Creation of new edge.
        TopoDS_Edge aNewEdge;
        TopoDS_Vertex aVf = TopExp::FirstVertex(anEdge);
        TopoDS_Vertex aVl = TopExp::LastVertex(anEdge);

        if (anEdge.Orientation() == TopAbs_FORWARD)
          aNewEdge = MakeEdge(CT2d, aNewFace, aVf, aVl);
        else 
          aNewEdge = MakeEdge(CT2d->Reversed(), aNewFace, aVf, aVl);

        aNewEdge.Orientation(anEdge.Orientation());

        anOldNewE.ChangeFromKey(anEdge) = aNewEdge;
      }
      else
      {
        gp_Pnt2d aP = CT2d->Value(CT2d->FirstParameter());
        CPrev = AdjustCurveEnd(CPrev, aP, Standard_False);
        theCurves.ChangeValue(currentContour).ChangeValue(aNbC) = CPrev;
        //Change previous edge
        TopoDS_Edge aNewEdge;
        TopoDS_Vertex aVf = TopExp::FirstVertex(aPrevEdge);
        TopoDS_Vertex aVl = TopExp::LastVertex(aPrevEdge);

        if (aPrevEdge.Orientation() == TopAbs_FORWARD)
          aNewEdge = MakeEdge(CPrev, aNewFace, aVf, aVl);
        else 
          aNewEdge = MakeEdge(CPrev->Reversed(), aNewFace, aVf, aVl);

        aNewEdge.Orientation(aPrevEdge.Orientation());

        anOldNewE.ChangeFromKey(aPrevEdge) = aNewEdge;
        
      }

    }

    aPLast = CT2d->Value(CT2d->LastParameter());
    Add(CT2d);
    aPrevEdge = anEdge;
  }

  // Check of the distance between the first and the last point of wire
  // if the wire is closed.
  if (myIsClosed(currentContour) && aPLast.Distance(aPFirst) > aTolConf) {
    isModif = Standard_True;

      //
    Standard_Integer aNbC = theCurves.Value(currentContour).Length();
    Handle(Geom2d_BoundedCurve) CPrev = 
        Handle(Geom2d_BoundedCurve)::DownCast(theCurves.ChangeValue(currentContour).ChangeValue(aNbC));
      //
    GeomAbs_CurveType TCPrev = GetCurveType(CPrev);
    GeomAbs_CurveType TCCurr = GetCurveType(aFirstCurve);
    //
    if(TCCurr <= TCPrev)
    {
      aFirstCurve = AdjustCurveEnd(aFirstCurve, aPLast, Standard_True);
      theCurves.ChangeValue(currentContour).ChangeValue(1) = aFirstCurve;
      // Creation of new edge.
      TopoDS_Edge aNewEdge;
      TopoDS_Vertex aVf = TopExp::FirstVertex(aFirstEdge);
      TopoDS_Vertex aVl = TopExp::LastVertex(aFirstEdge);

      if (aFirstEdge.Orientation() == TopAbs_FORWARD)
        aNewEdge = MakeEdge(aFirstCurve, aNewFace, aVf, aVl);
      else 
        aNewEdge = MakeEdge(aFirstCurve->Reversed(), aNewFace, aVf, aVl);

      aNewEdge.Orientation(aFirstEdge.Orientation());

      anOldNewE.ChangeFromKey(aFirstEdge) = aNewEdge;
    }
    else
    {
      gp_Pnt2d aP = aFirstCurve->Value(aFirstCurve->FirstParameter());
      CPrev = AdjustCurveEnd(CPrev, aP, Standard_False);
      theCurves.ChangeValue(currentContour).ChangeValue(aNbC) = CPrev;
      //Change previous edge
      TopoDS_Edge aNewEdge;
      TopoDS_Vertex aVf = TopExp::FirstVertex(aPrevEdge);
      TopoDS_Vertex aVl = TopExp::LastVertex(aPrevEdge);

      if (aPrevEdge.Orientation() == TopAbs_FORWARD)
        aNewEdge = MakeEdge(CPrev, aNewFace, aVf, aVl);
      else 
        aNewEdge = MakeEdge(CPrev->Reversed(), aNewFace, aVf, aVl);

      aNewEdge.Orientation(aPrevEdge.Orientation());

      anOldNewE.ChangeFromKey(aPrevEdge) = aNewEdge;

    }

  }

  TopoDS_Wire  aNewWire;
  BRep_Builder aBuilder;

  if (isModif) {
    Standard_Integer i;
    Standard_Integer aNbEdges = anOldNewE.Extent();

    aBuilder.MakeWire(aNewWire);

    for (i = 1; i <= aNbEdges; i++) {
      const TopoDS_Shape &aKey     = anOldNewE.FindKey(i);
      const TopoDS_Shape &aNewEdge = anOldNewE.FindFromIndex(i);

      aBuilder.Add(aNewWire, aNewEdge);
      myModifShapes.Add(aKey, aNewEdge);
    }

    if (myIsClosed(currentContour))
      aNewWire.Closed(Standard_True);

    //  Modified by skv - Fri Nov 12 17:22:12 2004 Integration Begin
    //  The orientation of wire is already taken into account.
    //    aNewWire.Orientation(Spine.Orientation());
    //  Modified by skv - Fri Nov 12 17:22:12 2004 Integration End
    myModifShapes.Add(Spine, aNewWire);
  } else
    aNewWire = Spine;

  aBuilder.Add(aNewFace, aNewWire);
  //  Modified by Sergey KHROMOV - Tue Nov 26 14:25:53 2002 End
}

//=======================================================================
//function : CheckConnection
//purpose  : 
//=======================================================================

//  Modified by Sergey KHROMOV - Tue Nov 26 17:21:44 2002 Begin
// void BRepMAT2d_Explorer::CheckConnection()
// {
//   for (Standard_Integer i = 1; i <= theCurves.Length(); i++)
//     for (Standard_Integer j = 2; j <= theCurves(i).Length(); j++)
//       {
// 	gp_Pnt2d P1 = theCurves(i)(j-1)->Value( theCurves(i)(j-1)->LastParameter() );
// 	gp_Pnt2d P2 = theCurves(i)(j)->Value( theCurves(i)(j)->FirstParameter() );
// 	if (P1.Distance( P2 ) > Precision::Confusion())
// 	  {
// 	    Handle( Geom2d_BSplineCurve ) BCurve;
// 	    if (theCurves(i)(j)->DynamicType() != STANDARD_TYPE(Geom2d_BSplineCurve))
// 	      BCurve = Geom2dConvert::CurveToBSplineCurve( theCurves(i)(j) );
// 	    else
// 	      BCurve = Handle( Geom2d_BSplineCurve )::DownCast( theCurves(i)(j) );
// 	    BCurve->SetPole( 1, P1 );
// 	    theCurves(i)(j) = new Geom2d_TrimmedCurve( BCurve, BCurve->FirstParameter(), BCurve->LastParameter() );
// 	  }
//       }
// }
//  Modified by Sergey KHROMOV - Tue Nov 26 17:21:29 2002 End

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::Clear()
{  
  theCurves.Clear() ;
  currentContour = 0;
  //  Modified by Sergey KHROMOV - Wed Mar  6 16:07:55 2002 Begin
  myIsClosed.Clear();
  myModifShapes.Clear();
  //  Modified by Sergey KHROMOV - Wed Mar  6 16:07:55 2002 End
}


//=======================================================================
//function : NewContour
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::NewContour()
{  
  TColGeom2d_SequenceOfCurve Contour;
  theCurves.Append(Contour);
  //  Modified by Sergey KHROMOV - Wed Mar  6 16:12:05 2002 Begin
  myIsClosed.Append(Standard_False);
  //  Modified by Sergey KHROMOV - Wed Mar  6 16:12:05 2002 End
  currentContour ++ ;
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::Add(const Handle(Geom2d_Curve)& aCurve)
{  
  theCurves.ChangeValue(currentContour).Append(aCurve);
}

//=======================================================================
//function : NumberOfContours
//purpose  : 
//=======================================================================

Standard_Integer BRepMAT2d_Explorer::NumberOfContours() const 
{  
  return theCurves.Length() ;
}


//=======================================================================
//function : NumberOfCurves
//purpose  : 
//=======================================================================

Standard_Integer BRepMAT2d_Explorer::NumberOfCurves
  (const Standard_Integer IndexContour)
  const 
{  
  return theCurves.Value(IndexContour).Length();
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::Init(const Standard_Integer IndexContour)
{  
  currentContour = IndexContour;
  current        = 1;
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean BRepMAT2d_Explorer::More() const 
{  
  return (current <= NumberOfCurves(currentContour));
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void BRepMAT2d_Explorer::Next()
{ 
  current++;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) BRepMAT2d_Explorer::Value() const 
{  
  return theCurves.Value(currentContour).Value(current);
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepMAT2d_Explorer::Shape() const
{
  return myShape;
}


//=======================================================================
//function : Contour
//purpose  : 
//=======================================================================

const TColGeom2d_SequenceOfCurve& BRepMAT2d_Explorer::Contour
  (const Standard_Integer IC)
  const
{
  return theCurves.Value(IC);
}


//  Modified by Sergey KHROMOV - Wed Mar  6 17:40:07 2002 Begin
//=======================================================================
//function : IsModified
//purpose  : 
//=======================================================================

Standard_Boolean BRepMAT2d_Explorer::IsModified
  (const TopoDS_Shape &aShape) const
{
  if (myModifShapes.Contains(aShape)) {
    const TopoDS_Shape     &aNewShape = myModifShapes.FindFromKey(aShape);
    const Standard_Boolean  isSame    = aNewShape.IsSame(aShape);

    return !isSame;
  }

  return Standard_False;
}

//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepMAT2d_Explorer::ModifiedShape
  (const TopoDS_Shape &aShape) const
{
  if (myModifShapes.Contains(aShape)) {
    const TopoDS_Shape &aNewShape = myModifShapes.FindFromKey(aShape);

    return aNewShape;
  }

  return aShape;
}

//=======================================================================
//function : GetIsClosed
//purpose  : 
//=======================================================================

const TColStd_SequenceOfBoolean &BRepMAT2d_Explorer::GetIsClosed() const
{
  return myIsClosed;
}

//=======================================================================
//function : MakeEdge
//purpose  : Creation of an edge by 2d curve, face and two vertices.
//=======================================================================

TopoDS_Edge MakeEdge(const Handle(Geom2d_Curve) &theCurve,
  const TopoDS_Face          &theFace,
  const TopoDS_Vertex        &theVFirst,
  const TopoDS_Vertex        &theVLast)
{
  TopoDS_Edge   aNewEdge;
  BRep_Builder  aBuilder;
  Standard_Real aTol  = Precision::Confusion();
  Standard_Real aFPar = theCurve->FirstParameter();
  Standard_Real aLPar = theCurve->LastParameter();

  aBuilder.MakeEdge(aNewEdge);
  aBuilder.UpdateEdge(aNewEdge, theCurve, theFace, aTol);
  aBuilder.Add(aNewEdge, theVFirst.Oriented(TopAbs_FORWARD));
  aBuilder.Add(aNewEdge, theVLast.Oriented(TopAbs_REVERSED));
  aBuilder.Range(aNewEdge, aFPar, aLPar);

  return aNewEdge;
}
//  Modified by Sergey KHROMOV - Wed Mar  6 17:40:14 2002 End
//
//=======================================================================
//function : GetCurveType
//purpose  : Get curve type.
//=======================================================================

GeomAbs_CurveType GetCurveType(const Handle(Geom2d_Curve)& theC2d)
{
  GeomAbs_CurveType aTypeCurve = GeomAbs_OtherCurve;
  Handle(Standard_Type) TheType = theC2d->DynamicType();
  if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    TheType = Handle(Geom2d_TrimmedCurve)::DownCast (theC2d)->BasisCurve()->DynamicType();
  }

  if ( TheType ==  STANDARD_TYPE(Geom2d_Circle)) {
    aTypeCurve = GeomAbs_Circle;
  }
  else if ( TheType ==STANDARD_TYPE(Geom2d_Line)) {
    aTypeCurve = GeomAbs_Line;
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_Ellipse)) {
    aTypeCurve = GeomAbs_Ellipse;
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_Parabola)) {
    aTypeCurve = GeomAbs_Parabola;
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_Hyperbola)) {
    aTypeCurve = GeomAbs_Hyperbola;
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_BezierCurve)) {
    aTypeCurve = GeomAbs_BezierCurve;
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_BSplineCurve)) {
    aTypeCurve = GeomAbs_BSplineCurve;
  }
  else if ( TheType == STANDARD_TYPE(Geom2d_OffsetCurve)) {
    aTypeCurve = GeomAbs_OffsetCurve;
  }
  else {
    aTypeCurve = GeomAbs_OtherCurve;
  }
  return aTypeCurve;    
}
//=======================================================================
//function : AdjustCurveEnd
//purpose  : 
//=======================================================================
Handle(Geom2d_TrimmedCurve) AdjustCurveEnd (const Handle(Geom2d_BoundedCurve)& theC2d,
                                            const gp_Pnt2d theP, const Standard_Boolean isFirst)
{
  GeomAbs_CurveType aType = GetCurveType(theC2d);
  if(aType == GeomAbs_Line)
  {
    //create new line
    if(isFirst)
    {
      gp_Pnt2d aP = theC2d->Value(theC2d->LastParameter());
      return GCE2d_MakeSegment(theP, aP);
    }
    else
    {
      gp_Pnt2d aP = theC2d->Value(theC2d->FirstParameter());
      return GCE2d_MakeSegment(aP, theP);
    }
  }
  else
  {
    //Convert to BSpline and adjust first pole
    Handle(Geom2d_BSplineCurve) BCurve = 
      Geom2dConvert::CurveToBSplineCurve(theC2d, Convert_QuasiAngular);
    if(isFirst)
    {
      BCurve->SetPole(1, theP);
      return new Geom2d_TrimmedCurve(BCurve, BCurve->FirstParameter(),
                                               BCurve->LastParameter());
    }
    else
    {
      BCurve->SetPole(BCurve->NbPoles(), theP);
      return new Geom2d_TrimmedCurve(BCurve, BCurve->FirstParameter(),
                                               BCurve->LastParameter());
    }
  }

}
