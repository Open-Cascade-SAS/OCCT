// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Graphic3d_GraphicDriverFactory_HeaderFile
#define _Graphic3d_GraphicDriverFactory_HeaderFile

#include <NCollection_List.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

class Aspect_DisplayConnection;
class Graphic3d_GraphicDriver;
class Graphic3d_GraphicDriverFactory;

//! This class for creation of Graphic3d_GraphicDriver.
class Graphic3d_GraphicDriverFactory : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_GraphicDriverFactory, Standard_Transient)
public:
  //! Registers factory.
  //! @param[in] theFactory      factory to register
  //! @param[in] theIsPreferred  add to the beginning of the list when TRUE, or add to the end
  //! otherwise
  Standard_EXPORT static void RegisterFactory(
    const occ::handle<Graphic3d_GraphicDriverFactory>& theFactory,
    bool                                          theIsPreferred = false);

  //! Unregisters factory.
  Standard_EXPORT static void UnregisterFactory(const TCollection_AsciiString& theName);

  //! Return default driver factory or NULL if no one was registered.
  Standard_EXPORT static occ::handle<Graphic3d_GraphicDriverFactory> DefaultDriverFactory();

  //! Return the global map of registered driver factories.
  Standard_EXPORT static const NCollection_List<occ::handle<Graphic3d_GraphicDriverFactory>>& DriverFactories();

public:
  //! Creates new empty graphic driver.
  virtual occ::handle<Graphic3d_GraphicDriver> CreateDriver(
    const occ::handle<Aspect_DisplayConnection>& theDisp) = 0;

  //! Return driver factory name.
  const TCollection_AsciiString& Name() const { return myName; }

protected:
  //! Empty constructor.
  Standard_EXPORT Graphic3d_GraphicDriverFactory(const TCollection_AsciiString& theName);

protected:
  TCollection_AsciiString myName;
};

#endif // _Graphic3d_GraphicDriverFactory_HeaderFile
