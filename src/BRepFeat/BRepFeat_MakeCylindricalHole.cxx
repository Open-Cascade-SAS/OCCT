// File:	LocOpe_MakeCylindricalHole.cxx
// Created:	Tue May 30 09:41:56 1995
// Author:	Jacques GOUSSARD
//		<jag@bravox>


#include <BRepFeat_MakeCylindricalHole.ixx>


#include <LocOpe_CurveShapeIntersector.hxx>
#include <LocOpe_PntFace.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrim_Cylinder.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>

#include <Geom_Curve.hxx>

#include <ElCLib.hxx>
#include <BRepTools.hxx>
#include <TopoDS.hxx>
#include <Precision.hxx>
#include <BRepBndLib.hxx>



static void Baryc(const TopoDS_Shape&,
		  gp_Pnt&);

static void BoxParameters(const TopoDS_Shape&,
			  const gp_Ax1&,
			  Standard_Real&,
			  Standard_Real&);


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFeat_MakeCylindricalHole::Perform(const Standard_Real Radius)
{
  if (myBuilder.OriginalShape().IsNull() || !myAxDef) {
    Standard_ConstructionError::Raise();
  }

  myIsBlind = Standard_False;
  myStatus = BRepFeat_NoError;

  LocOpe_CurveShapeIntersector theASI(myAxis,myBuilder.OriginalShape());
  if (!theASI.IsDone() || theASI.NbPoints() <= 0) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }
  
  TopTools_ListOfShape theList;
  for (Standard_Integer i=1; i<= theASI.NbPoints(); i++) {
    theList.Append(theASI.Point(i).Face());
  }

  // On ne peut pas utiliser de cylindre infini pour les ope topologiques...
  Standard_Real PMin,PMax;
  BoxParameters(myBuilder.OriginalShape(),myAxis,PMin,PMax);
  Standard_Real Heigth = 2.*(PMax-PMin);
  gp_XYZ theOrig = myAxis.Location().XYZ();
  theOrig += ((3.*PMin-PMax)/2.) * myAxis.Direction().XYZ();
  gp_Pnt p1_ao1(theOrig); gp_Ax2 a1_ao1(p1_ao1,myAxis.Direction());
  BRepPrim_Cylinder theCylinder(a1_ao1,
				Radius,
				Heigth);

  // Peut-etre vaudrait-il mieux faire directement une coupe

  BRep_Builder B;
  TopoDS_Solid theTool;
  B.MakeSolid(theTool);
  B.Add(theTool,theCylinder.Shell());
  theTool.Closed(Standard_True);

  myTopFace = theCylinder.TopFace();
  myBotFace = theCylinder.BottomFace();
  myValidate = Standard_False;

//  BRepTools::Dump(theTool,cout);
  Standard_Boolean Fuse = Standard_False;
  myBuilder.Perform(theTool,theList,Fuse);
//  myBuilder.PerformResult();
}


//=======================================================================
//function : PerformThruNext
//purpose  : 
//=======================================================================

void BRepFeat_MakeCylindricalHole::PerformThruNext(const Standard_Real Radius,
						   const Standard_Boolean Cont)
{

  if (myBuilder.OriginalShape().IsNull() || !myAxDef) {
    Standard_ConstructionError::Raise();
  }

  myIsBlind = Standard_False;
  myValidate = Cont;
  myStatus = BRepFeat_NoError;

  LocOpe_CurveShapeIntersector theASI(myAxis,myBuilder.OriginalShape());
  if (!theASI.IsDone()) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }
  
  Standard_Real First=0.,Last=0.,prm;
  Standard_Integer IndFrom,IndTo;
  TopAbs_Orientation theOr;
  Standard_Boolean ok = theASI.LocalizeAfter(0.,theOr,IndFrom,IndTo);
  if (ok) {
    if (theOr == TopAbs_FORWARD) {
      First = theASI.Point(IndFrom).Parameter();
      ok = theASI.LocalizeAfter(IndTo,theOr,IndFrom,IndTo);
      if (ok) {
	if (theOr != TopAbs_REVERSED) {
	  ok = Standard_False;
	}
	else {
	  Last = theASI.Point(IndTo).Parameter();
	}
      }
      
    }
    else { // TopAbs_REVERSED
      Last = theASI.Point(IndTo).Parameter();
      ok = theASI.LocalizeBefore(IndFrom,theOr,IndFrom,IndTo);
      if (ok) {
	if (theOr != TopAbs_FORWARD) {
	  ok = Standard_False;
	}
	else {
	  First = theASI.Point(IndFrom).Parameter();
	}
      }
    }
  }
  if (!ok) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  TopTools_ListOfShape theList;
  for (Standard_Integer i=1; i<= theASI.NbPoints(); i++) {
    prm = theASI.Point(i).Parameter();
    if (prm >= First && prm <= Last) {
      theList.Append(theASI.Point(i).Face());
    }
    else if (prm > Last) {
      break;
    }
  }

  // On ne peut pas utiliser de cylindre infini pour les ope topologiques...
  Standard_Real PMin,PMax;
  BoxParameters(myBuilder.OriginalShape(),myAxis,PMin,PMax);
  Standard_Real Heigth = 2.*(PMax-PMin);
  gp_XYZ theOrig = myAxis.Location().XYZ();
  theOrig += ((3.*PMin-PMax)/2.) * myAxis.Direction().XYZ();
  gp_Pnt p2_ao1(theOrig); gp_Ax2 a2_ao1(p2_ao1,myAxis.Direction());
  BRepPrim_Cylinder theCylinder(a2_ao1,
				Radius,
				Heigth);

  BRep_Builder B;
  TopoDS_Solid theTool;
  B.MakeSolid(theTool);
  B.Add(theTool,theCylinder.Shell());
  theTool.Closed(Standard_True);

  myTopFace = theCylinder.TopFace();
  myBotFace = theCylinder.BottomFace();

  //  BRepTools::Dump(theTool,cout);
  Standard_Boolean Fuse = Standard_False;
  myBuilder.Perform(theTool,theList,Fuse);
  myBuilder.BuildPartsOfTool();


  Standard_Integer nbparts = 0;
  TopTools_ListIteratorOfListOfShape its(myBuilder.PartsOfTool());
  for (; its.More(); its.Next()) {
    nbparts ++;
  }
  if (nbparts == 0) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  if (nbparts >= 2) { // on garde la plus petite en parametre 
                      // le long de l`axe
    TopoDS_Shape tokeep;
    Standard_Real parbar,parmin = Last;
    gp_Pnt Barycentre;
    for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
      Baryc(its.Value(),Barycentre);
      parbar = ElCLib::LineParameter(myAxis,Barycentre);
      if (parbar >= First && parbar <= Last && parbar <= parmin) {
	parmin = parbar;
	tokeep = its.Value();
      }
    }

    if (tokeep.IsNull()) { // on garde le plus proche de l`intervalle

      Standard_Real dmin = RealLast();
      for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
	Baryc(its.Value(),Barycentre);
	parbar = ElCLib::LineParameter(myAxis,Barycentre);
	if (parbar < First) {
	  if (First - parbar < dmin ) {
	    dmin = First-parbar;
	    tokeep = its.Value();
	  }
	  else { // parbar > Last
	    if (parbar - Last < dmin) {
	      dmin = parbar-Last;
	      tokeep = its.Value();
	    }
	  }
	}
      }
    }
    for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
      if (!tokeep.IsSame(its.Value())) {
	myBuilder.RemovePart(its.Value());
      }
    }
  }
//  myBuilder.PerformResult();
}

//=======================================================================
//function : PerformUntilEnd
//purpose  : 
//=======================================================================

void BRepFeat_MakeCylindricalHole::PerformUntilEnd(const Standard_Real Radius,
						   const Standard_Boolean Cont)
{

  if (myBuilder.OriginalShape().IsNull() || !myAxDef) {
    Standard_ConstructionError::Raise();
  }

  myIsBlind = Standard_False;
  myValidate = Cont;
  myStatus = BRepFeat_NoError;

  LocOpe_CurveShapeIntersector theASI(myAxis,myBuilder.OriginalShape());
  if (!theASI.IsDone()) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }
  
  Standard_Real First=0,Last=0,prm;
  Standard_Integer IndFrom,IndTo;
  TopAbs_Orientation theOr;
  Standard_Boolean ok = theASI.LocalizeAfter(0.,theOr,IndFrom,IndTo);
  
  if (ok) {
    if (theOr == TopAbs_REVERSED) {
      ok = theASI.LocalizeBefore(IndFrom,theOr,IndFrom,IndTo); // on remonte
      // On pourrait aller chercher la suivante...
    }
    if ( ok && theOr == TopAbs_FORWARD) {
      First = theASI.Point(IndFrom).Parameter();
      ok = theASI.LocalizeBefore(theASI.NbPoints()+1,theOr,IndFrom,IndTo);
      if (ok) {
	if (theOr != TopAbs_REVERSED) {
	  ok = Standard_False;
	}
	else {
	  Last = theASI.Point(IndTo).Parameter();
	}
      }
    }
  }
  if (!ok) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  TopTools_ListOfShape theList;
  for (Standard_Integer i=1; i<= theASI.NbPoints(); i++) {
    prm = theASI.Point(i).Parameter();
    if (prm >= First && prm <= Last) {
      theList.Append(theASI.Point(i).Face());
    }
    else if (prm > Last) {
      break;
    }
  }

  // On ne peut pas utiliser de cylindre infini pour les ope topologiques...
  Standard_Real PMin,PMax;
  BoxParameters(myBuilder.OriginalShape(),myAxis,PMin,PMax);
  Standard_Real Heigth = 2.*(PMax-PMin);
  gp_XYZ theOrig = myAxis.Location().XYZ();
  theOrig += ((3.*PMin-PMax)/2.) * myAxis.Direction().XYZ();
  gp_Pnt p3_ao1(theOrig); gp_Ax2 a3_ao1(p3_ao1,myAxis.Direction());
  BRepPrim_Cylinder theCylinder(a3_ao1,
				Radius,
				Heigth);

  BRep_Builder B;
  TopoDS_Solid theTool;
  B.MakeSolid(theTool);
  B.Add(theTool,theCylinder.Shell());
  theTool.Closed(Standard_True);

  myTopFace = theCylinder.TopFace();
  myBotFace = theCylinder.BottomFace();

  //  BRepTools::Dump(theTool,cout);
  Standard_Boolean Fuse = Standard_False;
  myBuilder.Perform(theTool,theList,Fuse);
  myBuilder.BuildPartsOfTool();


  Standard_Integer nbparts = 0;
  TopTools_ListIteratorOfListOfShape its(myBuilder.PartsOfTool());
  for (; its.More(); its.Next()) {
    nbparts ++;
  }
  if (nbparts == 0) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  if (nbparts >= 2) { // on garde tout ce qui est au dessus de First 
    Standard_Real parbar;
    gp_Pnt Barycentre;
    for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
      Baryc(its.Value(),Barycentre);
      parbar = ElCLib::LineParameter(myAxis,Barycentre);
      if (parbar < First) {
	myBuilder.RemovePart(its.Value());
      }
    }
  }
//  myBuilder.PerformResult();
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFeat_MakeCylindricalHole::Perform(const Standard_Real Radius,
					   const Standard_Real PFrom,
					   const Standard_Real PTo,
					   const Standard_Boolean Cont)
{

  if (myBuilder.OriginalShape().IsNull() || !myAxDef) {
    Standard_ConstructionError::Raise();
  }

  myIsBlind = Standard_False;
  myValidate = Cont;
  myStatus = BRepFeat_NoError;

  LocOpe_CurveShapeIntersector theASI(myAxis,myBuilder.OriginalShape());
  if (!theASI.IsDone()) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  Standard_Real thePFrom,thePTo;
  if (PFrom < PTo) {
    thePFrom = PFrom;
    thePTo   = PTo;
  }
  else {
    thePFrom = PTo;
    thePTo   = PFrom;
  }

  Standard_Real First=0,Last=0,prm;
  Standard_Integer IndFrom,IndTo;
  TopAbs_Orientation theOr;
  Standard_Boolean ok = theASI.LocalizeAfter(thePFrom,theOr,IndFrom,IndTo);
  if (ok) {
    if (theOr == TopAbs_REVERSED) {
      ok = theASI.LocalizeBefore(IndFrom,theOr,IndFrom,IndTo); // on remonte
      // On pourrait aller chercher la suivante...
    }
    if ( ok && theOr == TopAbs_FORWARD) {
      First = theASI.Point(IndFrom).Parameter();
      ok = theASI.LocalizeBefore(thePTo,theOr,IndFrom,IndTo);
      if (ok) {
	if (theOr == TopAbs_FORWARD) {
	  ok = theASI.LocalizeAfter(IndTo,theOr,IndFrom,IndTo);
	}
	if (ok && theOr == TopAbs_REVERSED) {
	  Last = theASI.Point(IndTo).Parameter();
	}
      }
    }
  }

  if (!ok) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  TopTools_ListOfShape theList;
  for (Standard_Integer i=1; i<= theASI.NbPoints(); i++) {
    prm = theASI.Point(i).Parameter();
    if (prm >= First && prm <= Last) {
      theList.Append(theASI.Point(i).Face());
    }
    else if (prm > Last) {
      break;
    }
  }

  // On ne peut pas utiliser de cylindre infini pour les ope topologiques...
  Standard_Real PMin,PMax;
  BoxParameters(myBuilder.OriginalShape(),myAxis,PMin,PMax);
  Standard_Real Heigth = 2.*(PMax-PMin);
  gp_XYZ theOrig = myAxis.Location().XYZ();
  theOrig += ((3.*PMin-PMax)/2.) * myAxis.Direction().XYZ();

  gp_Pnt p4_ao1(theOrig); gp_Ax2 a4_ao1(p4_ao1,myAxis.Direction());
  BRepPrim_Cylinder theCylinder(a4_ao1,
				Radius,
				Heigth);

  BRep_Builder B;
  TopoDS_Solid theTool;
  B.MakeSolid(theTool);
  B.Add(theTool,theCylinder.Shell());
  theTool.Closed(Standard_True);

  myTopFace = theCylinder.TopFace();
  myBotFace = theCylinder.BottomFace();

  //  BRepTools::Dump(theTool,cout);
  Standard_Boolean Fuse = Standard_False;
  myBuilder.Perform(theTool,theList,Fuse);
  myBuilder.BuildPartsOfTool();


  Standard_Integer nbparts = 0;
  TopTools_ListIteratorOfListOfShape its(myBuilder.PartsOfTool());
  for (; its.More(); its.Next()) {
    nbparts ++;
  }
  if (nbparts == 0) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  if (nbparts >= 2) { // on garde les parties entre First et Last

    TopoDS_Shape tokeep;
    Standard_Real parbar;
    gp_Pnt Barycentre;
    for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
      Baryc(its.Value(),Barycentre);
      parbar = ElCLib::LineParameter(myAxis,Barycentre);
      if (parbar < First || parbar > Last) {
	myBuilder.RemovePart(its.Value());
      }
    }
  }
//  myBuilder.PerformResult();
}


//=======================================================================
//function : PerformBlind
//purpose  : 
//=======================================================================

void BRepFeat_MakeCylindricalHole::PerformBlind(const Standard_Real Radius,
						const Standard_Real Length,
						const Standard_Boolean Cont)
{

  if (myBuilder.OriginalShape().IsNull() || !myAxDef || Length <= 0.) {
    Standard_ConstructionError::Raise();
  }

  myIsBlind = Standard_True;
  myValidate = Cont;
  myStatus = BRepFeat_NoError;

  LocOpe_CurveShapeIntersector theASI(myAxis,myBuilder.OriginalShape());
  if (!theASI.IsDone()) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }
  
//  Standard_Real First,prm;
  Standard_Real First;
  Standard_Integer IndFrom,IndTo;
  TopAbs_Orientation theOr;
  Standard_Boolean ok = theASI.LocalizeAfter(0.,theOr,IndFrom,IndTo);
  
  if (ok) {
    if (theOr == TopAbs_REVERSED) {
      ok = theASI.LocalizeBefore(IndFrom,theOr,IndFrom,IndTo); // on remonte
      // on pourrait aller chercher la suivante...
    }
    ok = ok && theOr == TopAbs_FORWARD;
  }
  if (!ok) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  // on verifie a priori la longueur du trou
  Standard_Integer IFNext,ITNext;
  ok = theASI.LocalizeAfter(IndTo,theOr,IFNext,ITNext);
  if (!ok) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }
  if (theASI.Point(IFNext).Parameter() <= Length) {
    myStatus = BRepFeat_HoleTooLong;
    return;
  }

  TopTools_ListOfShape theList;
/*
  for (Standard_Integer i=IndFrom; i<= IndTo; i++) {
    theList.Append(theASI.Point(i).Face());
  }
*/
  // version pour plus de controle
  for (Standard_Integer i=IndFrom; i<= ITNext; i++) {
    theList.Append(theASI.Point(i).Face());
  }

  First = theASI.Point(IndFrom).Parameter();

  // On ne peut pas utiliser de cylindre infini pour les ope topologiques...
  Standard_Real PMin,PMax;
  BoxParameters(myBuilder.OriginalShape(),myAxis,PMin,PMax);
  if (PMin > Length) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  Standard_Real Heigth = 3.*(Length-PMin)/2.;
  gp_XYZ theOrig = myAxis.Location().XYZ();
  theOrig += ((3.*PMin-Length)/2.) * myAxis.Direction().XYZ();
  gp_Pnt p5_ao1(theOrig); gp_Ax2 a5_ao1(p5_ao1,myAxis.Direction());
  BRepPrim_Cylinder theCylinder(a5_ao1,
				Radius,
				Heigth);

  BRep_Builder B;
  TopoDS_Solid theTool;
  B.MakeSolid(theTool);
  B.Add(theTool,theCylinder.Shell());
  theTool.Closed(Standard_True);

  myTopFace = theCylinder.TopFace();
  myBotFace.Nullify();

  //  BRepTools::Dump(theTool,cout);
  Standard_Boolean Fuse = Standard_False;
  myBuilder.Perform(theTool,theList,Fuse);
  myBuilder.BuildPartsOfTool();


  Standard_Integer nbparts = 0;
  TopTools_ListIteratorOfListOfShape its(myBuilder.PartsOfTool());
  for (; its.More(); its.Next()) {
    nbparts ++;
  }
  if (nbparts == 0) {
    myStatus = BRepFeat_InvalidPlacement;
    return;
  }

  if (nbparts >= 2) { // on garde la plus petite en parametre 
                      // le long de l`axe
    TopoDS_Shape tokeep;
    Standard_Real parbar,parmin = RealLast();
    gp_Pnt Barycentre;
    for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
      Baryc(its.Value(),Barycentre);
      parbar = ElCLib::LineParameter(myAxis,Barycentre);
      if (parbar >= First && parbar <= parmin) {
	parmin = parbar;
	tokeep = its.Value();
      }
    }

    if (tokeep.IsNull()) { // on garde le plus proche de l`intervalle

      Standard_Real dmin = RealLast();
      for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
	Baryc(its.Value(),Barycentre);
	parbar = ElCLib::LineParameter(myAxis,Barycentre);
	if (Abs(First - parbar) < dmin ) {
	  dmin = Abs(First-parbar);
	  tokeep = its.Value();
	}
      }
    }
    for (its.Initialize(myBuilder.PartsOfTool()); its.More(); its.Next()) {
      if (!tokeep.IsSame(its.Value())) {
	myBuilder.RemovePart(its.Value());
      }
    }
  }
//  myBuilder.PerformResult();
}



//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepFeat_MakeCylindricalHole::Build ()
{
  if (myStatus == BRepFeat_NoError) {
    myBuilder.PerformResult();
    if (myBuilder.IsDone()) {
      myStatus = (myValidate) ? Validate() : BRepFeat_NoError;
      if (myStatus == BRepFeat_NoError) {
	Done();
	myShape = myBuilder.ResultingShape();

	// creation of the Map.
	myMap.Clear();
	TopExp_Explorer ex;
	for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
	  myMap.Add(ex.Current());
	}
      }
      else {
	NotDone();
      }
    }
    else {
      NotDone();
      myStatus = BRepFeat_InvalidPlacement; // why not
    }
  }
  else {
    NotDone();
  }
}


//=======================================================================
//function : Validate
//purpose  : 
//=======================================================================

BRepFeat_Status BRepFeat_MakeCylindricalHole::Validate ()
{
  BRepFeat_Status thestat = BRepFeat_NoError;
  TopExp_Explorer ex(myBuilder.ResultingShape(),TopAbs_FACE);
  if (myIsBlind) { // trou borgne
    for (; ex.More(); ex.Next()) {
      if (ex.Current().IsSame(myTopFace) ) {
	break;
      }
    }
    if (!ex.More()) {
      thestat = BRepFeat_HoleTooLong;
    }
  }
  else {
    for (; ex.More(); ex.Next()) {
      if (ex.Current().IsSame(myTopFace) ) {
	return BRepFeat_InvalidPlacement;
      }
    }
    for (ex.ReInit(); ex.More(); ex.Next()) {
      if (ex.Current().IsSame(myBotFace) ) {
	return BRepFeat_InvalidPlacement;
      }
    }
  }
  return thestat;
}



void Baryc(const TopoDS_Shape& S, gp_Pnt& B)
{
  TopExp_Explorer exp(S,TopAbs_EDGE);
  gp_XYZ Bar(0.,0.,0.);
  TopLoc_Location L;
  Handle(Geom_Curve) C;
  Standard_Real prm,First,Last;

  Standard_Integer i, nbp= 0;
  for (; exp.More(); exp.Next()) {
    // On calcule 11 points par edges no degenerees
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    if (!BRep_Tool::Degenerated(E)) {
      C = BRep_Tool::Curve(E,L,First,Last);
      C = Handle(Geom_Curve)::DownCast(C->Transformed(L.Transformation()));
      for (i=1;i<=11; i++) {
	prm = ((11-i)*First + (i-1)*Last)/10.;
	Bar += C->Value(prm).XYZ();
	nbp++;
      }
    }
  }
  Bar.Divide((Standard_Real)nbp);
  B.SetXYZ(Bar);
}


void BoxParameters(const TopoDS_Shape& S,
		   const gp_Ax1& Axis,
		   Standard_Real& parmin,
		   Standard_Real& parmax)
{

  // calcul des parametres d`une boite englobante selon l`axe du trou
  Bnd_Box B;
  BRepBndLib::Add(S,B);
  Standard_Real c[6];
  B.Get(c[0],c[2],c[4],c[1],c[3],c[5]);
  gp_Pnt P;
  Standard_Integer i,j,k;
  parmin = RealLast();
  parmax = RealFirst();
  Standard_Real param;
  for (i=0; i<=1; i++) {
    P.SetX(c[i]);
    for (j=2; j<=3; j++) {
      P.SetY(c[j]);
      for (k=4; k<=5; k++) {
	P.SetZ(c[k]);
	param = ElCLib::LineParameter(Axis,P);
	parmin = Min(param,parmin);
	parmax = Max(param,parmax);
      }
    }
  }
}
