// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRep_HeaderFile
#define _TopOpeBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
#include <TopOpeBRep_TypeLineCurve.hxx>
class TopOpeBRep_VPointInter;
class TopOpeBRep_VPointInterIterator;
class TopOpeBRep_WPointInter;
class TopOpeBRep_WPointInterIterator;
class TopOpeBRep_LineInter;
class TopOpeBRep_Hctxff2d;
class TopOpeBRep_Hctxee2d;
class TopOpeBRep_Point2d;
class TopOpeBRep_PointClassifier;
class TopOpeBRep_VPointInterClassifier;
class TopOpeBRep_GeomTool;
class TopOpeBRep_FacesIntersector;
class TopOpeBRep_EdgesIntersector;
class TopOpeBRep_FaceEdgeIntersector;
class TopOpeBRep_ShapeScanner;
class TopOpeBRep_ShapeIntersector;
class TopOpeBRep_ShapeIntersector2d;
class TopOpeBRep_PointGeomTool;
class TopOpeBRep_FFTransitionTool;
class TopOpeBRep_Bipoint;
class TopOpeBRep_FacesFiller;
class TopOpeBRep_FFDumper;
class TopOpeBRep_EdgesFiller;
class TopOpeBRep_FaceEdgeFiller;
class TopOpeBRep_DSFiller;


//! This package provides  the topological  operations
//! on the BRep data structure.
class TopOpeBRep 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prints the name  of  <TLC>  as  a String  on the
  //! Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRep_TypeLineCurve TLC, Standard_OStream& OS);




protected:





private:




friend class TopOpeBRep_VPointInter;
friend class TopOpeBRep_VPointInterIterator;
friend class TopOpeBRep_WPointInter;
friend class TopOpeBRep_WPointInterIterator;
friend class TopOpeBRep_LineInter;
friend class TopOpeBRep_Hctxff2d;
friend class TopOpeBRep_Hctxee2d;
friend class TopOpeBRep_Point2d;
friend class TopOpeBRep_PointClassifier;
friend class TopOpeBRep_VPointInterClassifier;
friend class TopOpeBRep_GeomTool;
friend class TopOpeBRep_FacesIntersector;
friend class TopOpeBRep_EdgesIntersector;
friend class TopOpeBRep_FaceEdgeIntersector;
friend class TopOpeBRep_ShapeScanner;
friend class TopOpeBRep_ShapeIntersector;
friend class TopOpeBRep_ShapeIntersector2d;
friend class TopOpeBRep_PointGeomTool;
friend class TopOpeBRep_FFTransitionTool;
friend class TopOpeBRep_Bipoint;
friend class TopOpeBRep_FacesFiller;
friend class TopOpeBRep_FFDumper;
friend class TopOpeBRep_EdgesFiller;
friend class TopOpeBRep_FaceEdgeFiller;
friend class TopOpeBRep_DSFiller;

};







#endif // _TopOpeBRep_HeaderFile
