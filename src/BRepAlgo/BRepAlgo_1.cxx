// File:	BRepAlgo_1.cxx
// Created:	Thu Oct 21 18:14:08 1999
// Author:	Atelier CAS2000
//		<cas@brunox.paris1.matra-dtv.fr>


#include <BRepAlgo.ixx>

#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <gp.hxx>

//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo::IsValid(const TopoDS_Shape& S)
{
  BRepCheck_Analyzer ana(S);
  return ana.IsValid(); 
}

//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo::IsValid(const TopTools_ListOfShape& theArgs,
				     const TopoDS_Shape& theResult,
				     const Standard_Boolean closedSolid,
				     const Standard_Boolean GeomCtrl)
{
  if (theResult.IsNull()) return Standard_True;
  Standard_Boolean validate= Standard_False;

  TopTools_MapOfShape allFaces;
  TopExp_Explorer tEx;
  TopTools_ListIteratorOfListOfShape itLOS;
  for (itLOS.Initialize(theArgs);
       itLOS.More(); itLOS.Next()) {
    if (itLOS.Value().IsSame(theResult)) {
      validate = Standard_True;
      break;
    }
    for (tEx.Init(itLOS.Value(), TopAbs_FACE); tEx.More(); tEx.Next()) {
      allFaces.Add(tEx.Current());
    }
  }

  TopoDS_Compound toCheck;

  if (allFaces.IsEmpty()) {
    if (validate) return Standard_True;
    BRepCheck_Analyzer ana(theResult, GeomCtrl);
    if (!ana.IsValid()) return Standard_False;
  }
  else if (!validate) {
    BRep_Builder bB;
    TopoDS_Face curf;
    for (tEx.Init(theResult, TopAbs_FACE); tEx.More(); tEx.Next()) {
      curf=TopoDS::Face(tEx.Current());
      if (!allFaces.Contains(curf)) {
	if (toCheck.IsNull()) bB.MakeCompound(toCheck);
	BRepTools::Update(curf);
	bB.Add(toCheck, curf);
      }
    }
    if (toCheck.IsNull()) {
      validate = Standard_True;
    }
    else {
      BRepCheck_Analyzer ana(toCheck, Standard_True);
      if (!ana.IsValid()) {

// On verifie que le probleme ne soit pas juste BRepCheck_InvalidSameParameterFlag
	BRepCheck_ListIteratorOfListOfStatus itl;
	BRepCheck_Status sta;
	for (tEx.Init(toCheck, TopAbs_FACE); tEx.More(); tEx.Next()) {
	  if  (!ana.Result(tEx.Current()).IsNull()) {
	    for (itl.Initialize(ana.Result(tEx.Current())->Status()); itl.More(); itl.Next()) {
	      sta=itl.Value();
// Si une face est en erreur
	      if (sta != BRepCheck_NoError) {
		BRepCheck_ListIteratorOfListOfStatus ilt;
		TopExp_Explorer exp;
		for (exp.Init(tEx.Current(), TopAbs_EDGE); exp.More(); exp.Next()) {
		  const Handle(BRepCheck_Result)& res = ana.Result(exp.Current());
		  for (res->InitContextIterator(); res->MoreShapeInContext(); res->NextShapeInContext()) {
		    if (res->ContextualShape().IsSame(tEx.Current())) {
		      for (ilt.Initialize(res->StatusOnShape()); ilt.More(); ilt.Next()) {
			sta=ilt.Value();
// Si une edge est BRepCheck_InvalidSameParameterFlag ou BRepCheck_InvalidSameRangeFlag on force
			if (sta == BRepCheck_InvalidSameParameterFlag ||
			    sta == BRepCheck_InvalidSameRangeFlag) {
			  bB.SameRange(TopoDS::Edge(exp.Current()), Standard_False);
			  bB.SameParameter(TopoDS::Edge(exp.Current()), Standard_False);
			  BRepLib::SameParameter(TopoDS::Edge(exp.Current()), 
						 BRep_Tool::Tolerance(TopoDS::Edge(exp.Current())));
			  break;
			}
		      }
		    }
		  }
		}
		break;
	      }
	    }
	  }
	}
// On refait un controle (il pourrait y avoir un probleme d'un autre type ou non rectifiable.
	ana.Init(toCheck, Standard_True);
	if (!ana.IsValid()) return Standard_False;
      }
    }
  }

  Handle(BRepCheck_Shell) HR;
  for (tEx.Init(theResult, TopAbs_SHELL); tEx.More(); tEx.Next()) {
    if (HR.IsNull()) HR = new BRepCheck_Shell(TopoDS::Shell(tEx.Current()));
    else                             HR->Init(tEx.Current());
    if (HR->Status().First() != BRepCheck_NoError) return Standard_False;
    if (HR->Orientation(Standard_False) != BRepCheck_NoError) return Standard_False;
    if (closedSolid) {
      if (HR->Closed() != BRepCheck_NoError) return Standard_False;
    }
  }

  return Standard_True;
}


//=======================================================================
//function : IsTopologicallyValid
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo::IsTopologicallyValid(const TopoDS_Shape& S)
{
//

// pour permettre a Moliner de travailler jusqu'au 18/10/96 le temps que 
// l'on trouve une solution reflechie au probleme de performance de BRepCheck
//
//POP ON n'utilise plus la varaible d'environnement
// if (getenv("DONT_SWITCH_IS_VALID") != NULL) {
//   return Standard_True ; 
// }
// else {
   BRepCheck_Analyzer ana(S,Standard_False);
   return ana.IsValid(); 
 
// }
}



