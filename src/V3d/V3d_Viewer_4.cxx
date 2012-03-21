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
        Classe V3d_Viewer :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      16-07-98 : CAL ; S3892. Ajout grilles 3d.
      22-09-98 : CAL ; Erreur de compilation sur WNT dans V3d_Viewer_4.cxx.

************************************************************************/

#define IMP200100	//GG 
//			-> Add GridDrawMode() method.
//			-> Compute the case Aspect_TDM_None
//			   in ActivateGrid(..)

#define IMP240100	//GG
//			-> Add SetGridEcho() & GridEcho() methods
//			-> Add ShowGridEcho() private method.

#define OCC281          //SAV added new field to store grid echo aspect

/*----------------------------------------------------------------------*/
/*
 * Includes
 */
#include <V3d_Viewer.jxx>

/*----------------------------------------------------------------------*/

Handle(Aspect_Grid) V3d_Viewer::Grid () const {

	if (myGridType == Aspect_GT_Circular)
		return myCGrid;	
	else if (myGridType == Aspect_GT_Rectangular)
		     return myRGrid;
	     else
		     return myRGrid;
}

Aspect_GridType V3d_Viewer::GridType () const {

	return myGridType;
}

#ifdef IMP200100
Aspect_GridDrawMode V3d_Viewer::GridDrawMode () const {

	return Grid()->DrawMode();
}
#endif

void V3d_Viewer::ActivateGrid (const Aspect_GridType aType, const Aspect_GridDrawMode aMode) {

	Grid ()->Erase ();
	myGridType = aType;
	Grid ()->SetDrawMode (aMode);
#ifdef IMP200100
	if( aMode != Aspect_GDM_None ) Grid ()->Display ();
#else
	Grid ()->Display ();
#endif
	Grid ()->Activate ();
	for (InitActiveViews (); MoreActiveViews (); NextActiveViews ()) {
		ActiveView ()->SetGrid (myPrivilegedPlane, Grid ());
#ifndef IMP200100 	//Do nothing more than the previous Grid()->Activate()
		ActiveView ()->SetGridActivity (Standard_True);
#endif
	}
	Update ();
}

void V3d_Viewer::DeactivateGrid () {

	Grid ()->Erase ();
	myGridType = Aspect_GT_Rectangular;
	Grid ()->Deactivate ();
	for (InitActiveViews (); MoreActiveViews (); NextActiveViews ()) {
		ActiveView ()->SetGridActivity (Standard_False);
	}
	Update ();
}

Standard_Boolean V3d_Viewer::IsActive () const {

	return Grid ()->IsActive ();
}

void V3d_Viewer::RectangularGridValues (Quantity_Length& theXOrigin, Quantity_Length& theYOrigin, Quantity_Length& theXStep, Quantity_Length& theYStep, Quantity_PlaneAngle& theRotationAngle) const {

	theXOrigin = myRGrid->XOrigin ();
	theYOrigin = myRGrid->YOrigin ();
	theXStep = myRGrid->XStep ();
	theYStep = myRGrid->YStep ();
	theRotationAngle = myRGrid->RotationAngle ();
}

void V3d_Viewer::SetRectangularGridValues (const Quantity_Length theXOrigin, const Quantity_Length theYOrigin, const Quantity_Length theXStep, const Quantity_Length theYStep, const Quantity_PlaneAngle theRotationAngle) {

	myRGrid->SetGridValues
		(theXOrigin, theYOrigin, theXStep, theYStep, theRotationAngle);
	for (InitActiveViews (); MoreActiveViews (); NextActiveViews ())
		ActiveView ()->SetGrid (myPrivilegedPlane, myRGrid);
	Update ();
}

void V3d_Viewer::CircularGridValues (Quantity_Length& theXOrigin, Quantity_Length& theYOrigin, Quantity_Length& theRadiusStep, Standard_Integer& theDivisionNumber, Quantity_PlaneAngle& theRotationAngle) const {

	theXOrigin = myCGrid->XOrigin ();
	theYOrigin = myCGrid->YOrigin ();
	theRadiusStep = myCGrid->RadiusStep ();
	theDivisionNumber = myCGrid->DivisionNumber ();
	theRotationAngle = myCGrid->RotationAngle ();
}

void V3d_Viewer::SetCircularGridValues (const Quantity_Length theXOrigin, const Quantity_Length theYOrigin, const Quantity_Length theRadiusStep, const Standard_Integer theDivisionNumber, const Quantity_PlaneAngle theRotationAngle) {

	myCGrid->SetGridValues
		(theXOrigin, theYOrigin, theRadiusStep,
		 theDivisionNumber, theRotationAngle);
	for (InitActiveViews (); MoreActiveViews (); NextActiveViews ())
		ActiveView ()->SetGrid (myPrivilegedPlane, myCGrid);
	Update ();
}

void V3d_Viewer::RectangularGridGraphicValues (Quantity_Length& theXSize, Quantity_Length& theYSize, Quantity_Length& theOffSet) const {

	myRGrid->GraphicValues (theXSize, theYSize, theOffSet);
}

void V3d_Viewer::SetRectangularGridGraphicValues (const Quantity_Length theXSize, const Quantity_Length theYSize, const Quantity_Length theOffSet) {

	myRGrid->SetGraphicValues (theXSize, theYSize, theOffSet);
	for (InitActiveViews (); MoreActiveViews (); NextActiveViews ())
		ActiveView ()->SetGridGraphicValues (myRGrid);
	Update ();
}

void V3d_Viewer::CircularGridGraphicValues (Quantity_Length& theRadius, Quantity_Length& theOffSet) const {

	myCGrid->GraphicValues (theRadius, theOffSet);
}

void V3d_Viewer::SetCircularGridGraphicValues (const Quantity_Length theRadius, const Quantity_Length theOffSet) {

	myCGrid->SetGraphicValues (theRadius, theOffSet);
	for (InitActiveViews (); MoreActiveViews (); NextActiveViews ())
		ActiveView ()->SetGridGraphicValues (myCGrid);
	Update ();
}

#ifdef IMP240100
void V3d_Viewer::SetGridEcho( const Standard_Boolean showGrid ) {

  myGridEcho = showGrid; 
}

void V3d_Viewer::SetGridEcho( const Handle(Graphic3d_AspectMarker3d)& aMarker ) {
    if( myGridEchoStructure.IsNull() ) {
      myGridEchoStructure = new Graphic3d_Structure(Viewer());
      myGridEchoGroup = new Graphic3d_Group (myGridEchoStructure);
    }
#ifdef OCC281
    myGridEchoAspect = aMarker;
#endif
    myGridEchoGroup->SetPrimitivesAspect (aMarker);
}

Standard_Boolean V3d_Viewer::GridEcho() const {

  return myGridEcho;
}

#include <Visual3d_TransientManager.hxx>
void V3d_Viewer::ShowGridEcho( const Handle(V3d_View)& aView, 
					const Graphic3d_Vertex& aVertex ) {
  if( myGridEcho ) {
    if( myGridEchoStructure.IsNull() ) {
      myGridEchoStructure = new Graphic3d_Structure(Viewer());
      myGridEchoGroup = new Graphic3d_Group (myGridEchoStructure);
#ifdef OCC281
      myGridEchoAspect = new Graphic3d_AspectMarker3d( Aspect_TOM_STAR, 
                                                       Quantity_Color( Quantity_NOC_GRAY90 ),
                                                       3.0 );
      myGridEchoGroup->SetPrimitivesAspect( myGridEchoAspect );
#else
      Handle(Graphic3d_AspectMarker3d) markerAttrib =
	new Graphic3d_AspectMarker3d(Aspect_TOM_STAR,
		Quantity_Color(Quantity_NOC_GRAY90),3.0);
      myGridEchoGroup->SetPrimitivesAspect (markerAttrib);
#endif
    }

    static Graphic3d_Vertex lastVertex;
    if( Graphic3d_Vertex::Distance(aVertex,lastVertex) != 0.0 ) {
      lastVertex = aVertex;
      myGridEchoGroup->Clear();
#ifdef OCC281
      myGridEchoGroup->SetPrimitivesAspect( myGridEchoAspect );
#endif
      myGridEchoGroup->Marker(aVertex);
      Visual3d_TransientManager::BeginDraw(
		aView->View(), Standard_False, Standard_False);
      Visual3d_TransientManager::DrawStructure (myGridEchoStructure);
      Visual3d_TransientManager::EndDraw (Standard_True);
    }
  }
}
#endif
