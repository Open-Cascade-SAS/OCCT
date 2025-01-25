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

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepOffset_Mode.hxx>
#include <GeomAbs_JoinType.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepAlgo_Image.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepOffset_Error.hxx>
#include <BRepOffset_MakeLoops.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <Message_ProgressRange.hxx>
class BRepAlgo_AsDes;
class TopoDS_Face;
class BRepOffset_Inter3d;

class BRepOffset_MakeOffset
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepOffset_MakeOffset();

  Standard_EXPORT BRepOffset_MakeOffset(
    const TopoDS_Shape&          S,
    const Standard_Real          Offset,
    const Standard_Real          Tol,
    const BRepOffset_Mode        Mode           = BRepOffset_Skin,
    const Standard_Boolean       Intersection   = Standard_False,
    const Standard_Boolean       SelfInter      = Standard_False,
    const GeomAbs_JoinType       Join           = GeomAbs_Arc,
    const Standard_Boolean       Thickening     = Standard_False,
    const Standard_Boolean       RemoveIntEdges = Standard_False,
    const Message_ProgressRange& theRange       = Message_ProgressRange());

  Standard_EXPORT void Initialize(const TopoDS_Shape&    S,
                                  const Standard_Real    Offset,
                                  const Standard_Real    Tol,
                                  const BRepOffset_Mode  Mode           = BRepOffset_Skin,
                                  const Standard_Boolean Intersection   = Standard_False,
                                  const Standard_Boolean SelfInter      = Standard_False,
                                  const GeomAbs_JoinType Join           = GeomAbs_Arc,
                                  const Standard_Boolean Thickening     = Standard_False,
                                  const Standard_Boolean RemoveIntEdges = Standard_False);

  Standard_EXPORT void Clear();

  //! Changes the flag allowing the linearization
  Standard_EXPORT void AllowLinearization(const Standard_Boolean theIsAllowed);

  //! Add Closing Faces,  <F>  has to be  in  the initial
  //! shape S.
  Standard_EXPORT void AddFace(const TopoDS_Face& F);

  //! set the offset <Off> on the Face <F>
  Standard_EXPORT void SetOffsetOnFace(const TopoDS_Face& F, const Standard_Real Off);

  Standard_EXPORT void MakeOffsetShape(
    const Message_ProgressRange& theRange = Message_ProgressRange());

  Standard_EXPORT void MakeThickSolid(
    const Message_ProgressRange& theRange = Message_ProgressRange());

  Standard_EXPORT const BRepOffset_Analyse& GetAnalyse() const;

  Standard_EXPORT Standard_Boolean IsDone() const;

  Standard_EXPORT const TopoDS_Shape& Shape() const;

  const TopoDS_Shape& InitShape() const { return myInitialShape; }

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
  Standard_EXPORT Standard_Boolean CheckInputData(const Message_ProgressRange& theRange);

  //! Return bad shape, which obtained in CheckInputData.
  Standard_EXPORT const TopoDS_Shape& GetBadShape() const;

public: //! @name History methods
  //! Returns the  list of shapes generated from the shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Generated(const TopoDS_Shape& theS);

  //! Returns the list of shapes modified from the shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Modified(const TopoDS_Shape& theS);

  //! Returns true if the shape S has been deleted.
  Standard_EXPORT Standard_Boolean IsDeleted(const TopoDS_Shape& S);

protected:
  //! Analyze progress steps of the whole operation.
  //! @param theWhole - sum of progress of all operations.
  //! @oaram theSteps - steps of the operations supported by PI
  Standard_EXPORT void analyzeProgress(const Standard_Real   theWhole,
                                       TColStd_Array1OfReal& theSteps) const;

private:
  //! Check if shape consists of only planar faces
  //! If <myIsLinearizationAllowed> is TRUE, try to approximate images of faces
  //! by planar faces
  Standard_EXPORT Standard_Boolean IsPlanar();

  //! Set the faces that are to be removed
  Standard_EXPORT void SetFaces();

  //! Set the faces with special value of offset
  Standard_EXPORT void SetFacesWithOffset();

  Standard_EXPORT void BuildFaceComp();

  Standard_EXPORT void BuildOffsetByArc(const Message_ProgressRange& theRange);

  Standard_EXPORT void BuildOffsetByInter(const Message_ProgressRange& theRange);

  //! Make Offset faces
  Standard_EXPORT void MakeOffsetFaces(BRepOffset_DataMapOfShapeOffset& theMapSF,
                                       const Message_ProgressRange&     theRange);

  Standard_EXPORT void SelfInter(TopTools_MapOfShape& Modif);

  Standard_EXPORT void Intersection3D(BRepOffset_Inter3d&          Inter,
                                      const Message_ProgressRange& theRange);

  Standard_EXPORT void Intersection2D(const TopTools_IndexedMapOfShape& Modif,
                                      const TopTools_IndexedMapOfShape& NewEdges,
                                      const Message_ProgressRange&      theRange);

  Standard_EXPORT void MakeLoops(TopTools_IndexedMapOfShape&  Modif,
                                 const Message_ProgressRange& theRange);

  Standard_EXPORT void MakeLoopsOnContext(TopTools_MapOfShape& Modif);

  Standard_EXPORT void MakeFaces(TopTools_IndexedMapOfShape&  Modif,
                                 const Message_ProgressRange& theRange);

  Standard_EXPORT void MakeShells(const Message_ProgressRange& theRange);

  Standard_EXPORT void SelectShells();

  Standard_EXPORT void EncodeRegularity();

  //! Replace roots in history maps
  Standard_EXPORT void ReplaceRoots();

  Standard_EXPORT void MakeSolid(const Message_ProgressRange& theRange);

  Standard_EXPORT void ToContext(BRepOffset_DataMapOfShapeOffset& MapSF);

  //! Private method use to update the map face<->offset
  Standard_EXPORT void UpdateFaceOffset();

  //! Private method used to correct degenerated edges on conical faces
  Standard_EXPORT void CorrectConicalFaces();

  //! Private method used to build walls for thickening the shell
  Standard_EXPORT void MakeMissingWalls(const Message_ProgressRange& theRange);

  //! Removes INTERNAL edges from the result
  Standard_EXPORT void RemoveInternalEdges();

  //! Intersects edges
  Standard_EXPORT void IntersectEdges(const TopTools_ListOfShape&      theFaces,
                                      BRepOffset_DataMapOfShapeOffset& theMapSF,
                                      TopTools_DataMapOfShapeShape&    theMES,
                                      TopTools_DataMapOfShapeShape&    theBuild,
                                      Handle(BRepAlgo_AsDes)&          theAsDes,
                                      Handle(BRepAlgo_AsDes)&          theAsDes2d,
                                      const Message_ProgressRange&     theRange);

  //! Building of the splits of the offset faces for mode Complete
  //! and joint type Intersection. This method is an advanced alternative
  //! for BRepOffset_MakeLoops::Build method.
  //! Currently the Complete intersection mode is limited to work only on planar cases.
  Standard_EXPORT void BuildSplitsOfExtendedFaces(
    const TopTools_ListOfShape&         theLF,
    const BRepOffset_Analyse&           theAnalyse,
    const Handle(BRepAlgo_AsDes)&       theAsDes,
    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
    TopTools_DataMapOfShapeShape&       theFacesOrigins,
    TopTools_DataMapOfShapeShape&       theETrimEInf,
    BRepAlgo_Image&                     theImage,
    const Message_ProgressRange&        theRange);

  //! Building of the splits of the already trimmed offset faces for mode Complete
  //! and joint type Intersection.
  Standard_EXPORT void BuildSplitsOfTrimmedFaces(const TopTools_ListOfShape&   theLF,
                                                 const Handle(BRepAlgo_AsDes)& theAsDes,
                                                 BRepAlgo_Image&               theImage,
                                                 const Message_ProgressRange&  theRange);

  Standard_Real                      myOffset;
  Standard_Real                      myTol;
  TopoDS_Shape                       myInitialShape;
  TopoDS_Shape                       myShape;
  TopoDS_Compound                    myFaceComp;
  BRepOffset_Mode                    myMode;
  Standard_Boolean                   myIsLinearizationAllowed;
  Standard_Boolean                   myInter;
  Standard_Boolean                   mySelfInter;
  GeomAbs_JoinType                   myJoin;
  Standard_Boolean                   myThickening;
  Standard_Boolean                   myRemoveIntEdges;
  TopTools_DataMapOfShapeReal        myFaceOffset;
  TopTools_IndexedMapOfShape         myFaces;
  TopTools_IndexedMapOfShape         myOriginalFaces;
  BRepOffset_Analyse                 myAnalyse;
  TopoDS_Shape                       myOffsetShape;
  BRepAlgo_Image                     myInitOffsetFace;
  BRepAlgo_Image                     myInitOffsetEdge;
  BRepAlgo_Image                     myImageOffset;
  BRepAlgo_Image                     myImageVV;
  TopTools_ListOfShape               myWalls;
  Handle(BRepAlgo_AsDes)             myAsDes;
  TopTools_DataMapOfShapeListOfShape myEdgeIntEdges;
  Standard_Boolean                   myDone;
  BRepOffset_Error                   myError;
  BRepOffset_MakeLoops               myMakeLoops;
  Standard_Boolean                   myIsPerformSewing; // Handle bad walls in thicksolid mode.
  Standard_Boolean                   myIsPlanar;
  TopoDS_Shape                       myBadShape;
  TopTools_DataMapOfShapeShape       myFacePlanfaceMap;
  TopTools_ListOfShape               myGenerated;
  TopTools_MapOfShape                myResMap;
};

#endif // _BRepOffset_MakeOffset_HeaderFile
