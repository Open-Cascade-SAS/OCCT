/***********************************************************************


************************************************************************/

// for the class
#include <V3d_ColorScaleLayerItem.ixx>

//
//-Constructors
//
V3d_ColorScaleLayerItem::V3d_ColorScaleLayerItem (
  const Handle(V3d_ColorScale)& aColorScale ):
Visual3d_LayerItem(),
MyColorScale(aColorScale)
{
}

void V3d_ColorScaleLayerItem::ComputeLayerPrs()
{
  Visual3d_LayerItem::ComputeLayerPrs();
}

void V3d_ColorScaleLayerItem::RedrawLayerPrs()
{
  Visual3d_LayerItem::RedrawLayerPrs();
  if ( !MyColorScale.IsNull() )
    MyColorScale->DrawScale();
}
