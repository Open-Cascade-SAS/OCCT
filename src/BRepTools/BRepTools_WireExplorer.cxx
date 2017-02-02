// Created on: 1993-01-21
// Created by: Remi LEQUETTE
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


#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
// forward declarations of aux functions
//=======================================================================
static Standard_Boolean SelectDouble(TopTools_MapOfShape& Doubles,
				     TopTools_ListOfShape& L,
				     TopoDS_Edge&          E);

static Standard_Boolean SelectDegenerated(TopTools_ListOfShape& L,
					  TopoDS_Edge&          E);

static Standard_Real GetNextParamOnPC(const Handle(Geom2d_Curve)& aPC,
				      const gp_Pnt2d& aPRef,
				      const Standard_Real& fP,
				      const Standard_Real& lP,
				      const Standard_Real& tolU,
				      const Standard_Real& tolV,
				      const Standard_Boolean& reverse);

//=======================================================================
//function : BRepTools_WireExplorer
//purpose  : 
//=======================================================================
BRepTools_WireExplorer::BRepTools_WireExplorer() 
{
}

//=======================================================================
//function : BRepTools_WireExplorer
//purpose  : 
//=======================================================================
BRepTools_WireExplorer::BRepTools_WireExplorer(const TopoDS_Wire& W)
{
  TopoDS_Face F = TopoDS_Face();
  Init(W,F);
}

//=======================================================================
//function : BRepTools_WireExplorer
//purpose  : 
//=======================================================================
BRepTools_WireExplorer::BRepTools_WireExplorer(const TopoDS_Wire& W,
					       const TopoDS_Face& F)
{
  Init(W,F);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void  BRepTools_WireExplorer::Init(const TopoDS_Wire& W)
{  
  TopoDS_Face F = TopoDS_Face();
  Init(W,F);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void  BRepTools_WireExplorer::Init(const TopoDS_Wire& W,
				   const TopoDS_Face& F)
{
  myEdge = TopoDS_Edge();
  myVertex = TopoDS_Vertex();
  myMap.Clear();
  myDoubles.Clear();

  if( W.IsNull() )
    return;

  myFace = F;
  Standard_Real dfVertToler = 0.;
  myReverse = Standard_False;

  if (!myFace.IsNull())
    {
      BRepTools::Update(myFace);
      TopLoc_Location aL;  
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(myFace, aL);
      GeomAdaptor_Surface aGAS(aSurf);
      TopExp_Explorer anExp(W, TopAbs_VERTEX);
      for(; anExp.More(); anExp.Next())
	{
	  const TopoDS_Vertex& aV = TopoDS::Vertex(anExp.Current());
	  dfVertToler = Max(BRep_Tool::Tolerance(aV), dfVertToler);
	}
      myTolU = 2. * aGAS.UResolution(dfVertToler);
      myTolV = 2. * aGAS.VResolution(dfVertToler);
      
      // uresolution for cone with infinite vmin vmax is too small.
      if(aGAS.GetType() == GeomAbs_Cone)
	{
	  Standard_Real u1, u2, v1, v2;
	  BRepTools::UVBounds(myFace, u1, u2, v1, v2);
	  gp_Pnt aP;
	  gp_Vec aD1U, aD1V;
	  aGAS.D1(u1, v1, aP, aD1U, aD1V);
	  Standard_Real tol1, tol2, maxtol = .0005*(u2-u1);
	  Standard_Real a = aD1U.Magnitude();

	  if(a <= Precision::Confusion())
	    tol1 = maxtol;
	  else
	    tol1 = Min(maxtol, dfVertToler/a);

	  aGAS.D1(u1, v2, aP, aD1U, aD1V);
	  a = aD1U.Magnitude();
	  if(a <= Precision::Confusion())
	    tol2 = maxtol;
	  else
	    tol2 = Min(maxtol, dfVertToler/a);

	  myTolU = 2. * Max(tol1, tol2);
	}

      if( aGAS.GetType() == GeomAbs_BSplineSurface || 
	  aGAS.GetType() == GeomAbs_BezierSurface )
	{
	  Standard_Real maxTol = Max(myTolU,myTolV);
	  myTolU = maxTol;
	  myTolV = maxTol;
	}

      myReverse = (myFace.Orientation() == TopAbs_REVERSED);
    }
      
  // map of vertices to know if the wire is open
  TopTools_MapOfShape vmap;
  //  Modified by Sergey KHROMOV - Mon May 13 11:50:48 2002 Begin
  //  map of infinite edges
  TopTools_MapOfShape anInfEmap;
  //  Modified by Sergey KHROMOV - Mon May 13 11:50:49 2002 End

  // list the vertices
  TopoDS_Vertex V1,V2;
  TopTools_ListOfShape empty;

  TopoDS_Iterator it(W);
  while (it.More())
    {
      const TopoDS_Edge& E = TopoDS::Edge(it.Value());
      TopAbs_Orientation Eori = E.Orientation();
      if (Eori == TopAbs_INTERNAL || Eori == TopAbs_EXTERNAL)
	{
	  it.Next();
	  continue;
	}
      TopExp::Vertices(E,V1,V2,Standard_True);

      if( !V1.IsNull() )
	{
	  if( !myMap.IsBound(V1) )
	    myMap.Bind(V1,empty);
	  myMap(V1).Append(E);

	  // add or remove in the vertex map
	  V1.Orientation(TopAbs_FORWARD);
	  if( !vmap.Add(V1) )
	    vmap.Remove(V1);
	}

      if( !V2.IsNull() )
	{
	  V2.Orientation(TopAbs_REVERSED);
	  if(!vmap.Add(V2))
	    vmap.Remove(V2);
	}

      //  Modified by Sergey KHROMOV - Mon May 13 11:52:20 2002 Begin
      if (V1.IsNull() || V2.IsNull())
	{
	  Standard_Real aF = 0., aL = 0.;
	  BRep_Tool::Range(E, aF, aL);
	  
	  if(Eori == TopAbs_FORWARD)
	    {
	      if (aF == -Precision::Infinite())
		anInfEmap.Add(E);
	    }
	  else
	    { // Eori == TopAbs_REVERSED
	      if (aL == Precision::Infinite())
		anInfEmap.Add(E);
	    }
	}
      //  Modified by Sergey KHROMOV - Mon May 13 11:52:20 2002 End
      it.Next();
    }

  //Construction of the set of double edges.
  TopoDS_Iterator it2(W);  
  TopTools_MapOfShape emap;
  while (it2.More()) {
    if (!emap.Add(it2.Value())) 
      myDoubles.Add(it2.Value());
    it2.Next();
  }

  // if vmap is not empty the wire is open, let us find the first vertex
  if (!vmap.IsEmpty()) {
    TopTools_MapIteratorOfMapOfShape itt(vmap); // skl : I change "it" to "itt"
    while (itt.Key().Orientation() != TopAbs_FORWARD) {
      itt.Next();
      if (!itt.More()) break;
    }
    if (itt.More()) V1 = TopoDS::Vertex(itt.Key());
  }
  else {
//  Modified by Sergey KHROMOV - Mon May 13 12:05:30 2002 Begin
//   The wire is infinite Try to find the first vertex. It may be NULL.
    if (!anInfEmap.IsEmpty()) {
      TopTools_MapIteratorOfMapOfShape itt(anInfEmap);

      for (; itt.More(); itt.Next()) {
	TopoDS_Edge        anEdge = TopoDS::Edge(itt.Key());
	TopAbs_Orientation anOri  = anEdge.Orientation();
	Standard_Real      aF;
	Standard_Real      aL;

	BRep_Tool::Range(anEdge, aF, aL);
	if ((anOri == TopAbs_FORWARD  && aF == -Precision::Infinite()) ||
	    (anOri == TopAbs_REVERSED && aL ==  Precision::Infinite())) {
	  myEdge   = anEdge;
	  myVertex = TopoDS_Vertex();

	  return;
	}
      }
    }
//  Modified by Sergey KHROMOV - Mon May 13 12:05:31 2002 End


    // use the first vertex in iterator
    it.Initialize(W);
    while (it.More()) {
      const TopoDS_Edge& E = TopoDS::Edge(it.Value());
      TopAbs_Orientation Eori = E.Orientation();
      if (Eori == TopAbs_INTERNAL || Eori == TopAbs_EXTERNAL) {
	// JYL 10-03-97 : waiting for correct processing 
	// of INTERNAL/EXTERNAL edges
	it.Next();
	continue;
      }
      TopExp::Vertices(E,V1,V2,Standard_True);
      break;
    }
  }

  if (V1.IsNull() ) return;
  if (!myMap.IsBound(V1)) return;
  
  TopTools_ListOfShape& l = myMap(V1);
  myEdge = TopoDS::Edge(l.First());
  l.RemoveFirst();
  myVertex = TopExp::FirstVertex (myEdge, Standard_True);

}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================
Standard_Boolean  BRepTools_WireExplorer::More()const 
{
  return !myEdge.IsNull();
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
void  BRepTools_WireExplorer::Next()
{
  myVertex = TopExp::LastVertex (myEdge, Standard_True);

  if (myVertex.IsNull()) {
     myEdge = TopoDS_Edge();
     return;
  }
  if (!myMap.IsBound(myVertex)) {
     myEdge = TopoDS_Edge();
     return;
  }

  TopTools_ListOfShape& l = myMap(myVertex);

  if (l.IsEmpty()) {
    myEdge = TopoDS_Edge();
  }
  else if (l.Extent() == 1) {
//  Modified by Sergey KHROMOV - Fri Jun 21 10:28:01 2002 OCC325 Begin
    TopoDS_Vertex aV1;
    TopoDS_Vertex aV2;
    TopoDS_Edge   aNextEdge = TopoDS::Edge(l.First());

    TopExp::Vertices(aNextEdge, aV1, aV2, Standard_True);

    if (!aV1.IsSame(myVertex)) {
      myEdge = TopoDS_Edge();
      return;
    }
    if (!myFace.IsNull() && aV1.IsSame(aV2)) {
      Handle(Geom2d_Curve) aPrevPC;
      Handle(Geom2d_Curve) aNextPC;
      Standard_Real        aPar11, aPar12;
      Standard_Real        aPar21, aPar22;
      Standard_Real        aPrevPar;
      Standard_Real        aNextFPar;
      Standard_Real        aNextLPar;

      aPrevPC = BRep_Tool::CurveOnSurface(myEdge, myFace, aPar11, aPar12);
      aNextPC = BRep_Tool::CurveOnSurface(aNextEdge, myFace, aPar21, aPar22);

      if (aPrevPC.IsNull() || aNextPC.IsNull()) {
	      myEdge = TopoDS_Edge();
	      return;
      }

      if (myEdge.Orientation() == TopAbs_FORWARD)
	      aPrevPar = aPar12;
      else
	      aPrevPar = aPar11;

      if (aNextEdge.Orientation() == TopAbs_FORWARD) {
	      aNextFPar = aPar21;
	      aNextLPar = aPar22;
      } else {
	      aNextFPar = aPar22;
	      aNextLPar = aPar21;
      }

      gp_Pnt2d aPPrev  = aPrevPC->Value(aPrevPar);
      gp_Pnt2d aPNextF = aNextPC->Value(aNextFPar);
      gp_Pnt2d aPNextL = aNextPC->Value(aNextLPar);

      if (aPPrev.SquareDistance(aPNextF) > aPPrev.SquareDistance(aPNextL)) {
	      myEdge = TopoDS_Edge();
	      return;
      }
    }
//  Modified by Sergey KHROMOV - Fri Jun 21 11:08:16 2002 End
    myEdge = TopoDS::Edge(l.First());
    l.Clear();
  }
  else {
    if (myFace.IsNull()) {
      // Without Face - try to return edges
      // as logically as possible
      // At first degenerated edges.
      TopoDS_Edge E = myEdge;
      if (SelectDegenerated(l,E)) {
	      myEdge = E;
	      return;
      }
      // At second double edges.
      E = myEdge;
      if (SelectDouble(myDoubles,l,E)) {
	      myEdge = E;
	      return;
      }

      TopTools_ListIteratorOfListOfShape it(l); 
      Standard_Boolean notfound = Standard_True;
      while (it.More()) {
	      if (!it.Value().IsSame(myEdge)) {
	        myEdge = TopoDS::Edge(it.Value());
	        l.Remove(it);
	        notfound = Standard_False;
	        break;
	      }
	      it.Next();
      }
      
      if(notfound) {
	      myEdge = TopoDS_Edge();
	      return;
      }

    }
    else
    {
      // If we have more than one edge attached to the list
      // probably wire that we explore contains a loop or loops.
      Standard_Real dfFPar = 0., dfLPar = 0.;
      Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface (myEdge, myFace, dfFPar, dfLPar);
      if(aPCurve.IsNull())
      {
        myEdge = TopoDS_Edge();
        return;
      }
      // Note: current < myVertex > which is last on < myEdge >
      //       equals in 2D to following 2D points:
      //       edge is FORWARD  - point with MAX parameter on PCurve;
      //       edge is REVERSED - point with MIN parameter on PCurve.

      // Get 2D point equals to < myVertex > in 2D for current edge.
      gp_Pnt2d PRef;
      if( myEdge.Orientation() == TopAbs_REVERSED )
        aPCurve->D0(dfFPar, PRef);
      else 
        aPCurve->D0(dfLPar, PRef);

      // Get next 2D point from current edge's PCurve with parameter
      // F + dP (REV) or L - dP (FOR)
      Standard_Boolean isrevese = ( myEdge.Orientation() == TopAbs_REVERSED );
      Standard_Real dfMPar = GetNextParamOnPC(aPCurve,PRef,dfFPar,dfLPar,myTolU,myTolV,isrevese);

      gp_Pnt2d PRefm;
      aPCurve->D0(dfMPar, PRefm);
      // Get vector from PRef to PRefm
      gp_Vec2d anERefDir(PRef,PRefm);
      // Search the list of edges looking for the edge having hearest
      // 2D point of connected vertex to current one and smallest angle.
      // First process all degenerated edges, then - all others.

      TopTools_ListIteratorOfListOfShape it;
      Standard_Integer k = 1, kMin = 0, iDone = 0;
      Standard_Boolean isDegenerated = Standard_True;
      Standard_Real dmin = RealLast();
      Standard_Real dfMinAngle = 3.0*M_PI, dfCurAngle = 3.0*M_PI;

      for(iDone = 0; iDone < 2; iDone++)
      {
        it.Initialize(l);
        while( it.More() )
        {
	        const TopoDS_Edge& E = TopoDS::Edge(it.Value());
	        if( E.IsSame(myEdge) )
	        {
	          it.Next();
	          k++;
	          continue;
	        }
  		
	        TopoDS_Vertex aVert1, aVert2;
	        TopExp::Vertices (E, aVert1, aVert2, Standard_True);
	        if( aVert1.IsNull() || aVert2.IsNull() )
          {
            it.Next();
            k++;
            continue;
          }
  		
	        aPCurve = BRep_Tool::CurveOnSurface (E, myFace, dfFPar, dfLPar);
	        if( aPCurve.IsNull() )
          {
            it.Next();
            k++;
            continue;
          }
    		
	        gp_Pnt2d aPEb, aPEe;
	        if( aVert1.IsSame(aVert2) == isDegenerated )
	        {
	          if( E.Orientation() == TopAbs_REVERSED )
	            aPCurve->D0(dfLPar, aPEb);
	          else	
	            aPCurve->D0(dfFPar, aPEb);

	          if( Abs(dfLPar-dfFPar) > Precision::PConfusion() )
	          {
		          isrevese = ( E.Orientation() == TopAbs_REVERSED );
		          isrevese = !isrevese;
		          Standard_Real aEPm = GetNextParamOnPC(aPCurve,aPEb,dfFPar,dfLPar,myTolU,myTolV,isrevese);
        			
		          aPCurve->D0 (aEPm, aPEe);
              if(aPEb.SquareDistance(aPEe) <= gp::Resolution())
              {
                //seems to be very short curve
                gp_Vec2d aD;
                aPCurve->D1(aEPm, aPEe, aD);
	              if( E.Orientation() == TopAbs_REVERSED )
                  aPEe.SetXY(aPEb.XY()-aD.XY());
	              else	
                  aPEe.SetXY(aPEb.XY()+aD.XY());

                if(aPEb.SquareDistance(aPEe) <= gp::Resolution())
                {
                  it.Next();
                  k++;
                  continue;
                }
              }
		          gp_Vec2d anEDir(aPEb, aPEe);
		          dfCurAngle = Abs( anEDir.Angle(anERefDir) );
	          }

	          if( dfCurAngle <= dfMinAngle )
	          {
		          Standard_Real d = PRef.SquareDistance(aPEb);
		          if( d <= Precision::PConfusion() )
		            d = 0.;
		          if( Abs(aPEb.X()-PRef.X()) < myTolU  &&  Abs(aPEb.Y()-PRef.Y()) < myTolV )
		          {
		            if( d <= dmin )
		            {
			            dfMinAngle = dfCurAngle;
			            kMin = k;
			            dmin = d;
		            }
		          }
	          }
	        }
	        it.Next();
	        k++;
        }// while it

        if( kMin == 0 )
        {
	        isDegenerated = Standard_False;
	        k = 1;
	        dmin = RealLast();
        }
        else
          break;
      }// for iDone

      if(kMin == 0)
      {
        // probably unclosed in 2d space wire
        myEdge = TopoDS_Edge();
        return;
      }

      // Selection the edge.
      it.Initialize(l);
      k = 1;
      while( it.More() )
      {
        if( k == kMin )
        {
          myEdge = TopoDS::Edge(it.Value());
          l.Remove(it);
          break;
        }
        it.Next();
        k++;
      }
    }//else face != NULL && l > 1
  }//else l > 1
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================
const TopoDS_Edge&  BRepTools_WireExplorer::Current()const 
{
  return myEdge;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================
TopAbs_Orientation BRepTools_WireExplorer::Orientation() const
{
  TopoDS_Iterator it(myEdge,Standard_False);
  while (it.More()) {
    if (myVertex.IsSame(it.Value()))
      return it.Value().Orientation();
    it.Next();
  }
  throw Standard_NoSuchObject("BRepTools_WireExplorer::Orientation");
}

//=======================================================================
//function : CurrentVertex
//purpose  : 
//=======================================================================
const TopoDS_Vertex& BRepTools_WireExplorer::CurrentVertex() const
{
  return myVertex;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BRepTools_WireExplorer::Clear()
{
  myMap.Clear();
  myDoubles.Clear();
  myEdge = TopoDS_Edge();
  myFace = TopoDS_Face();
  myVertex = TopoDS_Vertex();
}


//=======================================================================
//function : SelectDouble
//purpose  : 
//=======================================================================

Standard_Boolean SelectDouble(TopTools_MapOfShape& Doubles,
			      TopTools_ListOfShape& L,
			      TopoDS_Edge&          E)
{
  TopTools_ListIteratorOfListOfShape it(L);
  
  for (; it.More(); it.Next()) {
    const TopoDS_Shape& CE = it.Value();
    if (Doubles.Contains(CE) && (!E.IsSame(CE))) {
      E = TopoDS::Edge(CE);
      L.Remove(it);
      return 1;
    }
  }
  return 0;
}

//=======================================================================
//function : SelectDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean SelectDegenerated(TopTools_ListOfShape& L,
				   TopoDS_Edge&          E)
{
  TopTools_ListIteratorOfListOfShape it(L);
  while (it.More()) {
    if (!it.Value().IsSame(E)) {
      E = TopoDS::Edge(it.Value());
      if (BRep_Tool::Degenerated(E)) {
	L.Remove(it);
	return 1;
      }
    }
    it.Next();
  } 
  return 0;
}

//=======================================================================
//function : GetNextParamOnPC
//purpose  : 
//=======================================================================
Standard_Real GetNextParamOnPC(const Handle(Geom2d_Curve)& aPC,
			       const gp_Pnt2d& aPRef,
			       const Standard_Real& fP,
			       const Standard_Real& lP,
			       const Standard_Real& tolU,
			       const Standard_Real& tolV,
			       const Standard_Boolean& reverse)
{
  Standard_Real result = ( reverse ) ? fP : lP;
  Standard_Real dP = Abs( lP - fP ) / 1000.; // was / 16.;
  if( reverse )
    {
      Standard_Real startPar = fP;
      Standard_Boolean nextPntOnEdge = Standard_False;
      while( !nextPntOnEdge && startPar < lP )
	{
	  gp_Pnt2d pnt;
	  startPar += dP;
	  aPC->D0(startPar, pnt);
	  if( Abs( aPRef.X() - pnt.X() ) < tolU && Abs( aPRef.Y() - pnt.Y() ) < tolV )
	    continue;
	  else
	    {
	      result = startPar;
	      nextPntOnEdge = Standard_True;
	      break;
	    }
	}
      
      if( !nextPntOnEdge )
	result = lP;

      if( result > lP )
	result = lP;
    }
  else
    {
      Standard_Real startPar = lP;
      Standard_Boolean nextPntOnEdge = Standard_False;
      while( !nextPntOnEdge && startPar > fP )
	{
	  gp_Pnt2d pnt;
	  startPar -= dP;
	  aPC->D0(startPar, pnt);
	  if( Abs( aPRef.X() - pnt.X() ) < tolU && Abs( aPRef.Y() - pnt.Y() ) < tolV )
	    continue;
	  else
	    {
	      result = startPar;
	      nextPntOnEdge = Standard_True;
	      break;
	    }
	}
      
      if( !nextPntOnEdge )
	result = fP;

      if( result < fP )
	result = fP;
    }

  return result;
}
