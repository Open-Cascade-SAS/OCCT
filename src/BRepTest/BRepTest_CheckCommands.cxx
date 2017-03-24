// Created on: 1996-02-23
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepTest.hxx>
#include <BRepAlgo.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Draw_SequenceOfDrawable3D.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Edge.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Draw.hxx>
#include <DBRep.hxx>
#include <BRepTest.hxx>
#include <GeometryTest.hxx>
#include <Precision.hxx>
#include <LocalAnalysis.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <DrawTrSurf.hxx>
#include <GeomAbs_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>

#include <TopOpeBRepTool_PurgeInternalEdges.hxx>
//#include <TopOpeBRepTool_FuseEdges.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FuseEdges.hxx>

#include <TopTools_HSequenceOfShape.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <stdio.h>

//Number of BRepCheck_Statuses in BRepCheck_Status.hxx file
//(BRepCheck_NoError is not considered, i.e. general status 
//is smaller by one specified in file)
static const Standard_Integer NumberOfStatus = 36;

static char* checkfaultyname = NULL;
Standard_EXPORT void BRepTest_CheckCommands_SetFaultyName(const char* name)
{
  if (checkfaultyname != NULL) {
    free(checkfaultyname);
    checkfaultyname = NULL;
  }
  if (name == NULL) {
    checkfaultyname = (char*)malloc(strlen("faulty_")+1);
    strcpy(checkfaultyname,"faulty_");
  }
  else {
    checkfaultyname = (char*)malloc(strlen(name)+1);
    strcpy(checkfaultyname,name);
  }
}


static TopTools_DataMapOfShapeListOfShape theMap;
static Standard_Integer nbfaulty = 0;
static Draw_SequenceOfDrawable3D lfaulty;

Standard_IMPORT Standard_Integer BRepCheck_Trace(const Standard_Integer phase);

//=======================================================================
//function : FindNamed
//=======================================================================
static Standard_Boolean FindNamed(const TopoDS_Shape& S,
				  char*& Name)
{
  for (Standard_Integer i = 1 ;i <= lfaulty.Length(); i++) {
    Handle(DBRep_DrawableShape) DS = 
      Handle(DBRep_DrawableShape)::DownCast(lfaulty(i));
    if (DS->Shape().IsSame(S)) {
      Name = (char*)DS->Name();
      return Standard_True;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : Contains
//=======================================================================
static Standard_Boolean Contains(const TopTools_ListOfShape& L,
				 const TopoDS_Shape& S)
{
  TopTools_ListIteratorOfListOfShape it;
  for (it.Initialize(L); it.More(); it.Next()) {
    if (it.Value().IsSame(S)) {
      return Standard_True;
    }
  }
  return Standard_False;
}



//=======================================================================
//function : PrintSub
//=======================================================================
static void PrintSub(Standard_OStream& OS,
		     const BRepCheck_Analyzer& Ana,
		     const TopoDS_Shape& S,
		     const TopAbs_ShapeEnum Subtype)
     
{
  char* Name;
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopExp_Explorer exp;
  for (exp.Init(S,Subtype); exp.More(); exp.Next()) {
    const Handle(BRepCheck_Result)& res = Ana.Result(exp.Current());
    const TopoDS_Shape& sub = exp.Current();
    for (res->InitContextIterator();
	 res->MoreShapeInContext(); 
	 res->NextShapeInContext()) {
      if (res->ContextualShape().IsSame(S) && 
	  !Contains(theMap(sub),S)) {
	theMap(sub).Append(S);
	itl.Initialize(res->StatusOnShape());
	if (itl.Value() != BRepCheck_NoError) {
	  if (!FindNamed(sub,Name)) {
	    nbfaulty++;
	    Name = (char*)malloc(18*sizeof(char));
	    Sprintf(Name,"%s%d",checkfaultyname,nbfaulty);
	    DBRep::Set(Name,sub);
	    lfaulty.Append(Draw::Get((Standard_CString&)Name));
	  }
	  OS << "Shape " << Name << " ";
	  if (!FindNamed(S,Name)) {
	    nbfaulty++;
	    Name = (char*)malloc(18*sizeof(char));
	    Sprintf(Name,"%s%d",checkfaultyname,nbfaulty);
	    DBRep::Set(Name,S);
	    lfaulty.Append(Draw::Get((Standard_CString&)Name));
	  }
	  OS << " on shape " << Name << " :\n";
	  for (;itl.More(); itl.Next()) {
	    BRepCheck::Print(itl.Value(),OS);
	  }
	}
	break;
      }
    }
  }
}


//=======================================================================
//function : Print
//=======================================================================
static void Print(Standard_OStream& OS,
		  const BRepCheck_Analyzer& Ana,
		  const TopoDS_Shape& S)
{
  for (TopoDS_Iterator iter(S); iter.More(); iter.Next()) {
    Print(OS,Ana,iter.Value());
  }

  char* Name;
  TopAbs_ShapeEnum styp = S.ShapeType();
  BRepCheck_ListIteratorOfListOfStatus itl;
  if (!Ana.Result(S).IsNull() && !theMap.IsBound(S)) {
    itl.Initialize(Ana.Result(S)->Status());
    if (itl.Value() != BRepCheck_NoError) {
      if (!FindNamed(S,Name)) {
	nbfaulty++;
	Name = (char*)malloc(18*sizeof(char));
	Sprintf(Name,"%s%d",checkfaultyname,nbfaulty);
	DBRep::Set(Name,S);
	lfaulty.Append(Draw::Get((Standard_CString&)Name));
      }
      OS << "On Shape " << Name << " :\n";
	
      for (;itl.More(); itl.Next()) {
        if (itl.Value() != BRepCheck_NoError)
	BRepCheck::Print(itl.Value(),OS);
      }
    }
  }
  if (!theMap.IsBound(S)) {
    TopTools_ListOfShape thelist;
    theMap.Bind(S, thelist);
  }

  switch (styp) {
  case TopAbs_EDGE:
    PrintSub(OS,Ana,S,TopAbs_VERTEX);
    break;
  case TopAbs_WIRE:
    PrintSub(OS,Ana,S,TopAbs_EDGE);
    PrintSub(OS,Ana,S,TopAbs_VERTEX);
    break;
  case TopAbs_FACE:
    PrintSub(OS,Ana,S,TopAbs_WIRE);
    PrintSub(OS,Ana,S,TopAbs_EDGE);
    PrintSub(OS,Ana,S,TopAbs_VERTEX);
    break;
  case TopAbs_SHELL:
//    PrintSub(OS,Ana,S,TopAbs_FACE);
    break;
  case TopAbs_SOLID:
//    PrintSub(OS,Ana,S,TopAbs_EDGE);
    PrintSub(OS,Ana,S,TopAbs_SHELL);
    break;
  default:
    break;
  }

}

//=======================================================================
//function : computetolerance
//purpose  : 
//=======================================================================
static Standard_Integer computetolerance(Draw_Interpretor& di,
					 Standard_Integer narg, const char** a)
{
  if (narg < 2) {
    //cout << "Usage: computetolerance shape" << endl;
    di << "Usage: computetolerance shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);
  Standard_Real tol;
  if (S.ShapeType() == TopAbs_EDGE) {
    BRepCheck_Edge bce(TopoDS::Edge(S));
    tol=bce.Tolerance();
    //cout<< "Tolerance de " << (void*) &(*S.TShape()) << " : " << tol << endl;
    Standard_SStream aSStream1;
    aSStream1<< "Tolerance de " << (void*) &(*S.TShape()) << " : " << tol << "\n";
    di << aSStream1;
  }
  else {
    TopTools_MapOfShape theEdges;
    TopExp_Explorer exp;
    for (exp.Init(S, TopAbs_EDGE); exp.More(); exp.Next()) {
      if (theEdges.Add(exp.Current())) {
	BRepCheck_Edge bce(TopoDS::Edge(exp.Current()));
	tol=bce.Tolerance();
	//cout<< "Tolerance de " << (void*) &(*exp.Current().TShape()) << " : " << tol << "\n";
	Standard_SStream aSStream2;
	aSStream2<< "Tolerance de " << (void*) &(*exp.Current().TShape()) << " : " << tol << "\n";
	di << aSStream2;
      }
    }
    //cout << endl;
    di << "\n";
  }
  return 0;
}

//=======================================================================
//function : checksection
//purpose  : Checks the closure of a section line
//=======================================================================
static Standard_Integer checksection(Draw_Interpretor& di,
				     Standard_Integer narg, const char** a)
{
  if (narg < 2) {
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);
  TopTools_MapOfShape theVertices;
  TopExp_Explorer exp;
  for (exp.Init(S, TopAbs_VERTEX); exp.More(); exp.Next()) {
    if (!theVertices.Add(exp.Current())) 
      theVertices.Remove(exp.Current());
  }
  //cout << " nb alone Vertices : " << theVertices.Extent() << endl;
  di << " nb alone Vertices : " << theVertices.Extent() << "\n";
  char Name[32];
  Standard_Integer ipp=0;
  TopTools_MapIteratorOfMapOfShape itvx;
  for (itvx.Initialize(theVertices); itvx.More(); itvx.Next()) {
    ipp++;
    Sprintf(Name,"alone_%d",ipp);
    DBRep::Set(Name, itvx.Key());
    //cout << Name << " " ;
    di << Name << " " ;
  }
  //cout << endl;
  di << "\n";
  return 0;
}
//=======================================================================

//=======================================================================
//function : checkdiff
//purpose  : Checks the differences beetween a result and his arguments
//=======================================================================
static Standard_Integer checkdiff(Draw_Interpretor& di,
				  Standard_Integer narg, const char** a)
{
  const char* syntaxe = "checkdiff arg1 [arg2..argn] result [closedSolid (0/1)] [geomCtrl (1/0)]";
  if (narg < 3) {
    if (narg==2) {
      Standard_Integer bcrtrace=Draw::Atoi(a[narg-1]);
      bcrtrace=BRepCheck_Trace(bcrtrace);
      //cout << "BRepCheck_Trace : " << bcrtrace << endl;
      di << "BRepCheck_Trace : " << bcrtrace << "\n";
      //cout << syntaxe << endl;
      di << syntaxe << "\n";
      return 0;
    }
    //cout << syntaxe << endl;
    di << syntaxe << "\n";
    return 1;
  }

  Standard_Integer lastArg=narg-2;
  Standard_Boolean closedSolid = Standard_False;
  Standard_Boolean geomCtrl = Standard_True;
  TopoDS_Shape resu = DBRep::Get(a[narg-1]);
  if (resu.IsNull()) {
    if (narg < 4) {
      //cout << syntaxe << endl;
      di << syntaxe << "\n";
      return 1;
    }
    closedSolid = Draw::Atoi(a[narg-1]) != 0;
    resu = DBRep::Get(a[narg-2]);
    lastArg=narg-3;
    if (resu.IsNull()) {
      if (narg < 5) {
	//cout << syntaxe << endl;
	di << syntaxe << "\n";
	return 1;
      }
      geomCtrl=closedSolid;
      closedSolid = Draw::Atoi(a[narg-2]) != 0;
      resu = DBRep::Get(a[narg-3]);
      lastArg=narg-4;
      if (resu.IsNull()) {
	//cout << syntaxe << endl;
	di << syntaxe << "\n";
	return 1;
      }
    }
  }
  
  TopTools_ListOfShape lesArgs;
  for (Standard_Integer id=1; id <=lastArg; id++) {
    lesArgs.Append(DBRep::Get(a[id]));
  }

  if (BRepAlgo::IsValid(lesArgs, resu, closedSolid, geomCtrl)) {
    //cout << "Difference is Valid." << endl;
    di << "Difference is Valid.\n";
  } else {
    //cout << "error : Difference is Not Valid !" << endl;
    di << "error : Difference is Not Valid !\n";
  }

  return 0;
}
//=======================================================================

//  Modified by skv - Tue Apr 27 13:38:44 2004 Begin
//=======================================================================
//function : CHK
//purpose  : Checks a shape
//=======================================================================

// static Standard_Integer CHK(Draw_Interpretor& theCommands,
// 			    Standard_Integer narg, const char** a)
// {
//   if (narg < 2) {
//     return 1;
//   }

//   Standard_Boolean doprint = Standard_True;
//   if (narg == 3) { if (!strcmp(a[2],"-short")) doprint = Standard_False; }

//   TopoDS_Shape S = DBRep::Get(a[1]);
//   if (S.IsNull()) {
//     cout<<"not a topological shape"<<endl;
//     return 1;
//   }

//   Standard_Boolean GeomCtrl = Standard_True;
//   if (!strcasecmp(a[0],"CHECKTOPSHAPE")) {
//     GeomCtrl = Standard_False;
//   }

//   BRepCheck_Analyzer ana(S,GeomCtrl);
//   if (ana.IsValid()) {
//     theCommands<<"This shape seems to be valid";
//   }
//   else {
//     theMap.Clear();
//     nbfaulty = 0;
//     lfaulty.Clear();
//     theMap.Clear();
//     if (doprint) {
//       Print(cout,ana,S);
//       cout<<"\n";
//       theMap.Clear();
//       if (nbfaulty !=0)
// 	cout<<"Faulty shapes in variables "<<checkfaultyname<<"1 to "<<checkfaultyname<<nbfaulty<<" \n";
//       cout<<endl;
//     }
//     else {
//       theCommands<<"This shape has faulty shapes";
//     }
//   }
//   return 0;
// }

//=======================================================================
//function : ContextualDump
//purpose  : Contextual (modeling) style of output.
//=======================================================================

//void ContextualDump(const BRepCheck_Analyzer &theAna,
//		    const TopoDS_Shape       &theShape)
void ContextualDump(Draw_Interpretor& theCommands,
		    const BRepCheck_Analyzer &theAna,
		    const TopoDS_Shape       &theShape)
{
  theMap.Clear();
  nbfaulty = 0;
  lfaulty.Clear();

  //Print(cout, theAna, theShape);
  Standard_SStream aSStream;
  Print(aSStream, theAna, theShape);
  theCommands << aSStream;
  //cout<<"\n";
  theCommands<<"\n";
  theMap.Clear();

  if (nbfaulty !=0)
    theCommands<<"Faulty shapes in variables "<<checkfaultyname<<"1 to "<<checkfaultyname<<nbfaulty<<" \n";
    //cout<<"Faulty shapes in variables "<<checkfaultyname<<"1 to "<<checkfaultyname<<nbfaulty<<" \n";

  //cout<<endl;
  theCommands<<"\n";
}


//=======================================================================
//function : FillProblems
// purpose : auxilary for StructuralDump
//=======================================================================
static void FillProblems(const BRepCheck_Status stat,
                         Handle(TColStd_HArray1OfInteger)& NbProblems)
{

  const Standard_Integer anID = static_cast<Standard_Integer> (stat);

  if((NbProblems->Upper() < anID) || (NbProblems->Lower() > anID))
    return;

  NbProblems->SetValue(anID, NbProblems->Value(anID)+1);

}


//=======================================================================
//function : GetProblemSub
// purpose : auxilary for StructuralDump
//=======================================================================
static void GetProblemSub(const BRepCheck_Analyzer& Ana,
                          const TopoDS_Shape& Shape,
                          Handle(TopTools_HSequenceOfShape)& sl,
                          Handle(TColStd_HArray1OfInteger)& NbProblems,
                          const TopAbs_ShapeEnum Subtype)
{
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopExp_Explorer exp;
  for (exp.Init(Shape,Subtype); exp.More(); exp.Next()) {
    const Handle(BRepCheck_Result)& res = Ana.Result(exp.Current());

    const TopoDS_Shape& sub = exp.Current();
    for (res->InitContextIterator();
	 res->MoreShapeInContext(); 
	 res->NextShapeInContext()) {
      if (res->ContextualShape().IsSame(Shape) && 
	  !Contains(theMap(sub),Shape)) {
	theMap(sub).Append(Shape);
	itl.Initialize(res->StatusOnShape());

	if (itl.Value() != BRepCheck_NoError) {
	  Standard_Integer ii = 0;

          for(ii=1; ii<=sl->Length(); ii++)
            if(sl->Value(ii).IsSame(sub)) break;

          if(ii>sl->Length()) {
            sl->Append(sub);
            FillProblems(itl.Value(),NbProblems);
          }
          for(ii=1; ii<=sl->Length(); ii++)
            if(sl->Value(ii).IsSame(Shape)) break;
          if(ii>sl->Length()) {
            sl->Append(Shape);
            FillProblems(itl.Value(),NbProblems);
          }
	}
	break;
      }
    }
  }
}


//=======================================================================
//function : GetProblemShapes
// purpose : auxilary for StructuralDump
//=======================================================================
static void GetProblemShapes(const BRepCheck_Analyzer& Ana,
                             const TopoDS_Shape& Shape,
                             Handle(TopTools_HSequenceOfShape)& sl,
                             Handle(TColStd_HArray1OfInteger)& NbProblems)
{
  for (TopoDS_Iterator iter(Shape); iter.More(); iter.Next()) {
    GetProblemShapes(Ana,iter.Value(),sl, NbProblems);
  }
  TopAbs_ShapeEnum styp = Shape.ShapeType();
  BRepCheck_ListIteratorOfListOfStatus itl;
  if (!Ana.Result(Shape).IsNull() && !theMap.IsBound(Shape)) {
    itl.Initialize(Ana.Result(Shape)->Status());

    if (itl.Value() != BRepCheck_NoError) {
      sl->Append(Shape);
      FillProblems(itl.Value(),NbProblems);
    }
  }
  if (!theMap.IsBound(Shape)) {
    TopTools_ListOfShape thelist;
    theMap.Bind(Shape, thelist);
  }

  switch (styp) {
  case TopAbs_EDGE:
    GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX);
    break;
  case TopAbs_FACE:
    GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_WIRE);
    GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_EDGE);
    GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX);
    break;
  case TopAbs_SHELL:
    break;
  case TopAbs_SOLID:
    GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_SHELL);
    break;
  default:
    break;
  }

}

//=======================================================================
//function : StructuralDump
//purpose  : Structural (data exchange) style of output.
//=======================================================================

//void StructuralDump(const BRepCheck_Analyzer &theAna,
//                    const Standard_CString   ShName,
//                    const Standard_CString   Pref,
//		    const TopoDS_Shape       &theShape)
void StructuralDump(Draw_Interpretor& theCommands,
		    const BRepCheck_Analyzer &theAna,
                    const Standard_CString   ShName,
                    const Standard_CString   Pref,
		    const TopoDS_Shape       &theShape)
{
  Standard_Integer i;
  //cout << "StructuralDump" << endl;
  //cout << " -- The Shape " << ShName << " has problems :"<<endl;
  //cout<<"  Check                                    Count"<<endl;
  //cout<<" ------------------------------------------------"<<endl;
  theCommands << " -- The Shape " << ShName << " has problems :\n";
  theCommands<<"  Check                                    Count\n";
  theCommands<<" ------------------------------------------------\n";

  Handle(TColStd_HArray1OfInteger) NbProblems = new 
                              TColStd_HArray1OfInteger(1,NumberOfStatus);
  for(i=1; i<=NumberOfStatus; i++) NbProblems->SetValue(i,0);
  Handle(TopTools_HSequenceOfShape) sl,slv,sle,slw,slf,sls,slo;
  sl = new TopTools_HSequenceOfShape();
  theMap.Clear();
  GetProblemShapes(theAna, theShape, sl, NbProblems);
  theMap.Clear();
  
  Standard_Integer aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidPointOnCurve);
  if(NbProblems->Value(aProblemID) > 0)
    theCommands<<"  Invalid Point on Curve ................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidPointOnCurveOnSurface);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Point on CurveOnSurface .......... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidPointOnSurface);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Point on Surface ................. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_No3DCurve);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  No 3D Curve .............................. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_Multiple3DCurve);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Multiple 3D Curve ........................ "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_Invalid3DCurve);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid 3D Curve ......................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_NoCurveOnSurface);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  No Curve on Surface ...................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidCurveOnSurface);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Curve on Surface ................. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidCurveOnClosedSurface);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Curve on closed Surface .......... "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidSameRangeFlag);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid SameRange Flag ................... "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidSameParameterFlag);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid SameParameter Flag ............... "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidDegeneratedFlag);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Degenerated Flag ................. "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_FreeEdge);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Free Edge ................................ "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidMultiConnexity);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid MultiConnexity ................... "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidRange);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Range ............................ "<<NbProblems->Value(aProblemID)<<"\n";
  
  aProblemID = static_cast<Standard_Integer>(BRepCheck_EmptyWire);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Empty Wire ............................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_RedundantEdge);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Redundant Edge ........................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_SelfIntersectingWire);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Self Intersecting Wire ................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_NoSurface);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  No Surface ............................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidWire);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Wire ............................. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_RedundantWire);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Redundant Wire ........................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_IntersectingWires);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Intersecting Wires ....................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidImbricationOfWires);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Imbrication of Wires ............. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_EmptyShell);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Empty Shell .............................. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_RedundantFace);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Redundant Face ........................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_UnorientableShape);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Unorientable Shape ....................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_NotClosed);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Not Closed ............................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_NotConnected);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Not Connected ............................ "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_SubshapeNotInShape);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Subshape not in Shape .................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_BadOrientation);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Bad Orientation .......................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_BadOrientationOfSubshape);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Bad Orientation of Subshape .............. "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidToleranceValue);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid tolerance value................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidPolygonOnTriangulation);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid polygon on triangulation.......... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_InvalidImbricationOfShells);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Invalid Imbrication of Shells............. "<<NbProblems->Value(aProblemID)<<"\n";
 
 aProblemID = static_cast<Standard_Integer>(BRepCheck_EnclosedRegion);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  Enclosed Region........................... "<<NbProblems->Value(aProblemID)<<"\n";

  aProblemID = static_cast<Standard_Integer>(BRepCheck_CheckFail);
  if(NbProblems->Value(aProblemID)>0)
    theCommands<<"  checkshape failure........................ "<<NbProblems->Value(aProblemID)<<"\n";



  theCommands<<" ------------------------------------------------\n";
  theCommands<<"*** Shapes with problems : "<<sl->Length()<<"\n";

  slv = new TopTools_HSequenceOfShape();
  sle = new TopTools_HSequenceOfShape();
  slw = new TopTools_HSequenceOfShape();
  slf = new TopTools_HSequenceOfShape();
  sls = new TopTools_HSequenceOfShape();
  slo = new TopTools_HSequenceOfShape();

  for(i=1; i<=sl->Length(); i++) {
    TopoDS_Shape shi = sl->Value(i);
    TopAbs_ShapeEnum sti = shi.ShapeType();
    switch (sti) {
      case TopAbs_VERTEX : slv->Append (shi); break;
      case TopAbs_EDGE   : sle->Append (shi); break;
      case TopAbs_WIRE   : slw->Append (shi); break;
      case TopAbs_FACE   : slf->Append (shi); break;
      case TopAbs_SHELL  : sls->Append (shi); break;
      case TopAbs_SOLID  : slo->Append (shi); break;
      default            : break;
    }
  }

  BRep_Builder B;
  if(slv->Length()>0) {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    Standard_Integer nb = slv->Length();
    for(i=1; i<=nb; i++)
      B.Add(comp,slv->Value(i));
    char aName[20];
    Sprintf(aName,"%s_v",Pref);
    DBRep::Set(aName,comp);
    //cout<<"VERTEX	: "<<(nb > 9 ? "" : " ")<<nb<<" Items -> compound named "<<aName<<endl;
    if (nb > 9)
      theCommands<<"VERTEX	: "<<nb<<" Items -> compound named "<<aName<<"\n";
    else
      theCommands<<"VERTEX	:  "<<nb<<" Items -> compound named "<<aName<<"\n";
  }
  if(sle->Length()>0) {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    Standard_Integer nb = sle->Length();
    for(i=1; i<=nb; i++)
      B.Add(comp,sle->Value(i));
    char aName[20];
    Sprintf(aName,"%s_e",Pref);
    DBRep::Set(aName,comp);
    //cout<<"EDGE	: "<<(nb > 9 ? "" : " ")<<nb<<" Items -> compound named "<<aName<<endl;
    if (nb > 9)
      theCommands<<"EDGE	: "<<nb<<" Items -> compound named "<<aName<<"\n";
    else
      theCommands<<"EDGE	:  "<<nb<<" Items -> compound named "<<aName<<"\n";
  }
  if(slw->Length()>0) {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    Standard_Integer nb = slw->Length();
    for(i=1; i<=nb; i++)
      B.Add(comp,slw->Value(i));
    char aName[20];
    Sprintf(aName,"%s_w",Pref);
    DBRep::Set(aName,comp);
    //cout<<"WIRE	: "<<(nb > 9 ? "" : " ")<<nb<<" Items -> compound named "<<aName<<endl;
    if (nb > 9)
      theCommands<<"WIRE	: "<<nb<<" Items -> compound named "<<aName<<"\n";
    else
      theCommands<<"WIRE	:  "<<nb<<" Items -> compound named "<<aName<<"\n";
  }
  if(slf->Length()>0) {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    Standard_Integer nb = slf->Length();
    for(i=1; i<=nb; i++)
      B.Add(comp,slf->Value(i));
    char aName[20];
    Sprintf(aName,"%s_f",Pref);
    DBRep::Set(aName,comp);
    //cout<<"FACE	: "<<(nb > 9 ? "" : " ")<<nb<<" Items -> compound named "<<aName<<endl;
    if (nb > 9)
      theCommands<<"FACE	: "<<nb<<" Items -> compound named "<<aName<<"\n";
    else
      theCommands<<"FACE	:  "<<nb<<" Items -> compound named "<<aName<<"\n";
  }
  if(sls->Length()>0) {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    Standard_Integer nb = sls->Length();
    for(i=1; i<=nb; i++)
      B.Add(comp,sls->Value(i));
    char aName[20];
    Sprintf(aName,"%s_s",Pref);
    DBRep::Set(aName,comp);
    //cout<<"SHELL	: "<<(nb > 9 ? "" : " ")<<nb<<" Items -> compound named "<<aName<<endl;
    if (nb > 9)
      theCommands<<"SHELL	: "<<nb<<" Items -> compound named "<<aName<<"\n";
    else
      theCommands<<"SHELL	:  "<<nb<<" Items -> compound named "<<aName<<"\n";
  }
  if(slo->Length()>0) {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    Standard_Integer nb = slo->Length();
    for(i=1; i<=nb; i++)
      B.Add(comp,slo->Value(i));
    char aName[20];
    Sprintf(aName,"%s_o",Pref);
    DBRep::Set(aName,comp);
    //cout<<"SOLID	: "<<(nb > 9 ? "" : " ")<<nb<<" Items -> compound named "<<aName<<endl;
    if (nb > 9)
      theCommands<<"SOLID	: "<<nb<<" Items -> compound named "<<aName<<"\n";
    else
      theCommands<<"SOLID	:  "<<nb<<" Items -> compound named "<<aName<<"\n";
  }
}

//=======================================================================
//function : checkshape
//purpose  : Checks a shape
//=======================================================================

static Standard_Integer checkshape(Draw_Interpretor& theCommands,
				   Standard_Integer narg, const char** a)
{
  if (narg == 1) {
    //cout << endl;
    //cout << "Usage : checkshape [-top] shape [result] [-short]" << endl;
    //cout << endl;
    //cout << "Where :" << endl;
    //cout << "   -top   - check topology only." << endl;
    //cout << "   shape  - the name of the shape to test." << endl;
    //cout << "   result - the prefix of the output shape names. If it is used, structural" << endl;
    //cout << "            output style will be used. Otherwise - contextual one." << endl;
    //cout << "   -short - short description of check." << endl;
    theCommands << "\n";
    theCommands << "Usage : checkshape [-top] shape [result] [-short]\n";
    theCommands << "\n";
    theCommands << "Where :\n";
    theCommands << "   -top   - check topology only.\n";
    theCommands << "   shape  - the name of the shape to test.\n";
    theCommands << "   result - the prefix of the output shape names. If it is used, structural\n";
    theCommands << "            output style will be used. Otherwise - contextual one.\n";
    theCommands << "   -short - short description of check.\n";

    return 0;
  }

  if (narg > 5) {
    //cout << "Invalid number of args!!!" << endl;
    //cout << "No args to have help." << endl;
    theCommands << "Invalid number of args!!!\n";
    theCommands << "No args to have help.\n";

    return 1;
  }

  Standard_Boolean aGeomCtrl = Standard_True;
  Standard_Integer aCurInd  = 1;

  if (!strcmp(a[1],"-top")) {
    aGeomCtrl = Standard_False;
    aCurInd++;
  }

  if (aCurInd > narg - 1) {
    //cout << "Invalid number of args!!!" << endl;
    //cout << "No args to have help." << endl;
    theCommands << "Invalid number of args!!!\n";
    theCommands << "No args to have help.\n";

    return 1;
  }

  Standard_CString aShapeName = a[aCurInd];
  TopoDS_Shape     aShape     = DBRep::Get(aShapeName);

  if (aShape.IsNull()) {
    //cout << a[aCurInd] << " is not a topological shape!!!" << endl;
    theCommands << a[aCurInd] << " is not a topological shape!!!\n";

    return 1;
  }

  Standard_Boolean IsShortDump   = Standard_False;
  Standard_Boolean IsContextDump = Standard_True;
  Standard_Integer aBackInd      = narg - 1;

  if (aCurInd < aBackInd) {
    if (!strcmp(a[aBackInd],"-short")) {
      IsShortDump = Standard_True;
      aBackInd--;
    }
  }

  if (aCurInd < aBackInd - 1) {
    //cout << "Invalid number of args!!!" << endl;
    //cout << "No args to have help." << endl;
    theCommands << "Invalid number of args!!!\n";
    theCommands << "No args to have help.\n";

    return 1;
  } else if (aCurInd < aBackInd) {
    IsContextDump = Standard_False;
  }

  try {
    OCC_CATCH_SIGNALS
    BRepCheck_Analyzer anAna(aShape,aGeomCtrl);
    Standard_Boolean   isValid = anAna.IsValid();

    if (isValid) {
      if (IsContextDump) {
	theCommands << "This shape seems to be valid";
      } else {
	theCommands << " -- The Shape " << aShapeName << " looks OK";
      }
    } else {
      if (IsShortDump) {
	theCommands<<"This shape has faulty shapes";
      } else {
	if (IsContextDump) {
	  //ContextualDump(anAna, aShape);
	  ContextualDump(theCommands, anAna, aShape);
	} else {
	  Standard_CString aPref = a[aCurInd+1];
	  //StructuralDump(anAna, aShapeName, aPref, aShape);
	  StructuralDump(theCommands, anAna, aShapeName, aPref, aShape);
	}
      }
    }
  }
  catch(Standard_Failure const& anException) {
    theCommands<<"checkshape exception : ";
    theCommands << anException.GetMessageString();
    theCommands<<"\n";
    return 1;
  }

  return 0;
}
//  Modified by skv - Tue Apr 27 13:38:24 2004 End
/***************************************************************/
static void InitEpsSurf(Standard_Real& epsnl,Standard_Real& epsdis, Standard_Real& epsangk1, 
			Standard_Real& epsangk2, Standard_Real& epsangn1, 
			Standard_Real& perce,Standard_Real& maxlen )
{ 
  epsnl  = 0.001;
  epsdis = 0.001;
  epsangk1 = 0.001 ; 
  epsangk2 = 0.001;
  epsangn1 = 0.001 ; 
  perce = 0.01;
  maxlen = 10000 ;
}

static Standard_Integer shapeG1continuity (Draw_Interpretor& di, Standard_Integer n, const char** a)

{ Standard_Real epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen;
  Standard_Integer nbeval;  
  InitEpsSurf(epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen);
  Standard_Boolean ISG1=Standard_True;
  if (n<4) return 1;
  TopoDS_Face face1,face2;
  Standard_Real f1,f2,l1,l2;
  TopoDS_Shape shape = DBRep::Get(a[1],TopAbs_SHAPE);
  if (shape.IsNull()) return 1;
  TopoDS_Shape  edge  = DBRep::Get(a[2],TopAbs_EDGE);
  if (edge.IsNull()) return 1;
// calcul des deux faces 
  TopTools_IndexedDataMapOfShapeListOfShape lface;
  TopExp::MapShapesAndAncestors(shape,TopAbs_EDGE,TopAbs_FACE,lface);
  const TopTools_ListOfShape& lfac = lface.FindFromKey(edge);

  Standard_Integer nelem= lfac.Extent();
  if(nelem!=2) return 1; 
  TopTools_ListIteratorOfListOfShape It;
  It.Initialize(lfac);
  face1=TopoDS::Face(It.Value());
  It.Next();
  face2=TopoDS::Face(It.Value());
    
// calcul des deux pcurves 
  const Handle(Geom2d_Curve) c1 = BRep_Tool::CurveOnSurface
                               (TopoDS::Edge(edge),face1,f1,l1);
  if (c1.IsNull()) return 1;
  const  Handle(Geom2d_Curve) c2  = BRep_Tool::CurveOnSurface
                               (TopoDS::Edge(edge),face2,f2,l2);
  if (c2.IsNull()) return 1;
	
   Handle(Geom2d_Curve) curv1= new Geom2d_TrimmedCurve(c1,f1,l1);
   
   Handle(Geom2d_Curve) curv2= new Geom2d_TrimmedCurve(c2,f2,l2);  
   
// calcul dees deux surfaces 
  TopLoc_Location L1,L2; 
  TopoDS_Face aLocalFace = face1;
  const Handle(Geom_Surface)& s1 = BRep_Tool::Surface(aLocalFace,L1);
//  const Handle(Geom_Surface)& s1 =
//    BRep_Tool::Surface(TopoDS::Face(face1),L1);
  if (s1.IsNull()) return 1;
  aLocalFace = face2;
  const Handle(Geom_Surface)& s2 = BRep_Tool::Surface(aLocalFace,L2);
//  const Handle(Geom_Surface)& s2 =
//    BRep_Tool::Surface(TopoDS::Face(face2),L2); 
  if (s2.IsNull()) return 1;	 


   Handle(Geom_Surface) surf1 = Handle(Geom_Surface)::
                                 DownCast(s1->Transformed(L1.Transformation()));
   if (surf1.IsNull()) return 1;
   Handle(Geom_Surface) surf2 = Handle(Geom_Surface)::
                                DownCast(s2->Transformed(L2.Transformation()));
   if (surf2.IsNull()) return 1;
   
 

  nbeval = (Standard_Integer ) Draw::Atof( a[3]);

  switch(n)
    { case 7  : epsG1 = Draw::Atof(a[6]);
      case 6  : epsC0   = Draw::Atof(a[5]);
      case 5  : epsnl    = Draw::Atof(a[4]);
      case 4  : {} break;
      default : return 1;
    }


  Standard_Real pard1, parf1, U, Uf, deltaU, nb = 0;
  Standard_Boolean isconti = Standard_True;
  Standard_Boolean isdone = Standard_True;
  pard1 = curv1->FirstParameter();
  parf1 = curv1->LastParameter();
  Standard_Real  MaxG0Value=0, MaxG1Angle=0; 
  U = Min( pard1,parf1);
  Uf = Max (pard1,parf1);

  deltaU =  Abs(parf1- pard1)/nbeval;
      
  do  
     { if ( nb == nbeval) 
	  { LocalAnalysis_SurfaceContinuity res(curv1, curv2, Uf,surf1, surf2,            GeomAbs_G1, epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen );
	    isdone = res.IsDone();
           if ( isdone) { isconti = res.IsG1();
                             if (isconti) 
                              {if (res.C0Value()>MaxG0Value)  MaxG0Value = res.C0Value();
                               if (res.G1Angle ()>MaxG1Angle) MaxG1Angle = res.G1Angle();}}
	    else isconti = Standard_False;}
	            
          else {LocalAnalysis_SurfaceContinuity res (curv1, curv2, (U+nb*deltaU                 ), surf1,surf2, GeomAbs_G1,epsnl,epsC0, epsC1, epsC2, epsG1,
                 percent,maxlen);
		 isdone = res.IsDone();
                 if ( isdone) { isconti = res.IsG1();
				  if ( nb == 0) { MaxG0Value = res.C0Value();
						  MaxG1Angle = res.G1Angle();}
			          if (res.C0Value()> MaxG0Value)  MaxG0Value = res.C0Value();
				  if (res.G1Angle()> MaxG1Angle)  MaxG1Angle= res.G1Angle();;}
				  
		 else isconti = Standard_False;}
       if (!isconti) ISG1=Standard_False; 	                      
       nb++;
       
     }
   while ((nb<nbeval)&& isdone );

  //if (!isdone)  { cout<<" Problem in computation "<<endl; return 1;}
  //if (ISG1) 
  //    {cout<<" the continuity is G1 "<<endl;}
  //else { cout<<" the continuity is not G1  "<<endl;}
  //cout<<"MaxG0Value :"<< MaxG0Value << endl;
  //cout<<"MaxG1Angle:"<<  MaxG1Angle << endl;
  if (!isdone)  { di<<" Problem in computation \n"; return 1;}
  if (ISG1) 
      {di<<" the continuity is G1 \n";}
  else { di<<" the continuity is not G1  \n";}
  di<<"MaxG0Value :"<< MaxG0Value << "\n";
  di<<"MaxG1Angle:"<<  MaxG1Angle << "\n";
  return 0;
}
/*****************************************************************************/
static Standard_Integer shapeG0continuity (Draw_Interpretor& di, Standard_Integer n, const char** a)

{ Standard_Real epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen; 
  Standard_Integer nbeval;
  Standard_Boolean ISG0;
  InitEpsSurf(epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen  );

  if (n<4) return 1;
  TopoDS_Face face1,face2;
  Standard_Real f1,f2,l1,l2;
  TopoDS_Shape shape = DBRep::Get(a[1],TopAbs_SHAPE);
  if (shape.IsNull()) return 1;
  TopoDS_Shape  edge  = DBRep::Get(a[2],TopAbs_EDGE);
  if (edge.IsNull()) return 1;
// calcul des deux faces 
  TopTools_IndexedDataMapOfShapeListOfShape lface;
  TopExp::MapShapesAndAncestors(shape,TopAbs_EDGE,TopAbs_FACE,lface);
  const TopTools_ListOfShape& lfac = lface.FindFromKey(edge);

  Standard_Integer nelem= lfac.Extent();
  if(nelem!=2) return 1; 
  TopTools_ListIteratorOfListOfShape It;
  It.Initialize(lfac);
  face1=TopoDS::Face(It.Value());
  It.Next();
  face2=TopoDS::Face(It.Value());
    
// calcul des deux pcurves 
  const Handle(Geom2d_Curve) c1 = BRep_Tool::CurveOnSurface
                               (TopoDS::Edge(edge),face1,f1,l1);
  if (c1.IsNull()) return 1;
  const  Handle(Geom2d_Curve) c2  = BRep_Tool::CurveOnSurface
                               (TopoDS::Edge(edge),face2,f2,l2);
  if (c2.IsNull()) return 1;
	
   Handle(Geom2d_Curve) curv1= new Geom2d_TrimmedCurve(c1,f1,l1);
   
   Handle(Geom2d_Curve) curv2= new Geom2d_TrimmedCurve(c2,f2,l2);  
   
// calcul des deux surfaces 
  TopLoc_Location L1,L2; 
  TopoDS_Face aLocalFace = face1;
  const Handle(Geom_Surface)& s1 = BRep_Tool::Surface(aLocalFace,L1);
//  const Handle(Geom_Surface)& s1 =
//    BRep_Tool::Surface(TopoDS::Face(face1),L1);
  if (s1.IsNull()) return 1;
  aLocalFace = face2;
  const Handle(Geom_Surface)& s2 = BRep_Tool::Surface(aLocalFace,L2); 
//  const Handle(Geom_Surface)& s2 =
//    BRep_Tool::Surface(TopoDS::Face(face2),L2); 
  if (s2.IsNull()) return 1;	 


   Handle(Geom_Surface) surf1 = Handle(Geom_Surface)::
                                 DownCast(s1->Transformed(L1.Transformation()));
   if (surf1.IsNull()) return 1;
   Handle(Geom_Surface) surf2 = Handle(Geom_Surface)::
                                DownCast(s2->Transformed(L2.Transformation()));
   if (surf2.IsNull()) return 1;
   
 

  nbeval = (Standard_Integer ) Draw::Atof( a[3]);

  switch(n)
    { case 6  : epsC0   = Draw::Atof(a[5]);
      case 5  : epsnl    = Draw::Atof(a[4]);
      case 4  : {} break;
      default : return 1;
    }


  Standard_Real pard1, parf1, U, Uf, deltaU, nb = 0;
  Standard_Boolean isconti = Standard_True;
  Standard_Boolean isdone = Standard_True;
  pard1 = curv1->FirstParameter();
  parf1 = curv1->LastParameter();
  Standard_Real  MaxG0Value=0;
  U = Min( pard1,parf1);
  Uf = Max (pard1,parf1);

  deltaU =  Abs(parf1- pard1)/nbeval;
  ISG0=Standard_True;    
  do  
     { if ( nb == nbeval) 
	  { LocalAnalysis_SurfaceContinuity res (curv1, curv2, Uf,surf1, surf2, GeomAbs_C0,epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen  );
	    isdone = res.IsDone();
            if ( isdone) { isconti = res.IsC0( );
                          if (isconti) 
                          if (res.C0Value()>MaxG0Value)  MaxG0Value= res.C0Value();}  
	    else isconti = Standard_False;}
	            
       else {LocalAnalysis_SurfaceContinuity res (curv1, curv2, (U+nb*deltaU), surf1, surf2, GeomAbs_C0,epsnl,epsC0, epsC1, epsC2, epsG1, percent,maxlen );
	     isdone = res.IsDone();
             if ( isdone) { isconti = res.IsC0() ;
				  if ( nb == 0) { MaxG0Value = res.C0Value();}
			          if (res.C0Value()> MaxG0Value)  MaxG0Value = res.C0Value();}
				  
		 else isconti = Standard_False;}
	                      
       nb++;
       if (!isconti) ISG0=Standard_False;
       
     }
   while ((nb<nbeval)&& isdone );

  //f (!isdone)  { cout<<" Problem in computation "<<endl; return 1;}
  //if (ISG0) 
  //    {cout<<" the continuity is G0 "<<endl;}

  //else { cout<<" the continuity is not G0  "<<endl;}
  //cout<<"MaxG0Value :"<< MaxG0Value << endl;
  if (!isdone)  { di<<" Problem in computation \n"; return 1;}
  if (ISG0) 
      {di<<" the continuity is G0 \n";}

  else { di<<" the continuity is not G0  \n";}
  di<<"MaxG0Value :"<< MaxG0Value << "\n";
  return 0;
}
/*****************************************************************************************/
static Standard_Integer shapeG2continuity (Draw_Interpretor& di, Standard_Integer n, const char** a)

{ Standard_Real epsnl,epsC0, epsC1, epsC2, epsG1, percent,maxlen;
  Standard_Boolean ISG2=Standard_True;
  Standard_Integer nbeval;
  Standard_Real MaxG0Value=0,MaxG1Angle=0,MaxG2Curvature=0;
  InitEpsSurf(epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen );

  if (n<4) return 1;
  TopoDS_Face face1,face2;
  Standard_Real f1,f2,l1,l2;
  TopoDS_Shape shape = DBRep::Get(a[1],TopAbs_SHAPE);
  if (shape.IsNull()) return 1;
  TopoDS_Shape  edge  = DBRep::Get(a[2],TopAbs_EDGE);
  if (edge.IsNull()) return 1;
// calcul des deux faces 
  TopTools_IndexedDataMapOfShapeListOfShape lface;
  TopExp::MapShapesAndAncestors(shape,TopAbs_EDGE,TopAbs_FACE,lface);
  const TopTools_ListOfShape& lfac = lface.FindFromKey(edge);

  Standard_Integer nelem= lfac.Extent();
  if(nelem!=2) return 1; 
  TopTools_ListIteratorOfListOfShape It;
  It.Initialize(lfac);
  face1=TopoDS::Face(It.Value());
  It.Next();
  face2=TopoDS::Face(It.Value());
// calcul des deux pcurves 
  const Handle(Geom2d_Curve) c1 = BRep_Tool::CurveOnSurface
                               (TopoDS::Edge(edge),face1,f1,l1);
  if (c1.IsNull()) return 1;
  const  Handle(Geom2d_Curve) c2  = BRep_Tool::CurveOnSurface
                               (TopoDS::Edge(edge),face2,f2,l2);
  if (c2.IsNull()) return 1;
	
   Handle(Geom2d_Curve) curv1= new Geom2d_TrimmedCurve(c1,f1,l1);
   
   Handle(Geom2d_Curve) curv2= new Geom2d_TrimmedCurve(c2,f2,l2);  
   
// calcul des deux surfaces 
  TopLoc_Location L1,L2; 
  TopoDS_Face aLocalFace = face1;
  const Handle(Geom_Surface)& s1 = BRep_Tool::Surface(aLocalFace,L1);
//  const Handle(Geom_Surface)& s1 = 
//    BRep_Tool::Surface(TopoDS::Face(face1),L1);
  if (s1.IsNull()) return 1;
  aLocalFace = face2;
  const Handle(Geom_Surface)& s2 = BRep_Tool::Surface(aLocalFace,L2); 
//  const Handle(Geom_Surface)& s2 =
//    BRep_Tool::Surface(TopoDS::Face(face2),L2); 
  if (s2.IsNull()) return 1;	 


   Handle(Geom_Surface) surf1 = Handle(Geom_Surface)::
                                 DownCast(s1->Transformed(L1.Transformation()));
   if (surf1.IsNull()) return 1;
   Handle(Geom_Surface) surf2 = Handle(Geom_Surface)::
                                DownCast(s2->Transformed(L2.Transformation()));
   if (surf2.IsNull()) return 1;
   
 

  nbeval = (Standard_Integer ) Draw::Atof( a[3]);

  switch(n)
    { 
      case 9  :  maxlen   = Draw::Atof(a[8]);
      case 8   : percent   = Draw::Atof(a[7]);      
      case 7   : epsG1 = Draw::Atof(a[6]);
      case 6  :  epsC0   = Draw::Atof(a[5]);
      case 5  :  epsnl   = Draw::Atof(a[4]);
      case 4  : {} break;
      default : return 1;
    }


  Standard_Real pard1, parf1, U, Uf, deltaU, nb = 0;
  Standard_Boolean isconti = Standard_True;
  Standard_Boolean isdone = Standard_True;
  pard1 = curv1->FirstParameter();
  parf1 = curv1->LastParameter();
  U = Min( pard1,parf1);
  Uf = Max (pard1,parf1);

  deltaU =  Abs(parf1- pard1)/nbeval;
      
  do  
     { if ( nb == nbeval) 
	  { LocalAnalysis_SurfaceContinuity res (curv1, curv2, Uf,surf1, surf2,             GeomAbs_G2,epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen );
	    isdone = res.IsDone();
            if (isdone){isconti = res.IsG2();
			if (isconti) 
                        {if(res.C0Value()>MaxG0Value) MaxG0Value=res.C0Value();
                         if(res.G1Angle()>MaxG1Angle) MaxG1Angle=res.G1Angle();
                         if(res.G2CurvatureGap()>MaxG2Curvature) 
                           MaxG2Curvature=res.G2CurvatureGap();
		        }}
	    else isconti = Standard_False;}
	            
       else { LocalAnalysis_SurfaceContinuity res (curv1, curv2, (U+nb*deltaU), surf1,surf2, GeomAbs_G2,epsnl,epsC0, epsC1, epsC2, epsG1,percent,maxlen );
	     isdone = res.IsDone();
             if ( isdone) {isconti = res.IsG2();
                           if (nb==0){MaxG0Value=res.C0Value();
                                      MaxG1Angle=res.G1Angle();
                                      MaxG2Curvature=res.G2CurvatureGap();}
                           if(res.C0Value()>MaxG0Value) 
                                MaxG0Value=res.C0Value();
                           if(res.G1Angle()>MaxG1Angle) 
                                MaxG1Angle=res.G1Angle();
                           if(res.G2CurvatureGap()>MaxG2Curvature) 
                              MaxG2Curvature=res.G2CurvatureGap();}
		 else isconti = Standard_False;}
	                      
       nb++;
       if (!isconti) ISG2=Standard_False;
       
     }
   while ((nb<nbeval)&& isdone );

  //if (!isdone)  { cout<<" Problem in computation "<<endl; return 1;}
  //if (ISG2) 
  //cout<<" the continuity is G2 "<<endl;
  //else cout<<" the continuity is not G2  "<<endl;
  //cout<<"MaxG0Value :"<< MaxG0Value << endl;
  //cout<<"MaxG1Angle:"<<  MaxG1Angle << endl;
  //cout<<"MaxG2Curvature:"<<MaxG2Curvature<<endl;
  if (!isdone)  { di<<" Problem in computation \n"; return 1;}
  if (ISG2) 
  di<<" the continuity is G2 \n";
  else di<<" the continuity is not G2  \n";
  di<<"MaxG0Value :"<< MaxG0Value << "\n";
  di<<"MaxG1Angle:"<<  MaxG1Angle << "\n";
  di<<"MaxG2Curvature:"<<MaxG2Curvature<<"\n";
  return 0;
}


//=======================================================================
//function : clintedge
//purpose  : 
//=======================================================================
static Standard_Integer clintedge(Draw_Interpretor& di,
					 Standard_Integer narg, const char** a)
{
  char newname[255];

  if (narg < 2) {
    //cout << "Usage: clintedge shape" << endl;
    di << "Usage: clintedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  TopTools_DataMapOfShapeListOfShape mymap;
  TopOpeBRepTool_PurgeInternalEdges mypurgealgo(S);
  Standard_Integer nbedges = mypurgealgo.NbEdges();
  if (nbedges > 0)
  {
    //cout<<nbedges<<" internal (or external) edges to be removed"<<endl;
    di<<nbedges<<" internal (or external) edges to be removed\n";

    Standard_Integer i = 1;
    char* temp = newname;

    Sprintf(newname,"%s_%d",a[1],i);
    DBRep::Set(temp,mypurgealgo.Shape());
    //cout<<newname<<" ";
    di<<newname<<" ";

    //cout<<endl;
    di<<"\n";
  }
  else
    di << "no internal (or external) edges\n";
    //cout << "no internal (or external) edges"<<endl;

  return 0;
}


//=======================================================================
//function : facintedge
//purpose  : 
//=======================================================================
static Standard_Integer facintedge(Draw_Interpretor& di,
					 Standard_Integer narg, const char** a)
{
  char newname[255];

  if (narg < 2) {
    //cout << "Usage: facintedge shape" << endl;
    di << "Usage: facintedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  TopTools_DataMapOfShapeListOfShape mymap;
  TopOpeBRepTool_PurgeInternalEdges mypurgealgo(S);
  mypurgealgo.Faces(mymap);

  Standard_Integer i = 1;
  char* temp = newname;

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itFacEdg;
  for (itFacEdg.Initialize(mymap); itFacEdg.More(); itFacEdg.Next()) {
      Sprintf(newname,"%s_%d",a[1],i);
      DBRep::Set(temp,itFacEdg.Key());
      //cout<<newname<<" ";
      di<<newname<<" ";
      i++;
    }

  //cout<<endl;
  di<<"\n";

  return 0;
}

//=======================================================================
//function : fuseedge
//purpose  : 
//=======================================================================
static Standard_Integer fuseedge(Draw_Interpretor& di,
					 Standard_Integer narg, const char** a)
{
  char newname[255];

  if (narg < 2) {
    //cout << "Usage: fuseedge shape" << endl;
    di << "Usage: fuseedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  TopTools_DataMapOfIntegerListOfShape mymap;
  //TopOpeBRepTool_FuseEdges myfusealgo(S);
  BRepLib_FuseEdges myfusealgo(S);
  myfusealgo.SetConcatBSpl();
  Standard_Integer nbvertices;
  nbvertices = myfusealgo.NbVertices();

  if (nbvertices > 0) {

    //cout<<nbvertices<<" vertices to be removed"<<endl;
    di<<nbvertices<<" vertices to be removed\n";

    Standard_Integer i = 1;
    char* temp = newname;

    Sprintf(newname,"%s_%d",a[1],i);
    DBRep::Set(temp,myfusealgo.Shape());
    //cout<<newname<<" ";
    di<<newname<<" ";

    //cout<<endl;
    di<<"\n";
  }
  else
    di << "no vertices to remove\n";
    //cout << "no vertices to remove"<<endl;

  return 0;
}


//=======================================================================
//function : listfuseedge
//purpose  : 
//=======================================================================
static Standard_Integer listfuseedge(Draw_Interpretor& di,
					 Standard_Integer narg, const char** a)
{
  char newname[255];

  if (narg < 2) {
    //cout << "Usage: listfuseedge shape" << endl;
    di << "Usage: listfuseedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  TopTools_DataMapOfIntegerListOfShape mymap;
  BRepLib_FuseEdges myfusealgo(S);
  myfusealgo.Edges(mymap);

  Standard_Integer i;
  char* temp = newname;

  TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape itLstEdg;
  for (itLstEdg.Initialize(mymap); itLstEdg.More(); itLstEdg.Next()) {
      const Standard_Integer& iLst = itLstEdg.Key();
      const TopTools_ListOfShape& LmapEdg = mymap.Find(iLst);
      TopTools_ListIteratorOfListOfShape itEdg; 
      i = 1;
      for (itEdg.Initialize(LmapEdg); itEdg.More(); itEdg.Next()) {
	Sprintf(newname,"%s_%d_%d",a[1],iLst,i);
	DBRep::Set(temp,itEdg.Value());
	//cout<<newname<<" ";
	di<<newname<<" ";
	i++;
      }

    }

  //cout<<endl;
  di<<"\n";

  return 0;
}

//=======================================================================
//function : tolsphere
//purpose  : 
//=======================================================================
static Standard_Integer tolsphere(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
  {
    di << "use toolsphere shape\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    di << "No such shape " << a[1] << "\n";
    return 1;
  }

  TopTools_IndexedMapOfShape aMapV;
  TopExp::MapShapes(aS, TopAbs_VERTEX, aMapV);
  for (Standard_Integer i = 1; i <= aMapV.Extent(); i++)
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aMapV.FindKey(i));
    Standard_Real aRadius = BRep_Tool::Tolerance(aV);
    gp_Pnt aCenter = BRep_Tool::Pnt(aV);
    Handle(Geom_Surface) aSph = new Geom_SphericalSurface(gp_Ax2(aCenter,gp::DZ()), aRadius);
    TCollection_AsciiString aName(a[1]);
    aName = aName + "_v" + i;
    DrawTrSurf::Set(aName.ToCString(), aSph);
    di << aName << " ";
  }
  return 0;
}

//=======================================================================
//function : validrange
//purpose  : 
//=======================================================================
static Standard_Integer validrange(Draw_Interpretor& di,
  Standard_Integer narg, const char** a)
{
  if (narg < 2)
  {
    di << "usage: validrange edge [(out) u1 u2]";
    return 1;
  }

  TopoDS_Edge aE = TopoDS::Edge(DBRep::Get(a[1],TopAbs_EDGE, true));
  if (aE.IsNull())
    return 1;

  Standard_Real u1, u2;
  if (BRepLib::FindValidRange(aE, u1, u2))
  {
    if (narg > 3)
    {
      Draw::Set(a[2], u1);
      Draw::Set(a[3], u2);
    }
    else
    {
      di << u1 << " " << u2;
    }
  }
  else
    di << "edge has no valid range";
  return 0;
}

//=======================================================================
//function : CheckCommands
//purpose  : 
//=======================================================================

void BRepTest::CheckCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  BRepTest_CheckCommands_SetFaultyName("faulty_");
  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Check commands";

//  Modified by skv - Tue Apr 27 13:35:35 2004 Begin
  theCommands.Add("checkshape", 
		  "checkshape : no args to have help",
		  __FILE__,
		  checkshape,
		  g);
//   theCommands.Add("checkshape", 
// 		  "checks the validity of a shape : checkshape name,\n                      short description of check : checkshape name -short",
// 		  __FILE__,
// 		  CHK,
// 		  g);
//   theCommands.Add("checktopshape", 
// 		  "checks the topological validity of a shape : checktopshape name",
// 		  __FILE__,
// 		  CHK,
// 		  g);
//  Modified by skv - Tue Apr 27 13:35:39 2004 End

  theCommands.Add("checksection", 
		  "checks the closure of a section : checksection name",
		  __FILE__,
		  checksection,
		  g);

  theCommands.Add("checkdiff", 
		  "checks the validity of the diff beetween the shapes arg1..argn and result :\n checkdiff arg1 [arg2..argn] result [closedSolid (1/0)] [geomCtrl (1/0)]",
		  __FILE__,
		  checkdiff,
		  g);

g = "TOPOLOGY Analysis of shapes ";

theCommands.Add("shapeG0continuity",
		 "shapeG0continuity  shape  edge nbeval [epsnul [epsG0]]",
		  __FILE__,
		  shapeG0continuity, g);

theCommands.Add("shapeG1continuity",
		"shapeG1continuity  shape  edge nbeval [epsnul [epsG0 [epsG1]]]",
		   __FILE__,
		shapeG1continuity ,g);
theCommands.Add("shapeG2continuity",
		  "shapeG2continuity shape  edge  nbeval [epsnul [epsG0 [epsG1 [maxlen [perce]]]]]",
		  __FILE__,
		  shapeG2continuity,g);

theCommands.Add("computetolerance",
		  "computetolerance shape",
		  __FILE__,
		  computetolerance,g);

theCommands.Add("clintedge",
		  "clintedge shape",
		  __FILE__,
		  clintedge,g);

theCommands.Add("facintedge",
		  "facintedge shape",
		  __FILE__,
		  facintedge,g);

theCommands.Add("fuseedge",
		  "fuseedge shape",
		  __FILE__,
		  fuseedge,g);

theCommands.Add("listfuseedge",
		  "listfuseedge shape",
		  __FILE__,
		  listfuseedge,g);
theCommands.Add("tolsphere", "toolsphere shape\n"
                "\t\tshows vertex tolerances by drawing spheres",
                __FILE__, tolsphere, g);
theCommands.Add("validrange",
                "validrange edge [(out) u1 u2]\n"
                "\t\tcomputes valid range of the edge, and\n"
                "\t\tprints first and last values or sets the variables u1 and u2",
                __FILE__, validrange, g);
}

