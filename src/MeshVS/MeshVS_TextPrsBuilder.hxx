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

#ifndef _MeshVS_TextPrsBuilder_HeaderFile
#define _MeshVS_TextPrsBuilder_HeaderFile

#include <MeshVS_DataMapOfIntegerAsciiString.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_BuilderPriority.hxx>

class MeshVS_Mesh;
class Quantity_Color;
class MeshVS_DataSource;
class TCollection_AsciiString;

DEFINE_STANDARD_HANDLE(MeshVS_TextPrsBuilder, MeshVS_PrsBuilder)

//! This class provides methods to create text data presentation.
//! It store map of texts assigned with nodes or elements.
class MeshVS_TextPrsBuilder : public MeshVS_PrsBuilder
{

public:

  
  Standard_EXPORT MeshVS_TextPrsBuilder(const Handle(MeshVS_Mesh)& Parent, const Standard_Real Height, const Quantity_Color& Color, const MeshVS_DisplayModeFlags& Flags = MeshVS_DMF_TextDataPrs, const Handle(MeshVS_DataSource)& DS = 0, const Standard_Integer Id = -1, const MeshVS_BuilderPriority& Priority = MeshVS_BP_Text);
  
  //! Builds presentation of text data
  Standard_EXPORT virtual void Build (const Handle(Prs3d_Presentation)& Prs, const TColStd_PackedMapOfInteger& IDs, TColStd_PackedMapOfInteger& IDsToExclude, const Standard_Boolean IsElement, const Standard_Integer theDisplayMode) const Standard_OVERRIDE;
  
  //! Returns map of text assigned with nodes ( IsElement = False ) or elements ( IsElement = True )
  Standard_EXPORT const MeshVS_DataMapOfIntegerAsciiString& GetTexts (const Standard_Boolean IsElement) const;
  
  //! Sets map of text assigned with nodes or elements
  Standard_EXPORT void SetTexts (const Standard_Boolean IsElement, const MeshVS_DataMapOfIntegerAsciiString& Map);
  
  //! Returns True if map isn't empty
  Standard_EXPORT Standard_Boolean HasTexts (const Standard_Boolean IsElement) const;
  
  //! Returns text assigned with single node or element
  Standard_EXPORT Standard_Boolean GetText (const Standard_Boolean IsElement, const Standard_Integer ID, TCollection_AsciiString& Text) const;
  
  //! Sets text assigned with single node or element
  Standard_EXPORT void SetText (const Standard_Boolean IsElement, const Standard_Integer ID, const TCollection_AsciiString& Text);




  DEFINE_STANDARD_RTTIEXT(MeshVS_TextPrsBuilder,MeshVS_PrsBuilder)

protected:




private:


  MeshVS_DataMapOfIntegerAsciiString myNodeTextMap;
  MeshVS_DataMapOfIntegerAsciiString myElemTextMap;


};







#endif // _MeshVS_TextPrsBuilder_HeaderFile
