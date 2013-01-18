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

/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_Viewer_3.cxx :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      27-12-98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)

     REMARQUES :
     -----------
 
************************************************************************/

#define IMP240300	//GG 
//			-> Remove the grid plane axis when it is requested.
//			-> Redraw the privilegied grid plane after any change

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_Viewer.jxx>

#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>

#include <Graphic3d_ArrayOfSegments.hxx>

/*----------------------------------------------------------------------*/
/*
 * Static variable
 */

#define LOPTIM
#ifndef LOPTIM
static TCollection_AsciiString XLetter("X");
static TCollection_AsciiString YLetter("Y");
static TCollection_AsciiString ZLetter("Z");
#else 
static TCollection_AsciiString _XLetter() {
    static TCollection_AsciiString XLetter("X");
return XLetter;
}
#define XLetter _XLetter()

static TCollection_AsciiString _YLetter() {
    static TCollection_AsciiString YLetter("Y");
return YLetter;
}
#define YLetter _YLetter()

static TCollection_AsciiString _ZLetter() {
    static TCollection_AsciiString ZLetter("Z");
return ZLetter;
}
#define ZLetter _ZLetter()

#endif // LOPTIM

/*----------------------------------------------------------------------*/

void V3d_Viewer::SetPrivilegedPlane(const gp_Ax3& aPlane)
{
  myPrivilegedPlane = aPlane;
  Grid()->SetDrawMode(Grid()->DrawMode());
  for (InitActiveViews (); MoreActiveViews (); NextActiveViews ())
    ActiveView ()->SetGrid (myPrivilegedPlane, Grid ());
  if(myDisplayPlane)
    DisplayPrivilegedPlane(Standard_True,myDisplayPlaneLength);
  else
    Update();
}

/*----------------------------------------------------------------------*/

gp_Ax3 V3d_Viewer::PrivilegedPlane() const
{
  return myPrivilegedPlane;
}

/*----------------------------------------------------------------------*/
void V3d_Viewer::DisplayPrivilegedPlane(const Standard_Boolean OnOff, const Quantity_Length aSize)
{
  Standard_Boolean Change = (myDisplayPlane != OnOff);
  myDisplayPlane = OnOff;
  myDisplayPlaneLength = aSize;

  if(myDisplayPlane)
  {
    if(myPlaneStructure.IsNull()) {
      myPlaneStructure = new Graphic3d_Structure(MyViewer);
      myPlaneStructure->SetInfiniteState(Standard_True);
      myPlaneStructure->Display();
    }
    else
      myPlaneStructure->Clear();

    Handle(Graphic3d_Group) Group = new Graphic3d_Group(myPlaneStructure);

    Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d() ;
    LineAttrib->SetColor(Quantity_Color(Quantity_NOC_GRAY60));
    Group->SetPrimitivesAspect(LineAttrib) ;

    Handle(Graphic3d_AspectText3d) TextAttrib = new Graphic3d_AspectText3d();
    TextAttrib->SetColor(Quantity_Color(Quantity_NOC_ROYALBLUE1));
    Group->SetPrimitivesAspect(TextAttrib);

    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(6);

    const gp_Pnt &p0 = myPrivilegedPlane.Location();

    const gp_Pnt pX(p0.XYZ() + myDisplayPlaneLength*myPrivilegedPlane.XDirection().XYZ());
    aPrims->AddVertex(p0);
    aPrims->AddVertex(pX);
    Group->Text(XLetter.ToCString(),Graphic3d_Vertex(pX.X(),pX.Y(),pX.Z()),1./81.);

    const gp_Pnt pY(p0.XYZ() + myDisplayPlaneLength*myPrivilegedPlane.YDirection().XYZ());
    aPrims->AddVertex(p0);
    aPrims->AddVertex(pY);
    Group->Text(YLetter.ToCString(),Graphic3d_Vertex(pY.X(),pY.Y(),pY.Z()),1./81.);
    
    const gp_Pnt pZ(p0.XYZ() + myDisplayPlaneLength*myPrivilegedPlane.Direction().XYZ());
    aPrims->AddVertex(p0);
    aPrims->AddVertex(pZ);
    Group->Text(ZLetter.ToCString(),Graphic3d_Vertex(pZ.X(),pZ.Y(),pZ.Z()),1./81.);

    Group->AddPrimitiveArray(aPrims);

	myPlaneStructure->Display();
  }
  else
  {
    if( !myPlaneStructure.IsNull() )  myPlaneStructure->Erase();
  }
  if(Change) Update();
}

/*----------------------------------------------------------------------*/
