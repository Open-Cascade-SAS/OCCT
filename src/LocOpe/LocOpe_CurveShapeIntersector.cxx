// File:	LocOpe_CurveShapeIntersector.cxx
// Created:	Mon May 29 16:10:18 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <LocOpe_CurveShapeIntersector.ixx>

#include <BRepIntCurveSurface_Inter.hxx>
#include <Geom_Circle.hxx>

#include <LocOpe_PntFace.hxx>


#include <Precision.hxx>


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
  GeomAdaptor_Curve AC(GC,0.,2.*PI);

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
    StdFail_NotDone::Raise();
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
    StdFail_NotDone::Raise();
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
    StdFail_NotDone::Raise();
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
    StdFail_NotDone::Raise();
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
#ifdef DEB
  TopAbs_Orientation theor, orface;
#else
  TopAbs_Orientation theor=TopAbs_FORWARD, orface;
#endif

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
