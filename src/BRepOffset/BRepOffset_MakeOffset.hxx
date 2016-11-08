// Created on: 1995-10-26
// Created by: Yves FRICAUD
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

#ifndef _BRepOffset_MakeOffset_HeaderFile
#define _BRepOffset_MakeOffset_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepOffset_Mode.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_JoinType.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepAlgo_Image.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepOffset_Error.hxx>
#include <BRepOffset_MakeLoops.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
class BRepAlgo_AsDes;
class TopoDS_Shape;
class TopoDS_Face;
class BRepOffset_Analyse;
class BRepAlgo_Image;
class BRepOffset_Inter3d;



class BRepOffset_MakeOffset 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffset_MakeOffset();
  
  Standard_EXPORT BRepOffset_MakeOffset(const TopoDS_Shape& S,
                                        const Standard_Real Offset,
                                        const Standard_Real Tol,
                                        const BRepOffset_Mode Mode = BRepOffset_Skin,
                                        const Standard_Boolean Intersection = Standard_False,
                                        const Standard_Boolean SelfInter = Standard_False,
                                        const GeomAbs_JoinType Join = GeomAbs_Arc,
                                        const Standard_Boolean Thickening = Standard_False,
                                        const Standard_Boolean RemoveIntEdges = Standard_False);
  
  Standard_EXPORT void Initialize (const TopoDS_Shape& S,
                                   const Standard_Real Offset,
                                   const Standard_Real Tol,
                                   const BRepOffset_Mode Mode = BRepOffset_Skin,
                                   const Standard_Boolean Intersection = Standard_False,
                                   const Standard_Boolean SelfInter = Standard_False,
                                   const GeomAbs_JoinType Join = GeomAbs_Arc,
                                   const Standard_Boolean Thickening = Standard_False,
                                   const Standard_Boolean RemoveIntEdges = Standard_False);
  
  Standard_EXPORT void Clear();
  
  //! Add Closing Faces,  <F>  has to be  in  the initial
  //! shape S.
  Standard_EXPORT void AddFace (const TopoDS_Face& F);
  
  //! set the offset <Off> on the Face <F>
  Standard_EXPORT void SetOffsetOnFace (const TopoDS_Face& F, const Standard_Real Off);
  
  Standard_EXPORT void MakeOffsetShape();
  
  Standard_EXPORT void MakeThickSolid();
  
  Standard_EXPORT const BRepOffset_Analyse& GetAnalyse() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT const TopoDS_Shape& Shape() const;
  
  //! returns information about offset state.
  Standard_EXPORT BRepOffset_Error Error() const;
  
  //! Returns <Image> containing links between initials
  //! shapes and offset faces.
  Standard_EXPORT const BRepAlgo_Image& OffsetFacesFromShapes() const;
  
  //! Returns myJoin.
  Standard_EXPORT GeomAbs_JoinType GetJoinType() const;
  
  //! Returns <Image> containing links between initials
  //! shapes and offset edges.
  Standard_EXPORT const BRepAlgo_Image& OffsetEdgesFromShapes() const;
  
  //! Returns the list of closing faces stores by AddFace
  Standard_EXPORT const TopTools_IndexedMapOfShape& ClosingFaces() const;

  //! Makes pre analysis of possibility offset perform. Use method Error() to get more information.
  //! Finds first error. List of checks:
  //! 1) Check for existence object with non-null offset.
  //! 2) Check for connectivity in offset shell.
  //! 3) Check continuity of input surfaces.
  //! 4) Check for normals existence on grid.
  //! @return True if possible make computations and false otherwise.
  Standard_EXPORT Standard_Boolean CheckInputData();

  //! Return bad shape, which obtained in CheckInputData.
  Standard_EXPORT const TopoDS_Shape& GetBadShape() const;


protected:


private:

  Standard_EXPORT void BuildOffsetByArc();
  
  Standard_EXPORT void BuildOffsetByInter();

  //! Make Offset faces
  Standard_EXPORT void MakeOffsetFaces(BRepOffset_DataMapOfShapeOffset& theMapSF);

  Standard_EXPORT void SelfInter (TopTools_MapOfShape& Modif);
  
  Standard_EXPORT void Intersection3D (BRepOffset_Inter3d& Inter);
  
  Standard_EXPORT void Intersection2D (const TopTools_IndexedMapOfShape& Modif, const TopTools_IndexedMapOfShape& NewEdges);
  
  Standard_EXPORT void MakeLoops (TopTools_IndexedMapOfShape& Modif);
  
  Standard_EXPORT void MakeLoopsOnContext (TopTools_MapOfShape& Modif);
  
  Standard_EXPORT void MakeFaces (TopTools_IndexedMapOfShape& Modif);
  
  Standard_EXPORT void MakeShells();
  
  Standard_EXPORT void SelectShells();
  
  Standard_EXPORT void EncodeRegularity();
  
  Standard_EXPORT void MakeSolid();
  
  Standard_EXPORT void ToContext (BRepOffset_DataMapOfShapeOffset& MapSF);
  
  //! Private method use to update the map face<->offset
  Standard_EXPORT void UpdateFaceOffset();
  
  //! Private method used to correct degenerated edges on conical faces
  Standard_EXPORT void CorrectConicalFaces();
  
  //! Private method used to build walls for thickening the shell
  Standard_EXPORT void MakeMissingWalls();

  //! Removes INTERNAL edges from the result
  Standard_EXPORT void RemoveInternalEdges();

  //! Intersects edges
  Standard_EXPORT void IntersectEdges (const TopoDS_Shape& theShape,
                                       BRepOffset_DataMapOfShapeOffset& theMapSF,
                                       TopTools_DataMapOfShapeShape& theMES,
                                       TopTools_DataMapOfShapeShape& theBuild,
                                       Handle(BRepAlgo_AsDes)& theAsDes,
                                       Handle(BRepAlgo_AsDes)& theAsDes2d);

  //! Building of the splits of the offset faces for mode Complete
  //! and joint type Intersection. This method is an advanced alternative
  //! for BRepOffset_MakeLoops::Build method.
  //! Currently the Complete intersection mode is limited to work only on planar cases.
  Standard_EXPORT void BuildSplitsOfExtendedFaces(const TopTools_ListOfShape& theLF,
                                                  Handle(BRepAlgo_AsDes)& theAsDes,
                                                  TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                                  TopTools_DataMapOfShapeShape& theFacesOrigins,
                                                  TopTools_DataMapOfShapeShape& theETrimEInf,
                                                  BRepAlgo_Image& theImage);

  //! Building of the splits of the already trimmed offset faces for mode Complete
  //! and joint type Intersection.
  Standard_EXPORT void BuildSplitsOfTrimmedFaces(const TopTools_ListOfShape& theLF,
                                                 Handle(BRepAlgo_AsDes)& theAsDes,
                                                 BRepAlgo_Image& theImage);

  Standard_Real myOffset;
  Standard_Real myTol;
  TopoDS_Shape myShape;
  BRepOffset_Mode myMode;
  Standard_Boolean myInter;
  Standard_Boolean mySelfInter;
  GeomAbs_JoinType myJoin;
  Standard_Boolean myThickening;
  Standard_Boolean myRemoveIntEdges;
  TopTools_DataMapOfShapeReal myFaceOffset;
  TopTools_IndexedMapOfShape myFaces;
  BRepOffset_Analyse myAnalyse;
  TopoDS_Shape myOffsetShape;
  BRepAlgo_Image myInitOffsetFace;
  BRepAlgo_Image myInitOffsetEdge;
  BRepAlgo_Image myImageOffset;
  TopTools_ListOfShape myWalls;
  Handle(BRepAlgo_AsDes) myAsDes;
  Standard_Boolean myDone;
  BRepOffset_Error myError;
  BRepOffset_MakeLoops myMakeLoops;
  Standard_Boolean myIsPerformSewing; // Handle bad walls in thicksolid mode.
  Standard_Boolean myIsPlanar;
  TopoDS_Shape myBadShape;

};

#endif // _BRepOffset_MakeOffset_HeaderFile
