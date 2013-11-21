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

#ifndef OpenGl_Flipper_Header
#define OpenGl_Flipper_Header

#include <OpenGl_Element.hxx>
#include <OpenGl_Vec.hxx>
#include <Handle_OpenGl_Workspace.hxx>

class gp_Ax2;

//! Being rendered, the elements modifies current model-view matrix such that the axes of
//! the specified reference system (in model space) become oriented in the following way:
//! - X    - heads to the right side of view.
//! - Y    - heads to the up side of view.
//! - N(Z) - heads towards the screen.
//! Originally, this element serves for need of flipping the 3D text of dimension presentations.
class OpenGl_Flipper : public OpenGl_Element
{
public:

  //! Construct rendering element to flip model-view matrix
  //! along the reference system to ensure up-Y, right-X orientation.
  //! @param theReferenceSystem [in] the reference coordinate system.
  Standard_EXPORT OpenGl_Flipper (const gp_Ax2& theReferenceSystem);

  //! Set options for the element.
  //! @param theIsEnabled [in] flag indicates whether the flipper
  //! matrix modification should be set up or restored back.
  void SetOptions (const Standard_Boolean theIsEnabled) { myIsEnabled = theIsEnabled; }

  Standard_EXPORT virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (const Handle(OpenGl_Context)&  theCtx);

public:

  DEFINE_STANDARD_ALLOC

protected:

  OpenGl_Vec4      myReferenceOrigin;
  OpenGl_Vec4      myReferenceX;
  OpenGl_Vec4      myReferenceY;
  OpenGl_Vec4      myReferenceZ;
  Standard_Boolean myIsEnabled;

};

#endif // OpenGl_Flipper_Header
