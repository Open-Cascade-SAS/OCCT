// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_GlueEnum_HeaderFile
#define _BOPAlgo_GlueEnum_HeaderFile

//! The Enumeration describes an additional option for the algorithms
//! in the Boolean Component such as General Fuse, Boolean operations,
//! Section, Maker Volume, Splitter and Cells Builder algorithms.<br>
//!
//! The Gluing options have been designed to speed up the computation
//! of the interference among arguments of the operations on special cases,
//! in which the arguments may be overlapping but do not have real intersections
//! between their sub-shapes.<br>
//!
//! This option cannot be used on the shapes having real intersections,
//! like intersection vertex between edges, or intersection vertex between
//! edge and a face or intersection line between faces.<br>
//!
//! There are two possibilities of overlapping shapes:<br>
//! 1. The shapes can be partially coinciding - the faces do not have
//!    intersection curves, but overlapping. The faces of such arguments will
//!    be split during the operation;<br>
//! 2. The shapes can be fully coinciding - there should be no partial
//!    overlapping of the faces, thus no intersection of type EDGE/FACE at all.
//!    In such cases the faces will not be split during the operation.<br>
//!
//! Even though there are no real intersections on such cases without Gluing options the algorithm
//! will still intersect the sub-shapes of the arguments with interfering bounding boxes.<br>
//!
//! The performance improvement in gluing mode is achieved by excluding
//! the most time consuming computations according to the given Gluing parameter:<br>
//! 1. Computation of FACE/FACE intersections for partial coincidence;<br>
//! 2. And computation of VERTEX/FACE, EDGE/FACE and FACE/FACE intersections for full coincidence.<br>
//! 
//! By setting the Gluing option for the operation user should guarantee
//! that the arguments are really coinciding. The algorithms do not check this itself.
//! Setting inappropriate option for the operation is likely to lead to incorrect result.<br>
//!
//! There are following items in the enumeration:<br>
//! **BOPAlgo_GlueOff** - default value for the algorithms, Gluing is switched off;<br>
//! **BOPAlgo_GlueShift** - Glue option for shapes with partial coincidence;<br>
//! **BOPAlgo_GlueFull** - Glue option for shapes with full coincidence.
//!
enum BOPAlgo_GlueEnum
{
  BOPAlgo_GlueOff,
  BOPAlgo_GlueShift,
  BOPAlgo_GlueFull
};

#endif // _BOPAlgo_GlueEnum_HeaderFile
