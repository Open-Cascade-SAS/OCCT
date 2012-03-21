// Created on: 2007-04-25
// Created by: Natalia KOPNOVA
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#ifndef MeshVS_SELECTION_MODE_FLAGS_HXX
#define MeshVS_SELECTION_MODE_FLAGS_HXX

Standard_EXPORT typedef enum
{
  MeshVS_SMF_Mesh    = 0x0000,
  MeshVS_SMF_Node    = 0x0001,
  MeshVS_SMF_0D      = 0x0002,
  MeshVS_SMF_Link    = 0x0004,
  MeshVS_SMF_Face    = 0x0008,
  MeshVS_SMF_Volume  = 0x0010,

  MeshVS_SMF_Element = MeshVS_SMF_0D | MeshVS_SMF_Link | MeshVS_SMF_Face | MeshVS_SMF_Volume,
  MeshVS_SMF_All     = MeshVS_SMF_Element | MeshVS_SMF_Node,

  MeshVS_SMF_Group   = 0x0100


}   MeshVS_SelectionModeFlags;

#endif

