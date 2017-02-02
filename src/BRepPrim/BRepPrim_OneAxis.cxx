// Created on: 1991-07-24
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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


#include <BRepPrim_Builder.hxx>
#include <BRepPrim_Direction.hxx>
#include <BRepPrim_OneAxis.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#define NBVERTICES 6
#define VAXISTOP    0
#define VAXISBOT    1
#define VTOPSTART   2
#define VTOPEND     3
#define VBOTSTART   4
#define VBOTEND     5
#define NBEDGES    9
#define EAXIS       0
#define ESTART      1
#define EEND        2
#define ETOPSTART   3
#define ETOPEND     4
#define EBOTSTART   5
#define EBOTEND     6
#define ETOP        7
#define EBOTTOM     8
#define NBWIRES    9
#define WLATERAL    0
#define WLATERALSTART    0
#define WLATERALEND      1
#define WTOP             2
#define WBOTTOM          3
#define WSTART           5
#define WAXISSTART       6
#define WEND             7
#define WAXISEND         8
#define NBFACES    5
#define FLATERAL    0
#define FTOP        1
#define FBOTTOM     2
#define FSTART      3
#define FEND        4

//=======================================================================
//function : BRepPrim_OneAxis_Check
//purpose  : raise Standard_DomainError if something was built
//=======================================================================

static void BRepPrim_OneAxis_Check(const Standard_Boolean V[],
				     const Standard_Boolean E[],
				     const Standard_Boolean W[],
				     const Standard_Boolean F[])
{
  Standard_Integer i;
  for (i = 0; i < NBVERTICES; i++)
    if (V[i]) throw Standard_DomainError();
  for (i = 0; i < NBEDGES; i++)
    if (E[i]) throw Standard_DomainError();
  for (i = 0; i < NBWIRES; i++)
    if (W[i]) throw Standard_DomainError();
  for (i = 0; i < NBFACES; i++)
    if (F[i]) throw Standard_DomainError();
}

//=======================================================================
//function : BRepPrim_OneAxis
//purpose  : 
//=======================================================================

BRepPrim_OneAxis::BRepPrim_OneAxis(const BRepPrim_Builder& B,
				       const gp_Ax2& A,
				       const Standard_Real VMin,
				       const Standard_Real VMax) :
       myBuilder(B),
       myAxes(A),
       myAngle(2*M_PI),
       myVMin(VMin),
       myVMax(VMax),
       myMeridianOffset(0)

{
  // init Built flags
  Standard_Integer i;
  ShellBuilt = Standard_False;
  for (i = 0; i < NBVERTICES; i++)
    VerticesBuilt[i] = Standard_False;
  for (i = 0; i < NBEDGES; i++)
    EdgesBuilt[i] = Standard_False;
  for (i = 0; i < NBWIRES; i++)
    WiresBuilt[i] = Standard_False;
  for (i = 0; i < NBFACES; i++)
    FacesBuilt[i] = Standard_False;

}

//=======================================================================
//function : ~BRepPrim_OneAxis
//purpose  : Destructor
//=======================================================================

BRepPrim_OneAxis::~BRepPrim_OneAxis()
{
}

//=======================================================================
//function : SetMeridianOffset
//purpose  : 
//=======================================================================

void BRepPrim_OneAxis::SetMeridianOffset(const Standard_Real O)
{
  myMeridianOffset = O;
}

//=======================================================================
//function : Axes, Angle, VMin, VMax
//purpose  : 
//=======================================================================

const gp_Ax2&  BRepPrim_OneAxis::Axes     () const 
{ 
  return myAxes;
}

void BRepPrim_OneAxis::Axes     (const gp_Ax2& A)
{ 
  BRepPrim_OneAxis_Check(VerticesBuilt,EdgesBuilt,WiresBuilt,FacesBuilt);
  myAxes = A;
}

Standard_Real BRepPrim_OneAxis::Angle () const
{
  return myAngle;
}

void BRepPrim_OneAxis::Angle (const Standard_Real A)
{
  BRepPrim_OneAxis_Check(VerticesBuilt,EdgesBuilt,WiresBuilt,FacesBuilt);
  myAngle = A;
}

Standard_Real BRepPrim_OneAxis::VMin () const
{
  return myVMin;
}

void BRepPrim_OneAxis::VMin (const Standard_Real V)
{
  BRepPrim_OneAxis_Check(VerticesBuilt,EdgesBuilt,WiresBuilt,FacesBuilt);
  myVMin = V;
}

Standard_Real BRepPrim_OneAxis::VMax () const
{
  return myVMax;
}

void BRepPrim_OneAxis::VMax (const Standard_Real V)
{
  BRepPrim_OneAxis_Check(VerticesBuilt,EdgesBuilt,WiresBuilt,FacesBuilt);
  myVMax = V;
}

//=======================================================================
//function : MeridianOnAxis
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::MeridianOnAxis
  (const Standard_Real V) const
{
  return Abs(MeridianValue(V).X()) < Precision::Confusion();
}

//=======================================================================
//function : MeridianClosed
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::MeridianClosed() const
{
  if (VMaxInfinite()) return Standard_False;
  if (VMinInfinite()) return Standard_False;
  return MeridianValue(myVMin).IsEqual(MeridianValue(myVMax),
				       Precision::Confusion());
}

//=======================================================================
//function : VMaxInfinite
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::VMaxInfinite() const
{
  return Precision::IsPositiveInfinite(myVMax);
}

//=======================================================================
//function : VMinInfinite
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::VMinInfinite() const
{
  return Precision::IsNegativeInfinite(myVMin);
}

//=======================================================================
//function : HasTop
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::HasTop() const
{
  if (VMaxInfinite())         return Standard_False;
  if (MeridianClosed())       return Standard_False;
  if (MeridianOnAxis(myVMax)) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : HasBottom
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::HasBottom() const
{
  if (VMinInfinite())         return Standard_False;
  if (MeridianClosed())       return Standard_False;
  if (MeridianOnAxis(myVMin)) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : HasSides
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrim_OneAxis::HasSides() const
{
  return 2*M_PI - myAngle > Precision::Angular();
}

//=======================================================================
//function : Shell
//purpose  : 
//=======================================================================

const TopoDS_Shell& BRepPrim_OneAxis::Shell()
{
  if (!ShellBuilt) {
    myBuilder.MakeShell(myShell);

    myBuilder.AddShellFace(myShell,LateralFace());
    if (HasTop())
      myBuilder.AddShellFace(myShell,TopFace());
    if (HasBottom())
      myBuilder.AddShellFace(myShell,BottomFace());
    if (HasSides()) {
      myBuilder.AddShellFace(myShell,StartFace());
      myBuilder.AddShellFace(myShell,EndFace());
    }

    myShell.Closed (BRep_Tool::IsClosed (myShell));
    myBuilder.CompleteShell(myShell);
    ShellBuilt = Standard_True;
  }
  return myShell;
}

//=======================================================================
//function : LateralFace
//purpose  : build the lateral face
//=======================================================================

const TopoDS_Face& BRepPrim_OneAxis::LateralFace ()
{
  // do it if not done
  if (!FacesBuilt[FLATERAL]) {

    // build an empty lateral face
    myFaces[FLATERAL] = MakeEmptyLateralFace();

    // add the wires
    if (VMaxInfinite() && VMinInfinite()) {
      myBuilder.AddFaceWire(myFaces[FLATERAL],LateralStartWire());
      myBuilder.AddFaceWire(myFaces[FLATERAL],LateralEndWire());
    }
    else
      myBuilder.AddFaceWire(myFaces[FLATERAL],LateralWire());

    // put the parametric curves
    if (MeridianClosed()) {
      // closed edge
      myBuilder.SetPCurve(myEdges[ETOP],myFaces[FLATERAL],
			  gp_Lin2d(gp_Pnt2d(0,myVMin),gp_Dir2d(1,0)),
			  gp_Lin2d(gp_Pnt2d(0,myVMax),gp_Dir2d(1,0)));
    }    
    else {
      if (!VMaxInfinite()) {
	myBuilder.SetPCurve(myEdges[ETOP],myFaces[FLATERAL],
			    gp_Lin2d(gp_Pnt2d(0,myVMax),gp_Dir2d(1,0)));
	if (!HasSides() || MeridianOnAxis(myVMax)) {
	  // closed edge set parameters
	  myBuilder.SetParameters(myEdges[ETOP],
				  TopEndVertex(),
				  0.,myAngle);
	}
      }
      if (!VMinInfinite()) {
	myBuilder.SetPCurve(myEdges[EBOTTOM],myFaces[FLATERAL],
			    gp_Lin2d(gp_Pnt2d(0,myVMin),gp_Dir2d(1,0)));
	if (!HasSides() || MeridianOnAxis(myVMin)) {
	  // closed edge set parameters
	  myBuilder.SetParameters(myEdges[EBOTTOM],
				  BottomEndVertex(),
				  0.,myAngle);
	}
      }
    }
    if (HasSides()) {
      myBuilder.SetPCurve(myEdges[ESTART],myFaces[FLATERAL],
			  gp_Lin2d(gp_Pnt2d(0,-myMeridianOffset),
				   gp_Dir2d(0,1)));
      
      myBuilder.SetPCurve(myEdges[EEND],myFaces[FLATERAL],
			  gp_Lin2d(gp_Pnt2d(myAngle,-myMeridianOffset),
				   gp_Dir2d(0,1)));
    }
    else {
      // closed edge
      myBuilder.SetPCurve(myEdges[ESTART],myFaces[FLATERAL],
			  gp_Lin2d(gp_Pnt2d(myAngle,-myMeridianOffset),
				   gp_Dir2d(0,1)),
			  gp_Lin2d(gp_Pnt2d(0,-myMeridianOffset),
				   gp_Dir2d(0,1)));
    }
    myBuilder.CompleteFace(myFaces[FLATERAL]);
    FacesBuilt[FLATERAL] = Standard_True;
  }
  return myFaces[FLATERAL];
}

//=======================================================================
//function : TopFace
//purpose  : build and return the TopFace
//=======================================================================

const TopoDS_Face& BRepPrim_OneAxis::TopFace ()
{
  // do it if not done
  if (!FacesBuilt[FTOP]) {

    Standard_DomainError_Raise_if(!HasTop(),
				  "BRepPrim_OneAxis::TopFace:No top face");
    
    // make the empty face by translating the axes
    Standard_Real z = MeridianValue(myVMax).Y();
    gp_Vec V = myAxes.Direction();
    V.Multiply(z);
    myBuilder.MakeFace(myFaces[FTOP],gp_Pln(myAxes.Translated(V)));

    myBuilder.AddFaceWire(myFaces[FTOP],TopWire());

    // put the parametric curves
    myBuilder.SetPCurve(myEdges[ETOP],myFaces[FTOP],
			gp_Circ2d(gp_Ax2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)),
				  MeridianValue(myVMax).X()));
    if (HasSides()) {
      myBuilder.SetPCurve(myEdges[ETOPSTART],myFaces[FTOP],
			  gp_Lin2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)));
      myBuilder.SetPCurve(myEdges[ETOPEND],myFaces[FTOP],
			  gp_Lin2d(gp_Pnt2d(0,0),
				   gp_Dir2d(Cos(myAngle),Sin(myAngle))));
    }
    
    myBuilder.CompleteFace(myFaces[FTOP]);
    FacesBuilt[FTOP] = Standard_True;
  }

  return myFaces[FTOP];
}

//=======================================================================
//function : BottomFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrim_OneAxis::BottomFace ()
{
  // do it if not done
  if (!FacesBuilt[FBOTTOM]) {

    Standard_DomainError_Raise_if(!HasBottom(),
				  "BRepPrim_OneAxis::BottomFace:No bottom face");
    
    // make the empty face by translating the axes
    Standard_Real z = MeridianValue(myVMin).Y();
    gp_Vec V = myAxes.Direction();
    V.Multiply(z);
    gp_Ax2 axes = myAxes.Translated(V);
    myBuilder.MakeFace(myFaces[FBOTTOM],gp_Pln(axes));
    myBuilder.ReverseFace(myFaces[FBOTTOM]);
    myBuilder.AddFaceWire(myFaces[FBOTTOM],BottomWire());

    // put the parametric curves
    myBuilder.SetPCurve(myEdges[EBOTTOM],myFaces[FBOTTOM],
			gp_Circ2d(gp_Ax2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)),
				  MeridianValue(myVMin).X()));
    if (HasSides()) {
      myBuilder.SetPCurve(myEdges[EBOTSTART],myFaces[FBOTTOM],
			  gp_Lin2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)));
      myBuilder.SetPCurve(myEdges[EBOTEND],myFaces[FBOTTOM],
			  gp_Lin2d(gp_Pnt2d(0,0),
				   gp_Dir2d(Cos(myAngle),Sin(myAngle))));
    }
    
    myBuilder.CompleteFace(myFaces[FBOTTOM]);
    FacesBuilt[FBOTTOM] = Standard_True;
  }

  return myFaces[FBOTTOM];
}

//=======================================================================
//function : StartFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrim_OneAxis::StartFace ()
{
  // do it if not done
  if (!FacesBuilt[FSTART]) {

    Standard_DomainError_Raise_if(!HasSides(),
				  "BRepPrim_OneAxes::StartFace:No side faces");

    // build the empty face, perpendicular to myTool.Axes()
    gp_Ax2 axes(myAxes.Location(),myAxes.YDirection().Reversed(),myAxes.XDirection());
    myBuilder.MakeFace(myFaces[FSTART],gp_Pln(axes));


    if (VMaxInfinite() && VMinInfinite()) 
      myBuilder.AddFaceWire(myFaces[FSTART],AxisStartWire());

    myBuilder.AddFaceWire(myFaces[FSTART],StartWire());

    // parametric curves
    SetMeridianPCurve(myEdges[ESTART],myFaces[FSTART]);
    if (EdgesBuilt[EAXIS])
      myBuilder.SetPCurve(myEdges[EAXIS],myFaces[FSTART],
			  gp_Lin2d(gp_Pnt2d(0,0),gp_Dir2d(0,1)));
    if (EdgesBuilt[ETOPSTART])
      myBuilder.SetPCurve(myEdges[ETOPSTART],myFaces[FSTART],
			  gp_Lin2d(gp_Pnt2d(0,MeridianValue(myVMax).Y()),gp_Dir2d(1,0)));
    if (EdgesBuilt[EBOTSTART])
      myBuilder.SetPCurve(myEdges[EBOTSTART],myFaces[FSTART],
			  gp_Lin2d(gp_Pnt2d(0,MeridianValue(myVMin).Y()),gp_Dir2d(1,0)));
    

    myBuilder.CompleteFace(myFaces[FSTART]);
    FacesBuilt[FSTART] = Standard_True;
  }

  return myFaces[FSTART];
}

//=======================================================================
//function : EndFace
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepPrim_OneAxis::EndFace ()
{
  // do it if not done
  if (!FacesBuilt[FEND]) {

    Standard_DomainError_Raise_if(!HasSides(),
				  "BRepPrim_OneAxes::EndFace:No side faces");

    // build the empty face, perpendicular to myTool.Axes()
    gp_Ax2 axes(myAxes.Location(),myAxes.YDirection().Reversed(),myAxes.XDirection());
    axes.Rotate(myAxes.Axis(),myAngle);
    myBuilder.MakeFace(myFaces[FEND],gp_Pln(axes));
    myBuilder.ReverseFace(myFaces[FEND]);

    if (VMaxInfinite() && VMinInfinite())
      myBuilder.AddFaceWire(myFaces[FEND],AxisEndWire());
    myBuilder.AddFaceWire(myFaces[FEND],EndWire());

    // parametric curves
    SetMeridianPCurve(myEdges[EEND],myFaces[FEND]);
    if (EdgesBuilt[EAXIS])
      myBuilder.SetPCurve(myEdges[EAXIS],myFaces[FEND],
			  gp_Lin2d(gp_Pnt2d(0,0),gp_Dir2d(0,1)));
    if (EdgesBuilt[ETOPEND])
      myBuilder.SetPCurve(myEdges[ETOPEND],myFaces[FEND],
			  gp_Lin2d(gp_Pnt2d(0,MeridianValue(myVMax).Y()),
				   gp_Dir2d(1,0)));
    if (EdgesBuilt[EBOTEND])
      myBuilder.SetPCurve(myEdges[EBOTEND],myFaces[FEND],
			  gp_Lin2d(gp_Pnt2d(0,MeridianValue(myVMin).Y()),
				   gp_Dir2d(1,0)));
    
    myBuilder.CompleteFace(myFaces[FEND]);
    FacesBuilt[FEND] = Standard_True;
  }

  return myFaces[FEND];
}

//=======================================================================
//function : LateralWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::LateralWire ()
{
  // do it if not done
  if (!WiresBuilt[WLATERAL]) {

    myBuilder.MakeWire(myWires[WLATERAL]);
  
    if (!VMaxInfinite())
      myBuilder.AddWireEdge(myWires[WLATERAL],TopEdge()    ,Standard_False);
    myBuilder.AddWireEdge(  myWires[WLATERAL],EndEdge()    ,Standard_True);
    if (!VMinInfinite())
      myBuilder.AddWireEdge(myWires[WLATERAL],BottomEdge() ,Standard_True);
    myBuilder.AddWireEdge(  myWires[WLATERAL],StartEdge()  ,Standard_False);

    myBuilder.CompleteWire(myWires[WLATERAL]);
    WiresBuilt[WLATERAL] = Standard_True;
  }
  
  return myWires[WLATERAL];
}


//=======================================================================
//function : LateralStartWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::LateralStartWire ()
{
  // do it if not done
  if (!WiresBuilt[WLATERALSTART]) {

    myBuilder.MakeWire(myWires[WLATERALSTART]);
  
    myBuilder.AddWireEdge(myWires[WLATERALSTART],StartEdge(),Standard_False);

    myBuilder.CompleteWire(myWires[WLATERALSTART]);
    WiresBuilt[WLATERALSTART] = Standard_True;
  }
  
  return myWires[WLATERALSTART];
}


//=======================================================================
//function : LateralEndWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::LateralEndWire ()
{
  // do it if not done
  if (!WiresBuilt[WLATERALEND]) {

    myBuilder.MakeWire(myWires[WLATERALEND]);
  
    myBuilder.AddWireEdge(myWires[WLATERALEND],EndEdge(),Standard_True);

    myBuilder.CompleteWire(myWires[WLATERALEND]);
    WiresBuilt[WLATERALEND] = Standard_True;
  }
  
  return myWires[WLATERALEND];
}

//=======================================================================
//function : TopWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::TopWire ()
{
  // do it if not done
  if (!WiresBuilt[WTOP]) {

    Standard_DomainError_Raise_if(!HasTop(),
				  "BRepPrim_OneAxis::TopWire: no top");

    myBuilder.MakeWire(myWires[WTOP]);
  
    myBuilder.AddWireEdge(myWires[WTOP],TopEdge()       ,Standard_True);
    if (HasSides()) {
      myBuilder.AddWireEdge(myWires[WTOP],StartTopEdge()  ,Standard_True);
      myBuilder.AddWireEdge(myWires[WTOP],EndTopEdge()    ,Standard_False);
    }
    myBuilder.CompleteWire(myWires[WTOP]);
    WiresBuilt[WTOP] = Standard_True;
  }

  return myWires[WTOP];
}

//=======================================================================
//function : BottomWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::BottomWire ()
{
  // do it if not done
  if (!WiresBuilt[WBOTTOM]) {

    Standard_DomainError_Raise_if(!HasBottom(),
				  "BRepPrim_OneAxis::BottomWire: no bottom");

    myBuilder.MakeWire(myWires[WBOTTOM]);
  
    myBuilder.AddWireEdge(myWires[WBOTTOM],BottomEdge()       ,Standard_False);
    if (HasSides()) {
      myBuilder.AddWireEdge(myWires[WBOTTOM],EndBottomEdge()  ,Standard_True);
      myBuilder.AddWireEdge(myWires[WBOTTOM],StartBottomEdge(),Standard_False);
    }

    myBuilder.CompleteWire(myWires[WBOTTOM]);
    WiresBuilt[WBOTTOM] = Standard_True;
  }
  
  return myWires[WBOTTOM];
}

//=======================================================================
//function : StartWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::StartWire ()
{
  // do it if not done
  if (!WiresBuilt[WSTART]) {

    Standard_DomainError_Raise_if(!HasSides(),
				  "BRepPrim_OneAxes::StartWire:no sides");
  
    myBuilder.MakeWire(myWires[WSTART]);
  
    if (HasBottom())
      myBuilder.AddWireEdge(myWires[WSTART],StartBottomEdge() ,Standard_True);

    if (!MeridianClosed()) {
      if (!VMaxInfinite() || !VMinInfinite())
	myBuilder.AddWireEdge(  myWires[WSTART],AxisEdge()    ,Standard_False);
    }

    if (HasTop())  
      myBuilder.AddWireEdge(myWires[WSTART],StartTopEdge()    ,Standard_False);
    myBuilder.AddWireEdge(  myWires[WSTART],StartEdge()       ,Standard_True);

    myBuilder.CompleteWire(myWires[WSTART]);
    WiresBuilt[WSTART] = Standard_True;
  }

  return myWires[WSTART];
}

//=======================================================================
//function : AxisStartWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::AxisStartWire ()
{
  // do it if not done
  if (!WiresBuilt[WAXISSTART]) {

    Standard_DomainError_Raise_if
      (!HasSides(),
       "BRepPrim_OneAxes::AxisStartWire:no sides");
  
    Standard_DomainError_Raise_if
      (!VMaxInfinite() || !VMinInfinite(),
       "BRepPrim_OneAxes::AxisStartWire:not infinite");

    Standard_DomainError_Raise_if
      (MeridianClosed(),
       "BRepPrim_OneAxes::AxisStartWire:meridian closed");
  
    myBuilder.MakeWire(myWires[WAXISSTART]);
  
    myBuilder.AddWireEdge(  myWires[WAXISSTART],AxisEdge()    ,Standard_False);

    myBuilder.CompleteWire(myWires[WAXISSTART]);
    WiresBuilt[WAXISSTART] = Standard_True;
  }

  return myWires[WAXISSTART];
}

//=======================================================================
//function : EndWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::EndWire ()
{
  // do it if not done
  if (!WiresBuilt[WEND]) {

    Standard_DomainError_Raise_if(!HasSides(),
				  "BRepPrim_OneAxes::EndWire:no sides");
  
    myBuilder.MakeWire(myWires[WEND]);
    
    if (HasTop())
      myBuilder.AddWireEdge(myWires[WEND],EndTopEdge(),    Standard_True);
    if (!MeridianClosed()) {
      if (!VMaxInfinite() || !VMinInfinite()) {
	myBuilder.AddWireEdge( myWires[WEND],AxisEdge(),      Standard_True);
      }
    }
    if (HasBottom()) 
      myBuilder.AddWireEdge(myWires[WEND],EndBottomEdge(), Standard_False);
    myBuilder.AddWireEdge(  myWires[WEND],EndEdge(),       Standard_False);

    myBuilder.CompleteWire(myWires[WEND]);
    WiresBuilt[WEND] = Standard_True;
  }
  return myWires[WEND];
}

//=======================================================================
//function : AxisEndWire
//purpose  : 
//=======================================================================

const TopoDS_Wire& BRepPrim_OneAxis::AxisEndWire ()
{
  // do it if not done
  if (!WiresBuilt[WAXISEND]) {

    Standard_DomainError_Raise_if
      (!HasSides(),
       "BRepPrim_OneAxes::AxisEndWire:no sides");
  
    Standard_DomainError_Raise_if
      (!VMaxInfinite() || !VMinInfinite(),
       "BRepPrim_OneAxes::AxisEndWire:not infinite");

    Standard_DomainError_Raise_if
      (MeridianClosed(),
       "BRepPrim_OneAxes::AxisEndWire:meridian closed");
  
    myBuilder.MakeWire(myWires[WAXISEND]);
    
    myBuilder.AddWireEdge( myWires[WAXISEND],AxisEdge(),      Standard_True);

    myBuilder.CompleteWire(myWires[WAXISEND]);
    WiresBuilt[WAXISEND] = Standard_True;
  }
  return myWires[WAXISEND];
}

//=======================================================================
//function : AxisEdge
//purpose  : make the edge on the axis, oriented +Z
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::AxisEdge ()
{
  // do it if not done
  if (!EdgesBuilt[EAXIS]) {

    Standard_DomainError_Raise_if(!HasSides(),
				  "BRepPrim_OneAxis::AxisEdge:no sides");
    Standard_DomainError_Raise_if(MeridianClosed(),
				  "BRepPrim_OneAxis::AxisEdge:closed");

    // build the empty edge.
    myBuilder.MakeEdge(myEdges[EAXIS],gp_Lin(myAxes.Axis()));
    
    if (!VMaxInfinite())
      myBuilder.AddEdgeVertex(myEdges[EAXIS],AxisTopVertex(),
			      MeridianValue(myVMax).Y(),Standard_False);
    if (!VMinInfinite())
      myBuilder.AddEdgeVertex(myEdges[EAXIS],AxisBottomVertex(),
			      MeridianValue(myVMin).Y(),Standard_True);

    myBuilder.CompleteEdge(myEdges[EAXIS]);
    EdgesBuilt[EAXIS] = Standard_True;
  }

  return myEdges[EAXIS];
}

//=======================================================================
//function : StartEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::StartEdge ()
{
  // do it if not done
  if (!EdgesBuilt[ESTART]) {

    // is it shared with the EndEdge

    if (!HasSides() && EdgesBuilt[EEND])
      myEdges[ESTART] = myEdges[EEND];
  
    else {
      // build the empty Edge
      myEdges[ESTART] = MakeEmptyMeridianEdge(0.);
      
      if (MeridianClosed()) {
	// Closed edge
	myBuilder.AddEdgeVertex(myEdges[ESTART],
				TopStartVertex(),
				myVMin+myMeridianOffset,
				myVMax+myMeridianOffset);
      }
      else {
	if (!VMaxInfinite()) {
	  myBuilder.AddEdgeVertex(myEdges[ESTART],
				  TopStartVertex(),
				  myVMax+myMeridianOffset,
				  Standard_False);
	}
	if (!VMinInfinite()) {
	  myBuilder.AddEdgeVertex(myEdges[ESTART],
				  BottomStartVertex(),
				  myVMin+myMeridianOffset,
				  Standard_True);
	}
      }
    }

    myBuilder.CompleteEdge(myEdges[ESTART]);
    EdgesBuilt[ESTART] = Standard_True;
    
  }

  return myEdges[ESTART];
}

//=======================================================================
//function : EndEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::EndEdge ()
{
  // do it if not done
  if (!EdgesBuilt[EEND]) {

    // is it shared with the start edge
    if (!HasSides() && EdgesBuilt[ESTART])
      myEdges[EEND] = myEdges[ESTART];

    else {
      // build the empty Edge
      myEdges[EEND] = MakeEmptyMeridianEdge(myAngle);
      
      
      if (MeridianClosed()) {
	// Closed edge
	myBuilder.AddEdgeVertex(myEdges[EEND],
				TopEndVertex(),
				myVMin+myMeridianOffset,
				myVMax+myMeridianOffset);
      }
      else {
	if (!VMaxInfinite()) {
	  myBuilder.AddEdgeVertex(myEdges[EEND],
				  TopEndVertex(),
				  myVMax+myMeridianOffset,
				  Standard_False);
	}
	if (!VMinInfinite()) {
	  myBuilder.AddEdgeVertex(myEdges[EEND],
				  BottomEndVertex(),
				  myVMin+myMeridianOffset,
				  Standard_True);
	}
      }
    }
    
    myBuilder.CompleteEdge(myEdges[EEND]);
    EdgesBuilt[EEND] = Standard_True;
    
  }
  
  return myEdges[EEND];
}

//=======================================================================
//function : StartTopEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::StartTopEdge ()
{
  // do it if not done
  if (!EdgesBuilt[ETOPSTART]) {

    Standard_DomainError_Raise_if
      (!HasTop() || !HasSides(),
       "BRepPrim_OneAxis::StartTopEdge:no sides or no top");

    // build the empty Edge
    gp_Vec V = myAxes.Direction();
    V.Multiply(MeridianValue(myVMax).Y());
    gp_Pnt P = myAxes.Location().Translated(V);
    myBuilder.MakeEdge(myEdges[ETOPSTART],gp_Lin(P,myAxes.XDirection()));

    myBuilder.AddEdgeVertex(myEdges[ETOPSTART],AxisTopVertex(),
			    0.,Standard_True);
    myBuilder.AddEdgeVertex(myEdges[ETOPSTART],TopStartVertex(),
			    MeridianValue(myVMax).X(),Standard_False);

    myBuilder.CompleteEdge(myEdges[ETOPSTART]);
    EdgesBuilt[ETOPSTART] = Standard_True;
  }

  return myEdges[ETOPSTART];
}

//=======================================================================
//function : StartBottomEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::StartBottomEdge ()
{
  // do it if not done
  if (!EdgesBuilt[EBOTSTART]) {

    Standard_DomainError_Raise_if
      (!HasBottom() || !HasSides(),
       "BRepPrim_OneAxis::StartBottomEdge:no sides or no top");

    // build the empty Edge
    gp_Vec V = myAxes.Direction();
    V.Multiply(MeridianValue(myVMin).Y());
    gp_Pnt P = myAxes.Location().Translated(V);
    myBuilder.MakeEdge(myEdges[EBOTSTART],gp_Lin(P,myAxes.XDirection()));

    myBuilder.AddEdgeVertex(myEdges[EBOTSTART],BottomStartVertex(),
			    MeridianValue(myVMin).X(),Standard_False);
    myBuilder.AddEdgeVertex(myEdges[EBOTSTART],AxisBottomVertex(),
			    0.,Standard_True);

    myBuilder.CompleteEdge(myEdges[EBOTSTART]);
    EdgesBuilt[EBOTSTART] = Standard_True;
  }

  return myEdges[EBOTSTART];
}

//=======================================================================
//function : EndTopEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::EndTopEdge ()
{
  // do it if not done
  if (!EdgesBuilt[ETOPEND]) {

    Standard_DomainError_Raise_if
      (!HasTop() || !HasSides(),
       "BRepPrim_OneAxis::EndTopEdge:no sides or no top");

    // build the empty Edge
    gp_Vec V = myAxes.Direction();
    V.Multiply(MeridianValue(myVMax).Y());
    gp_Pnt P = myAxes.Location().Translated(V);
    gp_Lin L(P,myAxes.XDirection());
    L.Rotate(myAxes.Axis(),myAngle);
    myBuilder.MakeEdge(myEdges[ETOPEND],L);

    myBuilder.AddEdgeVertex(myEdges[ETOPEND],AxisTopVertex(),
			    0.,Standard_True);
    myBuilder.AddEdgeVertex(myEdges[ETOPEND],TopEndVertex(),
			    MeridianValue(myVMax).X(),Standard_False);

    myBuilder.CompleteEdge(myEdges[ETOPEND]);
    EdgesBuilt[ETOPEND] = Standard_True;
  }

  return myEdges[ETOPEND];
}

//=======================================================================
//function : EndBottomEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::EndBottomEdge ()
{
  // do it if not done
  if (!EdgesBuilt[EBOTEND]) {


    Standard_DomainError_Raise_if
      (!HasBottom() || !HasSides(),
       "BRepPrim_OneAxis::EndBottomEdge:no sides or no bottom");

    // build the empty Edge
    gp_Vec V = myAxes.Direction();
    V.Multiply(MeridianValue(myVMin).Y());
    gp_Pnt P = myAxes.Location().Translated(V);
    gp_Lin L(P,myAxes.XDirection());
    L.Rotate(myAxes.Axis(),myAngle);
    myBuilder.MakeEdge(myEdges[EBOTEND],L);

    myBuilder.AddEdgeVertex(myEdges[EBOTEND],AxisBottomVertex(),
			    0.,Standard_True);
    myBuilder.AddEdgeVertex(myEdges[EBOTEND],BottomEndVertex(),
			    MeridianValue(myVMin).X(),Standard_False);

    myBuilder.CompleteEdge(myEdges[EBOTEND]);
    EdgesBuilt[EBOTEND] = Standard_True;
  }
  
  return myEdges[EBOTEND];
}

//=======================================================================
//function : TopEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::TopEdge ()
{
  // do it if not done
  if (!EdgesBuilt[ETOP]) {

    // Test if shared with bottom edge
    if (MeridianClosed() && EdgesBuilt[EBOTTOM]) {
      myEdges[ETOP] = myEdges[EBOTTOM];
    }

    else {

      // build the empty Edge
      if (!MeridianOnAxis(myVMax)) {
	gp_Pnt2d mp = MeridianValue(myVMax);
	gp_Vec V = myAxes.Direction();
	V.Multiply(mp.Y());
	gp_Pnt P = myAxes.Location().Translated(V);
	gp_Circ C(gp_Ax2(P,myAxes.Direction(),myAxes.XDirection()),mp.X());
	myBuilder.MakeEdge(myEdges[ETOP],C);
      }
      else
	myBuilder.MakeDegeneratedEdge(myEdges[ETOP]);
     
      if (!HasSides()) {
	// closed edge
	myBuilder.AddEdgeVertex(myEdges[ETOP],
				TopEndVertex(),
				0.,myAngle);
      }
      else {
	myBuilder.AddEdgeVertex(myEdges[ETOP],
				TopEndVertex(),
				myAngle,
				Standard_False);
	myBuilder.AddEdgeVertex(myEdges[ETOP],
				TopStartVertex(),
				0.,
				Standard_True);
      }
    }

    myBuilder.CompleteEdge(myEdges[ETOP]);
    EdgesBuilt[ETOP] = Standard_True;
  }

  return myEdges[ETOP];
}

//=======================================================================
//function : BottomEdge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepPrim_OneAxis::BottomEdge ()
{
  // do it if not done
  if (!EdgesBuilt[EBOTTOM]) {

    // Test if shared with top edge
    if (MeridianClosed() && EdgesBuilt[ETOP]) {
      myEdges[EBOTTOM] = myEdges[ETOP];
    }

    else {

    // build the empty Edge

      if (!MeridianOnAxis(myVMin)) {
	gp_Pnt2d mp = MeridianValue(myVMin);
	gp_Vec V = myAxes.Direction();
	V.Multiply(mp.Y());
	gp_Pnt P = myAxes.Location().Translated(V);
	gp_Circ C(gp_Ax2(P,myAxes.Direction(),myAxes.XDirection()),mp.X());
	myBuilder.MakeEdge(myEdges[EBOTTOM],C);
      }
      else
	myBuilder.MakeDegeneratedEdge(myEdges[EBOTTOM]);
      
      if (!HasSides()) {
	// closed edge
	myBuilder.AddEdgeVertex(myEdges[EBOTTOM],
				BottomEndVertex(),
				0.,myAngle);
      }
      else {
	myBuilder.AddEdgeVertex(myEdges[EBOTTOM],
				BottomEndVertex(),
				myAngle,
				Standard_False);
	myBuilder.AddEdgeVertex(myEdges[EBOTTOM],
				BottomStartVertex(),
				0.,
				Standard_True);
      }
    }

    myBuilder.CompleteEdge(myEdges[EBOTTOM]);
    EdgesBuilt[EBOTTOM] = Standard_True;
  }

  return myEdges[EBOTTOM];
}

//=======================================================================
//function : AxisTopVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& BRepPrim_OneAxis::AxisTopVertex ()
{
  // do it if not done
  if (!VerticesBuilt[VAXISTOP]) {

    // deduct from others
    if (MeridianOnAxis(myVMax) && VerticesBuilt[VTOPSTART])
      myVertices[VAXISTOP] = myVertices[VTOPSTART];
    
    else if (MeridianOnAxis(myVMax) && VerticesBuilt[VTOPEND])
      myVertices[VAXISTOP] = myVertices[VTOPEND];
    
    else {
      Standard_DomainError_Raise_if(MeridianClosed(),
				    "BRepPrim_OneAxis::AxisTopVertex");
      Standard_DomainError_Raise_if(VMaxInfinite(),
				    "BRepPrim_OneAxis::AxisTopVertex");
      
      gp_Vec V = myAxes.Direction();
      V.Multiply(MeridianValue(myVMax).Y());
      gp_Pnt P = myAxes.Location().Translated(V);
      myBuilder.MakeVertex(myVertices[VAXISTOP],P);
    }

    VerticesBuilt[VAXISTOP] = Standard_True;
  }
  
  return myVertices[VAXISTOP];
}

//=======================================================================
//function : AxisBottomVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& BRepPrim_OneAxis::AxisBottomVertex ()
{
  // do it if not done
  if (!VerticesBuilt[VAXISBOT]) {
    
    // deduct from others
    if (MeridianOnAxis(myVMin) && VerticesBuilt[VBOTSTART])
      myVertices[VAXISBOT] = myVertices[VBOTSTART];
    
    else if (MeridianOnAxis(myVMin) && VerticesBuilt[VBOTEND])
      myVertices[VAXISBOT] = myVertices[VBOTEND];
    
    else {
      Standard_DomainError_Raise_if(MeridianClosed(),
				    "BRepPrim_OneAxis::AxisBottomVertex");
      Standard_DomainError_Raise_if(VMinInfinite(),
				    "BRepPrim_OneAxis::AxisBottomVertex");
      
      gp_Vec V = myAxes.Direction();
      V.Multiply(MeridianValue(myVMin).Y());
      gp_Pnt P = myAxes.Location().Translated(V);
      myBuilder.MakeVertex(myVertices[VAXISBOT],P);
    }      
    
    VerticesBuilt[VAXISBOT] = Standard_True;
  }
  
  return myVertices[VAXISBOT];
}

//=======================================================================
//function : TopStartVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& BRepPrim_OneAxis::TopStartVertex ()
{
  // do it if not done
  if (!VerticesBuilt[VTOPSTART]) {

    // deduct from others
    if (MeridianOnAxis(myVMax) && VerticesBuilt[VAXISTOP])
      myVertices[VTOPSTART] = myVertices[VAXISTOP];
    else if ((MeridianOnAxis(myVMax) || !HasSides()) && VerticesBuilt[VTOPEND])
      myVertices[VTOPSTART] = myVertices[VTOPEND];
    else if (MeridianClosed() && VerticesBuilt[VBOTSTART])
      myVertices[VTOPSTART] = myVertices[VBOTSTART];
    else if ((MeridianClosed() && !HasSides()) && VerticesBuilt[VBOTEND])
      myVertices[VTOPSTART] = myVertices[VBOTEND];
    
    else{
      gp_Pnt2d mp = MeridianValue(myVMax);
      gp_Vec V = myAxes.Direction();
      V.Multiply(mp.Y());
      gp_Pnt P = myAxes.Location().Translated(V);
      V = myAxes.XDirection();
      V.Multiply(mp.X());
      P.Translate(V);
      myBuilder.MakeVertex(myVertices[VTOPSTART],P);
    }
      
    VerticesBuilt[VTOPSTART] = Standard_True;
  }
  
  return myVertices[VTOPSTART];
}

//=======================================================================
//function : TopEndVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& BRepPrim_OneAxis::TopEndVertex ()
{
  // do it if not done
  if (!VerticesBuilt[VTOPEND]) {
    

    // deduct from others
    if (MeridianOnAxis(myVMax) && VerticesBuilt[VAXISTOP])
      myVertices[VTOPEND] = myVertices[VAXISTOP];
    else if ((MeridianOnAxis(myVMax) || !HasSides()) && VerticesBuilt[VTOPSTART])
      myVertices[VTOPEND] = myVertices[VTOPSTART];
    else if (MeridianClosed() && VerticesBuilt[VBOTEND])
      myVertices[VTOPEND] = myVertices[VBOTEND];
    else if ((MeridianClosed() && !HasSides()) && VerticesBuilt[VBOTSTART])
      myVertices[VTOPEND] = myVertices[VBOTSTART];

    else {
      gp_Pnt2d mp = MeridianValue(myVMax);
      gp_Vec V = myAxes.Direction();
      V.Multiply(mp.Y());
      gp_Pnt P = myAxes.Location().Translated(V);
      V = myAxes.XDirection();
      V.Multiply(mp.X());
      P.Translate(V);
      P.Rotate(myAxes.Axis(),myAngle);
      myBuilder.MakeVertex(myVertices[VTOPEND],P);
    }
    
    VerticesBuilt[VTOPEND] = Standard_True;
  }

  return myVertices[VTOPEND];
}

//=======================================================================
//function : BottomStartVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& BRepPrim_OneAxis::BottomStartVertex ()
{
  // do it if not done
  if (!VerticesBuilt[VBOTSTART]) {
    
    // deduct from others
    if (MeridianOnAxis(myVMin) && VerticesBuilt[VAXISBOT])
      myVertices[VBOTSTART] = myVertices[VAXISBOT];
    else if ((MeridianOnAxis(myVMin) || !HasSides()) && VerticesBuilt[VBOTEND])
      myVertices[VBOTSTART] = myVertices[VBOTEND];
    else if (MeridianClosed() && VerticesBuilt[VTOPSTART])
      myVertices[VBOTSTART] = myVertices[VTOPSTART];
    else if ((MeridianClosed() && !HasSides()) && VerticesBuilt[VTOPEND])
      myVertices[VBOTSTART] = myVertices[VTOPEND];

    else {
      gp_Pnt2d mp = MeridianValue(myVMin);
      gp_Vec V = myAxes.Direction();
      V.Multiply(mp.Y());
      gp_Pnt P = myAxes.Location().Translated(V);
      V = myAxes.XDirection();
      V.Multiply(mp.X());
      P.Translate(V);
      myBuilder.MakeVertex(myVertices[VBOTSTART],P);
    }
    
    VerticesBuilt[VBOTSTART] = Standard_True;
  }

  return myVertices[VBOTSTART];
}

//=======================================================================
//function : BottomEndVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex& BRepPrim_OneAxis::BottomEndVertex ()
{
  // do it if not done
  if (!VerticesBuilt[VBOTEND]) {
    
    // deduct from others
    if (MeridianOnAxis(myVMin) && VerticesBuilt[VAXISBOT])
      myVertices[VBOTEND] = myVertices[VAXISBOT];
    else if ((MeridianOnAxis(myVMin) || !HasSides()) && VerticesBuilt[VBOTSTART])
      myVertices[VBOTEND] = myVertices[VBOTSTART];
    else if (MeridianClosed() && VerticesBuilt[VTOPEND])
      myVertices[VBOTEND] = myVertices[VTOPEND];
    else if (MeridianClosed() && !HasSides() && VerticesBuilt[VTOPSTART])
      myVertices[VBOTEND] = myVertices[VTOPSTART];

    else {
      gp_Pnt2d mp = MeridianValue(myVMin);
      gp_Vec V = myAxes.Direction();
      V.Multiply(mp.Y());
      gp_Pnt P = myAxes.Location().Translated(V);
      V = myAxes.XDirection();
      V.Multiply(mp.X());
      P.Translate(V);
      P.Rotate(myAxes.Axis(),myAngle);
      myBuilder.MakeVertex(myVertices[VBOTEND],P);
    }
    
    VerticesBuilt[VBOTEND] = Standard_True;
  }

  return myVertices[VBOTEND];
}
