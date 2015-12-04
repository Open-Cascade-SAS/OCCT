// Created on: 1999-09-13
// Created by: data exchange team
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_FixSmallFace_HeaderFile
#define _ShapeFix_FixSmallFace_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeAnalysis_CheckSmallFace.hxx>
#include <ShapeFix_Root.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Compound;


class ShapeFix_FixSmallFace;
DEFINE_STANDARD_HANDLE(ShapeFix_FixSmallFace, ShapeFix_Root)

//! Fixing face with small size
class ShapeFix_FixSmallFace : public ShapeFix_Root
{

public:

  
  Standard_EXPORT ShapeFix_FixSmallFace();
  
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  

  //! Fixing case of spot face
  Standard_EXPORT void Perform();
  
  //! Fixing case of spot face, if tol = -1 used local tolerance.
  Standard_EXPORT TopoDS_Shape FixSpotFace();
  
  //! Compute average vertex and replacing vertices by new one.
  Standard_EXPORT Standard_Boolean ReplaceVerticesInCaseOfSpot (TopoDS_Face& F, const Standard_Real tol) const;
  
  //! Remove spot face from compound
  Standard_EXPORT Standard_Boolean RemoveFacesInCaseOfSpot (const TopoDS_Face& F) const;
  
  //! Fixing case of strip face, if tol = -1 used local tolerance
  Standard_EXPORT TopoDS_Shape FixStripFace (const Standard_Boolean wasdone = Standard_False);
  
  //! Replace veretces and edges.
  Standard_EXPORT Standard_Boolean ReplaceInCaseOfStrip (TopoDS_Face& F, TopoDS_Edge& E1, TopoDS_Edge& E2, const Standard_Real tol) const;
  
  //! Remove strip face from compound.
  Standard_EXPORT Standard_Boolean RemoveFacesInCaseOfStrip (const TopoDS_Face& F) const;
  
  //! Compute average edge for strip face
  Standard_EXPORT TopoDS_Edge ComputeSharedEdgeForStripFace (const TopoDS_Face& F, const TopoDS_Edge& E1, const TopoDS_Edge& E2, const TopoDS_Face& F1, const Standard_Real tol) const;
  
  Standard_EXPORT TopoDS_Shape FixSplitFace (const TopoDS_Shape& S);
  
  //! Compute data for face splitting.
  Standard_EXPORT Standard_Boolean SplitOneFace (TopoDS_Face& F, TopoDS_Compound& theSplittedFaces);
  
  Standard_EXPORT TopoDS_Face FixFace (const TopoDS_Face& F);
  
  Standard_EXPORT TopoDS_Shape FixShape();
  
  Standard_EXPORT TopoDS_Shape Shape();
  
  Standard_EXPORT Standard_Boolean FixPinFace (TopoDS_Face& F);




  DEFINE_STANDARD_RTTIEXT(ShapeFix_FixSmallFace,ShapeFix_Root)

protected:




private:


  TopoDS_Shape myShape;
  TopoDS_Shape myResult;
  Standard_Integer myStatus;
  ShapeAnalysis_CheckSmallFace myAnalyzer;


};







#endif // _ShapeFix_FixSmallFace_HeaderFile
