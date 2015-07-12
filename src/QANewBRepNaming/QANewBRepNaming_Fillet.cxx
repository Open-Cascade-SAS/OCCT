// Created on: 2001-07-02
// Created by: Sergey ZARITCHNY
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <Adaptor3d_HCurve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GProp_GProps.hxx>
#include <QANewBRepNaming_Fillet.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <Standard_NullObject.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : QANewBRepNaming_Fillet
//purpose  :
//=======================================================================
QANewBRepNaming_Fillet::QANewBRepNaming_Fillet()
{}

//=======================================================================
//function : QANewBRepNaming_Fillet
//purpose  :
//=======================================================================

QANewBRepNaming_Fillet::QANewBRepNaming_Fillet(const TDF_Label& theLabel):
       QANewBRepNaming_TopNaming(theLabel)
{}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void QANewBRepNaming_Fillet::Init(const TDF_Label& theLabel)
{
  if(theLabel.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_Fillet::Init: The Result label is Null ...");
  myResultLabel = theLabel;
}


//=======================================================================
//function :
//purpose  :
//=======================================================================
static Standard_Boolean IsValidSurfType(const TopoDS_Face& theFace) {
  BRepAdaptor_Surface anAdapt(theFace);
  Handle( Adaptor3d_HCurve ) aBasisCurve;
  const GeomAbs_SurfaceType& aType = anAdapt.GetType();
  if(aType == GeomAbs_Cylinder || aType == GeomAbs_Cone)
    return Standard_True;
  else if(aType == GeomAbs_SurfaceOfRevolution){
    aBasisCurve = anAdapt.BasisCurve();
    if (aBasisCurve->GetType() == GeomAbs_Line)
      return Standard_True;
  }
  else if(aType == GeomAbs_SurfaceOfExtrusion) {
    aBasisCurve = anAdapt.BasisCurve();
    if (aBasisCurve->GetType() == GeomAbs_Circle || aBasisCurve->GetType() == GeomAbs_Ellipse)
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================

void QANewBRepNaming_Fillet::Load(const TopoDS_Shape& theContext,
			     BRepFilletAPI_MakeFillet& theMkFillet) const
{
  TNaming_Builder aFilletBuilder(ResultLabel());
  TopoDS_Shape aResult = theMkFillet.Shape();

  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(aResult);
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(aResult);
      if (itr.More()) aResult = itr.Value();
    }
  }
  if (aResult.IsNull()) aFilletBuilder.Generated(aResult);
  else
    aFilletBuilder.Modify(theContext, aResult);

  //WARNING: Not implemented case:
  // Cut cylinder by Shpere, shpere cuts lateral and top face of cylinder.
  // SIM edges of cylinder and sphere not intersected.
  // Result of fillet do not intersects SIM edge of cylinder

  // Try to edintify workaround with intersection of SIM edge.
  const TopAbs_ShapeEnum& Type = theContext.ShapeType();
  Standard_Boolean IsFound = Standard_False;
  if(Type  != TopAbs_COMPOUND || Type < TopAbs_FACE) {
    // Check for SIM edge presence in some face
    TopExp_Explorer anExp(theContext, TopAbs_FACE);

    for(;anExp.More();anExp.Next()) {
      if(IsValidSurfType(TopoDS::Face(anExp.Current()))) {
	IsFound = Standard_True;
	break;
      }
    }
  }

  Standard_Boolean IsWRCase = Standard_False;
  FacesFromEdges().ForgetAllAttributes();

  if(IsFound) {
    TopExp_Explorer ShapeExplorer (theContext, TopAbs_EDGE);
    for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
      const TopoDS_Shape& aRoot = ShapeExplorer.Current ();
      const TopTools_ListOfShape& aShapes = theMkFillet.Generated (aRoot);
      if(aShapes.Extent() == 2) {
	if(aShapes.First().ShapeType() == TopAbs_FACE && aShapes.Last().ShapeType() == TopAbs_FACE) {
	  IsWRCase = Standard_True;
	  GProp_GProps aF1Props, aF2Props;
	  BRepGProp::SurfaceProperties(aShapes.First(), aF1Props);
	  BRepGProp::SurfaceProperties(aShapes.Last(), aF2Props);
	  TNaming_Builder aF1Builder(WRFace1());
	  TNaming_Builder aF2Builder(WRFace2());

	  // Naming of Faces
	  TopoDS_Shape aLessFace;
	  if(aF1Props.Mass() < aF2Props.Mass()) {
	    aLessFace = aShapes.First();
	    aF1Builder.Generated(aShapes.First());
	    aF2Builder.Generated(aShapes.Last());
	  } else {
	    aLessFace = aShapes.Last();
	    aF1Builder.Generated(aShapes.Last());
	    aF2Builder.Generated(aShapes.First());
	  }

	  // Naming of Common Edges
	  TopExp_Explorer anExp1(aShapes.First(), TopAbs_EDGE);
	  TopTools_ListOfShape aCommonEdges;
	  for(;anExp1.More();anExp1.Next()) {
	    TopExp_Explorer anExp2(aShapes.Last(), TopAbs_EDGE);
	    for(;anExp2.More();anExp2.Next())
	      if(anExp1.Current().IsSame(anExp2.Current())) {
		aCommonEdges.Append(anExp1.Current());
		break;
	      }
	  }

	  if(aCommonEdges.Extent() != 2)
	    break;

	  anExp1.Init(aLessFace, TopAbs_EDGE);
	  TopTools_ListOfShape anOtherEdges;
	  for(;anExp1.More();anExp1.Next()) {
	    if(!anExp1.Current().IsSame(aCommonEdges.First()) && !anExp1.Current().IsSame(aCommonEdges.Last()))
	      anOtherEdges.Append(anExp1.Current());
	  }

	  GProp_GProps anE1Props, anE2Props;
	  BRepGProp::LinearProperties(anOtherEdges.First(), anE1Props);
	  BRepGProp::LinearProperties(anOtherEdges.Last(), anE2Props);
	  gp_Pnt P1, P2;
	  if(anE1Props.Mass() < anE2Props.Mass()) {
	    P1 = anE1Props.CentreOfMass();
	    P2 = anE2Props.CentreOfMass();
	  } else {
	    P1 = anE2Props.CentreOfMass();
	    P2 = anE1Props.CentreOfMass();
	  }

	  BRepGProp::LinearProperties(aCommonEdges.First(), anE1Props);
	  BRepGProp::LinearProperties(aCommonEdges.Last(), anE2Props);

	  gp_Pnt EP1 = anE1Props.CentreOfMass();
	  gp_Pnt EP2 = anE2Props.CentreOfMass();

	  gp_Vec aMainVec(P1, P2);
	  gp_Vec aV1(P1, EP1);
	  gp_Vec aV2(P1, EP2);
#ifdef OCCT_DEBUG
	  cout << "P1=" << P1.X() << " " << P1.Y() << " " << P1.Z() << endl;
	  cout << "P2=" << P2.X() << " " << P2.Y() << " " << P2.Z() << endl;
	  cout << "EP1=" << EP1.X() << " " << EP1.Y() << " " << EP1.Z() << endl;
	  cout << "EP2=" << EP2.X() << " " << EP2.Y() << " " << EP2.Z() << endl;
	  cout << endl;
#endif

	  gp_Vec aCr1 = aMainVec.Crossed(aV1);
	  gp_Vec aCr2 = aMainVec.Crossed(aV2);
	  TopoDS_Shape aFirst, aSecond;
	  if(aCr1.X() > 0 && aCr2.X() < 0) {
	    aFirst = aCommonEdges.First();
	    aSecond = aCommonEdges.Last();
	  } else if(aCr2.X() > 0 && aCr1.X() < 0) {
	    aFirst = aCommonEdges.Last();
	    aSecond = aCommonEdges.First();
	  } else if(aCr1.Y() > 0 && aCr2.Y() < 0) {
	    aFirst = aCommonEdges.First();
	    aSecond = aCommonEdges.Last();
	  } else if(aCr2.Y() > 0 && aCr1.Y() < 0) {
	    aFirst = aCommonEdges.Last();
	    aSecond = aCommonEdges.First();
	  } else if(aCr1.Z() > 0 && aCr2.Z() < 0) {
	    aFirst = aCommonEdges.First();
	    aSecond = aCommonEdges.Last();
	  } else if(aCr2.Z() > 0 && aCr1.Z() < 0) {
	    aFirst = aCommonEdges.Last();
	    aSecond = aCommonEdges.First();
	  }

	  TNaming_Builder anE1Builder(WREdge1());
	  TNaming_Builder anE2Builder(WREdge2());
	  anE1Builder.Generated(aFirst);
	  anE2Builder.Generated(aSecond);

	  break;
	}
      }
    }
  }

  if(!IsWRCase) {
    WRFace1().ForgetAllAttributes();
    WRFace2().ForgetAllAttributes();
    WREdge1().ForgetAllAttributes();
    WREdge2().ForgetAllAttributes();
    //New faces generated from edges
    TNaming_Builder anEFacesBuilder(FacesFromEdges());
    QANewBRepNaming_Loader::LoadGeneratedShapes(theMkFillet, theContext, TopAbs_EDGE, anEFacesBuilder);
  }

  //Faces of the initial shape modified by theMkFillet
  TNaming_Builder aMFacesBuilder(ModifiedFaces());
  QANewBRepNaming_Loader::LoadModifiedShapes(theMkFillet, theContext, TopAbs_FACE, aMFacesBuilder, Standard_False);

  //Deleted faces of the initial shape
  TNaming_Builder aDFacesBuilder(DeletedFaces());
    QANewBRepNaming_Loader::LoadDeletedShapes(theMkFillet, theContext, TopAbs_FACE, aDFacesBuilder);

  //New faces generated from vertices
  TNaming_Builder aVFacesBuilder(FacesFromVertices());
  QANewBRepNaming_Loader::LoadGeneratedShapes(theMkFillet, theContext, TopAbs_VERTEX, aVFacesBuilder);
}

//=======================================================================
//function : DeletedFaces
//purpose  :
//=======================================================================

TDF_Label QANewBRepNaming_Fillet::DeletedFaces() const
{
  return ResultLabel().FindChild(1, Standard_True);
}

//=======================================================================
//function : ModifiedFaces
//purpose  :
//=======================================================================

TDF_Label QANewBRepNaming_Fillet::ModifiedFaces() const
{
  return ResultLabel().FindChild(2, Standard_True);
}

//=======================================================================
//function : FacesFromEdges
//purpose  :
//=======================================================================

TDF_Label QANewBRepNaming_Fillet::FacesFromEdges() const
{
  return ResultLabel().FindChild(4, Standard_True);
}

//=======================================================================
//function : FacesFromVertices
//purpose  :
//=======================================================================

TDF_Label QANewBRepNaming_Fillet::FacesFromVertices() const
{
  return ResultLabel().FindChild(3, Standard_True);
}


//=======================================================================
//function :
//purpose  :
//=======================================================================
TDF_Label QANewBRepNaming_Fillet::WRFace1() const
{
  return ResultLabel().FindChild(4, Standard_True);
}


//=======================================================================
//function :
//purpose  :
//=======================================================================
TDF_Label QANewBRepNaming_Fillet::WRFace2() const
{
  return ResultLabel().FindChild(5, Standard_True);
}


//=======================================================================
//function :
//purpose  :
//=======================================================================
TDF_Label QANewBRepNaming_Fillet::WREdge1() const
{
  return ResultLabel().FindChild(6, Standard_True);
}


//=======================================================================
//function :
//purpose  :
//=======================================================================
TDF_Label QANewBRepNaming_Fillet::WREdge2() const
{
  return ResultLabel().FindChild(7, Standard_True);
}
