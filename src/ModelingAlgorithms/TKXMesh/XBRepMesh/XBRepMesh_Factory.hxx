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

#ifndef _XBRepMesh_Factory_HeaderFile
#define _XBRepMesh_Factory_HeaderFile

#include <BRepMesh_DiscretAlgoFactory.hxx>

//! Factory for creating XBRepMesh meshing algorithm instances.
//! This factory is registered under the name "XBRepMesh" and provides
//! an alternative meshing algorithm based on BRepMesh_IncrementalMesh.
class XBRepMesh_Factory : public BRepMesh_DiscretAlgoFactory
{
  DEFINE_STANDARD_RTTIEXT(XBRepMesh_Factory, BRepMesh_DiscretAlgoFactory)

public:
  //! Constructor. Registers this factory under the name "XBRepMesh".
  Standard_EXPORT XBRepMesh_Factory();

  //! Creates a new meshing algorithm instance.
  //! @param[in] theShape         shape to be meshed
  //! @param[in] theLinDeflection linear deflection for meshing
  //! @param[in] theAngDeflection angular deflection for meshing
  //! @return new meshing algorithm instance
  Standard_EXPORT Handle(BRepMesh_DiscretRoot) CreateAlgorithm(const TopoDS_Shape& theShape,
                                                               double              theLinDeflection,
                                                               double theAngDeflection) override;
};

#endif // _XBRepMesh_Factory_HeaderFile
