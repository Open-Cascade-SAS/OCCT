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

#ifndef _XCAFAnimObjects_InterpolationType_HeaderFile
#define _XCAFAnimObjects_InterpolationType_HeaderFile

//! Interpolation is a description of behavior between timestamps
enum XCAFAnimObjects_InterpolationType
{
  XCAFAnimObjects_InterpolationType_Custom = 0, //!< User-defined interpolation
  XCAFAnimObjects_InterpolationType_Step, //!< No interpolation, just apply timestamps step by step
  XCAFAnimObjects_InterpolationType_Linear,      //!< Linear dependency from t_(i) to t_(i+1)
  XCAFAnimObjects_InterpolationType_CubicSpline, //!< Cubic dependency from t_(i) to t_(i+1)
  XCAFAnimObjects_InterpolationType_Static //!< No animation in this time period, use the closest
                                           //!< static value
};

#endif // _XCAFAnimObjects_InterpolationType_HeaderFile
