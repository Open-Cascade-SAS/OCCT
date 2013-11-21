// Copyright (c) 1995-1999 Matra Datavision
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
#ifndef _AIS_DiameterDimension_HeaderFile
#define _AIS_DiameterDimension_HeaderFile

#include <AIS.hxx>
#include <AIS_Dimension.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(AIS_DiameterDimension,AIS_Dimension)

//! A framework to display diameter dimensions. <br>
//! A diameter is displayed with arrows and text. The <br>
//! text gives the length of the diameter. <br>
//! The algorithm takes a length along a face and <br>
//! analyzes it as an arc. It then reconstructs the circle <br>
//! corresponding to the arc and calculates the <br>
//! diameter of this circle. This diameter serves as a <br>
//! relational reference in 3d presentations of the surface. <br>
class AIS_DiameterDimension : public AIS_Dimension
{
public:
  //! Constructs a diameter display object defined by the <br>
  //! circle <theCircle>
  Standard_EXPORT  AIS_DiameterDimension(const gp_Circ& theCircle);
  //! Consctructor that allows to set a attach point <br>
  //! on the circle <theCircle> where to attach dimension
  Standard_EXPORT  AIS_DiameterDimension (const gp_Circ& theCircle,
                                          const gp_Pnt& theAttachPoint);

  Standard_EXPORT  AIS_DiameterDimension (const TopoDS_Shape& theShape);

  DEFINE_STANDARD_RTTI(AIS_DiameterDimension)

protected:

  Standard_EXPORT  virtual void computeValue();

    //! Fills default plane object if it is possible to count plane automatically.
  Standard_EXPORT  virtual void countDefaultPlane();

private: 

  virtual  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                         const Handle(Prs3d_Presentation)& thePresentation,
                         const Standard_Integer theMode = 0);

// Fields
private:

  gp_Circ myCircle;
};
#endif
