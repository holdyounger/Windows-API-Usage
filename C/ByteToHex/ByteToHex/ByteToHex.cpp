#include <iostream>

using namespace std;

// 把字节码转为十六进制码，一个字节两个十六进制，内部为字符串分配空间
char* ByteToHex(const unsigned char* vByte, const int vLen)
{
    if (!vByte)
        return NULL;

    char* tmp = new char[vLen * 2 + 1]; // 一个字节两个十六进制码，最后要多一个'/0'

    int tmp2;
    for (int i = 0; i < vLen; i++)
    {
        tmp2 = (int)(vByte[i]) / 16;
        tmp[i * 2] = (char)(tmp2 + ((tmp2 > 9) ? 'A' - 10 : '0'));
        tmp2 = (int)(vByte[i]) % 16;
        tmp[i * 2 + 1] = (char)(tmp2 + ((tmp2 > 9) ? 'A' - 10 : '0'));
    }

    tmp[vLen * 2 + 1] = { 0 };

    return tmp;
}

// 把十六进制字符串，转为字节码，每两个十六进制字符作为一个字节
unsigned char* HexToByte(const char* szHex)
{
    if (!szHex)
        return NULL;

    int iLen = strlen(szHex);

    if (iLen <= 0 || 0 != iLen % 2)
        return NULL;

    unsigned char* pbBuf = new unsigned char[iLen / 2];  // 数据缓冲区

    int tmp1, tmp2;
    int i = 0;
    for (; i < iLen / 2; i++)
    {
        tmp1 = (int)szHex[i * 2] - (((int)szHex[i * 2] >= 'A') ? 'A' - 10 : '0');

        if (tmp1 >= 16)
            return NULL;

        tmp2 = (int)szHex[i * 2 + 1] - (((int)szHex[i * 2 + 1] >= 'A') ? 'A' - 10 : '0');

        if (tmp2 >= 16)
            return NULL;

        pbBuf[i] = (tmp1 * 16 + tmp2);
    }

    pbBuf[i+1] = '\0';

    return pbBuf;
}

char HexCharArr[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
void byteToHex(char* pData, int nLen, char* pOut)
{
    int index = 0;
    for (int i = 0; i < nLen; i++)
    {
        pOut[index++] = HexCharArr[pData[i] >> 4 & 0x0f];
        pOut[index++] = HexCharArr[pData[i] & 0x0f];
    }
}

int main()
{
    const char* b = "啊啊!@#$%^&*()_+~`1234567890-=";
    char* Hexsb = new char[sizeof(b) * 2 + 1];
    byteToHex((char*)b, strlen(b), Hexsb);
    // Hexsb = (char*)ByteToHex((const unsigned char*)b, strlen(b));
    std::cout << Hexsb << endl;

    const char* a = (char*)"B0A1B0A121402324255E262A28295F2B7E60313233343536373839302D3D";
    char* Hexs = new char[sizeof(a) / 2 + 1];
    Hexs = (char*)HexToByte(a);
    std::cout << Hexs << endl;

    return 0;
}

static char hex_lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
'8','9', 'a', 'b', 'c', 'd', 'e', 'f' };
static void array2hex(char* in, int inlen, char* out)
{
    int i;
    for (i = 0; i < inlen; ++i)
    {
        out[2 * i] = hex_lookup[in[i] >> 4];
        out[2 * i + 1] = hex_lookup[in[i] & 0x0f];
    }
}

static void hex2array(char* in, int inlen, char* out)
{
    int i;
    unsigned char r;
    for (i = 0; i < inlen; i += 2)
    {
        r = in[i] - '0';
        if (r > 9) r += '0' + 10 - 'a';
        //printf("%c(%x): %x\n", in[i], in[i], r);
        out[i / 2] = r << 4;
        r = in[i + 1] - '0';
        if (r > 9) r += '0' + 10 - 'a';
        out[i / 2] += r;

        //printf("%c(%x): %x <%x>\n", in[i+1], in[i+1], r, out[i/2]);
    }
}
