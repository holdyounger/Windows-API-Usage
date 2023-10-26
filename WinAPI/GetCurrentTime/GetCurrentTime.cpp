#include <windows.h>  
#include <iostream>  
#include <atltime.h>  
#include <atlstr.h>  


// CTime��DWORD���͵��໥ת��

DWORD CTime2DWORD(CTime time)
{
    return time.GetTime();
}

CTime DWORD2Ctime(DWORD dwTime)
{
    CTime time(dwTime);   // DWORD->CTime
    CString strTime;
    strTime.Format(L"%d-%d-%d %d:%d:%d", time.GetYear(), time.GetMonth(), 
        time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond()); //CTime�ĸ�ʽ����ʾ

    return time;
}

CString DWORD2CString(DWORD dwTime)
{
    CTime time(dwTime);   // DWORD->CTime
    CString strTime;
    strTime.Format(L"%d-%d-%d %d:%d:%d", time.GetYear(), time.GetMonth(), 
        time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond()); //CTime�ĸ�ʽ����ʾ

    return strTime;
}


int main() {

    DWORD time = CTime2DWORD(CTime::GetCurrentTime());
    CTime ctime = DWORD2Ctime(time);
    CString strtime = DWORD2CString(time);


    // ��ȡ��ǰϵͳʱ��  
    SYSTEMTIME st;
    GetSystemTime(&st);

    // ��ϵͳʱ��ת��Ϊ�������α�׼ʱ��  
    TIME_ZONE_INFORMATION tzi;
    GetTimeZoneInformation(&tzi);

    // ����������α�׼ʱ��  
    std::cout << "GMT Time: " << (tzi.Bias * -60) + st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;

    return 0;
}