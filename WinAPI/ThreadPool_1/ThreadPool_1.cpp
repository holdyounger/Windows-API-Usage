// ThreadPool_1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <vld.h>  

#include "ThreadPools.h"

#define DebugStr(flag, product) printf(">>> Check <<< " #flag " [" product "]\n");

#define CheckFlagStart(X) DebugStr(Start, X); \
			DWORD dwStart_##X##_Time = GetTickCount64();
#define CheckFlagEnd(X) DebugStr(End, X); \
					DWORD dwend_##X##_Time = GetTickCount64(); \
					printf(">>> Check " X " Spent %dms <<<\n\n", (dwend_##X##_Time)-(dwStart_##X##_Time) );

#define APP_NAME_Thread "ThreadPool"
#define APP_NAME_Single "Single"
#define APP_NAME_Class  "Class"
#define APP_NAME_Work  "Work"

// 工作回调函数  
void CALLBACK WorkCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work) 
{
    Sleep(50);
    // 执行你的任务代码  
    std::cout << "Task executed by thread ID: " << GetCurrentThreadId() << std::endl;
    // 注意：不需要手动释放Work，因为线程池会管理它  
}

int Test_Normal()
{
    // 初始化线程池（通常这一步在程序开始时已经完成）  
   // 在Windows Vista及以后版本中，线程池是自动初始化的 

    CheckFlagStart(APP_NAME_Thread);

    // 创建工作对象  
    PTP_WORK work = CreateThreadpoolWork(WorkCallback, NULL, NULL);
    if (work == NULL) {
        // 处理错误  
        return 1;
    }

    // 提交工作到线程池（多次提交同一个工作对象也是安全的）  
    for (int i = 0; i < 50; i++)
        SubmitThreadpoolWork(work);

    // 但在这个例子中，我们不关心执行顺序，所以不需要等待  
    WaitForThreadpoolWorkCallbacks(work, FALSE);

    // 当不再需要工作对象时，关闭它  
    CloseThreadpoolWork(work);

    // 等待所有线程池中的工作完成并清理（这通常是在程序结束时进行的）  
    // 在这个例子中，我们简单地返回，因为不关心所有工作是否完成  

    CheckFlagEnd(APP_NAME_Thread);

    CheckFlagStart(APP_NAME_Single);
    for (int i = 0; i < 50; i++)
        WorkCallback(NULL, NULL, NULL);
    CheckFlagEnd(APP_NAME_Single);
}

bfptpool::CCheckThreadPools* g_threadPool = NULL;

void jenkins_thread_pool_init()
{
    g_threadPool = create_jenkins_check_thread_pool();
}

void jenkins_thread_pool_end()
{
    if (g_threadPool)
    {
        delete g_threadPool;
        g_threadPool = NULL;
    }
}


jenkinsCheck::JenkinsCheckData g_data;


void UpdateAllTaskState(std::vector<jenkinsCheck::CJenkinsThreadDT*>& tasks)
{
    do
    {
        std::vector<jenkinsCheck::CJenkinsThreadDT*>::iterator iter = tasks.begin();
        for (; iter != tasks.end();)
        {
            if ((*iter)->m_bDone == TRUE)
            {
                if (FALSE == (*iter)->m_strPwd.IsEmpty())
                {
                    std::cout << (*iter)->m_strPwd << " " << (*iter)->m_data.m_strHash << std::endl;
                }

                jenkinsCheck::CJenkinsThreadDT* temp = (*iter);
                iter = tasks.erase(iter);
                iter = tasks.begin();
                if (temp)
                    delete temp;
                continue;
            }

            iter++;
        }

    } while (tasks.empty() == FALSE);
}

void test_Class_WorkThreadPool()
{
    CheckFlagStart(APP_NAME_Work);

    // jenkins_thread_pool_init();

    std::set<CString> users{
        "Admin",
        "Admin1",
        "Admin2",
    };

    std::vector<bfptpool::CCheckThreadPools*> checkTPools;
    std::vector<jenkinsCheck::CJenkinsThreadDT*> tasks;

    for (auto it : users)
    {
        bfptpool::CCheckThreadPools* checkPool = create_jenkins_check_thread_pool();
        jenkinsCheck::CJenkinsThreadDT* ctask = new jenkinsCheck::CJenkinsThreadDT(g_data);

        checkTPools.emplace_back(checkPool);
        tasks.emplace_back(ctask);
        checkPool->CreateWorkThread(ctask);
    }

    UpdateAllTaskState(tasks);

    for (auto pool : checkTPools)
    {
        delete pool;
    }


    // Sleep(1000);

    // jenkins_thread_pool_end();

    CheckFlagEnd(APP_NAME_Work);
}

void test_Class_Submit()
{
    CheckFlagStart(APP_NAME_Class);

    jenkins_thread_pool_init();

    std::shared_ptr<jenkinsCheck::CJenkinsThreadDT> ctask(new jenkinsCheck::CJenkinsThreadDT(g_data, g_threadPool));
    std::shared_ptr<bfptpool::CCheckThreadPools> checkThread(g_threadPool);

    for (int i = 0; i < 50; i++)
    {
        checkThread->SubmitTask(ctask.get());
    }

    // jenkins_thread_pool_end();

    CheckFlagEnd(APP_NAME_Class);
}

void dataPrepare()
{
    g_data.m_strHash = "$2a$10$rhBIOBC6HRcOGLPdHDrBxuiORQN89.xbYX4xbRLBa4ryuWt1KLhsa";


    g_data.m_pwdInnerDicts = std::set<CString>{
        "1",
        "2",
        "3",
        "Admin@2020",
        "Admin@2022",
        "4",
        "5",
        "Admin@2023",
        "Admin@2024",
    };

    g_data.m_pwdUserDicts = std::set<CString>{
        "1",
        "2",
        "3",
        "Admin@2022",
        "4",
        "5",
        "Admin@2023",
        "Admin@2024",
    };

}

int main() 
{
    dataPrepare();

    // test_Class_Submit();
    
    test_Class_WorkThreadPool();
    
    
    // Test_Normal();

    return 0;
}
