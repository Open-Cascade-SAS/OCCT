// Created on: 2003-09-29
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


#ifndef MeshVS_ENTITYTYPEHXX
#define MeshVS_ENTITYTYPEHXX

Standard_EXPORT typedef enum
{
  MeshVS_ET_NONE   = 0x00,
  MeshVS_ET_Node   = 0x01,
  MeshVS_ET_0D     = 0x02,
  MeshVS_ET_Link   = 0x04,
  MeshVS_ET_Face   = 0x08,
  MeshVS_ET_Volume = 0x10,

  MeshVS_ET_Element = MeshVS_ET_0D | MeshVS_ET_Link | MeshVS_ET_Face | MeshVS_ET_Volume,
  MeshVS_ET_All     = MeshVS_ET_Element | MeshVS_ET_Node


}   MeshVS_EntityType;

#endif

