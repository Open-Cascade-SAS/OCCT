// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _XmlMDF_DerivedDriver_HeaderFile
#define _XmlMDF_DerivedDriver_HeaderFile

#include <XmlMDF_ADriver.hxx>
#include <TDF_DerivedAttribute.hxx>

//! A universal driver for the attribute that inherits another attribute with
//! ready to used persistence mechanism implemented (already has a driver to store/retrieve).
class XmlMDF_DerivedDriver : public XmlMDF_ADriver
{
  DEFINE_STANDARD_RTTIEXT(XmlMDF_DerivedDriver, XmlMDF_ADriver)
public:
  //! Creates a derivative persistence driver for theDerivative attribute by reusage of
  //! theBaseDriver
  //! @param theDerivative an instance of the attribute, just created, detached from any label
  //! @param theBaseDriver a driver of the base attribute, called by Paste methods
  XmlMDF_DerivedDriver(const Handle(TDF_Attribute)&  theDerivative,
                       const Handle(XmlMDF_ADriver)& theBaseDriver)
      : XmlMDF_ADriver(theBaseDriver->MessageDriver(), theBaseDriver->Namespace().ToCString()),
        myDerivative(theDerivative),
        myBaseDirver(theBaseDriver)
  {
  }

  //! Creates a new instance of the derivative attribute
  virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE
  {
    return myDerivative->NewEmpty();
  }

  //! Returns the full XML tag name (including NS prefix)
  const TCollection_AsciiString& TypeName() const
  {
    const TCollection_AsciiString& aRegistered =
      TDF_DerivedAttribute::TypeName(myDerivative->DynamicType()->Name());
    if (aRegistered.IsEmpty())
    {
      return XmlMDF_ADriver::TypeName();
    }
    return aRegistered;
  }

  //! Reuses the base driver to read the base fields
  virtual Standard_Boolean Paste(const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  theRelocTable) const Standard_OVERRIDE
  {
    Standard_Boolean aResult = myBaseDirver->Paste(theSource, theTarget, theRelocTable);
    // clang-format off
    theTarget->AfterRetrieval(); // to allow synchronization of the derived attribute with the base content
    // clang-format on
    return aResult;
  }

  //! Reuses the base driver to store the base fields
  virtual void Paste(const Handle(TDF_Attribute)& theSource,
                     XmlObjMgt_Persistent&        theTarget,
                     XmlObjMgt_SRelocationTable&  theRelocTable) const Standard_OVERRIDE
  {
    myBaseDirver->Paste(theSource, theTarget, theRelocTable);
  }

protected:
  Handle(TDF_Attribute)  myDerivative; //!< the derivative attribute that inherits the base
  Handle(XmlMDF_ADriver) myBaseDirver; //!< the base attribute driver to be reused here
};

#endif
