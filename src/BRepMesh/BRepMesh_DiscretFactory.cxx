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

#include <OSD_SharedLibrary.hxx>
#include <OSD_Function.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_DiscretRoot.hxx>

namespace
{
  //! Embedded triangulation tool(s)
  static TCollection_AsciiString THE_FAST_DISCRET_MESH ("FastDiscret");

  //! Generate system-dependent name for dynamic library
  //! (add standard prefixes and postfixes)
  static void MakeLibName (const TCollection_AsciiString& theDefaultName,
                                 TCollection_AsciiString& theLibName)
  {
    theLibName = "";
  #ifndef _WIN32
    theLibName += "lib";
  #endif
    theLibName += theDefaultName;
  #ifdef _WIN32
    theLibName += ".dll";
  #elif __APPLE__
    theLibName += ".dylib";
  #elif defined (HPUX) || defined(_hpux)
    theLibName += ".sl";
  #else
    theLibName += ".so";
  #endif
  }
}

//=======================================================================
//function : BRepMesh_DiscretFactory
//purpose  :
//=======================================================================
BRepMesh_DiscretFactory::BRepMesh_DiscretFactory()
: myPluginEntry  (NULL),
  myErrorStatus  (BRepMesh_FE_NOERROR),
  myDefaultName  (THE_FAST_DISCRET_MESH),
  myFunctionName ("DISCRETALGO")
{
  // register built-in meshing algorithms
  myNames.Add(THE_FAST_DISCRET_MESH);
}

//=======================================================================
//function : ~
//purpose  :
//=======================================================================
BRepMesh_DiscretFactory::~BRepMesh_DiscretFactory()
{
  clear();
}

//=======================================================================
//function : clear
//purpose  :
//=======================================================================
void BRepMesh_DiscretFactory::clear()
{
  // what should we do here? Unload dynamic libraries and reset plugins list?
}

//=======================================================================
//function : Get
//purpose  :
//=======================================================================
BRepMesh_DiscretFactory& BRepMesh_DiscretFactory::Get()
{
  //! global factory instance
  static BRepMesh_DiscretFactory THE_GLOBAL_FACTORY;
  return THE_GLOBAL_FACTORY;
}

//=======================================================================
//function : SetDefault
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_DiscretFactory::SetDefault(
  const TCollection_AsciiString& theName,
  const TCollection_AsciiString& theFuncName)
{
  myErrorStatus = BRepMesh_FE_NOERROR;
  if (theName == THE_FAST_DISCRET_MESH)
  {
    // built-in, nothing to do
    myPluginEntry  = NULL;
    myDefaultName  = theName;
    myFunctionName = theFuncName;
    return Standard_True;
  }
  else if (theName == myDefaultName && theFuncName == myFunctionName)
  {
    // already active
    return myPluginEntry != NULL;
  }

  TCollection_AsciiString aMeshAlgoId = theName + "_" + theFuncName;
  BRepMesh_PluginEntryType aFunc = NULL;
  if (myFactoryMethods.IsBound (aMeshAlgoId))
  {
    // retrieve from cache
    aFunc = (BRepMesh_PluginEntryType )myFactoryMethods (aMeshAlgoId);
  }
  else
  {
    TCollection_AsciiString aLibName;
    MakeLibName (theName, aLibName);
    OSD_SharedLibrary aSL (aLibName.ToCString());
    if (!aSL.DlOpen (OSD_RTLD_LAZY))
    {
      // library is not found
      myErrorStatus = BRepMesh_FE_LIBRARYNOTFOUND;
      return Standard_False;
    }

    // retrieve the function from plugin
    aFunc = (BRepMesh_PluginEntryType )aSL.DlSymb (theFuncName.ToCString());
    myFactoryMethods.Bind (aMeshAlgoId, (OSD_Function )aFunc);
  }

  if (aFunc == NULL)
  {
    // function is not found - invalid plugin?
    myErrorStatus = BRepMesh_FE_FUNCTIONNOTFOUND;
    return Standard_False;
  }

  // try to create dummy tool
  BRepMesh_DiscretRoot* anInstancePtr = NULL;
  Standard_Integer anErr = aFunc (TopoDS_Shape(), 0.001, 0.1, anInstancePtr);
  if (anErr != 0 || anInstancePtr == NULL)
  {
    // can not create the algo specified  
    myErrorStatus = BRepMesh_FE_CANNOTCREATEALGO;
    delete anInstancePtr;
    return Standard_False;
  }
  delete anInstancePtr;

  // if all checks done - switch to this tool
  myPluginEntry  = aFunc;
  myDefaultName  = theName;
  myFunctionName = theFuncName;
  myNames.Add (theName);
  return Standard_True;
}

//=======================================================================
//function : Discret
//purpose  :
//=======================================================================
Handle(BRepMesh_DiscretRoot) BRepMesh_DiscretFactory::Discret(
  const TopoDS_Shape& theShape,
  const Standard_Real theDeflection,
  const Standard_Real theAngle)
{
  Handle(BRepMesh_DiscretRoot) aDiscretRoot;
  BRepMesh_DiscretRoot* anInstancePtr = NULL;
  if (myPluginEntry != NULL)
  {
    // use plugin
    Standard_Integer anErr = myPluginEntry (theShape, 
      theDeflection, theAngle, anInstancePtr);
      
    if (anErr != 0 || anInstancePtr == NULL)
    {
      // can not create the algo specified - should never happens here
      myErrorStatus = BRepMesh_FE_CANNOTCREATEALGO;
      return aDiscretRoot;
    }
  }
  else //if (myDefaultName == THE_FAST_DISCRET_MESH)
  {
    // use built-in
    BRepMesh_IncrementalMesh::Discret (theShape, 
      theDeflection, theAngle, anInstancePtr);
  }

  // cover with handle
  aDiscretRoot = anInstancePtr;

  // return the handle
  return aDiscretRoot;
}
