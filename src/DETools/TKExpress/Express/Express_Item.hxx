// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#ifndef _Express_Item_HeaderFile
#define _Express_Item_HeaderFile

#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

class TCollection_AsciiString;
class TCollection_HAsciiString;

//! Base class for items of the schema. Stores a name of the class,
//! package name and flag used to mark items for generation.
//! Provides interface for writing generated class definitions to HXX
//! and CXX files.
class Express_Item : public Standard_Transient
{

public:
  enum GenMode
  {
    GM_NoGen,     // Item in existed list - no need to generate
    GM_GenByUser, // Item in new list - need to generate
    GM_GenByAlgo, // Item isn't in any list but is used by needed item
    GM_Undefined, // Item isn't in any list
    GM_Generated  // Item has been generated
  };

  //! Returns item name
  Standard_EXPORT const TCollection_AsciiString& Name() const;

  //! Returns a pointer to the item name to modify it
  Standard_EXPORT Handle(TCollection_HAsciiString) HName() const;

  //! Returns (generated) name for the item in CXX-style (Package_Class)
  Standard_EXPORT virtual const TCollection_AsciiString CPPName() const;

  //! Returns package name
  //! If not defined, returns unknown package name: "StepStep"
  Standard_EXPORT const TCollection_AsciiString& GetPackageName() const;

  //! Returns whether the package name is set.
  Standard_EXPORT Standard_Boolean IsPackageNameSet() const;

  //! Returns unknown package name: "StepStep"
  Standard_EXPORT static TCollection_AsciiString& GetUnknownPackageName();

  //! Sets package name
  Standard_EXPORT void SetPackageName(const TCollection_AsciiString& thePack);

  //! Returns item generation mode
  Standard_EXPORT GenMode GetGenMode() const;

  //! Change generation mode for item
  Standard_EXPORT void SetGenMode(const GenMode theGenMode);

  //! Reset loop flag
  Standard_EXPORT void ResetLoopFlag();

  //! General interface for creating HXX/CXX files from item
  Standard_EXPORT virtual Standard_Boolean GenerateClass() const = 0;

  //! Propagates the calls of Use function
  Standard_EXPORT virtual void PropagateUse() const = 0;

  //! Checks that item is marked for generation and if yes,
  //! generate it by calling GenerateClass. But firstly define
  //! PackageName to "StepStep" if not yet defined and drop Mark flag.
  Standard_EXPORT Standard_Boolean Generate();

  //! Declares item as used by other item being generated
  //! If Item is not mentioned by the user (as new or existed) but is used,
  //! then it sets GenMode to GM_GenByAlgo and Calls Generate().
  Standard_EXPORT Standard_Boolean Use();

  //! Mark Item as visited in PropagateUse flow and defined the package name if not set.
  Standard_EXPORT void Use2(const TCollection_AsciiString& theRefName,
                            const TCollection_AsciiString& theRefPack);

  //! Set category for item
  Standard_EXPORT void SetCategory(const Handle(TCollection_HAsciiString)& theCateg);

  //! Get item category
  Standard_EXPORT const TCollection_AsciiString& Category() const;

  //! Set short name for item
  Standard_EXPORT void SetShortName(const Handle(TCollection_HAsciiString)& theShName);

  //! Get item short name
  Standard_EXPORT Handle(TCollection_HAsciiString) ShortName() const;

  //! Set flag for presence of method Check in the class
  Standard_EXPORT void SetCheckFlag(const Standard_Boolean theCheckFlag);

  //! Get flag responsible for presence of method Check in the class
  Standard_EXPORT Standard_Boolean CheckFlag() const;

  //! Set flag for presence of method FillShared in the class
  Standard_EXPORT void SetFillSharedFlag(const Standard_Boolean theFillSharedFlag);

  //! Get flag responsible for presence of method FillShared in the class
  Standard_EXPORT Standard_Boolean FillSharedFlag() const;

  //! Set start entity index
  Standard_EXPORT static void SetIndex(const Standard_Integer theIndex);

  //! Get current entity index
  Standard_EXPORT static Standard_Integer Index();

  DEFINE_STANDARD_RTTIEXT(Express_Item, Standard_Transient)

protected:
  //! Creates object and initializes fields PackageName and
  //! CreateFlag by 0
  Standard_EXPORT Express_Item(const Standard_CString theName);

  //! Creates object and initializes fields PackageName and
  //! CreateFlag by 0
  Standard_EXPORT Express_Item(const Handle(TCollection_HAsciiString)& theName);

private:
  Handle(TCollection_HAsciiString) myName;
  Handle(TCollection_HAsciiString) myPack;
  // "Generate" mark. If is TRUE a class will be generated for the item
  GenMode myGenMode;
  // Flag to avoid looping
  Standard_Boolean                 myLoopFlag;
  Handle(TCollection_HAsciiString) myShortName;
  Handle(TCollection_HAsciiString) myCategory;
  Standard_Boolean                 myhasCheck;
  Standard_Boolean                 myhasFillShared;
  static Standard_Integer          myIndex;
};

#endif // _Express_Item_HeaderFile
