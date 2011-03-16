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

#include <Graphic3d_Array1OfVertex.hxx>

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

void V3d_Viewer::SetPrivilegedPlane(const gp_Ax3& aPlane) {
  myPrivilegedPlane = aPlane;
#ifdef IMP240300
  Grid()->SetDrawMode(Grid()->DrawMode());
  for (InitActiveViews (); MoreActiveViews (); NextActiveViews ()) {
    ActiveView ()->SetGrid (myPrivilegedPlane, Grid ());
  }
#endif
  if(myDisplayPlane) {
    Standard_Real s = myDisplayPlaneLength;
    DisplayPrivilegedPlane(Standard_True,s);
#ifdef IMP240300	
  } else {
    Update();
#else
    Update();
#endif
  }
}

/*----------------------------------------------------------------------*/
gp_Ax3  V3d_Viewer::PrivilegedPlane() const {
  return myPrivilegedPlane;
    
}

/*----------------------------------------------------------------------*/
void V3d_Viewer::DisplayPrivilegedPlane(const Standard_Boolean OnOff, const Quantity_Length aSize) {
  Standard_Boolean Change =  myDisplayPlane != OnOff;
  myDisplayPlane = OnOff;
  myDisplayPlaneLength = aSize;

  if(myDisplayPlane) {
    if(myPlaneStructure.IsNull()) {
      myPlaneStructure = new Graphic3d_Structure(MyViewer);
      myPlaneStructure->SetInfiniteState(Standard_True);
      myPlaneStructure->Display();
    }
    else
      myPlaneStructure->Clear();
    
//    Handle(Graphic3d_Structure) thePlaneStructure = new Graphic3d_Structure(MyViewer);
    Handle(Graphic3d_Group) Group = new Graphic3d_Group(myPlaneStructure) ;

    Handle(Graphic3d_AspectLine3d) LineAttrib = new Graphic3d_AspectLine3d() ;
    LineAttrib->SetColor(Quantity_Color(Quantity_NOC_GRAY60));
    Group->SetPrimitivesAspect(LineAttrib) ;

    Handle(Graphic3d_AspectText3d) TextAttrib = new Graphic3d_AspectText3d();
    TextAttrib->SetColor(Quantity_Color(Quantity_NOC_ROYALBLUE1));
    Group->SetPrimitivesAspect(TextAttrib);
    
    Graphic3d_Array1OfVertex Points(0,1) ;
    Standard_Real xl,yl,zl;
    myPrivilegedPlane.Location().Coord(xl,yl,zl);
    Points(0).SetCoord(xl,yl,zl);

    Standard_Real ay,by,cy;

    myPrivilegedPlane.XDirection().Coord(ay,by,cy);
    Points(1).SetCoord(xl+myDisplayPlaneLength*ay,
		       yl+myDisplayPlaneLength*by,
		       zl+myDisplayPlaneLength*cy);
    Group->Polyline(Points);
    Group->Text(XLetter.ToCString(),Points(1),1./81.);

    myPrivilegedPlane.YDirection().Coord(ay,by,cy);
    Points(1).SetCoord(xl+myDisplayPlaneLength*ay,
		       yl+myDisplayPlaneLength*by,
		       zl+myDisplayPlaneLength*cy);
    Group->Polyline(Points);
    Group->Text(YLetter.ToCString(),Points(1),1./81.);
    
    myPrivilegedPlane.Direction().Coord(ay,by,cy);
    Points(1).SetCoord(xl+myDisplayPlaneLength*ay,
		       yl+myDisplayPlaneLength*by,
		       zl+myDisplayPlaneLength*cy);
    Group->Polyline(Points);
    Group->Text(ZLetter.ToCString(),Points(1),1./81.);
#ifdef IMP240300
    myPlaneStructure->Display();
  } else {
    if( !myPlaneStructure.IsNull() )  myPlaneStructure->Erase();
#endif
  }
  if(Change) Update();
}

/*----------------------------------------------------------------------*/
