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

#include <BVH_DistanceField.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_BinnedBuilder.hxx>
#include <BVH_SweepPlaneBuilder.hxx>
#include <BVH_SpatialMedianBuilder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BVH_BuilderTransient, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BVH_TreeBaseTransient, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(BVH_ObjectTransient, Standard_Transient)

// Specific instantiations of struct templates to avoid compilation warnings

template class NCollection_Vec2<double>;
template class NCollection_Vec3<double>;
template class NCollection_Vec4<double>;
template class NCollection_Mat3<double>;
template class NCollection_Mat4<double>;

template class BVH_Box<double, 2>;
template class BVH_Box<double, 3>;
template class BVH_Box<double, 4>;

template class BVH_Box<float, 2>;
template class BVH_Box<float, 3>;
template class BVH_Box<float, 4>;

template class BVH_Set<double, 2>;
template class BVH_Set<double, 3>;
template class BVH_Set<double, 4>;

template class BVH_Set<float, 2>;
template class BVH_Set<float, 3>;
template class BVH_Set<float, 4>;

template class BVH_Object<double, 2>;
template class BVH_Object<double, 3>;
template class BVH_Object<double, 4>;

template class BVH_Object<float, 2>;
template class BVH_Object<float, 3>;
template class BVH_Object<float, 4>;

template class BVH_ObjectSet<double, 2>;
template class BVH_ObjectSet<double, 3>;
template class BVH_ObjectSet<double, 4>;

template class BVH_ObjectSet<float, 2>;
template class BVH_ObjectSet<float, 3>;
template class BVH_ObjectSet<float, 4>;

template class BVH_Geometry<double, 2>;
template class BVH_Geometry<double, 3>;
template class BVH_Geometry<double, 4>;

template class BVH_Geometry<float, 2>;
template class BVH_Geometry<float, 3>;
template class BVH_Geometry<float, 4>;

template class BVH_Tree<double, 2>;
template class BVH_Tree<double, 3>;
template class BVH_Tree<double, 4>;

template class BVH_Tree<float, 2>;
template class BVH_Tree<float, 3>;
template class BVH_Tree<float, 4>;

template class BVH_Builder<double, 2>;
template class BVH_Builder<double, 3>;
template class BVH_Builder<double, 4>;

template class BVH_Builder<float, 2>;
template class BVH_Builder<float, 3>;
template class BVH_Builder<float, 4>;

template class BVH_BinnedBuilder<double, 2>;
template class BVH_BinnedBuilder<double, 3>;
template class BVH_BinnedBuilder<double, 4>;

template class BVH_BinnedBuilder<float, 2>;
template class BVH_BinnedBuilder<float, 3>;
template class BVH_BinnedBuilder<float, 4>;

template class BVH_QuickSorter<double, 3>;
template class BVH_QuickSorter<double, 4>;

template class BVH_QuickSorter<float, 3>;
template class BVH_QuickSorter<float, 4>;

template class BVH_RadixSorter<double, 2>;
template class BVH_RadixSorter<double, 3>;
template class BVH_RadixSorter<double, 4>;

template class BVH_RadixSorter<float, 2>;
template class BVH_RadixSorter<float, 3>;
template class BVH_RadixSorter<float, 4>;

template class BVH_LinearBuilder<double, 2>;
template class BVH_LinearBuilder<double, 3>;
template class BVH_LinearBuilder<double, 4>;

template class BVH_LinearBuilder<float, 2>;
template class BVH_LinearBuilder<float, 3>;
template class BVH_LinearBuilder<float, 4>;

template class BVH_SweepPlaneBuilder<double, 2>;
template class BVH_SweepPlaneBuilder<double, 3>;
template class BVH_SweepPlaneBuilder<double, 4>;

template class BVH_SweepPlaneBuilder<float, 2>;
template class BVH_SweepPlaneBuilder<float, 3>;
template class BVH_SweepPlaneBuilder<float, 4>;

template class BVH_SpatialMedianBuilder<double, 2>;
template class BVH_SpatialMedianBuilder<double, 3>;
template class BVH_SpatialMedianBuilder<double, 4>;

template class BVH_SpatialMedianBuilder<float, 2>;
template class BVH_SpatialMedianBuilder<float, 3>;
template class BVH_SpatialMedianBuilder<float, 4>;

template class BVH_PrimitiveSet<double, 2>;
template class BVH_PrimitiveSet<double, 3>;
template class BVH_PrimitiveSet<double, 4>;

template class BVH_PrimitiveSet<float, 2>;
template class BVH_PrimitiveSet<float, 3>;
template class BVH_PrimitiveSet<float, 4>;

template class BVH_Triangulation<double, 2>;
template class BVH_Triangulation<double, 3>;
template class BVH_Triangulation<double, 4>;

template class BVH_Triangulation<float, 2>;
template class BVH_Triangulation<float, 3>;
template class BVH_Triangulation<float, 4>;

template class BVH_DistanceField<double, 3>;
template class BVH_DistanceField<double, 4>;

template class BVH_DistanceField<float, 3>;
template class BVH_DistanceField<float, 4>;

template class BVH_Transform<double, 4>;
template class BVH_Transform<float, 4>;
