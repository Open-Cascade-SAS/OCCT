//--------------------------------------------------------------------
//
//  File Name : IGESDraw_LabelDisplay.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDraw_LabelDisplay.ixx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <Interface_Macros.hxx>


    IGESDraw_LabelDisplay::IGESDraw_LabelDisplay ()    {  }

    void IGESDraw_LabelDisplay::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViews,
   const Handle(TColgp_HArray1OfXYZ)&              allTextLocations,
   const Handle(IGESDimen_HArray1OfLeaderArrow)&   allLeaderEntities,
   const Handle(TColStd_HArray1OfInteger)&         allLabelLevels,
   const Handle(IGESData_HArray1OfIGESEntity)&     allDisplayedEntities)
{
  Standard_Integer Ln  = allViews->Length();
  if ( allViews->Lower()          != 1 ||
      (allTextLocations->Lower()  != 1 || allTextLocations->Length()  != Ln) ||
      (allLeaderEntities->Lower() != 1 || allLeaderEntities->Length() != Ln) ||
      (allLabelLevels->Lower()    != 1 || allLabelLevels->Length()    != Ln) ||
      (allDisplayedEntities->Lower() != 1 || allDisplayedEntities->Length() != Ln))
    Standard_DimensionMismatch::Raise("IGESDraw_LabelDisplay : Init");

  theViews             = allViews; 
  theTextLocations     = allTextLocations; 
  theLeaderEntities    = allLeaderEntities; 
  theLabelLevels       = allLabelLevels; 
  theDisplayedEntities = allDisplayedEntities; 
  InitTypeAndForm(402,5);
}

    Standard_Integer IGESDraw_LabelDisplay::NbLabels () const
{
  return (theViews->Length());
}

    Handle(IGESData_ViewKindEntity) IGESDraw_LabelDisplay::ViewItem
  (const Standard_Integer ViewIndex) const
{
  return (theViews->Value(ViewIndex));
}

    gp_Pnt IGESDraw_LabelDisplay::TextLocation
  (const Standard_Integer ViewIndex) const
{
  return ( gp_Pnt (theTextLocations->Value(ViewIndex)) );
}

    Handle(IGESDimen_LeaderArrow) IGESDraw_LabelDisplay::LeaderEntity
  (const Standard_Integer ViewIndex) const
{
  return (theLeaderEntities->Value(ViewIndex));
}

    Standard_Integer IGESDraw_LabelDisplay::LabelLevel
  (const Standard_Integer ViewIndex) const
{
  return (theLabelLevels->Value(ViewIndex));
}

    Handle(IGESData_IGESEntity) IGESDraw_LabelDisplay::DisplayedEntity
  (const Standard_Integer EntityIndex) const
{
  return (theDisplayedEntities->Value(EntityIndex));
}

    gp_Pnt IGESDraw_LabelDisplay::TransformedTextLocation
  (const Standard_Integer ViewIndex) const
{
  gp_XYZ retXYZ;
  gp_XYZ tempXYZ = theTextLocations->Value(ViewIndex);

  Handle(IGESData_ViewKindEntity) tempView = theViews->Value(ViewIndex);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
    {
      DeclareAndCast(IGESDraw_View, thisView, tempView);
      retXYZ = thisView->ModelToView( tempXYZ );
    }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
    {
      DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
      retXYZ = thisView->ModelToView( tempXYZ );
    }
  return ( gp_Pnt(retXYZ) );
}
