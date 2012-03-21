// Created on: 2003-09-09
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef MeshVS_DISPLAY_MODE_FLAGS_HXX
#define MeshVS_DISPLAY_MODE_FLAGS_HXX

Standard_EXPORT typedef Standard_Integer MeshVS_DisplayModeFlags;

enum
{
  MeshVS_DMF_WireFrame             = 0x0001,
  MeshVS_DMF_Shading               = 0x0002,
  MeshVS_DMF_Shrink                = 0x0003,
  MeshVS_DMF_OCCMask               = ( MeshVS_DMF_WireFrame | MeshVS_DMF_Shading | MeshVS_DMF_Shrink ),
  // the mask which helps pick out CasCade display mode

  MeshVS_DMF_VectorDataPrs         = 0x0004,
  MeshVS_DMF_NodalColorDataPrs     = 0x0008,
  MeshVS_DMF_ElementalColorDataPrs = 0x0010,
  MeshVS_DMF_TextDataPrs           = 0x0020,
  MeshVS_DMF_EntitiesWithData      = 0x0040,
  MeshVS_DMF_DeformedPrsWireFrame  = 0x0080,
  MeshVS_DMF_DeformedPrsShading    = 0x0100,
  MeshVS_DMF_DeformedPrsShrink     = 0x0180,
  MeshVS_DMF_DeformedMask          = ( MeshVS_DMF_DeformedPrsWireFrame | MeshVS_DMF_DeformedPrsShading | MeshVS_DMF_DeformedPrsShrink ),
  MeshVS_DMF_SelectionPrs          = 0x0200,
  MeshVS_DMF_HilightPrs            = 0x0400,
  MeshVS_DMF_User                  = 0x0800
};


#endif
