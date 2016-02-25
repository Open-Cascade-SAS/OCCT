#include "FThread.h"
#include "graphwidget.h"

#include <TFunction_Function.hxx>
#include <TFunction_IFunction.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_GraphNode.hxx>

#include <TDataStd_Tick.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

FThread::FThread(QObject* parent):QThread(parent),thread_index(0)
{

}

FThread::~FThread()
{

}

void FThread::setIterator(const TFunction_Iterator& itr)
{
    this->itr = itr;
}

void FThread::setLogbook(const Handle(TFunction_Logbook)& log)
{
    this->log = log;
}

void FThread::setGraph(GraphWidget* graph)
{
    this->graph = graph;
}

void FThread::setThreadIndex(const int thread_index)
{
    this->thread_index = thread_index;
}

// Returns any free (not executed yet) function
TDF_Label FThread::getFreeFunction()
{
    TDF_Label L;
    TDF_ListIteratorOfLabelList itrl(itr.Current());
    for (; itrl.More(); itrl.Next())
    {
        if (itr.GetStatus(itrl.Value()) == TFunction_ES_NotExecuted)
        {
            L = itrl.Value();
            itr.SetStatus(L, TFunction_ES_Executing);
            break;
        }
    }
    return L;
}

void FThread::run()
{
    while (itr.More())
    {
        // Take current function,
        // choose one and set its status to "executing".
        TDF_Label L;
        for (; itr.More(); itr.Next())
        {
            L = getFreeFunction();
            if (L.IsNull())
            #ifdef __GNUC__
               sleep(0.001);
            #else
               ::Sleep(100);
            #endif
            else
                break;
        }

        // Nothing to compute? Finish.
        if (L.IsNull())
        {
            graph->setFinished();
            return;
        }

        // Check a Tick attribute - a marker of skipped for execution functions.
        // It is used only for visual presentation of skipped (not modified) functions.
        Handle(TDataStd_Tick) tick;
        if (L.FindAttribute(TDataStd_Tick::GetID(), tick))
            L.ForgetAttribute(tick);

        // Execute the function
        Handle(TFunction_Driver) D = TFunction_IFunction(L).GetDriver(thread_index);
        const bool must = D->MustExecute(log);
        if (must)
        {
            // Usage of mutex for execution of Open CASCADE code is the most stupid thing!!!
            // But it makes the execution more reliable...
            const int ret = D->Execute(log);
            if (ret == 0)
            {
                // Successfuly executed!
                itr.SetStatus(L, TFunction_ES_Succeeded);

                TDF_LabelList res;
                D->Results(res);
                TDF_ListIteratorOfLabelList itrr(res);
                for (; itrr.More(); itrr.Next())
                {
                    log->SetImpacted(itrr.Value());
                }
            }
            else
            {
                // Failed...
                itr.SetStatus(L, TFunction_ES_Failed);
                graph->setFinished();
                return;
            }
        }
        else if (itr.GetStatus(L) == TFunction_ES_Executing)
        {
            itr.SetStatus(L, TFunction_ES_Succeeded);
            TDataStd_Tick::Set(L);
        }

    }// while (More())

    graph->setFinished();
}
