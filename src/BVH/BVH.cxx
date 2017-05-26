// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <BVH_Geometry.hxx>
#include <BVH_QuickSorter.hxx>
#include <BVH_RadixSorter.hxx>
#include <BVH_Triangulation.hxx>
#include <BVH_DistanceField.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <BVH_SweepPlaneBuilder.hxx>
#include <BVH_SpatialMedianBuilder.hxx>

#include <Standard_Real.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BVH_BuilderTransient,  Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BVH_TreeBaseTransient, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BVH_ObjectTransient,   Standard_Transient)

// Specific instantiations of struct templates to avoid compilation warnings

template class NCollection_Vec2<Standard_Real>;
template class NCollection_Vec3<Standard_Real>;
template class NCollection_Vec4<Standard_Real>;

template class BVH_Box<Standard_Real, 2>;
template class BVH_Box<Standard_Real, 3>;
template class BVH_Box<Standard_Real, 4>;

template class BVH_Box<Standard_ShortReal, 2>;
template class BVH_Box<Standard_ShortReal, 3>;
template class BVH_Box<Standard_ShortReal, 4>;

template class BVH_Set<Standard_Real, 2>;
template class BVH_Set<Standard_Real, 3>;
template class BVH_Set<Standard_Real, 4>;

template class BVH_Set<Standard_ShortReal, 2>;
template class BVH_Set<Standard_ShortReal, 3>;
template class BVH_Set<Standard_ShortReal, 4>;

template class BVH_Object<Standard_Real, 2>;
template class BVH_Object<Standard_Real, 3>;
template class BVH_Object<Standard_Real, 4>;

template class BVH_Object<Standard_ShortReal, 2>;
template class BVH_Object<Standard_ShortReal, 3>;
template class BVH_Object<Standard_ShortReal, 4>;

template class BVH_ObjectSet<Standard_Real, 2>;
template class BVH_ObjectSet<Standard_Real, 3>;
template class BVH_ObjectSet<Standard_Real, 4>;

template class BVH_ObjectSet<Standard_ShortReal, 2>;
template class BVH_ObjectSet<Standard_ShortReal, 3>;
template class BVH_ObjectSet<Standard_ShortReal, 4>;

template class BVH_Geometry<Standard_Real, 2>;
template class BVH_Geometry<Standard_Real, 3>;
template class BVH_Geometry<Standard_Real, 4>;

template class BVH_Geometry<Standard_ShortReal, 2>;
template class BVH_Geometry<Standard_ShortReal, 3>;
template class BVH_Geometry<Standard_ShortReal, 4>;

template class BVH_Tree<Standard_Real, 2>;
template class BVH_Tree<Standard_Real, 3>;
template class BVH_Tree<Standard_Real, 4>;

template class BVH_Tree<Standard_ShortReal, 2>;
template class BVH_Tree<Standard_ShortReal, 3>;
template class BVH_Tree<Standard_ShortReal, 4>;

template class BVH_Builder<Standard_Real, 2>;
template class BVH_Builder<Standard_Real, 3>;
template class BVH_Builder<Standard_Real, 4>;

template class BVH_Builder<Standard_ShortReal, 2>;
template class BVH_Builder<Standard_ShortReal, 3>;
template class BVH_Builder<Standard_ShortReal, 4>;

template class BVH_BinnedBuilder<Standard_Real, 2>;
template class BVH_BinnedBuilder<Standard_Real, 3>;
template class BVH_BinnedBuilder<Standard_Real, 4>;

template class BVH_BinnedBuilder<Standard_ShortReal, 2>;
template class BVH_BinnedBuilder<Standard_ShortReal, 3>;
template class BVH_BinnedBuilder<Standard_ShortReal, 4>;

template class BVH_QuickSorter<Standard_Real, 3>;
template class BVH_QuickSorter<Standard_Real, 4>;

template class BVH_QuickSorter<Standard_ShortReal, 3>;
template class BVH_QuickSorter<Standard_ShortReal, 4>;

template class BVH_RadixSorter<Standard_Real, 3>;
template class BVH_RadixSorter<Standard_Real, 4>;

template class BVH_RadixSorter<Standard_ShortReal, 3>;
template class BVH_RadixSorter<Standard_ShortReal, 4>;

template class BVH_LinearBuilder<Standard_Real, 3>;
template class BVH_LinearBuilder<Standard_Real, 4>;

template class BVH_LinearBuilder<Standard_ShortReal, 3>;
template class BVH_LinearBuilder<Standard_ShortReal, 4>;

template class BVH_SweepPlaneBuilder<Standard_Real, 2>;
template class BVH_SweepPlaneBuilder<Standard_Real, 3>;
template class BVH_SweepPlaneBuilder<Standard_Real, 4>;

template class BVH_SweepPlaneBuilder<Standard_ShortReal, 2>;
template class BVH_SweepPlaneBuilder<Standard_ShortReal, 3>;
template class BVH_SweepPlaneBuilder<Standard_ShortReal, 4>;

template class BVH_SpatialMedianBuilder<Standard_Real, 2>;
template class BVH_SpatialMedianBuilder<Standard_Real, 3>;
template class BVH_SpatialMedianBuilder<Standard_Real, 4>;

template class BVH_SpatialMedianBuilder<Standard_ShortReal, 2>;
template class BVH_SpatialMedianBuilder<Standard_ShortReal, 3>;
template class BVH_SpatialMedianBuilder<Standard_ShortReal, 4>;

template class BVH_PrimitiveSet<Standard_Real, 2>;
template class BVH_PrimitiveSet<Standard_Real, 3>;
template class BVH_PrimitiveSet<Standard_Real, 4>;

template class BVH_PrimitiveSet<Standard_ShortReal, 2>;
template class BVH_PrimitiveSet<Standard_ShortReal, 3>;
template class BVH_PrimitiveSet<Standard_ShortReal, 4>;

template class BVH_Triangulation<Standard_Real, 2>;
template class BVH_Triangulation<Standard_Real, 3>;
template class BVH_Triangulation<Standard_Real, 4>;

template class BVH_Triangulation<Standard_ShortReal, 2>;
template class BVH_Triangulation<Standard_ShortReal, 3>;
template class BVH_Triangulation<Standard_ShortReal, 4>;

template class BVH_DistanceField<Standard_Real, 3>;
template class BVH_DistanceField<Standard_Real, 4>;

template class BVH_DistanceField<Standard_ShortReal, 3>;
template class BVH_DistanceField<Standard_ShortReal, 4>;

template class BVH_Transform<Standard_Real, 4>;
template class BVH_Transform<Standard_ShortReal, 4>;
