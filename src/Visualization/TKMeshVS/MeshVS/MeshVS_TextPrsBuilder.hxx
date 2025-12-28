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

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <MeshVS_DisplayModeFlags.hxx>
#include <MeshVS_BuilderPriority.hxx>

class MeshVS_Mesh;
class Quantity_Color;
class MeshVS_DataSource;
class TCollection_AsciiString;

//! This class provides methods to create text data presentation.
//! It store map of texts assigned with nodes or elements.
class MeshVS_TextPrsBuilder : public MeshVS_PrsBuilder
{

public:
  Standard_EXPORT MeshVS_TextPrsBuilder(
    const occ::handle<MeshVS_Mesh>&       Parent,
    const double                          Height,
    const Quantity_Color&                 Color,
    const MeshVS_DisplayModeFlags&        Flags    = MeshVS_DMF_TextDataPrs,
    const occ::handle<MeshVS_DataSource>& DS       = nullptr,
    const int                             Id       = -1,
    const MeshVS_BuilderPriority&         Priority = MeshVS_BP_Text);

  //! Builds presentation of text data
  Standard_EXPORT void Build(const occ::handle<Prs3d_Presentation>& Prs,
                                     const TColStd_PackedMapOfInteger&      IDs,
                                     TColStd_PackedMapOfInteger&            IDsToExclude,
                                     const bool                             IsElement,
                                     const int theDisplayMode) const override;

  //! Returns map of text assigned with nodes ( IsElement = False ) or elements ( IsElement = True )
  Standard_EXPORT const NCollection_DataMap<int, TCollection_AsciiString>& GetTexts(
    const bool IsElement) const;

  //! Sets map of text assigned with nodes or elements
  Standard_EXPORT void SetTexts(const bool                                               IsElement,
                                const NCollection_DataMap<int, TCollection_AsciiString>& Map);

  //! Returns True if map isn't empty
  Standard_EXPORT bool HasTexts(const bool IsElement) const;

  //! Returns text assigned with single node or element
  Standard_EXPORT bool GetText(const bool               IsElement,
                               const int                ID,
                               TCollection_AsciiString& Text) const;

  //! Sets text assigned with single node or element
  Standard_EXPORT void SetText(const bool                     IsElement,
                               const int                      ID,
                               const TCollection_AsciiString& Text);

  DEFINE_STANDARD_RTTIEXT(MeshVS_TextPrsBuilder, MeshVS_PrsBuilder)

private:
  NCollection_DataMap<int, TCollection_AsciiString> myNodeTextMap;
  NCollection_DataMap<int, TCollection_AsciiString> myElemTextMap;
};

#endif // _MeshVS_TextPrsBuilder_HeaderFile
