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

#ifndef _Prs3d_DatumAttribute_HeaderFile
#define _Prs3d_DatumAttribute_HeaderFile

//! Enumeration defining a part of datum aspect, see Prs3d_Datum.
enum Prs3d_DatumAttribute
{
  Prs3d_DA_XAxisLength = 0,
  Prs3d_DA_YAxisLength,
  Prs3d_DA_ZAxisLength,
  Prs3d_DP_ShadingTubeRadiusPercent,
  Prs3d_DP_ShadingConeRadiusPercent,
  Prs3d_DP_ShadingConeLengthPercent,
  Prs3d_DP_ShadingOriginRadiusPercent,
  Prs3d_DP_ShadingNumberOfFacettes
};

#endif // _Prs3d_DatumAttribute_HeaderFile
