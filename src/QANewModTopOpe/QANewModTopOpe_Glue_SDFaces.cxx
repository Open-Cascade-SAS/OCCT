// Created on: 2001-05-05
// Created by: Sergey KHROMOV
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <QANewModTopOpe_Tools.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

static Standard_Boolean isAnalitic(const TopoDS_Shape &theShape)
{
  if (theShape.ShapeType() != TopAbs_FACE)
    return Standard_False;

  TopoDS_Face          aFace = TopoDS::Face(theShape);
  BRepAdaptor_Surface  aSurf(aFace);
  Standard_Boolean     isAna = Standard_False;

  switch (aSurf.GetType()) {
  case GeomAbs_Plane :
  case GeomAbs_Cylinder :
  case GeomAbs_Cone :
  case GeomAbs_Sphere :
  case GeomAbs_Torus :
    isAna = Standard_True;
    break;
  default:
    isAna = Standard_False;
  }

  return isAna;
}

static void DoPCurveOnF(const TopoDS_Edge &theEdge, const TopoDS_Face &theFace)
{
  BRep_Builder              aBuilder;
  TopLoc_Location           aCLoc;
  TopLoc_Location           aSLoc;
  Standard_Real             aF;
  Standard_Real             aL;
  Handle(Geom_Curve)        aCurve   = BRep_Tool::Curve(theEdge, aCLoc, aF, aL);
  Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve, aF, aL);
  Handle(Geom_Surface)      aSurface = BRep_Tool::Surface(theFace, aSLoc);
  TopLoc_Location           aCTLoc   = aSLoc.Inverted().Multiplied(aCLoc);

  aTrCurve->Transform(aCTLoc.Transformation());
  
  Handle(Geom2d_Curve) aCurve2d = GeomProjLib::Curve2d (aTrCurve, aSurface);

  aBuilder.UpdateEdge(theEdge, aCurve2d, aSurface,
		      aSLoc, Precision::Confusion());
}

static TopoDS_Face GetAdjacentFace
                   (const TopoDS_Shape                              &theEdge,
		    const TopoDS_Shape                              &theFace,
		    const TopTools_IndexedDataMapOfShapeListOfShape &theAncMap)
{
  TopoDS_Face aFace;

  if (theAncMap.Contains(theEdge)) {
    const TopTools_ListOfShape         &aLOfFaces =
                                              theAncMap.FindFromKey(theEdge);
    TopTools_ListIteratorOfListOfShape  anIter(aLOfFaces);

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aLocalFace = anIter.Value();

      if (!theFace.IsSame(aLocalFace)) {
	aFace = TopoDS::Face(aLocalFace);
	break;
      }
    }
  }

  return aFace;
}

//=======================================================================
//function : SubstitudeSDFaces
//purpose  : 
//=======================================================================

Standard_Boolean QANewModTopOpe_Glue::SubstitudeSDFaces
          (const TopoDS_Shape                       &theFirstSDFace,
	   const TopoDS_Shape                       &theSecondSDFace,
		 TopoDS_Shape                       &theNewSolid1,
		 TopoDS_Shape                       &theNewSolid2,
		 TopTools_DataMapOfShapeListOfShape &theMapOfChangedFaces) 
{
// If the first face is already splited, we use its splits
// to recursively call this function.
  if (theMapOfChangedFaces.IsBound(theFirstSDFace)) {
    const TopTools_ListOfShape &aLocalList = 
                                theMapOfChangedFaces(theFirstSDFace);
    TopTools_ListIteratorOfListOfShape anIter(aLocalList);

    for (;anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aNewShape = anIter.Value();
      if (!SubstitudeSDFaces(aNewShape,    theSecondSDFace,
			     theNewSolid1, theNewSolid2,
			     theMapOfChangedFaces))
	return Standard_False;
    }
    return Standard_True;
  }

// If the second face is already splited, we use its splits
// to recursively call this function.
  if (theMapOfChangedFaces.IsBound(theSecondSDFace)) {
    const TopTools_ListOfShape &aLocalList = 
                                theMapOfChangedFaces(theSecondSDFace);
    TopTools_ListIteratorOfListOfShape anIter(aLocalList);

    for (;anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aNewShape = anIter.Value();
      if (!SubstitudeSDFaces(theFirstSDFace,    aNewShape,
			     theNewSolid1, theNewSolid2,
			     theMapOfChangedFaces))
	return Standard_False;
    }
    return Standard_True;
  }


// If neither the first face nor the second one were
//  splited before, we begin calculation:
  TopTools_IndexedDataMapOfShapeListOfShape aHistory;
  Standard_Boolean                          isCommonFound;

  if (!QANewModTopOpe_Tools::BoolOpe(theFirstSDFace.Oriented(TopAbs_FORWARD),
				theSecondSDFace.Oriented(TopAbs_FORWARD),
				isCommonFound, aHistory))
    return Standard_False;

  if (!isCommonFound)
    return Standard_True;

  TopTools_IndexedDataMapOfShapeListOfShape anAncMap1;
  TopTools_IndexedDataMapOfShapeListOfShape anAncMap2;

  TopExp::MapShapesAndAncestors(theNewSolid1, TopAbs_EDGE,
				TopAbs_FACE, anAncMap1);
  TopExp::MapShapesAndAncestors(theNewSolid2, TopAbs_EDGE,
				TopAbs_FACE, anAncMap2);

// Creation of a compound of old solids.
// The substitution operation will be built with this
// compound.
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;

  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, theNewSolid1);
  aBuilder.Add(aCompound, theNewSolid2);

// Substitution of updated sub-shapes of the first solid.
  BRepTools_Substitution aSubstTool;
  Standard_Integer       aNbModifShape = aHistory.Extent();
  Standard_Integer       i;

  for (i = 1; i <= aNbModifShape; i++) {
    TopTools_ListOfShape                aModifShapes;
    const TopoDS_Shape                 &anAncestor = aHistory.FindKey(i);
    TopTools_ListIteratorOfListOfShape  anIter(aHistory.FindFromIndex(i));

    if (anAncestor.IsSame(theSecondSDFace)) {
      for (; anIter.More(); anIter.Next())
	aModifShapes.Append(anIter.Value());
    } else {
      for (; anIter.More(); anIter.Next())
	aModifShapes.Append(anIter.Value().Oriented(TopAbs_FORWARD));
    }

    if (anAncestor.ShapeType() == TopAbs_EDGE) {
// Check if the edges from common contain pcurves on both shapes.
// If they do not, create them.
      TopoDS_Edge anAncEdge  = TopoDS::Edge(anAncestor);

      if (anAncMap1.Contains(anAncestor)) {
	TopoDS_Face   aFace = GetAdjacentFace(anAncestor, theFirstSDFace,
					      anAncMap1);
	if(!aFace.IsNull()) {//added to fix 4086
	  Standard_Real aFirst;
	  Standard_Real aLast;
	  
	  anIter.Initialize(aHistory.FindFromIndex(i));
	  for (; anIter.More(); anIter.Next()) {
	    TopoDS_Edge          aSplit  = TopoDS::Edge(anIter.Value());
	    Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface
	      (aSplit, aFace, aFirst, aLast);
	    
	    if (aPCurve.IsNull())
	      DoPCurveOnF(aSplit, aFace);
	  }
	}
      }
      
      if (anAncMap2.Contains(anAncestor)) {
	TopoDS_Face   aFace = GetAdjacentFace(anAncestor, theSecondSDFace,
					      anAncMap2);
	if(!aFace.IsNull()) {//added to fix 4086
	  Standard_Real aFirst;
	  Standard_Real aLast;
	  
	  anIter.Initialize(aHistory.FindFromIndex(i));
	  for (; anIter.More(); anIter.Next()) {
	    TopoDS_Edge          aSplit  = TopoDS::Edge(anIter.Value());
	    Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface
	      (aSplit, aFace, aFirst, aLast);
	    
	    if (aPCurve.IsNull())
	      DoPCurveOnF(aSplit, aFace);
	  }
	}
      }
    }
    
//--------------------------------------------------------------
    if (!myMapModif.IsBound(anAncestor))
      myMapModif.Bind(anAncestor, aModifShapes);
//--------------------------------------------------------------

    aSubstTool.Substitute(anAncestor, aModifShapes);
  }

  aSubstTool.Build(aCompound);
  
// Update the map theMapOfChangedFaces and 
// obtain a new solid from the first one.
  if (aSubstTool.IsCopied(theNewSolid1)) {
    // Add changed faces of the first solid to theMapOfChangedFaces:
    TopExp_Explorer  anExp(theNewSolid1, TopAbs_FACE);
    for (; anExp.More(); anExp.Next()) {
      // For each face from solid
      const TopoDS_Shape &aFace = anExp.Current();

      if (aSubstTool.IsCopied(aFace)) {
	const TopTools_ListOfShape &aList = aSubstTool.Copy(aFace);

	TopTools_ListOfShape aNewList;
	if (!theMapOfChangedFaces.IsBound(aFace))
	  theMapOfChangedFaces.Bind(aFace, aNewList);
	
	TopTools_ListIteratorOfListOfShape anIter(aList);
	for (; anIter.More(); anIter.Next()) {
	  TopoDS_Shape aLocalFace = anIter.Value();

	  if (aSubstTool.IsCopied(aLocalFace))
	    aLocalFace = aSubstTool.Copy(aLocalFace).First();

	  theMapOfChangedFaces(aFace).Append(aLocalFace);
	}
      }
    }
    // Obtain a new solid.
    theNewSolid1 = aSubstTool.Copy(theNewSolid1).First();
  }

// Update the map theMapOfChangedFaces and 
// obtain a new solid from the second one.
  if (aSubstTool.IsCopied(theNewSolid2)) {
    // Add changed faces of the second solid to theMapOfChangedFaces:
    TopExp_Explorer  anExp(theNewSolid2, TopAbs_FACE);
    for (; anExp.More(); anExp.Next()) {
      // For each face from solid
      const TopoDS_Shape &aFace = anExp.Current();

      if (aSubstTool.IsCopied(aFace)) {
	const TopTools_ListOfShape &aList = aSubstTool.Copy(aFace);

	TopTools_ListOfShape aNewList;
	if (!theMapOfChangedFaces.IsBound(aFace))
	  theMapOfChangedFaces.Bind(aFace, aNewList);
	
	TopTools_ListIteratorOfListOfShape anIter(aList);
	for (; anIter.More(); anIter.Next()) {
	  TopoDS_Shape aLocalFace = anIter.Value();

	  if (aSubstTool.IsCopied(aLocalFace))
	    aLocalFace = aSubstTool.Copy(aLocalFace).First();

	  theMapOfChangedFaces(aFace).Append(aLocalFace);
	}
      }
    }
    // Obtain a new solid.
    theNewSolid2 = aSubstTool.Copy(theNewSolid2).First();
  }

  return Standard_True;
}

//=======================================================================
//function : PerformSolidSolid
//purpose  : 
//=======================================================================

void
QANewModTopOpe_Glue::PerformSDFaces()
{
  TopoDS_Shape& myS1=myArguments.First();
  TopoDS_Shape& myS2=myTools.First();

  TopExp_Explorer anExp;
  TopoDS_Shape aS1, aS2;
  Standard_Boolean aWire1 = Standard_False, aWire2 = Standard_False; 
  anExp.Init(myS1, TopAbs_WIRE, TopAbs_FACE);
  if(anExp.More()) {
    aS1 = myS1;
    aWire1 = Standard_True;
  }
  else {
    anExp.Init(myS1, TopAbs_EDGE, TopAbs_WIRE);
    if(anExp.More()) {
      aS1 = myS1;
      aWire1 = Standard_True;
    }
  }
   
  anExp.Init(myS2, TopAbs_WIRE, TopAbs_FACE);
  if(anExp.More()) {
    aS2 = myS2;
    aWire2 = Standard_True;
  }
  else {
    anExp.Init(myS2, TopAbs_EDGE, TopAbs_WIRE);
    if(anExp.More()) {
      aS2 = myS2;
      aWire2 = Standard_True;
    }
  }
 
  if(aWire1) {
    BRep_Builder aBld;
    myS1.Nullify();
    aBld.MakeCompound(TopoDS::Compound(myS1));
    anExp.Init(aS1, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
    anExp.Init(aS1, TopAbs_SOLID, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
    anExp.Init(aS1, TopAbs_SHELL, TopAbs_SOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
    anExp.Init(aS1, TopAbs_FACE, TopAbs_SHELL);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS1, anExp.Current());
    }
    
  }

  if(aWire2) {
    BRep_Builder aBld;
    myS2.Nullify();
    aBld.MakeCompound(TopoDS::Compound(myS2));
    anExp.Init(aS2, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
    anExp.Init(aS2, TopAbs_SOLID, TopAbs_COMPSOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
    anExp.Init(aS2, TopAbs_SHELL, TopAbs_SOLID);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
    anExp.Init(aS2, TopAbs_FACE, TopAbs_SHELL);
    for(; anExp.More(); anExp.Next()) {
      aBld.Add(myS2, anExp.Current());
    }
    
  }

  BRepAlgoAPI_BooleanOperation::Build();
  if (!BuilderCanWork())
    return;

  if(aWire1) myS1 = aS1;
  if(aWire2) myS2 = aS2;

  myShape.Nullify();

  TopoDS_Shape                         aNewShape1 = myS1;
  TopoDS_Shape                         aNewShape2 = myS2;
  TopTools_DataMapOfShapeListOfShape   theMapOfChangedFaces;

  Standard_Boolean aHasSDF = Standard_False;
  anExp.Init(myS1, TopAbs_FACE);
  for (; anExp.More(); anExp.Next()) {
    TopoDS_Shape aFirstFace = anExp.Current();

    if (!isAnalitic(aFirstFace))
      continue;

    if (QANewModTopOpe_Tools::HasSameDomain(myBuilder, aFirstFace)) {

      if(!aHasSDF) aHasSDF = Standard_True;

      TopTools_ListOfShape               aLOfSDFace;
      TopTools_ListIteratorOfListOfShape anIter;

      QANewModTopOpe_Tools::SameDomain(myBuilder, aFirstFace, aLOfSDFace);
      anIter.Initialize(aLOfSDFace);

      for(; anIter.More(); anIter.Next()) {
	TopoDS_Shape aSecondFace = anIter.Value();

	if (!isAnalitic(aSecondFace))
	  continue;

	if (!SubstitudeSDFaces(aFirstFace, aSecondFace,
			       aNewShape1, aNewShape2,
			       theMapOfChangedFaces))
	  return;
      }
    }
  }

  if(myS1.IsSame(aNewShape1) && myS2.IsSame(aNewShape2)) return;

  if(aHasSDF) {
    BRep_Builder aBuilder;

//    aBuilder.MakeCompSolid(TopoDS::CompSolid(myShape));
    aBuilder.MakeCompound(TopoDS::Compound(myShape));

    aBuilder.Add(myShape, aNewShape1);
    aBuilder.Add(myShape, aNewShape2);

    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape anIter(theMapOfChangedFaces);
    for(; anIter.More(); anIter.Next()) {
      myMapModif.Bind(anIter.Key(), anIter.Value());
    }

    //--------------- creation myMapGener for common faces
    
    TopExp_Explorer anExp1, anExp2;
    TopTools_MapOfShape aM;
    anExp1.Init(aNewShape1, TopAbs_FACE);
    for(; anExp1.More(); anExp1.Next()) {
      const TopoDS_Shape& aF1 = anExp1.Current();
      anExp2.Init(aNewShape2, TopAbs_FACE);
      for(; anExp2.More(); anExp2.Next()) {
	const TopoDS_Shape& aF2 = anExp2.Current();
	if(aF1.IsSame(aF2)) {
	  aM.Add(aF1);
	}
      }
    }

    anIter.Initialize(myMapModif);
    TopTools_ListIteratorOfListOfShape anI1;
    TopTools_MapIteratorOfMapOfShape anI2;
    for(; anIter.More(); anIter.Next()) {
      const TopoDS_Shape& aS = anIter.Key();
      if(aS.ShapeType() == TopAbs_FACE) {
	anI1.Initialize(anIter.Value());
	for(; anI1.More(); anI1.Next()) {
	  const TopoDS_Shape& aSS1 = anI1.Value();
	  anI2.Initialize(aM);
	  for(; anI2.More(); anI2.Next()) {
	    const TopoDS_Shape& aSS2 = anI2.Key();
	    if(aSS1.IsSame(aSS2)) {
	      if(!myMapGener.IsBound(aS)) {
                // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(aS, TopTools_ListOfShape());
                TopTools_ListOfShape aListOfShape1;
                myMapGener.Bind(aS, aListOfShape1);
              }
	      myMapGener(aS).Append(aSS1);
	      myMapModif(aS).Remove(anI1);
	    }
	  }
	  if(!anI1.More()) break;
	}
      }
      
//      if(anIter.Value().Extent() == 0) myMapModif.UnBind(aS);

    }
	      
    //--------------- creation myMapGener for common edges
    
    aM.Clear();
    anExp1.Init(aNewShape1, TopAbs_EDGE);
    for(; anExp1.More(); anExp1.Next()) {
      const TopoDS_Shape& anE1 = anExp1.Current();
      if(aM.Contains(anE1)) continue;
      anExp2.Init(aNewShape2, TopAbs_EDGE);
      for(; anExp2.More(); anExp2.Next()) {
	const TopoDS_Shape& anE2 = anExp2.Current();
	if(aM.Contains(anE2)) continue;
	if(anE1.IsSame(anE2)) {
	  aM.Add(anE1);
	}
      }
    }

    TopTools_MapOfShape aComVerMap;
    TopTools_MapOfShape aLocVerMap;

    anExp1.Init(myS1, TopAbs_VERTEX);
    for(; anExp1.More();  anExp1.Next()) aComVerMap.Add(anExp1.Current());
    anExp1.Init(myS2, TopAbs_VERTEX);
    for(; anExp1.More();  anExp1.Next()) aComVerMap.Add(anExp1.Current());

    TopTools_ListOfShape aShapesToRemove; // record items to be removed from the map (should be done after iteration)
    anIter.Initialize(myMapModif);
    for(; anIter.More(); anIter.Next()) {
      const TopoDS_Shape& aS = anIter.Key();
      if(aS.ShapeType() == TopAbs_EDGE) {
	aLocVerMap.Clear();
	anI1.Initialize(anIter.Value());
	for(; anI1.More(); anI1.Next()) {
	  const TopoDS_Shape& aSS1 = anI1.Value();
	  anI2.Initialize(aM);
	  for(; anI2.More(); anI2.Next()) {
	    const TopoDS_Shape& aSS2 = anI2.Key();
	    if(aSS1.IsSame(aSS2)) {
	      if(!aS.IsSame(aSS1)) {
		if(!myMapGener.IsBound(aS)) {
                  // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(aS, TopTools_ListOfShape());
                  TopTools_ListOfShape aListOfShape2;
                  myMapGener.Bind(aS, aListOfShape2);
                }
		myMapGener(aS).Append(aSS1);
		TopoDS_Vertex aV1, aV2;
		TopExp::Vertices(TopoDS::Edge(aSS1), aV1, aV2);
		if(!aComVerMap.Contains(aV1)) { 
		  if(aLocVerMap.Add(aV1)) {
		    myMapGener(aS).Append(aV1);
		  }
		}
		if(!aComVerMap.Contains(aV2)) { 
		  if(aLocVerMap.Add(aV2)) {
		    myMapGener(aS).Append(aV2);
		  }
		}
		myMapModif(aS).Remove(anI1);
	      }
	      else {
                aShapesToRemove.Append (aS);
	      }
	    }
	  }
	  if(!anI1.More()) break;
	}
      }
    }
      
    // remove items from the data map
    for(TopTools_ListIteratorOfListOfShape anIt(aShapesToRemove); anIt.More(); anIt.Next())
      myMapModif.UnBind(anIt.Value());

    // Deleted vertices
    anExp1.Init(myShape, TopAbs_VERTEX);
    for(; anExp1.More();  anExp1.Next()) {
      const TopoDS_Shape& aV = anExp1.Current();
      aComVerMap.Remove(aV);
    }

    anI2.Initialize(aComVerMap);
    for(; anI2.More(); anI2.Next()) {
      // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(anI2.Key(), TopTools_ListOfShape());
      TopTools_ListOfShape aListOfShape3;
      myMapModif.Bind(anI2.Key(), aListOfShape3);
    }
	      
    Done();
  }

  return;

}
