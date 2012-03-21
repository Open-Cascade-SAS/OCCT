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

//Updates:
//	GG 24/03/98	Add useMFT parameter to SetFontMap method.
//	GG 07/07/98	BUC60258 Add SetMarkMap() method

#define PRO10988	//DCB Resets grid color indices after colormap change

#define IMP080300	//GG 
//			-> Optimize the SetColorMap() and InitializeColor() methods
//			Review the 14/09/01 to avoid regression
//			See new deferred method in Aspect_ColorMap

#include <V2d_Viewer.ixx>
#include <V2d_View.hxx>
#include <V2d_DefaultMap.hxx>
#include <Graphic2d_View.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_WindowDriver.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <Aspect_ColorMapEntry.hxx>

/*=================================================================*/
V2d_Viewer::V2d_Viewer(const Handle(Aspect_GraphicDevice)& aGraphicDevice,
		       const Standard_ExtString aName,
		       const Standard_CString aDomain) 
:Viewer_Viewer(aGraphicDevice,aName,aDomain,1),
myColorMap(V2d_DefaultMap::ColorMap()),
myTypeMap(V2d_DefaultMap::TypeMap()),
myWidthMap(V2d_DefaultMap::WidthMap()),
myFontMap(V2d_DefaultMap::FontMap()),
myMarkMap(V2d_DefaultMap::MarkMap()),
myGraphicView(new Graphic2d_View()),
myViews(),
myViewsIterator(),
myRGrid(),
myCGrid()
{
  Init();
}

/*=================================================================*/
V2d_Viewer::V2d_Viewer(const Handle(Aspect_GraphicDevice)& aGraphicDevice,
		       const Handle(Graphic2d_View)& aView,
		       const Standard_ExtString aName,
		       const Standard_CString aDomain) 
:Viewer_Viewer(aGraphicDevice,aName,aDomain,1),
myColorMap(V2d_DefaultMap::ColorMap()),
myTypeMap(V2d_DefaultMap::TypeMap()),
myWidthMap(V2d_DefaultMap::WidthMap()),
myFontMap(V2d_DefaultMap::FontMap()),
myMarkMap(V2d_DefaultMap::MarkMap()),
myGraphicView(aView),
myViews(),
myViewsIterator(),
myRGrid(),
myCGrid()
{
  Init();
}

/*=================================================================*/
void V2d_Viewer::Init() {
  myHitPointMarkerIndex = 1;
  myHitPointColorIndex = 0;
  Standard_Integer i1 = InitializeColor(Quantity_NOC_GRAY50);
  Standard_Integer i2 = InitializeColor(Quantity_NOC_GRAY70);
  myHitPointColorIndex = InitializeColor(Quantity_NOC_WHITE);
  myCoordinatesColorIndex = myHitPointColorIndex ;
  myGridType = Aspect_GT_Rectangular; 

  myUseMFT = Standard_True;
#ifdef PRO10988
  if( myRGrid.IsNull() ) myRGrid = new V2d_RectangularGrid(this,i1,i2);
  else myRGrid -> SetColorIndices (i1, i2);
  if( myCGrid.IsNull() ) myCGrid = new V2d_CircularGrid(this,i1,i2);
  else myCGrid -> SetColorIndices (i1, i2);
#else
  myRGrid = new V2d_RectangularGrid(this,i1,i2);
  myCGrid = new V2d_CircularGrid(this,i1,i2);
#endif
}


/*=================================================================*/
void V2d_Viewer::AddView(const Handle(V2d_View)& aView) {
  myViews.Append(aView);
  IncrCount();
}

/*=================================================================*/
void V2d_Viewer::RemoveView(const Handle(V2d_View)& aView) {

  myViewsIterator.Initialize(myViews);
  while(myViewsIterator.More())
    if(aView == myViewsIterator.Value()) {
      myViews.Remove(myViewsIterator);}
  else
    myViewsIterator.Next();
}

/*=================================================================*/
void V2d_Viewer::SetColorMap(const Handle(Aspect_ColorMap)& aColorMap) {
  myColorMap = aColorMap;
#ifdef PRO10988
  // Need to call to redefine myColorIndex1 and myColorIndex2,
  // which may not be available in new colormap.
#ifdef IMP080300
  if( myColorMap != aColorMap ) Init ();
#else
  Init();
#endif
  // We need to pass myColorMap in the views because
  // it could be changed in Init() method by InitializeColor().
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->Driver()->SetColorMap(myColorMap);
  }
#else
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->Driver()->SetColorMap(aColorMap);
  }
#endif
}

/*=================================================================*/
Handle(Aspect_ColorMap) V2d_Viewer::ColorMap() const {
  return myColorMap;
}

/*=================================================================*/
void V2d_Viewer::SetTypeMap(const Handle(Aspect_TypeMap)& aTypeMap) {
  myTypeMap = aTypeMap;
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->Driver()->SetTypeMap(aTypeMap);
  }
}

/*=================================================================*/
Handle(Aspect_TypeMap) V2d_Viewer::TypeMap() const {
  return myTypeMap;
}

/*=================================================================*/
void V2d_Viewer::SetWidthMap(const Handle(Aspect_WidthMap)& aWidthMap) {
  myWidthMap = aWidthMap;
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->Driver()->SetWidthMap(aWidthMap);
  }
}

/*=================================================================*/
Handle(Aspect_WidthMap) V2d_Viewer::WidthMap() const {
  return myWidthMap;
}

/*=================================================================*/
void V2d_Viewer::SetFontMap(const Handle(Aspect_FontMap)& aFontMap,
			    const Standard_Boolean useMFT) {
  myFontMap = aFontMap;
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->Driver()->SetFontMap(aFontMap,useMFT);
  }
}

/*=================================================================*/
void V2d_Viewer::SetMarkMap(const Handle(Aspect_MarkMap)& aMarkMap) {
  myMarkMap = aMarkMap;
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->Driver()->SetMarkMap(aMarkMap);
  }
}

/*=================================================================*/
Handle(Aspect_FontMap) V2d_Viewer::FontMap() const {
  return myFontMap;
}

/*=================================================================*/
Handle(Aspect_MarkMap) V2d_Viewer::MarkMap() const {
  return myMarkMap;
}

/*=================================================================*/
Handle(Graphic2d_View) V2d_Viewer::View () const {
  return myGraphicView;
}

/*=================================================================*/
Standard_Boolean V2d_Viewer::UseMFT() const {
  return myUseMFT;
}

/*=================================================================*/
void V2d_Viewer::Update() {
  for (InitActiveViews();MoreActiveViews();NextActiveViews()){
    ActiveView()->Update();
  }
}

/*=================================================================*/
void V2d_Viewer::UpdateNew() {
  for (InitActiveViews();MoreActiveViews();NextActiveViews()){
    ActiveView()->UpdateNew();
  }
}

/*=================================================================*/
Standard_Integer V2d_Viewer::InitializeColor (const Quantity_NameOfColor aColor) {
  Standard_Integer Size = myColorMap->Size();
  Quantity_Color color(aColor);
#ifdef IMP080300
  Standard_Integer index = myColorMap->AddEntry(color);
  if( Size != myColorMap->Size() ) {
    for (InitActiveViews();MoreActiveViews();NextActiveViews()){
      ActiveView()->Driver()->SetColorMap(myColorMap);
    }
  }
#else	// ??? why this gazworks ???
  Standard_Integer i,index=0;
  for ( i=1; i<= Size; i++) {
   if(myColorMap->Entry(i).Color().IsEqual(color)) 
	index=myColorMap->Entry(i).Index();
  }
  if (index == 0) {
    Handle(Aspect_GenericColorMap) map = new Aspect_GenericColorMap;
    for (i=1; i<= Size; i++) {
      map->AddEntry(myColorMap->Entry(i));
    }
// finding a free index.
    TColStd_MapOfInteger M;
    for ( i=1; i<= Size; i++) {
      M.Add(myColorMap->Entry(i).Index());
    }
#ifndef PRO10988
    index = Size + 1;
#endif
    i=1;
    while (index == 0) {
      if(!M.Contains(i)) index =i;
      i++;
    }
    map->AddEntry(Aspect_ColorMapEntry(index,color));
    for (InitActiveViews();MoreActiveViews();NextActiveViews()){
      ActiveView()->Driver()->SetColorMap(map);
    }
    myColorMap = map;
  }
#endif
  return index;
}

/*=================================================================*/
void V2d_Viewer::InitActiveViews() {
myViewsIterator.Initialize(myViews);
}

/*=================================================================*/
Standard_Boolean V2d_Viewer::MoreActiveViews () const {
  return myViewsIterator.More();
}

/*=================================================================*/
void V2d_Viewer::NextActiveViews () {
  myViewsIterator.Next();
}

/*=================================================================*/
Handle(V2d_View) V2d_Viewer::ActiveView() const {
  return (Handle(V2d_View)&)(myViewsIterator.Value());
}

/*=================================================================*/
Handle(Aspect_Grid) V2d_Viewer::Grid () const {
Handle(Aspect_Grid) grid;

  switch (myGridType) {
  case Aspect_GT_Circular:
    grid = myCGrid;
    break;
  case Aspect_GT_Rectangular:
  default:
    grid = myRGrid;
  }
  return grid;
}

/*=================================================================*/
void V2d_Viewer::ActivateGrid(const Aspect_GridType aType,
			      const Aspect_GridDrawMode aMode){
  
  Grid()->Erase();
  myGridType = aType;
  Grid()->SetDrawMode(aMode);
  Grid()->Display();
  Grid()->Activate();
  Update();
}

/*=================================================================*/
void V2d_Viewer::DeactivateGrid() {
  Grid()->Erase();
  Grid()->Deactivate();
  Update();
}

/*=================================================================*/
Standard_Boolean V2d_Viewer::IsActive () const {
  return Grid()->IsActive();
}

/*=================================================================*/
void V2d_Viewer::RectangularGridValues 
(Quantity_Length& theXOrigin,
 Quantity_Length& theYOrigin,
 Quantity_Length& theXStep,
 Quantity_Length& theYStep,
 Quantity_PlaneAngle& theRotationAngle) const {
   
   theXOrigin        = myRGrid->XOrigin();
   theYOrigin        = myRGrid->YOrigin();
   theXStep          = myRGrid->XStep();
   theYStep          = myRGrid->YStep();
   theRotationAngle  = myRGrid->RotationAngle();
}

/*=================================================================*/
void V2d_Viewer::SetRectangularGridValues
(const Quantity_Length theXOrigin,
 const Quantity_Length theYOrigin,
 const Quantity_Length theXStep,
 const Quantity_Length theYStep,
 const Quantity_PlaneAngle theRotationAngle) {
   
   myRGrid->SetXOrigin(theXOrigin);
   myRGrid->SetYOrigin(theYOrigin);
   myRGrid->SetXStep(theXStep);
   myRGrid->SetYStep(theYStep);
   myRGrid->SetRotationAngle(theRotationAngle);
   Update();
}

/*=================================================================*/
void V2d_Viewer::CircularGridValues 
(Quantity_Length& theXOrigin,
 Quantity_Length& theYOrigin,
 Quantity_Length& theRadiusStep,
 Standard_Integer& theDivisionNumber,
 Quantity_PlaneAngle& theRotationAngle) const {
   
   theXOrigin        = myCGrid->XOrigin();
   theYOrigin        = myCGrid->YOrigin();
   theRadiusStep    = myCGrid->RadiusStep();
   theDivisionNumber = myCGrid->DivisionNumber();
   theRotationAngle  = myCGrid->RotationAngle();
}

/*=================================================================*/
void V2d_Viewer::SetCircularGridValues
(const Quantity_Length theXOrigin,
 const Quantity_Length theYOrigin,
 const Quantity_Length theRadiusStep,
 const Standard_Integer theDivisionNumber,
 const Quantity_PlaneAngle theRotationAngle) {
   
   myCGrid->SetXOrigin(theXOrigin);
   myCGrid->SetYOrigin(theYOrigin);
   myCGrid->SetRadiusStep(theRadiusStep);
   myCGrid->SetDivisionNumber(theDivisionNumber);
   myCGrid->SetRotationAngle(theRotationAngle);
   Update();
}

/*=================================================================*/
Standard_Integer V2d_Viewer::HitPointColor() const {
  return myHitPointColorIndex;
}

/*=================================================================*/
Standard_Integer V2d_Viewer::CoordinatesColor() const {
  return myCoordinatesColorIndex;
}

/*=================================================================*/
Standard_Integer V2d_Viewer::HitPointMarkerIndex() const {
  return myHitPointMarkerIndex;
}

/*=================================================================*/
void V2d_Viewer::Hit(const Quantity_Length rx,
		   const Quantity_Length ry,
		   Quantity_Length& gx,
		   Quantity_Length& gy) const {

  Grid()->Hit(rx,ry,gx,gy);
}

/*=================================================================*/
Aspect_GridType V2d_Viewer::GridType() const {
  return myGridType;
}

/*=================================================================*/
Standard_Boolean V2d_Viewer::IsEmpty() const {
  
  return myViews.IsEmpty();
}

//SAV
void V2d_Viewer::SetGridColor( const Quantity_Color& color1,
			       const Quantity_Color& color2 )
{
  Standard_Integer i1 = InitializeColor( color1.Name() );
  Standard_Integer i2 = InitializeColor( color2.Name() );

  Standard_Boolean restart = IsActive();
  if ( restart ) {
    DeactivateGrid();
    Grid()->Erase();
  }

  Quantity_Length xOrigin, yOrigin, xStep, yStep;
  Quantity_PlaneAngle angle;
  Standard_Integer theDivisionNumber;
  Aspect_GridDrawMode gMode = myRGrid->DrawMode();

  RectangularGridValues( xOrigin, yOrigin, xStep, yStep, angle );
  myRGrid.Nullify();
  if ( myRGrid.IsNull() ) {
    myRGrid = new V2d_RectangularGrid( this, i1, i2 );
    myRGrid->SetDrawMode( gMode );
    SetRectangularGridValues( xOrigin, yOrigin, xStep, yStep, angle );
  }

  gMode = myCGrid->DrawMode();
  CircularGridValues( xOrigin, yOrigin, xStep, theDivisionNumber, angle );
  myCGrid.Nullify();
  if ( myCGrid.IsNull() ) {
    myCGrid = new V2d_CircularGrid( this, i1, i2 );
    myCGrid->SetDrawMode( gMode );
    SetCircularGridValues( xOrigin, yOrigin, xStep, theDivisionNumber, angle );
  }
  if ( restart ) {
    Grid()->Display();
    Grid()->Activate();
    Update();
  }    
}
