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

#include <Graphic3d_GraphicDriverFactory.hxx>

#include <Graphic3d_GraphicDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_GraphicDriverFactory, Standard_Transient)

namespace
{
  static Graphic3d_GraphicDriverFactoryList& getDriverFactories()
  {
    static Graphic3d_GraphicDriverFactoryList TheFactories;
    return TheFactories;
  }
}

// =======================================================================
// function : DriverFactories
// purpose  :
// =======================================================================
const Graphic3d_GraphicDriverFactoryList& Graphic3d_GraphicDriverFactory::DriverFactories()
{
  return getDriverFactories();
}

// =======================================================================
// function : RegisterFactory
// purpose  :
// =======================================================================
void Graphic3d_GraphicDriverFactory::RegisterFactory (const Handle(Graphic3d_GraphicDriverFactory)& theFactory,
                                                      bool theIsPreferred)
{
  const TCollection_AsciiString aName = theFactory->Name();
  Graphic3d_GraphicDriverFactoryList& aFactories = getDriverFactories();
  if (theIsPreferred)
  {
    UnregisterFactory (aName);
    aFactories.Prepend (theFactory);
    return;
  }

  for (Graphic3d_GraphicDriverFactoryList::Iterator anIter (aFactories); anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString (anIter.Value()->Name(), aName, false))
    {
      return;
    }
  }
  aFactories.Append (theFactory);
}

// =======================================================================
// function : UnregisterFactory
// purpose  :
// =======================================================================
void Graphic3d_GraphicDriverFactory::UnregisterFactory (const TCollection_AsciiString& theName)
{
  Graphic3d_GraphicDriverFactoryList& aFactories = getDriverFactories();
  for (Graphic3d_GraphicDriverFactoryList::Iterator anIter (aFactories); anIter.More();)
  {
    if (TCollection_AsciiString::IsSameString (anIter.Value()->Name(), theName, false))
    {
      aFactories.Remove (anIter);
    }
    else
    {
      anIter.Next();
    }
  }
}

// =======================================================================
// function : DefaultDriverFactory
// purpose  :
// =======================================================================
Handle(Graphic3d_GraphicDriverFactory) Graphic3d_GraphicDriverFactory::DefaultDriverFactory()
{
  const Graphic3d_GraphicDriverFactoryList& aMap = getDriverFactories();
  return !aMap.IsEmpty()
        ? aMap.First()
        : Handle(Graphic3d_GraphicDriverFactory)();
}

// =======================================================================
// function : Graphic3d_GraphicDriverFactory
// purpose  :
// =======================================================================
Graphic3d_GraphicDriverFactory::Graphic3d_GraphicDriverFactory (const TCollection_AsciiString& theName)
: myName (theName)
{
  //
}
