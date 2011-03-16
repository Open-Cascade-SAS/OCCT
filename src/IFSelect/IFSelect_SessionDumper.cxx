#include <IFSelect_SessionDumper.ixx>
#include <IFSelect_BasicDumper.hxx>




static Handle(IFSelect_SessionDumper)  thefirst;
static int cefait = 0;
// On commence la serie avec celui-la


    IFSelect_SessionDumper::IFSelect_SessionDumper ()
{
  Handle(IFSelect_BasicDumper) bid;
  if (!cefait)
    { cefait = 1;  Handle(IFSelect_BasicDumper) bid = new IFSelect_BasicDumper; }
  else thenext  = thefirst;
  thefirst = this;    // as Handle
}

    Handle(IFSelect_SessionDumper)  IFSelect_SessionDumper::First ()
      {  return thefirst;  }

    Handle(IFSelect_SessionDumper)  IFSelect_SessionDumper::Next () const
      {  return thenext;   }
