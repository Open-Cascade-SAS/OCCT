// Created on: 2017-02-16
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_AssemblyItemRef_HeaderFile
#define _XCAFDoc_AssemblyItemRef_HeaderFile

#include <Standard.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_AssemblyItemId.hxx>

class TDF_RelocationTable;

class XCAFDoc_AssemblyItemRef;
DEFINE_STANDARD_HANDLE(XCAFDoc_AssemblyItemRef, TDF_Attribute)

//! An attribute that describes a weak reference to an assembly item
//! or to a subshape or to an assembly label attribute.
class XCAFDoc_AssemblyItemRef : public TDF_Attribute
{

public:
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_AssemblyItemRef, TDF_Attribute)

  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds a reference attribute on the given label and returns it, if it is found
  Standard_EXPORT static Handle(XCAFDoc_AssemblyItemRef) Get(const TDF_Label& theLabel);

  //! @name Set reference attribute functions.
  //! @{

  //! Create (if not exist) a reference to an assembly item.
  //! \param[in]  theLabel  - label to add the attribute.
  //! \param[in]  theItemId - assembly item ID.
  //! \return A handle to the attribute instance.
  Standard_EXPORT static Handle(XCAFDoc_AssemblyItemRef) Set(
    const TDF_Label&              theLabel,
    const XCAFDoc_AssemblyItemId& theItemId);

  //! Create (if not exist) a reference to an assembly item's label attribute.
  //! \param[in]  theLabel  - label to add the attribute.
  //! \param[in]  theItemId - assembly item ID.
  //! \param[in]  theGUID   - assembly item's label attribute ID.
  //! \return A handle to the attribute instance.
  Standard_EXPORT static Handle(XCAFDoc_AssemblyItemRef) Set(
    const TDF_Label&              theLabel,
    const XCAFDoc_AssemblyItemId& theItemId,
    const Standard_GUID&          theGUID);

  //! Create (if not exist) a reference to an assembly item's subshape.
  //! \param[in]  theLabel      - label to add the attribute.
  //! \param[in]  theItemId     - assembly item ID.
  //! \param[in]  theShapeIndex - assembly item's subshape index.
  //! \return A handle to the attribute instance.
  Standard_EXPORT static Handle(XCAFDoc_AssemblyItemRef) Set(
    const TDF_Label&              theLabel,
    const XCAFDoc_AssemblyItemId& theItemId,
    const Standard_Integer        theShapeIndex);

  //! @}

  //! Creates an empty reference attribute.
  Standard_EXPORT XCAFDoc_AssemblyItemRef();

  //! Checks if the reference points to a really existing item in XDE document.
  Standard_EXPORT Standard_Boolean IsOrphan() const;

  //! @name Extra reference functions.
  //! @{

  //! Checks if the reference points on an item's shapeindex or attribute.
  Standard_EXPORT Standard_Boolean HasExtraRef() const;

  //! Checks is the reference points to an item's attribute.
  Standard_EXPORT Standard_Boolean IsGUID() const;

  //! Checks is the reference points to an item's subshape.
  Standard_EXPORT Standard_Boolean IsSubshapeIndex() const;

  //! Returns the assembly item's attribute that the reference points to.
  //! If the reference doesn't point to an attribute, returns an empty GUID.
  Standard_EXPORT Standard_GUID GetGUID() const;

  //! Returns the assembly item's subshape that the reference points to.
  //! If the reference doesn't point to a subshape, returns 0.
  Standard_EXPORT Standard_Integer GetSubshapeIndex() const;

  //! @}

  //! Returns the assembly item ID that the reference points to.
  Standard_EXPORT const XCAFDoc_AssemblyItemId& GetItem() const;

  //! @name Set reference data functions.
  //! @{

  //! Sets the assembly item ID that the reference points to.
  //! Extra reference data (if any) will be cleared.
  Standard_EXPORT void SetItem(const XCAFDoc_AssemblyItemId& theItemId);

  //! Sets the assembly item ID from a list of label entries
  //! that the reference points to.
  //! Extra reference data (if any) will be cleared.
  Standard_EXPORT void SetItem(const TColStd_ListOfAsciiString& thePath);

  //! Sets the assembly item ID from a formatted path
  //! that the reference points to.
  //! Extra reference data (if any) will be cleared.
  Standard_EXPORT void SetItem(const TCollection_AsciiString& theString);

  //! Sets the assembly item's label attribute that the reference points to.
  //! The base assembly item will not change.
  Standard_EXPORT void SetGUID(const Standard_GUID& theAttrGUID);

  //! Sets the assembly item's subshape that the reference points to.
  //! The base assembly item will not change.
  Standard_EXPORT void SetSubshapeIndex(Standard_Integer theShapeIndex);

  //! @}

  //! Reverts the reference to empty state.
  Standard_EXPORT void ClearExtraRef();

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

public:
  // Overrides TDF_Attribute pure virtuals
  Standard_EXPORT const Standard_GUID&  ID() const Standard_OVERRIDE;
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

private:
  XCAFDoc_AssemblyItemId  myItemId;   ///< Assembly item ID
  Standard_Integer        myExtraRef; ///< Type of extra reference: subshape or attribute
  TCollection_AsciiString myExtraId;  ///< Extra reference data
};

#endif // _XCAFDoc_AssemblyItemRef_HeaderFile
