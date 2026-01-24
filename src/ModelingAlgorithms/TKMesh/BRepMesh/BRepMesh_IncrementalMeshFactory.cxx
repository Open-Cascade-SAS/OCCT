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

#include <BRepMesh_IncrementalMeshFactory.hxx>

#include <BRepMesh_IncrementalMesh.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_IncrementalMeshFactory, BRepMesh_DiscretAlgoFactory)

namespace
{
//! Self-registering factory instance
static occ::handle<BRepMesh_IncrementalMeshFactory> THE_FACTORY_INSTANCE;

//! Initialization helper that registers the factory at startup
struct FactoryInitializer
{
  FactoryInitializer()
  {
    if (THE_FACTORY_INSTANCE.IsNull())
    {
      THE_FACTORY_INSTANCE = new BRepMesh_IncrementalMeshFactory();
    }
  }
};

static FactoryInitializer THE_FACTORY_INIT;
} // namespace

//==================================================================================================

BRepMesh_IncrementalMeshFactory::BRepMesh_IncrementalMeshFactory()
    : BRepMesh_DiscretAlgoFactory("FastDiscret")
{
  RegisterFactory(this, true);
}

//==================================================================================================

occ::handle<BRepMesh_DiscretRoot> BRepMesh_IncrementalMeshFactory::CreateAlgorithm(
  const TopoDS_Shape& theShape,
  double              theLinDeflection,
  double              theAngDeflection)
{
  occ::handle<BRepMesh_IncrementalMesh> anAlgo = new BRepMesh_IncrementalMesh();
  anAlgo->ChangeParameters().Deflection        = theLinDeflection;
  anAlgo->ChangeParameters().Angle             = theAngDeflection;
  anAlgo->ChangeParameters().InParallel        = BRepMesh_IncrementalMesh::IsParallelDefault();
  anAlgo->SetShape(theShape);
  return anAlgo;
}
