#include <windows.h>  
#include <iostream>  
#include <atltime.h>  
#include <atlstr.h>  


// CTime和DWORD类型的相互转换

DWORD CTime2DWORD(CTime time)
{
    return time.GetTime();
}

CTime DWORD2Ctime(DWORD dwTime)
{
    CTime time(dwTime);   // DWORD->CTime
    CString strTime;
    strTime.Format(L"%d-%d-%d %d:%d:%d", time.GetYear(), time.GetMonth(), 
        time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond()); //CTime的格式化显示

    return time;
}

CString DWORD2CString(DWORD dwTime)
{
    CTime time(dwTime);   // DWORD->CTime
    CString strTime;
    strTime.Format(L"%d-%d-%d %d:%d:%d", time.GetYear(), time.GetMonth(), 
        time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond()); //CTime的格式化显示

    return strTime;
}


int main() {

    DWORD time = CTime2DWORD(CTime::GetCurrentTime());
    CTime ctime = DWORD2Ctime(time);
    CString strtime = DWORD2CString(time);


    // 获取当前系统时间  
    SYSTEMTIME st;
    GetSystemTime(&st);

    // 将系统时间转换为格林尼治标准时间  
    TIME_ZONE_INFORMATION tzi;
    GetTimeZoneInformation(&tzi);

    // 输出格林尼治标准时间  
    std::cout << "GMT Time: " << (tzi.Bias * -60) + st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;

    return 0;
}