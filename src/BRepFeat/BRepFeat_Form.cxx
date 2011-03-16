// File:	BRepFeat_Form.cxx
// Created:	Tue Feb 13 13:58:51 1996
// Author:	Olga KOULECHOVA


#include <BRepFeat_Form.ixx>

#include <LocOpe.hxx>
#include <LocOpe_Builder.hxx>
#include <LocOpe_Gluer.hxx>
#include <LocOpe_FindEdges.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_SequenceOfCirc.hxx>
#include <LocOpe_PntFace.hxx>
#include <LocOpe_BuildShape.hxx>

#include <TopExp_Explorer.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Precision.hxx>

#include <BRep_Tool.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>

#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepFeat.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopoDS.hxx>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepLib.hxx>

#include <ElCLib.hxx>

#include <BRepAlgo.hxx>
//modified by NIZNHY-PKV Thu Mar 21 17:30:25 2002 f
//#include <BRepAlgo_Cut.hxx>
//#include <BRepAlgo_Fuse.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
//modified by NIZNHY-PKV Thu Mar 21 17:30:29 2002 t

#ifdef DEB
Standard_IMPORT Standard_Boolean BRepFeat_GettraceFEAT();
#endif

static void Descendants(const TopoDS_Shape&,
			const LocOpe_Builder&,
			TopTools_MapOfShape&);



//=======================================================================
//function : Perform
//purpose  : reconstruction topologique du resultat
//=======================================================================
  void BRepFeat_Form::GlobalPerform () 
{

#ifdef DEB
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_Form::GlobalPerform ()" << endl;
#endif

  if (!mySbOK || !myGSOK || !mySFOK || !mySUOK || !myGFOK || 
      !mySkOK || !myPSOK) {
#ifdef DEB
    if (trc) cout << " Fields not initialized in BRepFeat_Form" << endl;
#endif
    myStatusError = BRepFeat_NotInitialized;
    NotDone();
    return;
  }


//--- Initialisation
  Standard_Integer addflag = 0;

  TopExp_Explorer exp,exp2;
  Standard_Integer theOpe = 2;

  if(myJustFeat && !myFuse) {
#ifdef DEB
    if (trc) cout << " Invalid option : myJustFeat + Cut" << endl;
#endif
    myStatusError = BRepFeat_InvOption;
    NotDone();
    return;    
  }
  else if(myJustFeat) {
    theOpe = 2;
  }
  else if (!myGluedF.IsEmpty()) {
    theOpe = 1;
  }
  else {}
  Standard_Boolean ChangeOpe = Standard_False;


//--- Ajout Shape From et Until dans la map pour ne pas les mettre dans LShape
  Standard_Boolean FromInShape = Standard_False;
  Standard_Boolean UntilInShape = Standard_False;
  TopTools_MapOfShape M;
  
  if (!mySFrom.IsNull()) {
    FromInShape = Standard_True;
    for (exp2.Init(mySFrom,TopAbs_FACE); exp2.More(); exp2.Next()) {
      const TopoDS_Shape& ffrom = exp2.Current();
      for (exp.Init(mySbase,TopAbs_FACE); exp.More(); exp.Next()) {
	if (exp.Current().IsSame(ffrom)) {
	  break;
	}
      }
      if (!exp.More()) {
	FromInShape = Standard_False;
#ifdef DEB
	if (trc) cout << " From not in Shape" << endl;
#endif
	break;
      }
      else {
	addflag++;
	M.Add(ffrom);
      }
    }
  }

  if (!mySUntil.IsNull()) {
    UntilInShape = Standard_True;
    for (exp2.Init(mySUntil,TopAbs_FACE); exp2.More(); exp2.Next()) {
      const TopoDS_Shape& funtil = exp2.Current();
      for (exp.Init(mySbase,TopAbs_FACE); exp.More(); exp.Next()) {
	if (exp.Current().IsSame(funtil)) {
	  break;
	}
      }
      if (!exp.More()) {
	UntilInShape = Standard_False;
#ifdef DEB
	if (trc) cout << " Until not in Shape" << endl;
#endif
	break;
      }
      else {
	addflag++;
	M.Add(funtil);
      }
    }
  }


//--- Ajout Faces de collage dans la map pour ne pas les mettre dans LShape
  TopTools_DataMapIteratorOfDataMapOfShapeShape itm;
  for (itm.Initialize(myGluedF);itm.More();itm.Next()) {
    M.Add(itm.Value());
  }


//--- Recherche de la liste LShape des faces concernees par la feature

  TopTools_ListOfShape LShape;
  TopTools_ListIteratorOfListOfShape it,it2;
  Standard_Integer sens = 0;

  TColGeom_SequenceOfCurve scur;
  Curves(scur);

  Standard_Integer tempo;
  Standard_Real locmin;
  Standard_Real locmax;
  Standard_Real mf, Mf, mu, Mu;

  TopAbs_Orientation Orifuntil = TopAbs_INTERNAL;
  TopAbs_Orientation Oriffrom = TopAbs_INTERNAL;
  TopoDS_Face FFrom,FUntil;
  
  LocOpe_CSIntersector ASI1;
  LocOpe_CSIntersector ASI2;


#ifndef VREF  
  LocOpe_CSIntersector ASI3(mySbase);
  ASI3.Perform(scur);
#endif

  TopTools_ListOfShape IntList;
  IntList.Clear();


//--- 1) par intersection

// Intersection Outil Shape From
  if (!mySFrom.IsNull()) {
    ASI1.Init(mySFrom);
    ASI1.Perform(scur);
  }

// Intersection Outil Shape Until
  if (!mySUntil.IsNull()) {
    ASI2.Init(mySUntil);
    ASI2.Perform(scur);
  }

#ifndef VREF
// Intersection Outil Shape de base
  if (!ASI3.IsDone()) {
    theOpe = 2;
    LShape.Clear();
  }
  else
#endif
  {
//  Determination sens,locmin,locmax,FFrom,FUntil
    tempo=0;
    locmin = RealFirst();
    locmax = RealLast();
    for (Standard_Integer jj=1; jj<=scur.Length(); jj++) {
      if (ASI1.IsDone() && ASI2.IsDone()) {
	if (ASI1.NbPoints(jj) <= 0) {
	  continue;
	}
	mf = ASI1.Point(jj,1).Parameter();
	Mf = ASI1.Point(jj,ASI1.NbPoints(jj)).Parameter();
	if (ASI2.NbPoints(jj) <= 0) {
	  continue;
	}
	mu = ASI2.Point(jj,1).Parameter();
	Mu = ASI2.Point(jj,ASI2.NbPoints(jj)).Parameter();
	if (scur(jj)->IsPeriodic()) {
	  Standard_Real period = scur(jj)->Period();
	  locmin = mf;
	  locmax = ElCLib::InPeriod(Mu,locmin,locmin+period);
	}
	else {
	  Standard_Integer ku, kf;
	  if (! (mu > Mf || mf > Mu)) { //chevauchement des intervales
	    sens = 1;
	    kf = 1;
	    ku = ASI2.NbPoints(jj);
	    locmin = mf;
	    locmax = Max(Mf, Mu);
	  }   
	  else if (mu > Mf) {    
	    if (sens == -1) {
	      myStatusError = BRepFeat_IntervalOverlap;
	      NotDone();
	      return;
	    }
	    sens = 1;
	    kf = 1;
	    ku = ASI2.NbPoints(jj);
	    locmin = mf;
	    locmax = Mu;
	  }
	  else {
	    if (sens == 1) {
	      myStatusError = BRepFeat_IntervalOverlap;
	      NotDone();
	      return;
	    }
	    sens = -1;
	    kf = ASI1.NbPoints(jj);
	    ku = 1;
	    locmin = mu;
	    locmax = Mf;
	  }
	  if (Oriffrom == TopAbs_INTERNAL) {
	    TopAbs_Orientation Oript = ASI1.Point(jj,kf).Orientation();
	    if (Oript == TopAbs_FORWARD || Oript == TopAbs_REVERSED) {
	      if (sens == -1) {
		Oript = TopAbs::Reverse(Oript);
	      }
	      Oriffrom = TopAbs::Reverse(Oript);
	      FFrom = ASI1.Point(jj,kf).Face();
	    }
	  }
	  if (Orifuntil == TopAbs_INTERNAL) {
	      TopAbs_Orientation Oript = ASI2.Point(jj,ku).Orientation();
	      if (Oript == TopAbs_FORWARD || Oript == TopAbs_REVERSED) {
		if (sens == -1) {
		  Oript = TopAbs::Reverse(Oript);
		}
		Orifuntil = Oript;
		FUntil = ASI2.Point(jj,ku).Face();
	      }
	    }
	}
      }
      else if (ASI2.IsDone()) {
	if (ASI2.NbPoints(jj) <= 0) 
	  continue;

// pour cas base prism a cheval sur mySUntil -> sens ambigu
//      -> on privilegie sens = 1
	if(sens != 1) {
	  if (ASI2.Point(jj,1).Parameter()*
	      ASI2.Point(jj,ASI2.NbPoints(jj)).Parameter()<=0) 
	    sens=1;
	  else if (ASI2.Point(jj,1).Parameter()<0.) 
	    sens =-1;
	  else 
	    sens =1;
	}

	Standard_Integer ku;
	if (sens == -1) {
	  ku = 1;
	  locmax = -ASI2.Point(jj,ku).Parameter();
	  locmin = 0.;
	}
	else {
	  ku = ASI2.NbPoints(jj);
	  locmin = 0;
	  locmax =  ASI2.Point(jj,ku).Parameter();
	}
	if (Orifuntil == TopAbs_INTERNAL && sens != 0) {
	  TopAbs_Orientation Oript = ASI2.Point(jj,ku).Orientation();
	  if (Oript == TopAbs_FORWARD || Oript == TopAbs_REVERSED) {
	    if (sens == -1) {
	      Oript = TopAbs::Reverse(Oript);
	    }
	    Orifuntil = Oript;
	    FUntil = ASI2.Point(jj,ku).Face();
	  }
	}
      }
      else { 
	locmin = 0.;
	locmax = RealLast();
	sens = 1;
	break;
      }


// Mise a jour LShape par ajout des faces du Shape de base
//        qui sont OK pour (sens, locmin et locmax)
//        qui ne sont pas deja dans la map (Shape From Until et faces collage)
#ifndef VREF
      if (theOpe == 2) {
	for (Standard_Integer i=1; i<=ASI3.NbPoints(jj); i++) {
	  Standard_Real theprm = ASI3.Point(jj,i).Parameter() ;
	  if(locmin > locmax) {
	    Standard_Real temp = locmin;
	    locmin = locmax; locmax = temp;
	  }
	  if (theprm <= locmax &&
	      theprm >= locmin) {
	    const TopoDS_Face& fac = ASI3.Point(jj,i).Face();
	    if (M.Add(fac)) { 
	      LShape.Append(ASI3.Point(jj,i).Face());
	    }
	  }
	}
      }
      else {
	TopAbs_Orientation Or;
	Standard_Integer Indfm,IndtM,i;
	Standard_Real Tol = -Precision::Confusion();
	if(sens == 1) {
	  if (ASI3.LocalizeAfter(jj,locmin,Tol,Or,Indfm,i) && 
	      ASI3.LocalizeBefore(jj,locmax,Tol,Or,i,IndtM)) {
	    for (i= Indfm; i<=IndtM; i++) {
	      const TopoDS_Face& fac = ASI3.Point(jj,i).Face();
	      if (M.Add(fac)) { 
		LShape.Append(fac);
	      }
	    }
	  }
	}
	else if(sens == -1) {
	  if (ASI3.LocalizeBefore(jj,locmin,Tol,Or,Indfm,i) && 
	      ASI3.LocalizeAfter(jj,-locmax,Tol,Or,i,IndtM)) {
	    for (i= Indfm; i<=IndtM; i++) {
	      const TopoDS_Face& fac = ASI3.Point(jj,i).Face();
	      if (M.Add(fac)) { 
		LShape.Append(fac);
	      }
	    }
	  }	  
	}
      }
#endif
    }
  }

#ifndef VREF 
      
//--- 2) par section avec la boite englobante
    
  Bnd_Box prbox;
  BRepBndLib::Add(myGShape,prbox);
  
  Bnd_Box bbb;
  if(!mySFrom.IsNull() && !mySUntil.IsNull()) {
    BRepBndLib::Add(mySUntil, bbb);
    BRepBndLib::Add(mySFrom, bbb);
  }
  else if(!mySUntil.IsNull() && !mySkface.IsNull()) {
    BRepBndLib::Add(mySUntil, bbb);
    BRepBndLib::Add(mySkface, bbb);
  }
  else {
    bbb.SetWhole();
  }
  
  TopExp_Explorer exx1(mySbase, TopAbs_FACE);
  Standard_Integer counter = 0;
  

// On ne traite pas : la face de collage
//                    les faces du Shape From
//                    les faces du Shape Until
//                    les faces deja dans LShape
//                    les faces de myGluedF
// Si la face n'a pas ete eliminee ... on la garde si boite enblobante
// est en collision avec celle de myGShape = outil
//                    ou celle des faces limites (mySFrom mySUntil mySkface)    
  for(; exx1.More(); exx1.Next()) {
    const TopoDS_Face& sh = TopoDS::Face(exx1.Current());
    counter++;    
    
    if(sh.IsSame(mySkface) && theOpe==1) continue;

    if(!mySFrom.IsNull()) {
      TopExp_Explorer explor(mySFrom, TopAbs_FACE);
      for(; explor.More(); explor.Next()) {
	const TopoDS_Face& fff = TopoDS::Face(explor.Current());
	if(fff.IsSame(sh)) break;
      }
      if(explor.More()) continue;
    }
    
    if(!mySUntil.IsNull()) {
      TopExp_Explorer explor(mySUntil, TopAbs_FACE);
      for(; explor.More(); explor.Next()) {
	const TopoDS_Face& fff = TopoDS::Face(explor.Current());
	if(fff.IsSame(sh)) break;
      }
      if(explor.More()) continue;
    }
    
    TopTools_ListIteratorOfListOfShape iter1(LShape);
    for(; iter1.More(); iter1.Next()) {
      if(iter1.Value().IsSame(sh)) break;
    }
    if(iter1.More()) continue;
    
    for (itm.Initialize(myGluedF);itm.More();itm.Next()) {
      const TopoDS_Face& glf = TopoDS::Face(itm.Value());
      if(glf.IsSame(sh)) break;
    }
    if(itm.More()) continue;
    
    Bnd_Box shbox;
    BRepBndLib::Add(sh,shbox);
    
    if(shbox.IsOut(bbb) || shbox.IsOut(prbox)) continue;
    //    if(shbox.IsOut(prbox)) continue;
    
    if(M.Add(sh)) {
      LShape.Append(sh);
    }
  }

#endif

#ifdef VREF
// test de performance : ajout de toutes les faces du Shape de base dans LShape
// (pas de phase de tri mais plus de faces) -> pas concluant
  TopExp_Explorer exx1;
  for (exx1.Init(mySbase, TopAbs_FACE);
       exx1.More(); exx1.Next()) {
    const TopoDS_Shape& fac = exx1.Current();
    if (M.Add(fac)) {
      LShape.Append(fac);
    }
  }

#endif

  LocOpe_Gluer theGlue;
  
//--- cas de collage

  if (theOpe == 1) {
#ifdef DEB
    if (trc) cout << " Gluer" << endl;
#endif
    Standard_Boolean Collage = Standard_True;  
    // on coupe par FFrom && FUntil
    TopoDS_Shape Comp;
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Comp));
    if (!mySFrom.IsNull()) {
      TopoDS_Solid S = BRepFeat::Tool(mySFrom,FFrom,Oriffrom);
      if (!S.IsNull()) {
	B.Add(Comp,S);
      }
    }
    if (!mySUntil.IsNull()) {
      TopoDS_Solid S = BRepFeat::Tool(mySUntil,FUntil,Orifuntil);
      if (!S.IsNull()) {
	B.Add(Comp,S);
      }
    }

    LocOpe_FindEdges theFE;
    TopTools_DataMapOfShapeListOfShape locmap;
    TopExp_Explorer expp(Comp, TopAbs_SOLID);
    if(expp.More() && !Comp.IsNull() && !myGShape.IsNull())  {
      //modified by NIZNHY-PKV Thu Mar 21 17:15:36 2002 f
      //BRepAlgo_Cut trP(myGShape,Comp);
      BRepAlgoAPI_Cut trP(myGShape, Comp);
      //modified by NIZNHY-PKV Thu Mar 21 17:15:58 2002 t
      exp.Init(trP.Shape(), TopAbs_SOLID);
      if (exp.Current().IsNull()) {
	theOpe = 2;
	ChangeOpe = Standard_True;
	Collage = Standard_False;
      }
      else {// else X0
	// On ne garde que les solides
	TopoDS_Shape theGShape;
	BRep_Builder B;
	B.MakeCompound(TopoDS::Compound(theGShape));
	for (; exp.More(); exp.Next()) {
	  B.Add(theGShape,exp.Current());
	}
	if (!BRepAlgo::IsValid(theGShape)) {
	  theOpe = 2;
	  ChangeOpe = Standard_True;
	  Collage = Standard_False;
	}
	else {// else X1
	  if(!mySFrom.IsNull()) { 
	    TopExp_Explorer ex;
	    ex.Init(mySFrom, TopAbs_FACE);
	    for(; ex.More(); ex.Next()) {
	      const TopoDS_Face& fac = TopoDS::Face(ex.Current());
	      if (!FromInShape) {
                TopTools_ListOfShape thelist;
		myMap.Bind(fac, thelist);
	      }
	      else {
                TopTools_ListOfShape thelist1;
		locmap.Bind(fac, thelist1);
	      }
	      if (trP.IsDeleted(fac)) {
	      }
	      else if (!FromInShape) {
		myMap(fac) = trP.Modified(fac);
		if (myMap(fac).IsEmpty()) myMap(fac).Append(fac);
	      }
	      else {
		locmap(fac) =trP.Modified(fac) ;
		if (locmap(fac).IsEmpty()) locmap(fac).Append(fac);
	      }
	    }
	  }// if(!mySFrom.IsNull()) 
	  //
	  if(!mySUntil.IsNull()) { 
	    TopExp_Explorer ex;
	    ex.Init(mySUntil, TopAbs_FACE);
	    for(; ex.More(); ex.Next()) {
	      const TopoDS_Face& fac = TopoDS::Face(ex.Current());
	      if (!UntilInShape) {		
		TopTools_ListOfShape thelist2;
                myMap.Bind(fac,thelist2);
	      }
	      else {
                TopTools_ListOfShape thelist3;
		locmap.Bind(fac,thelist3);
	      }
	      if (trP.IsDeleted(fac)) {
	      }
	      else if (!UntilInShape) {
		myMap(fac) = trP.Modified(fac);
		if (myMap(fac).IsEmpty()) myMap(fac).Append(fac);
	      }
	      else {
		locmap(fac) = trP.Modified(fac);
		if (locmap(fac).IsEmpty()) locmap(fac).Append(fac);
	      }
	    }
	  }// if(!mySUntil.IsNull())
	  //
	  //modified by NIZNHY-PKV Thu Mar 21 17:21:49 2002 f
	  //UpdateDescendants(trP.Builder(),theGShape,Standard_True); // skip faces
	  UpdateDescendants(trP,theGShape,Standard_True); // skip faces
	  //modified by NIZNHY-PKV Thu Mar 21 17:22:32 2002 t

	  theGlue.Init(mySbase,theGShape);
	  for (itm.Initialize(myGluedF);itm.More();itm.Next()) {
	    const TopoDS_Face& gl = TopoDS::Face(itm.Key());
	    TopTools_ListOfShape ldsc;
	    if (trP.IsDeleted(gl)) {
	    }
	    else {
	      ldsc = trP.Modified(gl);
	      if (ldsc.IsEmpty()) ldsc.Append(gl);
	    }
	    const TopoDS_Face& glface = TopoDS::Face(itm.Value());	
	    for (it.Initialize(ldsc);it.More();it.Next()) {
	      const TopoDS_Face& fac = TopoDS::Face(it.Value());
	      Collage = BRepFeat::IsInside(fac, glface);
	      if(!Collage) {
		theOpe = 2;
		ChangeOpe = Standard_True;
		break;
	      }
	      else {
		theGlue.Bind(fac,glface);
		theFE.Set(fac,glface);
		for (theFE.InitIterator(); theFE.More();theFE.Next()) {
		  theGlue.Bind(theFE.EdgeFrom(),theFE.EdgeTo());
		}
	      }
	    }
	  }
	}// else X1
      }// else X0
    }// if(expp.More() && !Comp.IsNull() && !myGShape.IsNull()) 
    else {
      theGlue.Init(mySbase,myGShape);
      for (itm.Initialize(myGluedF); itm.More();itm.Next()) {
	const TopoDS_Face& glface = TopoDS::Face(itm.Key());
	const TopoDS_Face& fac = TopoDS::Face(myGluedF(glface));
	for (exp.Init(myGShape,TopAbs_FACE); exp.More(); exp.Next()) {
	  if (exp.Current().IsSame(glface)) {
	    break;
	  }
	}
	if (exp.More()) {
	  Collage = BRepFeat::IsInside(glface, fac);
	  if(!Collage) {
	    theOpe = 2;
	    ChangeOpe = Standard_True;
	    break;
	  }
	  else {
	    theGlue.Bind(glface, fac);
	    theFE.Set(glface, fac);
	    for (theFE.InitIterator(); theFE.More();theFE.Next()) {
	      theGlue.Bind(theFE.EdgeFrom(),theFE.EdgeTo());
	    }
	  }
	}
      }
    }

    // On rajoute le collage sur face de depart et fin , si necessaire !!!
    if (FromInShape && Collage) {
      TopExp_Explorer ex(mySFrom,TopAbs_FACE);
      for(; ex.More(); ex.Next()) {
	const TopoDS_Face& fac2 = TopoDS::Face(ex.Current());
//	for (it.Initialize(myMap(fac2)); it.More(); it.Next()) {
	for (it.Initialize(locmap(fac2)); it.More(); it.Next()) {
	  const TopoDS_Face& fac1 = TopoDS::Face(it.Value());
	  theFE.Set(fac1, fac2);
	  theGlue.Bind(fac1, fac2);
	  for (theFE.InitIterator(); theFE.More();theFE.Next()) {
	    theGlue.Bind(theFE.EdgeFrom(),theFE.EdgeTo());
	  }
	}
//	myMap.UnBind(fac2);
      }
    }

    if (UntilInShape && Collage) {
      TopExp_Explorer ex(mySUntil, TopAbs_FACE);
      for(; ex.More(); ex.Next()) {
	const TopoDS_Face& fac2 = TopoDS::Face(ex.Current());
//	for (it.Initialize(myMap(fac2)); it.More(); it.Next()) {
	for (it.Initialize(locmap(fac2)); it.More(); it.Next()) {
	  const TopoDS_Face& fac1 = TopoDS::Face(it.Value());
	  theGlue.Bind(fac1, fac2);
	  theFE.Set(fac1, fac2);
	  for (theFE.InitIterator(); theFE.More();theFE.Next()) {
	    theGlue.Bind(theFE.EdgeFrom(),theFE.EdgeTo());
	  }
	}
	//myMap.UnBind(fac2); // pour ne pas avoir fac2 dans la Map quand
	// on .appelle UpdateDescendants(theGlue)
      }
    }

    LocOpe_Operation ope = theGlue.OpeType();
    if (ope == LocOpe_INVALID ||
	(myFuse && ope != LocOpe_FUSE) ||
	(!myFuse && ope != LocOpe_CUT) ||
	(!Collage)) {
      theOpe = 2;
      ChangeOpe = Standard_True;
    }
  }

//--- si le collage est toujours applicable

  if (theOpe == 1) {
#ifdef DEB
    if (trc) cout << " still Gluer" << endl;
#endif
    theGlue.Perform();
    if (theGlue.IsDone()) {
      TopoDS_Shape shshs = theGlue.ResultingShape();
//      if (BRepOffsetAPI::IsTopologicallyValid(shshs)) {
      if (BRepAlgo::IsValid(shshs)) {
	UpdateDescendants(theGlue);
	myNewEdges = theGlue.Edges();
	myTgtEdges = theGlue.TgtEdges();
//	TopTools_ListIteratorOfListOfShape itt1;
	if (myModify && !LShape.IsEmpty()) {
	  TopTools_ListOfShape LTool,LGShape;
	  LocOpe_Builder theTOpe(theGlue.ResultingShape());
	  
	  for (it2.Initialize(LShape);it2.More();it2.Next()) {
	    const TopTools_ListOfShape& ldf = myMap(it2.Value());
	    if (ldf.Extent() == 1 && ldf.First().IsSame(it2.Value())) {
//	      const TopoDS_Shape& s = it2.Value();
	      LGShape.Append(it2.Value());
	    }
	    else {
	      for (it.Initialize(ldf);it.More();it.Next()) {
		if (M.Add(it.Value())) {
//		  const TopoDS_Shape& s = it.Value();
		  LGShape.Append(it.Value());
		}
	      }
	    }
	  }
	  for (exp.Init(theGlue.GluedShape(),TopAbs_FACE);
	       exp.More();exp.Next()) {
	    for (it.Initialize(theGlue.DescendantFaces
			       (TopoDS::Face(exp.Current())));
		 it.More();it.Next()) {
	      if (M.Add(it.Value())) {
		LTool.Append(it.Value());
	      }
	    }
	  }
	  
	  if (!(LGShape .IsEmpty() || LTool.IsEmpty())) {
//	    TopTools_ListIteratorOfListOfShape it1(LGShape);
//	    TopTools_ListIteratorOfListOfShape it2(LTool);
	    theTOpe.Perform(LGShape,LTool,myFuse);
	    theTOpe.PerformResult();
	    TopTools_ListOfShape TOpeNewEdges, TOpeTgtEdges;
	    TOpeNewEdges = theTOpe.Edges();
	    TOpeTgtEdges = theTOpe.TgtEdges();
	    TopTools_ListIteratorOfListOfShape itt1, itt2;
	    itt1.Initialize(TOpeNewEdges);
	    itt2.Initialize(myNewEdges);	  
	    for(; itt1.More(); itt1.Next()) {
	      TopoDS_Edge e1 = TopoDS::Edge(itt1.Value());
	      Standard_Boolean Adde1 = Standard_True;
	      for(; itt2.More(); itt2.Next()) {
		TopoDS_Edge e2 = TopoDS::Edge(itt2.Value());
		if(e1.IsSame(e2))  {
		  Adde1 = Standard_False;
		  break;
		}
	      }
	      if(Adde1) {
		myNewEdges.Append(e1);
	      }
	    }
	    itt1.Initialize(TOpeTgtEdges);
	    itt2.Initialize(myTgtEdges);	  
	    for(; itt1.More(); itt1.Next()) {
	      TopoDS_Edge e1 = TopoDS::Edge(itt1.Value());
	      Standard_Boolean Adde1 = Standard_True;
	      for(; itt2.More(); itt2.Next()) {
		TopoDS_Edge e2 = TopoDS::Edge(itt2.Value());
		if(e1.IsSame(e2))  {
		  Adde1 = Standard_False;
		  break;
		}
	      }
	      if(Adde1) {
		myTgtEdges.Append(e1);
	      }
	    }
	    
	    if (theTOpe.IsDone()) {
	      Done();
	      myShape = theTOpe.ResultingShape();
	      TopExp_Explorer exf(myShape, TopAbs_FACE);
	      if (exf.More() && BRepAlgo::IsTopologicallyValid(myShape)) {
		theOpe = 3; // ???
//		UpdateDescendants(theTOpe.Builder(),myShape);
		UpdateDescendants(theTOpe.History(),myShape);
	      }
	      else {
		theOpe = 2;
		ChangeOpe = Standard_True;
	      }
	    }
	    else {
	      theOpe = 2;
	      ChangeOpe = Standard_True;
	    }
	  }
	  else {
#ifdef DEB
	    if (trc) cout << " WARNING Gluer failure" << endl;
#endif
	    Done();
	    myShape = theGlue.ResultingShape();
	  }
	}
	else {
#ifdef DEB
	  if (trc) cout << " Gluer result" << endl;
#endif
	  Done();
	  myShape = theGlue.ResultingShape();
	}
      }
      else {
	theOpe = 2;
	ChangeOpe = Standard_True;
      }
    }
    else {
      theOpe = 2;
      ChangeOpe = Standard_True;
    }
  }


//--- cas sans collage + Outil aux bonnes dimensions

  if (theOpe == 2 && ChangeOpe && myJustGluer) {
#ifdef DEB
    if (trc) cout << " Gluer failure" << endl;
#endif
    myJustGluer = Standard_False;
    theOpe = 0;
//    Done();
//    return;
  }

//--- cas sans collage

  if (theOpe == 2) {
#ifdef DEB
    if (trc) cout << " No Gluer" << endl;
#endif
    TopoDS_Shape theGShape = myGShape;
    if (ChangeOpe) {
#ifdef DEB
      if (trc) cout << " Passage en ope. topologique" << endl;
#endif
      for (itm.Initialize(myGluedF); itm.More();itm.Next()) {
	const TopoDS_Face& fac = TopoDS::Face(itm.Value());
	Standard_Boolean found = Standard_False;
	TopTools_ListIteratorOfListOfShape it(LShape);
	for(; it.More(); it.Next()) {
	  if(it.Value().IsSame(fac)) {
	    found = Standard_True;
	    break;	  
	  }
	  if(found) break;
	}
	if(!found) {
// echec collage -> on remet les faces de collage dans LShape
	  LShape.Append(fac);
	}
      }
    }    

    TopoDS_Shape Comp;
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Comp));
    if (!mySFrom.IsNull() || !mySUntil.IsNull()) {
      if (!mySFrom.IsNull() && !FromInShape) {
	TopoDS_Solid S = BRepFeat::Tool(mySFrom,FFrom,Oriffrom);
	if (!S.IsNull()) {
	  B.Add(Comp,S);
	}
      }
      if (!mySUntil.IsNull() && !UntilInShape) {
	if (!mySFrom.IsNull()) {
	  if (!mySFrom.IsSame(mySUntil)) {
	    TopoDS_Solid S = BRepFeat::Tool(mySUntil,FUntil,Orifuntil);
	    if (!S.IsNull()) {
	      B.Add(Comp,S);
	    }
	  }
	}
	else {
	  TopoDS_Solid S = BRepFeat::Tool(mySUntil,FUntil,Orifuntil);
	  if (!S.IsNull()) {
	    B.Add(Comp,S);
	  }
	}
      }
    }

// mise a jour type de selection
    if(myPerfSelection == BRepFeat_SelectionU && !UntilInShape) {
      myPerfSelection = BRepFeat_NoSelection;
    }
    else if(myPerfSelection == BRepFeat_SelectionFU &&
	    !FromInShape && !UntilInShape) {
      myPerfSelection = BRepFeat_NoSelection;
    }
    else if(myPerfSelection == BRepFeat_SelectionShU && !UntilInShape) {
      myPerfSelection = BRepFeat_NoSelection;
    }
    else {}

    TopExp_Explorer expp(Comp, TopAbs_SOLID);
    if(expp.More() && !Comp.IsNull() && !myGShape.IsNull())  {
      //modified by NIZNHY-PKV Thu Mar 21 17:24:52 2002 f
      //BRepAlgo_Cut trP(myGShape,Comp);
      BRepAlgoAPI_Cut trP(myGShape, Comp);
      //modified by NIZNHY-PKV Thu Mar 21 17:24:56 2002 t
      // le resultat est necessairement un compound.
      exp.Init(trP.Shape(),TopAbs_SOLID);
      if (!exp.More()) {
	myStatusError = BRepFeat_EmptyCutResult;
	NotDone();
	return;
      }
      // On ne garde que les solides
      theGShape.Nullify();
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(theGShape));
      for (; exp.More(); exp.Next()) {
	B.Add(theGShape,exp.Current());
      }
      if (!BRepAlgo::IsValid(theGShape)) {
	myStatusError = BRepFeat_InvShape;
	NotDone();
	return;
      }
      if(!mySFrom.IsNull()) {
	if(!FromInShape) {
	  TopExp_Explorer ex(mySFrom, TopAbs_FACE);
	  for(; ex.More(); ex.Next()) {
	    const TopoDS_Face& fac = TopoDS::Face(ex.Current());
            TopTools_ListOfShape thelist4;
	    myMap.Bind(fac,thelist4);
	    if (trP.IsDeleted(fac)) {
	    }
	    else {
	      myMap(fac) = trP.Modified(fac);
	     if (myMap(fac).IsEmpty())  myMap(fac).Append(fac);
	    }
	  }
	}
	else {
	  TopExp_Explorer ex(mySFrom, TopAbs_FACE);
	  for(; ex.More(); ex.Next()) {
	    const TopoDS_Face& fac = TopoDS::Face(ex.Current());
	    Standard_Boolean found = Standard_False;
	    TopTools_ListIteratorOfListOfShape it(LShape);
	    for(; it.More(); it.Next()) {
	      if(it.Value().IsSame(fac)) {
		found = Standard_True;
		break;	  
	      }
	      if(found) break;
	    }
	    if(!found) {
	      LShape.Append(fac);    
	    }
	  }
	}
      }
      if(!mySUntil.IsNull()) {
	if(!UntilInShape) {
	  TopExp_Explorer ex(mySUntil, TopAbs_FACE);
	  for(; ex.More(); ex.Next()) {
	    const TopoDS_Face& fac = TopoDS::Face(ex.Current());
            TopTools_ListOfShape thelist5;
	    myMap.Bind(fac,thelist5);
	    if (trP.IsDeleted(fac)) {
	    }
	    else {
	      myMap(fac) = trP.Modified(fac);
	      if (myMap.IsEmpty()) myMap(fac).Append(fac);
	    }
	  }
	}
	else {
	  TopExp_Explorer ex(mySUntil, TopAbs_FACE);
	  for(; ex.More(); ex.Next()) {
	    const TopoDS_Face& fac = TopoDS::Face(ex.Current());
	    Standard_Boolean found = Standard_False;
	    TopTools_ListIteratorOfListOfShape it(LShape);
	    for(; it.More(); it.Next()) {
	      if(it.Value().IsSame(fac)) {
		found = Standard_True;
		break;	  
	      }
	      if(found) break;
	    }
	    if(!found) {
	      LShape.Append(fac);
	    }
	  }
	}
      }
      //modified by NIZNHY-PKV Thu Mar 21 17:27:23 2002 f
      //UpdateDescendants(trP.Builder(),theGShape,Standard_True); 
      UpdateDescendants(trP,theGShape,Standard_True); 
      //modified by NIZNHY-PKV Thu Mar 21 17:27:31 2002 t
    }//if(expp.More() && !Comp.IsNull() && !myGShape.IsNull())  {
    //
    else {
      if(!mySFrom.IsNull()) {
	TopExp_Explorer ex(mySFrom, TopAbs_FACE);
	for(; ex.More(); ex.Next()) {
	  const TopoDS_Face& fac = TopoDS::Face(ex.Current());
	  Standard_Boolean found = Standard_False;
	  TopTools_ListIteratorOfListOfShape it(LShape);
	  for(; it.More(); it.Next()) {
	    if(it.Value().IsSame(fac)) {
	      found = Standard_True;
	      break;	  
	    }
	    if(found) break;
	  }
	  if(!found) {
	    LShape.Append(fac);	  
	  }
	}
      }
      if(!mySUntil.IsNull()) {
	TopExp_Explorer ex(mySUntil, TopAbs_FACE);
	for(; ex.More(); ex.Next()) {
	  const TopoDS_Face& fac = TopoDS::Face(ex.Current());
	  Standard_Boolean found = Standard_False;
	  TopTools_ListIteratorOfListOfShape it(LShape);
	  for(; it.More(); it.Next()) {
	    if(it.Value().IsSame(fac)) {
	      found = Standard_True;
	      break;	  
	    }
	    if(found) break;
	  }
	  if(!found) {
	    LShape.Append(fac);
	  }
	}
      }
    }


//--- generation de "just feature" pour assemblage = Parties d'outil
    TopTools_ListOfShape lshape; 
    LocOpe_Builder theTOpe;
    Standard_Real pbmin, pbmax, prmin, prmax;
    Standard_Boolean flag1;
    Handle(Geom_Curve) C;
    if(!myJustFeat) {
      theTOpe.Init(mySbase);
      theTOpe.Perform(theGShape,LShape,myFuse);
      theTOpe.BuildPartsOfTool();
      lshape = theTOpe.PartsOfTool();
    }
    else {
      theTOpe.Init(theGShape, mySbase);
      TopTools_ListOfShape list;
      theTOpe.Perform(list,LShape,Standard_False);
      theTOpe.PerformResult();
      if (theTOpe.IsDone()) {
	myShape = theTOpe.ResultingShape();
//	UpdateDescendants(theTOpe.Builder(),myShape); // a priori bug de mise a jour
	UpdateDescendants(theTOpe.History(),myShape); // a priori bug de mise a jour
	// a faire apres selection des parties a garder
	myNewEdges = theTOpe.Edges();
	myTgtEdges = theTOpe.TgtEdges();
	TopExp_Explorer explo(theTOpe.ResultingShape(), TopAbs_SOLID);
	for (; explo.More(); explo.Next()) {
	  lshape.Append(explo.Current());
	}
      }
      else {
	myStatusError = BRepFeat_LocOpeNotDone;
	NotDone();
	return;
      }
    }

//--- Selection des morceaux d'outil a garder
    TopoDS_Solid thePartsOfTool;
    if(!lshape.IsEmpty() && myPerfSelection != BRepFeat_NoSelection) {

//Recherche ParametricMinMax en fonction contraintes des Shape From et Until
//   -> prmin, prmax, pbmin et pbmax
      C = BarycCurve();
      if (C.IsNull()) {
	myStatusError = BRepFeat_EmptyBaryCurve; 
	NotDone();
	return;
      }

      if(myPerfSelection == BRepFeat_SelectionSh) {
	BRepFeat::ParametricMinMax(mySbase,C, 
				   prmin, prmax, pbmin, pbmax, flag1);
      }
      else if(myPerfSelection == BRepFeat_SelectionFU) {
	Standard_Real prmin1, prmax1, prmin2, prmax2;
	Standard_Real prbmin1, prbmax1, prbmin2, prbmax2;
      
	BRepFeat::ParametricMinMax(mySFrom,C, 
				   prmin1, prmax1, prbmin1, prbmax1, flag1);
	BRepFeat::ParametricMinMax(mySUntil,C, 
				   prmin2, prmax2, prbmin2, prbmax2, flag1);

// cas des revol
	if (C->IsPeriodic()) {
	  Standard_Real period = C->Period();
	  prmax = prmax2;
	  if (flag1) {
	    prmin = ElCLib::InPeriod(prmin1,prmax-period,prmax);
	  }
	  else {
	    prmin = Min(prmin1, prmin2);
	  }
	  pbmax = prbmax2;
	  pbmin = ElCLib::InPeriod(prbmin1,pbmax-period,pbmax);
	}
	else {
	  prmin = Min(prmin1, prmin2);
	  prmax = Max(prmax1, prmax2);
	  pbmin = Min(prbmin1, prbmin2);
	  pbmax = Max(prbmax1, prbmax2);
	}
      }
      else if(myPerfSelection == BRepFeat_SelectionShU) {
	Standard_Real prmin1, prmax1, prmin2, prmax2;
	Standard_Real prbmin1, prbmax1, prbmin2, prbmax2;
	
	if(!myJustFeat && sens == 0) sens =1;
	if (sens == 0) {
	  myStatusError = BRepFeat_IncDirection;
	  NotDone();
	  return;
	}
	
	BRepFeat::ParametricMinMax(mySUntil,C, 
				   prmin1, prmax1, prbmin1, prbmax1, flag1);

	BRepFeat::ParametricMinMax(mySbase,C, 
				   prmin2, prmax2, prbmin2, prbmax2, flag1);
	if (sens == 1) {
	  prmin = prmin2;
	  prmax = prmax1;
	  pbmin = prbmin2;
	  pbmax = prbmax1;
	}
	else if (sens == -1) {
	  prmin = prmin1;
	  prmax = prmax2;
	  pbmin = prbmin1;
	  pbmax = prbmax2;
	}
      }
      else if (myPerfSelection == BRepFeat_SelectionU) {
	Standard_Real prmin1, prmax1, prbmin1, prbmax1;
      	if (sens == 0) {
	  myStatusError = BRepFeat_IncDirection;
	  NotDone();
	  return;
	}
	
	// On cherche les parties de l`outil contenant les descendants du
	// Shape Until
	BRepFeat::ParametricMinMax(mySUntil,C, 
				   prmin1, prmax1, prbmin1, prbmax1, flag1);
	if (sens == 1) {
	  prmin = RealFirst();
	  prmax = prmax1;
	  pbmin = RealFirst();
	  pbmax = prbmax1;
	}
	else if(sens == -1) {
	  prmin = prmin1;
	  prmax = RealLast();
	  pbmin = prbmin1;
	  pbmax = RealLast();
	}
      }


// Choix plus fin des ParametricMinMax dans le cas ou l'outil 
// intersecte les Shapes From et Until
//       cas de plusieurs intersections (retenir PartsOfTool en accord avec selection)  
//       position de la face d`intersection dans PartsOfTool (avant ou arriere)
      Standard_Real delta = Precision::Confusion();

      if (myPerfSelection != BRepFeat_NoSelection) {
// modif du test pour cts21181 : (prbmax2 et prnmin2) -> (prbmin1 et prbmax1)
// correction prise en compte de flag2 pour pro15323 et de flag3 pour pro16060
	if (!mySUntil.IsNull()) {
	  TopTools_MapOfShape mapFuntil;
	  Descendants(mySUntil,theTOpe,mapFuntil);
	  if (!mapFuntil.IsEmpty()) {
	    for (it.Initialize(lshape); it.More(); it.Next()) {
	      TopExp_Explorer expf;
	      for (expf.Init(it.Value(),TopAbs_FACE); 
		   expf.More(); expf.Next()) {
		if (mapFuntil.Contains(expf.Current())) {
		  Standard_Boolean flag2,flag3;
		  Standard_Real prmin1, prmax1, prbmin1, prbmax1;
		  Standard_Real prmin2, prmax2, prbmin2, prbmax2;
		  BRepFeat::ParametricMinMax(expf.Current(),C, prmin1, prmax1,
					     prbmin1, prbmax1,flag3);
		  BRepFeat::ParametricMinMax(it.Value(),C, prmin2, prmax2,
					     prbmin2, prbmax2,flag2);
		  if (sens == 1) {
		    Standard_Boolean testOK = !flag2;
		    if (flag2) {
		      testOK = !flag1;
		      if (flag1 && prmax2 > prmin + delta) {
			testOK = !flag3;
			if (flag3 && prmax1 == prmax2) {
			  testOK = Standard_True;
			}
		      }
		    }
		    if (prbmin1 < pbmax && testOK) {
		      if (flag2) {
			flag1 = flag2;
			prmax  = prmax2;
		      }
		      pbmax = prbmin1;
		    }
		  }
		  else if (sens == -1){ 
		    Standard_Boolean testOK = !flag2;
		    if (flag2) {
		      testOK = !flag1;
		      if (flag1 && prmin2 < prmax - delta) {
			testOK = !flag3;
			if (flag3 && prmin1 == prmin2) {
			  testOK = Standard_True;
			}
		      }
		    }
		    if (prbmax1 > pbmin && testOK) {
		      if (flag2) {
			flag1 = flag2;
			prmin  = prmin2;
		      }
		      pbmin = prbmax1;
		    }
		  }
		  break;
		}		
	      }
	    }
	    it.Initialize(lshape);
	  }
	}
	if (!mySFrom.IsNull()) {
	  TopTools_MapOfShape mapFfrom;
	  Descendants(mySFrom,theTOpe,mapFfrom);
	  if (!mapFfrom.IsEmpty()) {
	    for (it.Initialize(lshape); it.More(); it.Next()) {
	      TopExp_Explorer expf;
	      for (expf.Init(it.Value(),TopAbs_FACE); 
		   expf.More(); expf.Next()) {
		if (mapFfrom.Contains(expf.Current())) {
		  Standard_Boolean flag2,flag3;
		  Standard_Real prmin1, prmax1, prbmin1, prbmax1;
		  Standard_Real prmin2, prmax2, prbmin2, prbmax2;
		  BRepFeat::ParametricMinMax(expf.Current(),C, prmin1, prmax1,
					     prbmin1, prbmax1,flag3);
		  BRepFeat::ParametricMinMax(it.Value(),C, prmin2, prmax2,
					     prbmin2, prbmax2,flag2);
		  if (sens == 1) {
		    Standard_Boolean testOK = !flag2;
		    if (flag2) {
		      testOK = !flag1;
		      if (flag1 && prmin2 < prmax - delta) {
			testOK = !flag3;
			if (flag3 && prmin1 == prmin2) {
			  testOK = Standard_True;
			}
		      }
		    }
		    if (prbmax1 > pbmin && testOK) {
		      if (flag2) {
			flag1 = flag2;
			prmin  = prmin2;
		      }
		      pbmin = prbmax1;
		    }
		  }
		  else if (sens == -1){
		    Standard_Boolean testOK = !flag2;
		    if (flag2) {
		      testOK = !flag1;
		      if (flag1 && prmax2 > prmin + delta) {
			testOK = !flag3;
			if (flag3 && prmax1 == prmax2) {
			  testOK = Standard_True;
			}
		      }
		    }
		    if (prbmin1 < pbmax && testOK) {
		      if (flag2) {
			flag1 = flag2;
			prmax  = prmax2;
		      }
		      pbmax = prbmin1;
		    }
		  }
		  break;
		}		
	      }
	    }
	    it.Initialize(lshape);
	  }
	}
      }


// Tri des PartsOfTool a garder ou non en fonction des ParametricMinMax
      if (!myJustFeat) {
	Standard_Boolean KeepParts = Standard_False;
	BRep_Builder BB;
	BB.MakeSolid(thePartsOfTool);
	Standard_Real prmin1, prmax1, prbmin1, prbmax1;
	Standard_Real min, max, pmin, pmax;
	Standard_Boolean flag2;
	for (it.Initialize(lshape); it.More(); it.Next()) {
	  if (C->IsPeriodic()) {
	    Standard_Real period = C->Period();
	    Standard_Real pr, prb;
	    BRepFeat::ParametricMinMax(it.Value(),C, pr, prmax1,
				       prb, prbmax1,flag2,Standard_True);
	    if (flag2) {
	      prmin1 = ElCLib::InPeriod(pr,prmax1-period,prmax1);
	    }
	    else {
	      prmin1 = pr;
	    }
	    prbmin1 = ElCLib::InPeriod(prb,prbmax1-period,prbmax1);
	  }
	  else {
	    BRepFeat::ParametricMinMax(it.Value(),C, 
				       prmin1, prmax1, prbmin1, prbmax1,flag2);
	  }
	  if(flag2 == Standard_False || flag1 == Standard_False) {
	    pmin = pbmin;
	    pmax = pbmax;
	    min = prbmin1;
	    max = prbmax1;
	  }
	  else {
	    pmin = prmin;
	    pmax = prmax;
	    min = prmin1;
	    max = prmax1;
	  }
	  if ((min > pmax - delta) || 
	      (max < pmin + delta)){
	    theTOpe.RemovePart(it.Value());
	  }
	  else {
	    KeepParts = Standard_True;
	    const TopoDS_Shape& S = it.Value();
	    B.Add(thePartsOfTool,S);
	  }
	}

// Cas ou on ne garde aucune partie d`outil
	if (!KeepParts) {
#ifdef DEB
	  if (trc) cout << " No parts of tool kept" << endl;
#endif
	  myStatusError = BRepFeat_NoParts;
	  NotDone();
	  return;
	}
      }
      else {
// cas JustFeature -> on garde tous les PartsOfTool
	Standard_Real prmin1, prmax1, prbmin1, prbmax1;
	Standard_Real min, max, pmin, pmax;
	Standard_Boolean flag2;
	TopoDS_Shape Compo;
	BRep_Builder B;
	B.MakeCompound(TopoDS::Compound(Compo));
	for (it.Initialize(lshape); it.More(); it.Next()) {
	  BRepFeat::ParametricMinMax(it.Value(),C, 
				     prmin1, prmax1, prbmin1, prbmax1,flag2);
	  if(flag2 == Standard_False || flag1 == Standard_False) {
	    pmin = pbmin;
	    pmax = pbmax;
	    min = prbmin1;
	    max = prbmax1;
	  }
	  else { 
	    pmin = prmin;
	    pmax = prmax;
	    min = prmin1;
	    max = prmax1;
	  }
	  if ((min < pmax - delta) && 
	      (max > pmin + delta)){
	    if (!it.Value().IsNull()) {
	      B.Add(Compo,it.Value());
	    }
	  }
	}
	myShape = Compo;
      }
    }

 
//--- Generation du resultat myShape

    if (!myJustFeat) {
// suppression des edges de section qui n'ont aucun vertex commun
// avec les PartsOfTool converves
      theTOpe.PerformResult();
      if (theTOpe.IsDone()) {
	Done();
	myShape = theTOpe.ResultingShape();
//
	BRepLib::SameParameter(myShape, 1.e-7, Standard_True);
//
// mise a jour des new et tangent edges
//	UpdateDescendants(theTOpe.Builder(),myShape);
	UpdateDescendants(theTOpe.History(),myShape);
	myNewEdges = theTOpe.Edges();
	if(!theTOpe.TgtEdges().IsEmpty()) {
	  myTgtEdges = theTOpe.TgtEdges();
	}
	//	else myTgtEdges.Clear();
      }
      else {// dernier recours (attention new et tangent edges)
	if (!thePartsOfTool.IsNull()) {
#ifdef DEB
	  if (trc) cout << " Parts of Tool : direct Ope. Top." << endl;
#endif
	  if(myFuse == 1) {
	    //modified by NIZNHY-PKV Thu Mar 21 17:28:09 2002 f
	    //BRepAlgo_Fuse f(mySbase, thePartsOfTool);
	    BRepAlgoAPI_Fuse f(mySbase, thePartsOfTool);
	    //modified by NIZNHY-PKV Thu Mar 21 17:28:18 2002 t
	    myShape = f.Shape();
	    //modified by NIZNHY-PKV Thu Mar 21 17:28:41 2002 f
	    //UpdateDescendants(f.Builder(), myShape, Standard_False);
	    UpdateDescendants(f, myShape, Standard_False);
	    //modified by NIZNHY-PKV Thu Mar 21 17:28:46 2002 t
	    Done();
	    return;
	  }
	  else if(myFuse == 0) {
	    //modified by NIZNHY-PKV Thu Mar 21 17:29:04 2002 f
	    //BRepAlgo_Cut c(mySbase, thePartsOfTool);
	    BRepAlgoAPI_Cut c(mySbase, thePartsOfTool);
	    //modified by NIZNHY-PKV Thu Mar 21 17:29:07 2002 t
	    myShape = c.Shape();
	    //modified by NIZNHY-PKV Thu Mar 21 17:29:23 2002 f
	    //UpdateDescendants(c.Builder(), myShape, Standard_False);
	    UpdateDescendants(c, myShape, Standard_False);
	    //modified by NIZNHY-PKV Thu Mar 21 17:29:32 2002 t
	    Done();
	    return;
	  }
	}
	if (theTOpe.IsInvDone()) {  
	  myStatusError = BRepFeat_LocOpeNotDone;
	}
	else {
	  myStatusError = BRepFeat_LocOpeInvNotDone;
	}
	NotDone();
	return;
      }
    }
    else {
      // tout est deja fait
      Done();
    }
  }

  myStatusError = BRepFeat_OK;
}



//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFeat_Form::IsDeleted(const TopoDS_Shape& F)
{
  return (myMap(F).IsEmpty());
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Form::Modified
   (const TopoDS_Shape& F)
{
  if (myMap.IsBound(F)) {
    static TopTools_ListOfShape list;
    list.Clear(); // Pour le second passage DPF
    TopTools_ListIteratorOfListOfShape ite(myMap(F));
    for(; ite.More(); ite.Next()) {
      const TopoDS_Shape& sh = ite.Value();
      if(!sh.IsSame(F)) 
	list.Append(sh);
    }
    return list;
  }
  return myGenerated; // empty list
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Form::Generated
   (const TopoDS_Shape& S)
{
  if (myMap.IsBound(S) && 
      S.ShapeType() != TopAbs_FACE) { // voir si on filtre sur face ou pas
    static TopTools_ListOfShape list;
    list.Clear(); // Pour le second passage DPF
    TopTools_ListIteratorOfListOfShape ite(myMap(S));
    for(; ite.More(); ite.Next()) {
      const TopoDS_Shape& sh = ite.Value();
      if(!sh.IsSame(S)) 
	list.Append(sh);
    }
    return list;
  }
  else return myGenerated;
}



//=======================================================================
//function : UpdateDescendants
//purpose  : 
//=======================================================================

void BRepFeat_Form::UpdateDescendants(const LocOpe_Gluer& G)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm;
  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_MapIteratorOfMapOfShape itm;

  for (itdm.Initialize(myMap);itdm.More();itdm.Next()) {
    const TopoDS_Shape& orig = itdm.Key();
    TopTools_MapOfShape newdsc;
    for (it.Initialize(itdm.Value());it.More();it.Next()) {
      const TopoDS_Face& fdsc = TopoDS::Face(it.Value()); 
      for (it2.Initialize(G.DescendantFaces(fdsc));
	   it2.More();it2.Next()) {
	newdsc.Add(it2.Value());
      }
    }
    myMap.ChangeFind(orig).Clear();
    for (itm.Initialize(newdsc);itm.More();itm.Next()) {
      myMap.ChangeFind(orig).Append(itm.Key());
    }
  }
}





//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Form::FirstShape() const
{
  if (!myFShape.IsNull()) {
    return myMap(myFShape);
  }
  return myGenerated; // empty list
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Form::LastShape() const
{
  if (!myLShape.IsNull()) {
    return myMap(myLShape);
  }
  return myGenerated; // empty list
}


//=======================================================================
//function : NewEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Form::NewEdges() const
{
  return myNewEdges;
}


//=======================================================================
//function : NewEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Form::TgtEdges() const
{
  return myTgtEdges;
}


//=======================================================================
//function : TransformSUntil
//purpose  : Limitation du shape until dans le cas des faces infinies
//=======================================================================

Standard_Boolean BRepFeat_Form::TransformShapeFU(const Standard_Integer flag)
{
#ifdef DEB
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
#endif
  Standard_Boolean Trf = Standard_False;

  TopoDS_Shape shapefu;
  if(flag == 0)
    shapefu = mySFrom;
  else if(flag == 1)
    shapefu = mySUntil;
  else 
    return Trf;

  TopExp_Explorer exp(shapefu, TopAbs_FACE);
  if (!exp.More()) { // pas de faces... Il faudrait renvoyer une erreur
#ifdef DEB
    if (trc) cout << " BRepFeat_Form::TransformShapeFU : invalid Shape" << endl;
#endif
    return Trf;
  }

  exp.Next();
  if (!exp.More()) { // une seule face. Est-elle infinie??
    exp.ReInit();
    TopoDS_Face fac = TopoDS::Face(exp.Current());

    Handle(Geom_Surface) S = BRep_Tool::Surface(fac);
    Handle(Standard_Type) styp = S->DynamicType();
    if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
      styp =  S->DynamicType();
    }

    if (styp == STANDARD_TYPE(Geom_Plane) ||
	styp == STANDARD_TYPE(Geom_CylindricalSurface) ||
	styp == STANDARD_TYPE(Geom_ConicalSurface)) {
      TopExp_Explorer exp1(fac, TopAbs_WIRE);
      if (!exp1.More()) {
	Trf = Standard_True;
      }
      else {
	Trf = BRep_Tool::NaturalRestriction(fac);
      }

    }
    if (Trf) {
      BRepFeat::FaceUntil(mySbase, fac);
    }

    if(flag == 0) {
      TopTools_ListOfShape thelist6;
      myMap.Bind(mySFrom,thelist6);
      myMap(mySFrom).Append(fac);
      mySFrom = fac;
    }
    else if(flag == 1) {
      TopTools_ListOfShape thelist7;
      myMap.Bind(mySUntil,thelist7);
      myMap(mySUntil).Append(fac);
      mySUntil = fac;
    }
    else {
    }
  }
  else {
    for (exp.ReInit(); exp.More(); exp.Next()) {
      const TopoDS_Shape& fac = exp.Current();
      TopTools_ListOfShape thelist8;
      myMap.Bind(fac,thelist8);
      myMap(fac).Append(fac);
    }
  }
#ifdef DEB
  if (trc) {
    if (Trf && (flag == 0)) cout << " TransformShapeFU From" << endl;
    if (Trf && (flag == 1)) cout << " TransformShapeFU Until" << endl;
  }
#endif
  return Trf;
}


//=======================================================================
//function : CurrentStatusError
//purpose  : 
//=======================================================================

BRepFeat_StatusError BRepFeat_Form::CurrentStatusError() const
{
  return myStatusError;
}

/*
//=======================================================================
//function : Descendants
//purpose  : 
//=======================================================================

static void Descendants(const TopoDS_Shape& S,
			const LocOpe_Builder& theTOpe,
			TopTools_MapOfShape& mapF)
{
  mapF.Clear();
  const Handle(TopOpeBRepBuild_HBuilder) B = theTOpe.Builder();
  TopTools_ListIteratorOfListOfShape it;
  TopExp_Explorer exp;
  for (exp.Init(S,TopAbs_FACE); exp.More(); exp.Next()) {
    const TopoDS_Face& fdsc = TopoDS::Face(exp.Current());
    if (B->IsSplit(fdsc, TopAbs_OUT)) {
      for (it.Initialize(B->Splits(fdsc,TopAbs_OUT));
	   it.More();it.Next()) {
	mapF.Add(it.Value());
      }
    }
    if (B->IsSplit(fdsc, TopAbs_IN)) {
      for (it.Initialize(B->Splits(fdsc,TopAbs_IN));
	   it.More();it.Next()) {
	mapF.Add(it.Value());
      }
    }
    if (B->IsSplit(fdsc, TopAbs_ON)) {
      for (it.Initialize(B->Splits(fdsc,TopAbs_ON));
	   it.More();it.Next()) {
	mapF.Add(it.Value());
      }
    }
    if (B->IsMerged(fdsc, TopAbs_OUT)) {
      for (it.Initialize(B->Merged(fdsc,TopAbs_OUT));
	   it.More();it.Next()) {
	mapF.Add(it.Value());
      }
    }
    if (B->IsMerged(fdsc, TopAbs_IN)) {
      for (it.Initialize(B->Merged(fdsc,TopAbs_IN));
	   it.More();it.Next()) {
	mapF.Add(it.Value());
      }
    }
    if (B->IsMerged(fdsc, TopAbs_ON)) {
      for (it.Initialize(B->Merged(fdsc,TopAbs_ON));
	   it.More();it.Next()) {
	mapF.Add(it.Value());
      }
    }
  }
}
*/

//=======================================================================
//function : Descendants
//purpose  : 
//=======================================================================

static void Descendants(const TopoDS_Shape& S,
			const LocOpe_Builder& theTOpe,
			TopTools_MapOfShape& mapF)
{
  mapF.Clear();
  const Handle(BOP_HistoryCollector)& B = theTOpe.History();
  TopTools_ListIteratorOfListOfShape it;
  TopExp_Explorer exp;
  for (exp.Init(S,TopAbs_FACE); exp.More(); exp.Next()) {
   
    const TopoDS_Face& fdsc = TopoDS::Face(exp.Current());
    const TopTools_ListOfShape& aLM=B->Modified(fdsc);
    it.Initialize(aLM);
    for (; it.More(); it.Next()) {
      mapF.Add(it.Value());
    }
    
  }
}

//=======================================================================
//function : UpdateDescendants
//purpose  : 
//=======================================================================
  void BRepFeat_Form::UpdateDescendants(const Handle(TopOpeBRepBuild_HBuilder)& B,
					const TopoDS_Shape& S,
					const Standard_Boolean SkipFace)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm;
  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_MapIteratorOfMapOfShape itm;
  TopExp_Explorer exp;

  for (itdm.Initialize(myMap);itdm.More();itdm.Next()) {
    const TopoDS_Shape& orig = itdm.Key();
    if (SkipFace && orig.ShapeType() == TopAbs_FACE) {
      continue;
    }
    TopTools_MapOfShape newdsc;

    if (itdm.Value().IsEmpty()) {myMap.ChangeFind(orig).Append(orig);}

    for (it.Initialize(itdm.Value());it.More();it.Next()) {
      const TopoDS_Shape& sh = it.Value();
      if(sh.ShapeType() != TopAbs_FACE) continue;
      const TopoDS_Face& fdsc = TopoDS::Face(it.Value()); 
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
	if (exp.Current().IsSame(fdsc)) { // preserved
	  newdsc.Add(fdsc);
	  break;
	}
      }
      if (!exp.More()) {
	if (B->IsSplit(fdsc, TopAbs_OUT)) {
	  for (it2.Initialize(B->Splits(fdsc,TopAbs_OUT));
	       it2.More();it2.Next()) {
	    newdsc.Add(it2.Value());
	  }
	}
	if (B->IsSplit(fdsc, TopAbs_IN)) {
	  for (it2.Initialize(B->Splits(fdsc,TopAbs_IN));
	       it2.More();it2.Next()) {
	    newdsc.Add(it2.Value());
	  }
	}
	if (B->IsSplit(fdsc, TopAbs_ON)) {
	  for (it2.Initialize(B->Splits(fdsc,TopAbs_ON));
	       it2.More();it2.Next()) {
	    newdsc.Add(it2.Value());
	  }
	}
	if (B->IsMerged(fdsc, TopAbs_OUT)) {
	  for (it2.Initialize(B->Merged(fdsc,TopAbs_OUT));
	       it2.More();it2.Next()) {
	    newdsc.Add(it2.Value());
	  }
	}
	if (B->IsMerged(fdsc, TopAbs_IN)) {
	  for (it2.Initialize(B->Merged(fdsc,TopAbs_IN));
	       it2.More();it2.Next()) {
	    newdsc.Add(it2.Value());
	  }
	}
	if (B->IsMerged(fdsc, TopAbs_ON)) {
	  for (it2.Initialize(B->Merged(fdsc,TopAbs_ON));
	       it2.More();it2.Next()) {
	    newdsc.Add(it2.Value());
	  }
	}
      }
    }
    myMap.ChangeFind(orig).Clear();
    for (itm.Initialize(newdsc); itm.More(); itm.Next()) {
       // on verifie l`appartenance au shape...
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
	if (exp.Current().IsSame(itm.Key())) {
//	  const TopoDS_Shape& sh = itm.Key();
	  myMap.ChangeFind(orig).Append(itm.Key());
	  break;
	}
      }
    }
  }
}
//modified by NIZNHY-PKV Thu Mar 21 18:43:18 2002 f
//=======================================================================
//function : UpdateDescendants
//purpose  : 
//=======================================================================
  void BRepFeat_Form::UpdateDescendants(const BRepAlgoAPI_BooleanOperation& aBOP,
					const TopoDS_Shape& S,
					const Standard_Boolean SkipFace)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm;
  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_MapIteratorOfMapOfShape itm;
  TopExp_Explorer exp;

  for (itdm.Initialize(myMap);itdm.More();itdm.Next()) {
    const TopoDS_Shape& orig = itdm.Key();
    if (SkipFace && orig.ShapeType() == TopAbs_FACE) {
      continue;
    }
    TopTools_MapOfShape newdsc;

    if (itdm.Value().IsEmpty()) {myMap.ChangeFind(orig).Append(orig);}

    for (it.Initialize(itdm.Value());it.More();it.Next()) {
      const TopoDS_Shape& sh = it.Value();
      if(sh.ShapeType() != TopAbs_FACE) continue;
      const TopoDS_Face& fdsc = TopoDS::Face(it.Value()); 
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
	if (exp.Current().IsSame(fdsc)) { // preserved
	  newdsc.Add(fdsc);
	  break;
	}
      }
      if (!exp.More()) {
	BRepAlgoAPI_BooleanOperation* pBOP=(BRepAlgoAPI_BooleanOperation*)&aBOP;
	const TopTools_ListOfShape& aLM=pBOP->Modified(fdsc);
	it2.Initialize(aLM);
	for (; it2.More(); it2.Next()) {
	  const TopoDS_Shape& aS=it2.Value();
	  newdsc.Add(aS);
	}
	
      }
    }
    myMap.ChangeFind(orig).Clear();
    for (itm.Initialize(newdsc); itm.More(); itm.Next()) {
       // on verifie l`appartenance au shape...
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
	if (exp.Current().IsSame(itm.Key())) {
//	  const TopoDS_Shape& sh = itm.Key();
	  myMap.ChangeFind(orig).Append(itm.Key());
	  break;
	}
      }
    }
  }
}
//modified by NIZNHY-PKV Thu Mar 21 18:43:36 2002 t
//=======================================================================
//function : UpdateDescendants
//purpose  : 
//=======================================================================
  void BRepFeat_Form::UpdateDescendants(const Handle(BOP_HistoryCollector)& aBOP,
					const TopoDS_Shape& S,
					const Standard_Boolean SkipFace)
{
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itdm;
  TopTools_ListIteratorOfListOfShape it,it2;
  TopTools_MapIteratorOfMapOfShape itm;
  TopExp_Explorer exp;

  for (itdm.Initialize(myMap);itdm.More();itdm.Next()) {
    const TopoDS_Shape& orig = itdm.Key();
    if (SkipFace && orig.ShapeType() == TopAbs_FACE) {
      continue;
    }
    TopTools_MapOfShape newdsc;

    if (itdm.Value().IsEmpty()) {myMap.ChangeFind(orig).Append(orig);}

    for (it.Initialize(itdm.Value());it.More();it.Next()) {
      const TopoDS_Shape& sh = it.Value();
      if(sh.ShapeType() != TopAbs_FACE) continue;
      const TopoDS_Face& fdsc = TopoDS::Face(it.Value()); 
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
	if (exp.Current().IsSame(fdsc)) { // preserved
	  newdsc.Add(fdsc);
	  break;
	}
      }
      if (!exp.More()) {
	const TopTools_ListOfShape& aLM=aBOP->Modified(fdsc);
	it2.Initialize(aLM);
	for (; it2.More(); it2.Next()) {
	  const TopoDS_Shape& aS=it2.Value();
	  newdsc.Add(aS);
	}
	
      }
    }
    myMap.ChangeFind(orig).Clear();
    for (itm.Initialize(newdsc); itm.More(); itm.Next()) {
       // on verifie l`appartenance au shape...
      for (exp.Init(S,TopAbs_FACE);exp.More();exp.Next()) {
	if (exp.Current().IsSame(itm.Key())) {
//	  const TopoDS_Shape& sh = itm.Key();
	  myMap.ChangeFind(orig).Append(itm.Key());
	  break;
	}
      }
    }
  }
}
