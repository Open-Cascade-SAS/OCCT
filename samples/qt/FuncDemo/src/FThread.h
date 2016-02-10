#ifndef _FTHREAD_H_
#define _FTHREAD_H_

#include <QThread>

#include <TFunction_Logbook.hxx>
#include <TFunction_Iterator.hxx>
#include <TFunction_Driver.hxx>

class GraphWidget; // shows graphically execution of functions

class FThread : public QThread
{

    Q_OBJECT

public:

    FThread(QObject* parent = 0);
    ~FThread();

    void setIterator(const TFunction_Iterator& ); // to iterate and call functions
    void setLogbook(const Handle(TFunction_Logbook)& );         // to set logbook with modifications
    void setGraph(GraphWidget* );                 // to change color of a graph circle
    void setThreadIndex(const int );              // to set the index of the thread

protected:

    void run();
    virtual TDF_Label getFreeFunction();          // Returns any free (not executed yet) function

private:

    TFunction_Iterator itr;
    Handle(TFunction_Logbook) log;
    int                thread_index;

    GraphWidget*       graph;
};

#endif // _FTHREAD_H_