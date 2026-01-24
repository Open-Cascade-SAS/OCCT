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
#include <BRepMesh_DiscretRoot.hxx>
#include <TCollection_AsciiString.hxx>

class TopoDS_Shape;

//! Factory for retrieving triangulation algorithms.
//! Use BRepMesh_DiscretFactory::Get() static method to retrieve global Factory instance.
//! Use BRepMesh_DiscretFactory::Discret() method to retrieve meshing tool.
//!
//! This class delegates to BRepMesh_DiscretAlgoFactory registry for algorithm creation.
//! @sa BRepMesh_DiscretAlgoFactory
class BRepMesh_DiscretFactory
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the global factory instance.
  Standard_EXPORT static BRepMesh_DiscretFactory& Get();

  //! Setup meshing algorithm by name.
  //! Returns TRUE if requested algorithm is available.
  //! On fail Factory will continue to use previous algorithm.
  //! @param[in] theName name of the algorithm to use
  Standard_EXPORT bool SetDefaultName(const TCollection_AsciiString& theName);

  //! Returns name of current meshing algorithm.
  const TCollection_AsciiString& DefaultName() const { return myDefaultName; }

  //! Returns triangulation algorithm instance.
  //! @param[in] theShape         shape to be meshed
  //! @param[in] theLinDeflection linear deflection to be used for meshing
  //! @param[in] theAngDeflection angular deflection to be used for meshing
  //! @return new meshing algorithm instance, or NULL if no algorithm available
  Standard_EXPORT Handle(BRepMesh_DiscretRoot) Discret(const TopoDS_Shape& theShape,
                                                       double              theLinDeflection,
                                                       double              theAngDeflection);

protected:
  //! Constructor
  BRepMesh_DiscretFactory();

  TCollection_AsciiString myDefaultName;
};

#endif
