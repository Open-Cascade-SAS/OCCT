// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

/*============================================================================*/
/*==== Titre: Graphic3d_CTexture.hxx				              */
/*==== Role : The header file of primitive type "CTexture" from Graphic3d     */
/*====									      */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*==== created: 1/07/97 ; PCT : texture mapping                               */
/*============================================================================*/

#ifndef _Graphic3d_CTexture_HeaderFile
#define _Graphic3d_CTexture_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
typedef CALL_DEF_TEXTURE Graphic3d_CTexture;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/
#include <Standard_Type.hxx>
const Handle(Standard_Type)& TYPE(Graphic3d_CTexture);
/*============================================================================*/

#endif
#endif /*Graphic3d_CTexture_HeaderFile*/
