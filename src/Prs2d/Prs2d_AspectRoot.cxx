#include <Prs2d_AspectRoot.ixx>
#include <Prs2d_AspectName.hxx>

Prs2d_AspectRoot::Prs2d_AspectRoot( const Prs2d_AspectName anAspectName ):
    myAspectName( anAspectName ) {}

Prs2d_AspectName Prs2d_AspectRoot::GetAspectName() const
{ return myAspectName; }

