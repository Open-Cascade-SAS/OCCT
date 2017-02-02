// Created on: 1994-02-07
// Created by: Modelistation
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


#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepIntCurveSurface_Inter.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <StdFail_NotDone.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//===========================================================================
//function :BRepIntCurveSurface_Inter::BRepIntCurveSurface_Inte
//purpose  : 
//===========================================================================
BRepIntCurveSurface_Inter::BRepIntCurveSurface_Inter()
{
  myFastClass = new  BRepTopAdaptor_TopolTool();
  Clear();
}

//===========================================================================
//function :Init
//purpose  : 
//===========================================================================

void BRepIntCurveSurface_Inter::Init(const TopoDS_Shape& theShape,
				     const GeomAdaptor_Curve& theCurve,
				     const Standard_Real theTol) 
{ 
  Load(theShape, theTol);
  Init(theCurve);
}

//===========================================================================
//function :Init
//purpose  : 
//===========================================================================

void BRepIntCurveSurface_Inter::Init(const TopoDS_Shape& theShape,
				     const gp_Lin&       theLine,
				     const Standard_Real theTol) 
{ 
 
  Handle(Geom_Line) geomline = new Geom_Line(theLine);
  GeomAdaptor_Curve aCurve(geomline);
  Load(theShape, theTol);
  Init(aCurve);
  
}

//===========================================================================
//function :Clear
//purpose  : 
//===========================================================================

void BRepIntCurveSurface_Inter::Clear()
{  
  myCurrentindex = 0;
  myCurrentnbpoints = 0;
  myIndFace = 0;
  myCurrentstate = TopAbs_UNKNOWN;
  myCurrentU = 0;
  myCurrentV = 0;

}

//===========================================================================
//function :Load
//purpose  : 
//===========================================================================

void BRepIntCurveSurface_Inter::Load(const TopoDS_Shape& theShape ,const Standard_Real theTol)
{
  Clear();
  myFaces.Clear();
  myFaceBoxes.Nullify();
  myTolerance = theTol;
  TopExp_Explorer explorer(theShape,TopAbs_FACE);
  for( ; explorer.More(); explorer.Next())
    myFaces.Append(explorer.Current());

}

//===========================================================================
//function :Init
//purpose  : 
//===========================================================================

void BRepIntCurveSurface_Inter::Init(const GeomAdaptor_Curve& theCurve )
{
  Clear();
  myCurveBox.SetVoid();
  Standard_Real aFirst =  theCurve.FirstParameter();
  Standard_Real aLast =   theCurve.LastParameter();
  myCurve = new GeomAdaptor_HCurve(theCurve );
  if( !Precision::IsInfinite(aFirst) && !Precision::IsInfinite(aLast) )
    BndLib_Add3dCurve::Add(myCurve->Curve(),0., myCurveBox);
  Find();
}

//===========================================================================
//function :More
//purpose  : 
//===========================================================================
Standard_Boolean BRepIntCurveSurface_Inter::More() const 
{
  return (myIndFace <= myFaces.Length()  );
}

//===========================================================================
//function :Next
//purpose  : 
//===========================================================================
void BRepIntCurveSurface_Inter::Next() 
{ 
  if(myCurrentnbpoints)
    myCurrentindex++;
  Find();
}

//===========================================================================
//function :Find
//purpose  : 
//===========================================================================
void BRepIntCurveSurface_Inter::Find() 
{ 
  if(myCurrentnbpoints && myCurrentindex <= myCurrentnbpoints && FindPoint())
    return;
  
  myCurrentnbpoints = 0;
  myCurrentindex = 0;

  Standard_Integer i = myIndFace +1;
  for( ; i <= myFaces.Length(); i++)
  {
    TopoDS_Shape aCurface= myFaces(i);       
    if( myFaceBoxes.IsNull())
      myFaceBoxes = new Bnd_HArray1OfBox(1, myFaces.Length());
    Bnd_Box& aFaceBox  = myFaceBoxes->ChangeValue(i);
    if( aFaceBox.IsVoid())
    {
      BRepBndLib::Add(aCurface, aFaceBox);
      aFaceBox.SetGap(myTolerance);//Precision::Confusion());
    }
    Standard_Boolean isOut = ( myCurve->GetType() == GeomAbs_Line ? aFaceBox.IsOut(myCurve->Line()) :
      ( !myCurveBox.IsVoid() ? aFaceBox.IsOut(myCurveBox ) : Standard_False ) );
    if(isOut )
      continue;
    Handle(BRepAdaptor_HSurface) aSurfForFastClass = new BRepAdaptor_HSurface(TopoDS::Face(aCurface));
    myIntcs.Perform(myCurve,aSurfForFastClass);
    myCurrentnbpoints = myIntcs.NbPoints();
    if( !myCurrentnbpoints)
      continue;

    const Handle(Adaptor3d_HSurface)& aSurf = aSurfForFastClass; // to avoid ambiguity
    myFastClass->Initialize(aSurf);
    myIndFace = i;
    if(FindPoint())
      return; 
    myCurrentnbpoints = 0;
  }

  if(!myCurrentnbpoints && i > myFaces.Length())
  {
    myIndFace = i;
    return;
  }
}

//===========================================================================
//function :FindPoint
//purpose  : 
//===========================================================================
Standard_Boolean BRepIntCurveSurface_Inter::FindPoint()
{
  Standard_Integer j =  (!myCurrentindex ?  1 : myCurrentindex);
  
  for( ; j <= myCurrentnbpoints; j++ )
  {
    Standard_Real anU = myIntcs.Point(j).U();
    Standard_Real aV = myIntcs.Point(j).V();
  
    gp_Pnt2d Puv( anU,aV );

    myCurrentstate = myFastClass->Classify(Puv,myTolerance); 
    if(myCurrentstate == TopAbs_ON || myCurrentstate == TopAbs_IN) 
    { 
      myCurrentindex = j;
      myCurrentU = anU;
      myCurrentV = aV;	
      return Standard_True; 
    }
  }
  return Standard_False;
}

//===========================================================================
//function :Point
//purpose  : 
//===========================================================================

IntCurveSurface_IntersectionPoint BRepIntCurveSurface_Inter::Point() const 
{
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  const IntCurveSurface_IntersectionPoint& ICPS = myIntcs.Point(myCurrentindex);
  return(IntCurveSurface_IntersectionPoint(ICPS.Pnt(),
					   myCurrentU,     // ICPS.U(),
					   myCurrentV,     // ICPS.V(),
					   ICPS.W(),
					   ICPS.Transition()));
  //-- return(myIntcs.Point(myCurrentindex));
}

//===========================================================================
//function :U
//purpose  : 
//===========================================================================
Standard_Real BRepIntCurveSurface_Inter::U() const 
{
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  //-- return(myIntcs.Point(myCurrentindex).U());
  return(myCurrentU);
}

//===========================================================================
//function :V
//purpose  : 
//===========================================================================
Standard_Real BRepIntCurveSurface_Inter::V() const 
{
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  //-- return(myIntcs.Point(myCurrentindex).V());
  return(myCurrentV);
}

//===========================================================================
//function :W
//purpose  : 
//===========================================================================
Standard_Real BRepIntCurveSurface_Inter::W() const 
{
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  return(myIntcs.Point(myCurrentindex).W());
}

//===========================================================================
//function :State
//purpose  : 
//===========================================================================
TopAbs_State BRepIntCurveSurface_Inter::State() const 
{
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  //-- return(classifier.State());
  return(myCurrentstate);
}

//===========================================================================
//function :Transition
//purpose  : 
//===========================================================================
IntCurveSurface_TransitionOnCurve BRepIntCurveSurface_Inter::Transition() const 
{
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  return(myIntcs.Point(myCurrentindex).Transition());
}

//===========================================================================
//function :Face
//purpose  : 
//===========================================================================
const TopoDS_Face& BRepIntCurveSurface_Inter::Face() const 
{ 
  return(TopoDS::Face(myFaces.Value(myIndFace)));
}

//===========================================================================
//function :Pnt
//purpose  : 
//===========================================================================
const gp_Pnt& BRepIntCurveSurface_Inter::Pnt() const { 
  if(myCurrentindex==0) 
    throw StdFail_NotDone();
  return(myIntcs.Point(myCurrentindex).Pnt());
}

