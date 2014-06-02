// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_DiscretFactory_HeaderFile
#define _BRepMesh_DiscretFactory_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepMesh_PluginEntryType.hxx>
#include <BRepMesh_FactoryError.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <Plugin_MapOfFunctions.hxx>
#include <BRepMesh_DiscretRoot.hxx>

class TopoDS_Shape;

//! This class intended to setup / retrieve default triangulation algorithm. <br>
//! Use BRepMesh_DiscretFactory::Get() static method to retrieve global Factory instance. <br>
//! Use BRepMesh_DiscretFactory::Discret() method to retrieve meshing tool. <br>
class BRepMesh_DiscretFactory
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns the global factory instance.
  Standard_EXPORT static BRepMesh_DiscretFactory& Get();
  
  //! Returns the list of registered meshing algorithms.
  inline const TColStd_MapOfAsciiString& Names() const
  {
    return myNames;
  }
  
  //! Setup meshing algorithm by name. <br>
  //! Returns TRUE if requested tool is available. <br>
  //! On fail Factory will continue to use previous algo.
  Standard_EXPORT Standard_Boolean SetDefaultName(const TCollection_AsciiString& theName)
  {
    return SetDefault(theName, myFunctionName);
  }
  
  //! Returns name for current meshing algorithm.
  inline const TCollection_AsciiString& DefaultName() const
  {
    return myDefaultName;
  }
  
  //! Advanced function. Changes function name to retrieve from plugin. <br>
  //! Returns TRUE if requested tool is available. <br>
  //! On fail Factory will continue to use previous algo.
  Standard_EXPORT Standard_Boolean SetFunctionName(const TCollection_AsciiString& theFuncName)
  {
    return SetDefault(myDefaultName, theFuncName);
  }
  
  //! Returns function name that should be exported by plugin.
  inline const TCollection_AsciiString& FunctionName() const
  {
    return myFunctionName;
  }
  
  //! Returns error status for last meshing algorithm switch.
  inline BRepMesh_FactoryError ErrorStatus() const
  {
    return myErrorStatus;
  }

  //! Setup meshing algorithm that should be created by this Factory. <br>
  //! Returns TRUE if requested tool is available. <br>
  //! On fail Factory will continue to use previous algo. <br>
  //! Call ::ErrorStatus() method to retrieve fault reason.
  Standard_EXPORT Standard_Boolean SetDefault(const TCollection_AsciiString& theName,
                                              const TCollection_AsciiString& theFuncName = "DISCRETALGO");

  //! Returns triangulation algorithm instance.
  //! @param theShape shape to be meshed.
  //! @param theLinDeflection linear deflection to be used for meshing.
  //! @param theAngDeflection angular deflection to be used for meshing.
  Standard_EXPORT Handle(BRepMesh_DiscretRoot) Discret(const TopoDS_Shape& theShape,
                                                       const Standard_Real theLinDeflection,
                                                       const Standard_Real theAngDeflection);

protected:
  
  //! Constructor
  Standard_EXPORT BRepMesh_DiscretFactory();

  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_DiscretFactory();

  //! Clears factory data.
  Standard_EXPORT void clear();

  BRepMesh_PluginEntryType  myPluginEntry;
  BRepMesh_FactoryError     myErrorStatus;
  TColStd_MapOfAsciiString  myNames;
  TCollection_AsciiString   myDefaultName;
  TCollection_AsciiString   myFunctionName;
  Plugin_MapOfFunctions     myFactoryMethods;
};

#endif
