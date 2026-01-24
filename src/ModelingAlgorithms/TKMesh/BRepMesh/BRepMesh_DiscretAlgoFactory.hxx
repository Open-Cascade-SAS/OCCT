// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _BRepMesh_DiscretAlgoFactory_HeaderFile
#define _BRepMesh_DiscretAlgoFactory_HeaderFile

#include <NCollection_List.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

class BRepMesh_DiscretRoot;
class TopoDS_Shape;

//! Abstract factory for creating meshing algorithms.
//! This class provides a registry-based factory pattern that allows multiple
//! meshing algorithms to coexist without symbol collisions.
//! It follows the pattern established by Graphic3d_GraphicDriverFactory.
class BRepMesh_DiscretAlgoFactory : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(BRepMesh_DiscretAlgoFactory, Standard_Transient)

public:
  //! Registers a factory in the global registry.
  //! @param[in] theFactory     factory to register
  //! @param[in] theIsPreferred if TRUE, add to the beginning of the list (making it default),
  //!                           otherwise add to the end
  Standard_EXPORT static void RegisterFactory(
    const occ::handle<BRepMesh_DiscretAlgoFactory>& theFactory,
    bool                                            theIsPreferred = false);

  //! Unregisters a factory by name.
  //! @param[in] theName name of the factory to unregister
  Standard_EXPORT static void UnregisterFactory(const TCollection_AsciiString& theName);

  //! Returns the default (first registered) factory, or NULL if none registered.
  Standard_EXPORT static occ::handle<BRepMesh_DiscretAlgoFactory> DefaultFactory();

  //! Finds a factory by name.
  //! @param[in] theName name of the factory to find
  //! @return factory handle, or NULL if not found
  Standard_EXPORT static occ::handle<BRepMesh_DiscretAlgoFactory> FindFactory(
    const TCollection_AsciiString& theName);

  //! Returns the global list of registered factories.
  Standard_EXPORT static const NCollection_List<occ::handle<BRepMesh_DiscretAlgoFactory>>&
    Factories();

public:
  //! Creates a new meshing algorithm instance.
  //! @param[in] theShape         shape to be meshed
  //! @param[in] theLinDeflection linear deflection for meshing
  //! @param[in] theAngDeflection angular deflection for meshing
  //! @return new meshing algorithm instance
  virtual occ::handle<BRepMesh_DiscretRoot> CreateAlgorithm(const TopoDS_Shape& theShape,
                                                            double              theLinDeflection,
                                                            double theAngDeflection) = 0;

  //! Returns the factory name.
  const TCollection_AsciiString& Name() const { return myName; }

protected:
  //! Constructor.
  //! @param[in] theName factory name used for identification
  Standard_EXPORT BRepMesh_DiscretAlgoFactory(const TCollection_AsciiString& theName);

protected:
  TCollection_AsciiString myName;
};

#endif // _BRepMesh_DiscretAlgoFactory_HeaderFile
