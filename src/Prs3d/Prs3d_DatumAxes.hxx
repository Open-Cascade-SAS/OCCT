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

#ifndef _Prs3d_DatumAxes_HeaderFile
#define _Prs3d_DatumAxes_HeaderFile

//! Enumeration defining axes used in datum aspect, see Prs3d_Datum.
enum Prs3d_DatumAxes
{
  Prs3d_DatumAxes_XAxis  = 0x01,                                          //!< X axis of the datum
  Prs3d_DatumAxes_YAxis  = 0x02,                                          //!< Y axis of the datum
  Prs3d_DatumAxes_ZAxis  = 0x04,                                          //!< Z axis of the datum
  Prs3d_DatumAxes_XYAxes = Prs3d_DatumAxes_XAxis | Prs3d_DatumAxes_YAxis, //!< XOY 2D axes
  Prs3d_DatumAxes_YZAxes = Prs3d_DatumAxes_YAxis | Prs3d_DatumAxes_ZAxis, //!< YOZ 2D axes
  Prs3d_DatumAxes_XZAxes = Prs3d_DatumAxes_XAxis | Prs3d_DatumAxes_ZAxis, //!< XOZ 2D axes
  Prs3d_DatumAxes_XYZAxes =
    Prs3d_DatumAxes_XAxis | Prs3d_DatumAxes_YAxis | Prs3d_DatumAxes_ZAxis, //!< XYZ 3D axes

  // old aliases
  Prs3d_DA_XAxis   = Prs3d_DatumAxes_XAxis,
  Prs3d_DA_YAxis   = Prs3d_DatumAxes_YAxis,
  Prs3d_DA_ZAxis   = Prs3d_DatumAxes_ZAxis,
  Prs3d_DA_XYAxis  = Prs3d_DatumAxes_XYAxes,
  Prs3d_DA_YZAxis  = Prs3d_DatumAxes_YZAxes,
  Prs3d_DA_XZAxis  = Prs3d_DatumAxes_XZAxes,
  Prs3d_DA_XYZAxis = Prs3d_DatumAxes_XYZAxes
};

#endif // _Prs3d_DatumParts_HeaderFile
