// Created on: 2003-11-12
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

#ifndef _MeshVS_ElementalColorPrsBuilder_HeaderFile
#define _MeshVS_ElementalColorPrsBuilder_HeaderFile

#include <MeshVS_DataMapOfIntegerColor.hxx>
#include <MeshVS_DataMapOfIntegerTwoColors.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_BuilderPriority.hxx>
#include <MeshVS_TwoColors.hxx>

class MeshVS_Mesh;
class MeshVS_DataSource;
class Quantity_Color;


class MeshVS_ElementalColorPrsBuilder;
DEFINE_STANDARD_HANDLE(MeshVS_ElementalColorPrsBuilder, MeshVS_PrsBuilder)

//! This class provides methods to create presentation of elements with
//! assigned colors. The class contains two color maps: map of same colors for front
//! and back side of face and map of different ones,
class MeshVS_ElementalColorPrsBuilder : public MeshVS_PrsBuilder
{

public:

  
  //! Constructor
  Standard_EXPORT MeshVS_ElementalColorPrsBuilder(const Handle(MeshVS_Mesh)& Parent, const MeshVS_DisplayModeFlags& Flags = MeshVS_DMF_ElementalColorDataPrs, const Handle(MeshVS_DataSource)& DS = 0, const Standard_Integer Id = -1, const MeshVS_BuilderPriority& Priority = MeshVS_BP_ElemColor);
  
  //! Builds presentation of elements with assigned colors.
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& Prs, const TColStd_PackedMapOfInteger& IDs, TColStd_PackedMapOfInteger& IDsToExclude, const Standard_Boolean IsElement, const Standard_Integer DisplayMode) const Standard_OVERRIDE;
  
  //! Returns map of colors same for front and back side of face.
  Standard_EXPORT const MeshVS_DataMapOfIntegerColor& GetColors1() const;
  
  //! Sets map of colors same for front and back side of face.
  Standard_EXPORT void SetColors1 (const MeshVS_DataMapOfIntegerColor& Map);
  
  //! Returns true, if map of colors isn't empty
  Standard_EXPORT Standard_Boolean HasColors1() const;
  
  //! Returns color assigned with element number ID
  Standard_EXPORT Standard_Boolean GetColor1 (const Standard_Integer ID, Quantity_Color& theColor) const;
  
  //! Sets color assigned with element number ID
  Standard_EXPORT void SetColor1 (const Standard_Integer ID, const Quantity_Color& theColor);
  
  //! Returns map of different colors for front and back side of face
  Standard_EXPORT const MeshVS_DataMapOfIntegerTwoColors& GetColors2() const;
  
  //! Sets map of different colors for front and back side of face
  Standard_EXPORT void SetColors2 (const MeshVS_DataMapOfIntegerTwoColors& Map);
  
  //! Returns true, if map isn't empty
  Standard_EXPORT Standard_Boolean HasColors2() const;
  
  //! Returns colors assigned with element number ID
  Standard_EXPORT Standard_Boolean GetColor2 (const Standard_Integer ID, MeshVS_TwoColors& theColor) const;
  
  //! Returns colors assigned with element number ID
  //! theColor1 is the front element color
  //! theColor2 is the back element color
  Standard_EXPORT Standard_Boolean GetColor2 (const Standard_Integer ID, Quantity_Color& theColor1, Quantity_Color& theColor2) const;
  
  //! Sets colors assigned with element number ID
  Standard_EXPORT void SetColor2 (const Standard_Integer ID, const MeshVS_TwoColors& theTwoColors);
  
  //! Sets color assigned with element number ID
  //! theColor1 is the front element color
  //! theColor2 is the back element color
  Standard_EXPORT void SetColor2 (const Standard_Integer ID, const Quantity_Color& theColor1, const Quantity_Color& theColor2);




  DEFINE_STANDARD_RTTIEXT(MeshVS_ElementalColorPrsBuilder,MeshVS_PrsBuilder)

protected:




private:


  MeshVS_DataMapOfIntegerColor myElemColorMap1;
  MeshVS_DataMapOfIntegerTwoColors myElemColorMap2;


};







#endif // _MeshVS_ElementalColorPrsBuilder_HeaderFile
