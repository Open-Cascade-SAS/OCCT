#include <Prs3d_Root.ixx>

Handle (Graphic3d_Group) Prs3d_Root::CurrentGroup (const Handle (Prs3d_Presentation)& Prs3d) 
{
  return Prs3d->CurrentGroup();
}
Handle (Graphic3d_Group) Prs3d_Root::NewGroup (const Handle (Prs3d_Presentation)& Prs3d) 
{
  return Prs3d->NewGroup();
}
