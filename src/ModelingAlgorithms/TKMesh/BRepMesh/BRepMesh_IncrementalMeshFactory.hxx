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

#ifndef _BRepMesh_IncrementalMeshFactory_HeaderFile
#define _BRepMesh_IncrementalMeshFactory_HeaderFile

#include <BRepMesh_DiscretAlgoFactory.hxx>

//! Factory for creating BRepMesh_IncrementalMesh instances.
//! This factory is registered under the name "FastDiscret" and provides
//! the default built-in meshing algorithm.
class BRepMesh_IncrementalMeshFactory : public BRepMesh_DiscretAlgoFactory
{
  DEFINE_STANDARD_RTTIEXT(BRepMesh_IncrementalMeshFactory, BRepMesh_DiscretAlgoFactory)

public:
  //! Constructor. Registers this factory under the name "FastDiscret".
  Standard_EXPORT BRepMesh_IncrementalMeshFactory();

  //! Creates a new BRepMesh_IncrementalMesh instance.
  //! @param[in] theShape         shape to be meshed
  //! @param[in] theLinDeflection linear deflection for meshing
  //! @param[in] theAngDeflection angular deflection for meshing
  //! @return new meshing algorithm instance
  Standard_EXPORT occ::handle<BRepMesh_DiscretRoot> CreateAlgorithm(
    const TopoDS_Shape& theShape,
    double              theLinDeflection,
    double              theAngDeflection) override;
};

#endif // _BRepMesh_IncrementalMeshFactory_HeaderFile
