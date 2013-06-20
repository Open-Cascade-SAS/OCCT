// Copyright (c) 2013 OPEN CASCADE SAS
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
#ifndef _DsgPrs_DatumPrs_H__
#define _DsgPrs_DatumPrs_H__

#include <gp_Ax2.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>

class DsgPrs_DatumPrs : public Prs3d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void Add
          (const Handle(Prs3d_Presentation)& thePresentation,
           const gp_Ax2&                     theDatum,
           const Handle(Prs3d_Drawer)&       theDrawer);

};
#endif
