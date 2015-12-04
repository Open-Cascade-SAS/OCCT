// Created on: 2003-10-13
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_VectorPrsBuilder_HeaderFile
#define _MeshVS_VectorPrsBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <MeshVS_DataMapOfIntegerVector.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <Standard_Integer.hxx>
#include <MeshVS_BuilderPriority.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
class MeshVS_Mesh;
class Quantity_Color;
class MeshVS_DataSource;
class Prs3d_Presentation;
class gp_Trsf;
class Graphic3d_ArrayOfPrimitives;
class gp_Vec;


class MeshVS_VectorPrsBuilder;
DEFINE_STANDARD_HANDLE(MeshVS_VectorPrsBuilder, MeshVS_PrsBuilder)

//! This class provides methods to create vector data presentation.
//! It store map of vectors assigned with nodes or elements.
//! In simplified mode vectors draws with thickened ends instead of arrows
class MeshVS_VectorPrsBuilder : public MeshVS_PrsBuilder
{

public:

  
  Standard_EXPORT MeshVS_VectorPrsBuilder(const Handle(MeshVS_Mesh)& Parent, const Standard_Real MaxLength, const Quantity_Color& VectorColor, const MeshVS_DisplayModeFlags& Flags = MeshVS_DMF_VectorDataPrs, const Handle(MeshVS_DataSource)& DS = 0, const Standard_Integer Id = -1, const MeshVS_BuilderPriority& Priority = MeshVS_BP_Vector, const Standard_Boolean IsSimplePrs = Standard_False);
  
  //! Builds vector data presentation
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& Prs, const TColStd_PackedMapOfInteger& IDs, TColStd_PackedMapOfInteger& IDsToExclude, const Standard_Boolean IsElement, const Standard_Integer theDisplayMode) const Standard_OVERRIDE;
  
  //! Adds to array of polygons and polylines some primitive representing single vector
  Standard_EXPORT void DrawVector (const gp_Trsf& theTrsf, const Standard_Real Length, const Standard_Real MaxLength, const TColgp_Array1OfPnt& ArrowPoints, const Handle(Graphic3d_ArrayOfPrimitives)& Lines, const Handle(Graphic3d_ArrayOfPrimitives)& ArrowLines, const Handle(Graphic3d_ArrayOfPrimitives)& Triangles) const;
  
  //! Calculates points of arrow presentation
  Standard_EXPORT static Standard_Real calculateArrow (TColgp_Array1OfPnt& Points, const Standard_Real Length, const Standard_Real ArrowPart);
  
  //! Returns map of vectors assigned with nodes or elements
  Standard_EXPORT const MeshVS_DataMapOfIntegerVector& GetVectors (const Standard_Boolean IsElement) const;
  
  //! Sets map of vectors assigned with nodes or elements
  Standard_EXPORT void SetVectors (const Standard_Boolean IsElement, const MeshVS_DataMapOfIntegerVector& Map);
  
  //! Returns true, if map isn't empty
  Standard_EXPORT Standard_Boolean HasVectors (const Standard_Boolean IsElement) const;
  
  //! Returns vector assigned with certain node or element
  Standard_EXPORT Standard_Boolean GetVector (const Standard_Boolean IsElement, const Standard_Integer ID, gp_Vec& Vect) const;
  
  //! Sets vector assigned with certain node or element
  Standard_EXPORT void SetVector (const Standard_Boolean IsElement, const Standard_Integer ID, const gp_Vec& Vect);
  
  //! Calculates minimal and maximal length of vectors in map
  //! ( nodal, if IsElement = False or elemental, if IsElement = True )
  Standard_EXPORT void GetMinMaxVectorValue (const Standard_Boolean IsElement, Standard_Real& MinValue, Standard_Real& MaxValue) const;
  
  //! Sets flag that indicates is simple vector arrow mode uses or not
  //! default value is False
  Standard_EXPORT void SetSimplePrsMode (const Standard_Boolean IsSimpleArrow);
  
  //! Sets parameters of simple vector arrwo presentation
  //! theLineWidthParam - coefficient of vector line width (to draw line instead of arrow)
  //! theStartParam and theEndParam parameters of start and end of thickened ends
  //! position of thickening calculates according to parameters and maximum vector length
  //! default values are:
  //! theLineWidthParam = 2.5
  //! theStartParam     = 0.85
  //! theEndParam       = 0.95
  Standard_EXPORT void SetSimplePrsParams (const Standard_Real theLineWidthParam, const Standard_Real theStartParam, const Standard_Real theEndParam);




  DEFINE_STANDARD_RTTIEXT(MeshVS_VectorPrsBuilder,MeshVS_PrsBuilder)

protected:




private:


  Standard_Boolean myIsSimplePrs;
  Standard_Real mySimpleWidthPrm;
  Standard_Real mySimpleStartPrm;
  Standard_Real mySimpleEndPrm;
  MeshVS_DataMapOfIntegerVector myNodeVectorMap;
  MeshVS_DataMapOfIntegerVector myElemVectorMap;


};







#endif // _MeshVS_VectorPrsBuilder_HeaderFile
