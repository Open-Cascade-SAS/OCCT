// Created on: 2008-04-10
// Created by: Peter KURNEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <BRepMesh_DiscretFactory.hxx>

#include <BRepMesh_DiscretAlgoFactory.hxx>

//==================================================================================================

BRepMesh_DiscretFactory::BRepMesh_DiscretFactory()
    : myDefaultName("FastDiscret")
{
}

//==================================================================================================

BRepMesh_DiscretFactory& BRepMesh_DiscretFactory::Get()
{
  static BRepMesh_DiscretFactory THE_GLOBAL_FACTORY;
  return THE_GLOBAL_FACTORY;
}

//==================================================================================================

bool BRepMesh_DiscretFactory::SetDefaultName(const TCollection_AsciiString& theName)
{
  Handle(BRepMesh_DiscretAlgoFactory) aFactory = BRepMesh_DiscretAlgoFactory::FindFactory(theName);
  if (aFactory.IsNull())
  {
    return false;
  }

  myDefaultName = theName;
  return true;
}

//==================================================================================================

Handle(BRepMesh_DiscretRoot) BRepMesh_DiscretFactory::Discret(const TopoDS_Shape& theShape,
                                                              double              theLinDeflection,
                                                              double              theAngDeflection)
{
  Handle(BRepMesh_DiscretAlgoFactory) aFactory =
    myDefaultName.IsEmpty() ? BRepMesh_DiscretAlgoFactory::DefaultFactory()
                            : BRepMesh_DiscretAlgoFactory::FindFactory(myDefaultName);

  if (aFactory.IsNull())
  {
    return Handle(BRepMesh_DiscretRoot)();
  }

  return aFactory->CreateAlgorithm(theShape, theLinDeflection, theAngDeflection);
}
