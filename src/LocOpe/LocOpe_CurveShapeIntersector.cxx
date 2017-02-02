// Created on: 1995-05-29
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


#include <BRepIntCurveSurface_Inter.hxx>
#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <LocOpe_CurveShapeIntersector.hxx>
#include <LocOpe_PntFace.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Shape.hxx>

static void Perform(BRepIntCurveSurface_Inter&,
		    LocOpe_SequenceOfPntFace&);


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void LocOpe_CurveShapeIntersector::Init(const gp_Ax1& Axis,
					const TopoDS_Shape& S)
{
  myDone = Standard_False;
  myPoints.Clear();
  if (S.IsNull()) {
    return ;
  }
  Standard_Real Tol = Precision::Confusion();

  BRepIntCurveSurface_Inter theInt;
  theInt.Init(S,gp_Lin(Axis),Tol);
  Perform(theInt,myPoints);
  myDone = Standard_True;
}
    

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void LocOpe_CurveShapeIntersector::Init(const gp_Circ& C,
					const TopoDS_Shape& S)
{
  myDone = Standard_False;
  myPoints.Clear();
  if (S.IsNull()) {
    return ;
  }
  Standard_Real Tol = Precision::Confusion();

  Handle(Geom_Circle) GC = new Geom_Circle(C);
  GeomAdaptor_Curve AC(GC,0.,2.*M_PI);

  BRepIntCurveSurface_Inter theInt;
  theInt.Init(S,AC,Tol);

  Perform(theInt,myPoints);
  myDone = Standard_True;
}
    

//=======================================================================
//function : LocalizeAfter
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_CurveShapeIntersector::LocalizeAfter
   (const Standard_Real From,
    TopAbs_Orientation& Or,
    Standard_Integer& IndFrom,
    Standard_Integer& IndTo) const
{
  if (!myDone) {
    throw StdFail_NotDone();
  }
  Standard_Real Eps = Precision::Confusion();
  Standard_Real param,FMEPS = From - Eps;
  Standard_Integer i,ifirst,nbpoints = myPoints.Length();
  for (ifirst=1; ifirst<=nbpoints; ifirst++) {
    if (myPoints(ifirst).Parameter() >= FMEPS) {
      break;
    }
  }
  Standard_Boolean RetVal = Standard_False;
  if (ifirst <= nbpoints) {
    i = ifirst;
    IndFrom = ifirst;
    Standard_Boolean found = Standard_False;
    while (!found) {
      Or = myPoints(i).Orientation();
      param = myPoints(i).Parameter();
      i = i+1;
      while (i<=nbpoints) {
	if (myPoints(i).Parameter()-param <= Eps) {
	  if (Or != TopAbs_EXTERNAL && Or != myPoints(i).Orientation()) {
	    Or = TopAbs_EXTERNAL;
	  }
	  i++;
	}
	else {
	  break;
	}
      }
      if (Or == TopAbs_EXTERNAL) {
	found = (i > nbpoints);
	IndFrom = i;
      }
      else { // on a une intersection franche
	IndTo = i-1;
	found = Standard_True;
	RetVal = Standard_True;
      }
    }
  }

  return RetVal;
}

//=======================================================================
//function : LocalizeBefore
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_CurveShapeIntersector::LocalizeBefore
   (const Standard_Real From,
     TopAbs_Orientation& Or,
    Standard_Integer& IndFrom,
    Standard_Integer& IndTo) const
{
  if (!myDone) {
    throw StdFail_NotDone();
  }
  Standard_Real Eps = Precision::Confusion();
  Standard_Real param,FPEPS = From + Eps;
  Standard_Integer i,ifirst,nbpoints = myPoints.Length();
  for (ifirst=nbpoints; ifirst>=1; ifirst--) {
    if (myPoints(ifirst).Parameter() <= FPEPS) {
      break;
    }
  }
  Standard_Boolean RetVal = Standard_False;
  if (ifirst >= 1) {
    i = ifirst;
    IndTo = ifirst;
    Standard_Boolean found = Standard_False;
    while (!found) {
      Or = myPoints(i).Orientation();
      param = myPoints(i).Parameter();
      i = i-1;
      while (i>=1) {
	if (param - myPoints(i).Parameter() <= Eps) {
	  if (Or != TopAbs_EXTERNAL && Or != myPoints(i).Orientation()) {
	    Or = TopAbs_EXTERNAL;
	  }
	  i--;
	}
	else {
	  break;
	}
      }
      if (Or == TopAbs_EXTERNAL) {
	found = (i < 1);
	IndTo = i;
      }
      else { // on a une intersection franche
	IndFrom = i+1;
	found = Standard_True;
	RetVal = Standard_True;
      }
    }
  }

  return RetVal;
}

//=======================================================================
//function : LocalizeAfter
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_CurveShapeIntersector::LocalizeAfter
   (const Standard_Integer FromInd,
    TopAbs_Orientation& Or,
    Standard_Integer& IndFrom,
    Standard_Integer& IndTo) const
{
  if (!myDone) {
    throw StdFail_NotDone();
  }
  Standard_Integer nbpoints = myPoints.Length();
  if (FromInd >= nbpoints) {
    return Standard_False;
  }
 
  Standard_Real Eps = Precision::Confusion();
  Standard_Real param,FMEPS;
  Standard_Integer i,ifirst;
  if (FromInd >= 1) {
    FMEPS = myPoints(FromInd).Parameter() - Eps;
    for (ifirst=FromInd+1; ifirst<=nbpoints; ifirst++) {
      if (myPoints(ifirst).Parameter() >= FMEPS) {
	break;
      }
    }
  }
  else {
    ifirst = 1;
  }

  Standard_Boolean RetVal = Standard_False;
  if (ifirst <= nbpoints) {
    i = ifirst;
    IndFrom = ifirst;
    Standard_Boolean found = Standard_False;
    while (!found) {
      Or = myPoints(i).Orientation();
      param = myPoints(i).Parameter();
      i = i+1;
      while (i<=nbpoints) {
	if (myPoints(i).Parameter()-param <= Eps) {
	  if (Or != TopAbs_EXTERNAL && Or != myPoints(i).Orientation()) {
	    Or = TopAbs_EXTERNAL;
	  }
	  i++;
	}
	else {
	  break;
	}
      }
      if (Or == TopAbs_EXTERNAL) {
	found = (i > nbpoints);
	IndFrom = i;
      }
      else { // on a une intersection franche
	IndTo = i-1;
	found = Standard_True;
	RetVal = Standard_True;
      }
    }
  }
  return RetVal;
}


//=======================================================================
//function : LocalizeBefore
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe_CurveShapeIntersector::LocalizeBefore
   (const Standard_Integer FromInd,
     TopAbs_Orientation& Or,
    Standard_Integer& IndFrom,
    Standard_Integer& IndTo) const
{
  if (!myDone) {
    throw StdFail_NotDone();
  }
  Standard_Integer nbpoints = myPoints.Length();
  if (FromInd <= 1) {
    return Standard_False;
  }
 
  Standard_Real Eps = Precision::Confusion();
  Standard_Real param,FPEPS;
  Standard_Integer i,ifirst;
  if (FromInd <= nbpoints) {
    FPEPS = myPoints(FromInd).Parameter() + Eps;
    for (ifirst=FromInd-1; ifirst>=1; ifirst--) {
      if (myPoints(ifirst).Parameter() <= FPEPS) {
	break;
      }
    }
  }
  else {
    ifirst = nbpoints;
  }

  Standard_Boolean RetVal = Standard_False;
  if (ifirst >= 1) {
    i = ifirst;
    IndTo = ifirst;
    Standard_Boolean found = Standard_False;
    while (!found) {
      Or = myPoints(i).Orientation();
      param = myPoints(i).Parameter();
      i = i-1;
      while (i>=1) {
	if (param - myPoints(i).Parameter() <= Eps) {
	  if (Or != TopAbs_EXTERNAL && Or != myPoints(i).Orientation()) {
	    Or = TopAbs_EXTERNAL;
	  }
	  i--;
	}
	else {
	  break;
	}
      }
      if (Or == TopAbs_EXTERNAL) {
	found = (i < 1);
	IndTo = i;
      }
      else { // on a une intersection franche
	IndFrom = i+1;
	found = Standard_True;
	RetVal = Standard_True;
      }
    }
  }
  return RetVal;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

static void Perform(BRepIntCurveSurface_Inter & theInt,
		    LocOpe_SequenceOfPntFace& thePoints)
{
  Standard_Real param,paramu,paramv;
  Standard_Integer i, nbpoints=0;

  TopAbs_Orientation theor=TopAbs_FORWARD, orface;

  while (theInt.More()) {
    const gp_Pnt& thept = theInt.Pnt();
    const TopoDS_Face& theface = theInt.Face();
    orface = theface.Orientation();
    param = theInt.W();
    paramu = theInt.U();
    paramv = theInt.V();

    switch (theInt.Transition()) {
    case IntCurveSurface_In:
      if ( orface == TopAbs_FORWARD) {
	theor = TopAbs_FORWARD;
      }
      else if (orface == TopAbs_REVERSED) {
	theor = TopAbs_REVERSED;
      }
      else {
	theor = TopAbs_EXTERNAL;
      }
      break;
    case IntCurveSurface_Out:
      if ( orface == TopAbs_FORWARD) {
	theor = TopAbs_REVERSED;
      }
      else if (orface == TopAbs_REVERSED) {
	theor = TopAbs_FORWARD;
      }
      else {
	theor = TopAbs_EXTERNAL;
      }
      break;
    case IntCurveSurface_Tangent:
      theor = TopAbs_EXTERNAL;
      break;

    }

    LocOpe_PntFace newpt(thept,theface,theor,param,paramu,paramv);

    for (i=1; i <= nbpoints; i++) {
      if (thePoints(i).Parameter() - param > 0.) {
	break;
      }
    }
    if (i <= nbpoints) {
      thePoints.InsertBefore(i,newpt);
    }
    else {
      thePoints.Append(newpt);
    }
    nbpoints++;
    theInt.Next();
  }
}
