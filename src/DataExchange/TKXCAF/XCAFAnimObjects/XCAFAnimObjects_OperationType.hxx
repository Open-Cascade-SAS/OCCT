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

#ifndef _XCAFAnimObjects_OperationType_HeaderFile
#define _XCAFAnimObjects_OperationType_HeaderFile

//! Enumeration of supported animation operation types.
//! Each type represents a specific transformation that can be applied to a shape.
enum XCAFAnimObjects_OperationType
{
  XCAFAnimObjects_OperationType_Custom = 0, //!< User-defined custom transformation operation
  XCAFAnimObjects_OperationType_Orient,     //!< Orientation change (quaternion-based)
  XCAFAnimObjects_OperationType_Rotate,     //!< Rotation around an axis
  XCAFAnimObjects_OperationType_Scale,      //!< Uniform or non-uniform scaling
  XCAFAnimObjects_OperationType_Skew,       //!< Skew/shear transformation
  XCAFAnimObjects_OperationType_Transform,  //!< General transformation matrix
  XCAFAnimObjects_OperationType_Translate   //!< Translation along axes
};

#endif // _XCAFAnimObjects_OperationType_HeaderFile
