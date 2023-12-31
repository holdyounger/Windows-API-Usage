﻿#include <iostream>  
#include <string>  
#include <vector>  
#include <Windows.h>  
#include <Psapi.h>  
#include <sstream>  
#include <iomanip>  
#include <fstream>  
#include <algorithm>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#include "base64.h"

#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")

std::string InvokeDecryptAES(const std::vector<unsigned char>& salt,
    const std::vector<unsigned char>& encryptionKey,
    const std::vector<unsigned char>& pass,
    const std::vector<unsigned char>& decodedCipherText) {
    std::string encryptionCipher = "AES-128-CBC";
    std::string encryptionKeyCipher = "PBEWITHSHAAND128BITRC2-CBC";

    std::vector<unsigned char> iv(16);
    std::copy(decodedCipherText.begin(), decodedCipherText.begin() + 16, iv.begin());

    std::vector<unsigned char> cipherText(decodedCipherText.begin() + 16, decodedCipherText.end());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);

    EVP_BytesToKey(EVP_aes_128_cbc(), EVP_sha1(), salt.data(), pass.data(), pass.size(), 5, (unsigned char*)(encryptionKey.data()), iv.data());

    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, encryptionKey.data(), iv.data());

    std::vector<unsigned char> clearText(cipherText.size() + EVP_MAX_BLOCK_LENGTH);
    int clearTextLength = 0;

    EVP_DecryptUpdate(ctx, clearText.data(), &clearTextLength, cipherText.data(), cipherText.size());
    int finalLength = 0;
    EVP_DecryptFinal_ex(ctx, clearText.data() + clearTextLength, &finalLength);

    EVP_CIPHER_CTX_cleanup(ctx);
    EVP_CIPHER_CTX_free(ctx);

    clearText.resize(clearTextLength + finalLength);

    return std::string(clearText.begin(), clearText.end());
}

string DecryptWebLogicPassword(string serializedSystemIni, string cipherText, string bouncyCastle) {
    if (bouncyCastle.empty()) {
        bouncyCastle = ".\\BouncyCastle.Crypto.dll";
    }

    // Load BouncyCastle DLL  
    HMODULE bouncyCastleDLL = LoadLibraryA(bouncyCastle.c_str());
    if (!bouncyCastleDLL) {
        cerr << "Error loading BouncyCastle DLL." << endl;
        return "";
    }

    // Decrypt password  
    string pass = "0xccb97558940b82637c8bec3c770f86fa3a391a56";
    vector<char> passChars(pass.begin(), pass.end());
    string decodedCipherText = "";
    if (cipherText.rfind("{AES}", 0)) {
        decodedCipherText = cipherText.substr(5);
    }
    else if (cipherText.rfind("{3DES}", 0)) {
        decodedCipherText = cipherText.substr(6);
    }
    vector<char> decodedCipherTextChars = vector<char>(decodedCipherText.begin(), decodedCipherText.end());
    decodedCipherTextChars.resize(decodedCipherTextChars.size() / 4 * 3);  // Remove padding  
    string decryptedPassword = "";
    typedef void (*DecryptFunc)(vector<char>&, vector<char>&, std::string&, int);
    DecryptFunc decryptFunc = (DecryptFunc)GetProcAddress(bouncyCastleDLL, "DecryptAES");
    if (!decryptFunc) {
        cerr << "Error getting DecryptAES function address." << endl;
        return "";
    }
    decryptFunc(passChars, decodedCipherTextChars, decryptedPassword, 0);
    FreeLibrary(bouncyCastleDLL);
    return decryptedPassword;
}

int main1() {
    string serializedSystemIni = "SerializedSystemIni.dat";
    // string cipherText = "{3DES}JMRazF/vClP1WAgy1czd2Q==";  // or "{AES}8/rTjIuC4mwlrlZgJK++LKmAThcoJMHyigbcJGIztug=" for AES decryption  
    string cipherText = "{AES}tV6KX+O4kjVAtXrnlkR6hcyGA2iAneG2nKG61AVrQBw=";  // or "{AES}8/rTjIuC4mwlrlZgJK++LKmAThcoJMHyigbcJGIztug=" for AES decryption  
    string bouncyCastle = "";  // Optional path to BouncyCastle DLL. If empty, the default path ".\BouncyCastle.Crypto.dll" is used.  
    string decryptedPassword = DecryptWebLogicPassword(serializedSystemIni, cipherText, bouncyCastle);
    cout << "Decrypted password: " << decryptedPassword << endl;
    return 0;
}

std::vector<unsigned char> ReadBytesFromFile(const std::string& filePath, int numberOfBytes) {
    std::ifstream file(filePath, std::ios::binary);
    std::vector<unsigned char> bytes(numberOfBytes);
    file.read(reinterpret_cast<char*>(bytes.data()), numberOfBytes);
    return bytes;
}

bool ParseDatFile(const std::string& serializedSystemIni,
    __out int& nVersion,
    __out std::vector<unsigned char>& RTSalt,
    __out std::vector<unsigned char>& RTEncryptionKey,
    __out std::vector<unsigned char>& RTPass)
{
    bool bRet = false;
    // std::string serializedSystemIni = "SerializedSystemIni.dat";
    std::ifstream file(serializedSystemIni, std::ios::binary);

    std::vector<unsigned char> Pass = { 0xcc, 0xb9, 0x75, 0x58, 0x94, 0x0b, 0x82, 0x63, 0x7c, 0x8b, 0xec, 0x3c, 0x77, 0x0f, 0x86, 0xfa, 0x3a, 0x39, 0x1a, 0x56 };
    unsigned char numberOfBytes;
    file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
    std::cout << "NumberOfBytes = " << static_cast<int>(numberOfBytes) << std::endl;

    std::vector<unsigned char> salt(numberOfBytes);
    file.read(reinterpret_cast<char*>(salt.data()), numberOfBytes);
    std::cout << "Salt = ";
    for (const auto& byte : salt) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    unsigned char version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    std::cout << "Version = " << static_cast<int>(version) << std::endl;
    nVersion = static_cast<int>(version);

    file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
    std::cout << "NumberOfBytes = " << static_cast<int>(numberOfBytes) << std::endl;

    std::vector<unsigned char> encryptionKey(numberOfBytes);
    file.read(reinterpret_cast<char*>(encryptionKey.data()), numberOfBytes);
    std::cout << "EncryptionKey = ";
    for (const auto& byte : encryptionKey) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;


    RTEncryptionKey = encryptionKey;
    RTSalt = salt;
    RTPass = Pass;
    file.close();

    bRet = true;
_END:
    return bRet;
}


bool ParseDatFile(const std::string& serializedSystemIni,
    __out int& nVersion,
    __out char* RTSalt,
    __out char* RTEncryptionKey,
    __out char* RTPass)
{
    bool bRet = false;
    // std::string serializedSystemIni = "SerializedSystemIni.dat";
    std::ifstream file(serializedSystemIni, std::ios::binary);

    std::vector<unsigned char> Pass = { 0xcc, 0xb9, 0x75, 0x58, 0x94, 0x0b, 0x82, 0x63, 0x7c, 0x8b, 0xec, 0x3c, 0x77, 0x0f, 0x86, 0xfa, 0x3a, 0x39, 0x1a, 0x56 };
    unsigned char numberOfBytes;
    file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
    std::cout << "NumberOfBytes = " << static_cast<int>(numberOfBytes) << std::endl;

    RTSalt = (char*)malloc(sizeof(char) * numberOfBytes);
    file.read(reinterpret_cast<char*>(RTSalt), numberOfBytes);
    printf("Salt=%s\n", RTSalt);

    unsigned char version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    std::cout << "Version = " << static_cast<int>(version) << std::endl;
    nVersion = static_cast<int>(version);

    file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
    std::cout << "NumberOfBytes = " << static_cast<int>(numberOfBytes) << std::endl;

    RTEncryptionKey = (char*)malloc(numberOfBytes * sizeof(unsigned char));
    file.read(reinterpret_cast<char*>(RTEncryptionKey), numberOfBytes);
    printf("EncryptionKey=%s\n", RTEncryptionKey);


    RTPass = reinterpret_cast<char*>(Pass.data());
    file.close();

    bRet = true;
_END:
    return bRet;
}


//将文本形式的HEX串进行转换
unsigned char* str2hex(char* str)
{
    unsigned char* ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    if ((str_len % 2) != 0)
    {
        return 0;
    }
    ret = (unsigned char*)malloc((str_len / 2)+1);
    for (i = 0; i < str_len; i = i + 2)
    {
        sscanf(str + i, "%2hhx", &ret[i / 2]);
    }
    ret[str_len] = '\0';
    return ret;
}

//将HEX串的文本进行转换
char* hex2str(unsigned char* hex)
{
    char* ret = NULL;
    int hex_len = strlen((char*)hex);
    int i = 0;
    if ((hex_len % 2) != 0)
    {
        return NULL;  // 返回错误值，表示输入的hex字符串长度不正确  
    }
    ret = (char*)malloc((hex_len * 2) + 1);  // 分配足够的内存空间以存储原始字符串  
    memset(ret, 0, (hex_len * 2) + 1);
    if (ret == NULL) {
        return NULL;  // 如果内存分配失败，返回错误值  
    }
    for (i = 0; i < hex_len * 2; i = i + 2)
    {
        sprintf(ret + i, "%02x", hex[i / 2]);
    }
    ret[(hex_len * 2)] = '\0';
    return ret;
}

#define AESKEY "df98b715d5c6ed2b25817b6f255411a1"	//HEX密钥
#define AESIV  "2841ae97419c2973296a0d4bdfe19a4f"	//HEX初始向量

void DecrypyTest(unsigned char* key, std::string strAES)
{
    unsigned char plain_text[32];
    unsigned char encrypted_text[32];
    unsigned char* stdiv;

    std::string CipherText = strAES.substr(5);

    AES_KEY decryptkey;
    AES_set_encrypt_key(key, 128, &decryptkey);
    stdiv = str2hex((char*)CipherText.substr(0,16).data());

    unsigned char tmpiv[16];
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt((const unsigned char*)strAES.data(), plain_text, 32, &decryptkey, tmpiv, AES_DECRYPT);

    std::string strResult((char*)plain_text);

    std::cout << strResult;
}

int ParseAESTest()
{
    int nVersion = 0;
    char* salt = nullptr;
    char* EncryptionKey = nullptr;
    char* pass = nullptr;
    ParseDatFile("SerializedSystemIni.dat", nVersion, salt, EncryptionKey, pass);


    return 0;
}

void EnAndDecrpytDemo()
{
    AES_KEY encryptkey;
    AES_KEY decryptkey;

    unsigned char* key;
    unsigned char* stdiv;
    char* keyDe;
    char* stdivDe;

    key = str2hex((char*)AESKEY);
    stdiv = str2hex((char*)AESIV);

    {
        printf("hex1.%s\n", key);
        printf("hex2.%s\n", stdiv);

        keyDe = hex2str(key);
        stdivDe = hex2str(stdiv);

        printf("1.%s\n", keyDe);
        printf("2.%s\n", stdivDe);

        free(keyDe);
        free(stdivDe);
    }

    AES_set_encrypt_key(key, 128, &encryptkey);
    AES_set_decrypt_key(key, 128, &decryptkey);

    unsigned char plain_text[32];

    //需要将加密区块长度填充为16字节整数倍，此处使用zero-padding，即末尾全用0填充
    memcpy(plain_text, "AES encrypt in openssl demo", 27);
    memset(plain_text + 27, 0, 5);

    printf("plain_text: ");
    for (int i = 0; i < 32; i++)
    {
        printf(" % 02X ", plain_text[i]);
    }
    printf("\n");

    unsigned char encrypted_text[32];

    memset(encrypted_text, 0, 32);
    unsigned char tmpiv[16];
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt(plain_text, encrypted_text, 32, &encryptkey, tmpiv, AES_ENCRYPT);
    //初始向量这个参数每次使用都会将其改变，有兴趣的话可以把调用前后这个地址下的内容打印出来。所以如果要多次加密且每次使用固定的初始向量，可以先用tmpiv接收

    printf("encrypted_text: ");
    for (int i = 0; i < 32; i++)
    {
        printf(" % 02X ", encrypted_text[i]);
    }
    printf("\n");

    char* plainEncrypText;
    plainEncrypText = hex2str(encrypted_text);
    printf("\nplainEncrypText: %s", plainEncrypText);
    printf("\n");

    unsigned char decrypted_text[32];

    memset(decrypted_text, 0, 32);
    memcpy(tmpiv, stdiv, 16);
    AES_cbc_encrypt(encrypted_text, decrypted_text, 32, &decryptkey, tmpiv, AES_DECRYPT);

    printf("decrypted_text: ");
    for (int i = 0; i < 32; i++)
    {
        printf(" % 02X ", decrypted_text[i]);
    }

    printf("\n %s", decrypted_text);
}

/*
* @fn			AES_128_Decode
* @brief		AES解密封装接口，仅支持AES 128
* @param[in]
*   key: 密钥
*   tmpiv: 偏移量
*   origin: 待解密值
* @param[out]   
* @return		返回解密后的内容
*
* @detail      	
* @author		mingming.shi
* @date			
*/
unsigned char* AES_128_Encode(unsigned char* key, unsigned char* tmpiv, unsigned char* origin)
{
    AES_KEY decryptkey;

    // unsigned char* HexKey = str2hex(key);

    AES_set_encrypt_key(key, 128, &decryptkey);

    unsigned char* output_text;
    output_text = (unsigned char*)malloc(sizeof(128));
    memset(output_text, '\0', 128);
    AES_cbc_encrypt(origin, output_text, 32, &decryptkey, tmpiv, AES_ENCRYPT);

    return output_text;
}

/*
* @fn			AES_128_Decode
* @brief		AES解密封装接口，仅支持AES 128
* @param[in]
*   key: 密钥
*   tmpiv: 偏移量
*   origin: 待解密值
* @param[out]   
* @return		返回解密后的内容
*
* @detail      	
* @author		mingming.shi
* @date			
*/
unsigned char* AES_128_Decode(unsigned char* key, unsigned char *tmpiv, unsigned char* origin)
{
    AES_KEY aesKey;

    // unsigned char* HexKey = str2hex(key);

    AES_set_decrypt_key(key, 128, &aesKey);

    unsigned char *output_text;
    output_text = (unsigned char*)malloc(sizeof(128));
    memset(output_text, '\0', 128);
    AES_cbc_encrypt(origin, output_text, 32, &aesKey, tmpiv, AES_DECRYPT);

    return output_text;
}

void DeCodeWebLogic()
{
    // 获取key
    int nVersion = 0;
    std::vector<unsigned char> salt;
    std::vector<unsigned char> EncryptionKey;
    std::vector<unsigned char> pass;

    ParseDatFile("SerializedSystemIni.dat", nVersion, salt, EncryptionKey, pass);

    if (nVersion >= 2)
    {
        unsigned char* strAESKey;

        strAESKey = (unsigned char*)malloc(EncryptionKey.size() + 1);
        memset(strAESKey, '\0', EncryptionKey.size() + 1);
        std::copy(EncryptionKey.begin(), EncryptionKey.end(), strAESKey);

        printf("AESKey Value:%s\n", strAESKey);


        // 获取base64解码后的文本
        char* EncryptText = TestBase64();
        {
            printf("AES Origin Value:%s\n", EncryptText);
            for (int i = 0; i < strlen(EncryptText); i++)
            {
                // printf(" % 02X ", EncryptText[i]);
            }
            std::cout << "\n-------------------------\n";
        }

        // 获取文本的hex形式
        char* HexAESOrigin = hex2str((unsigned char *)EncryptText);

        // 获取偏移量
        unsigned char ivec[16];
        unsigned char* iv;
        iv = (unsigned char*)malloc(16);
        memset(iv, 0, 16);
        memcpy(iv, HexAESOrigin, 16);
        unsigned char* TempIv = str2hex((char*)iv);
        memcpy(ivec, TempIv, 16);

        free(TempIv);
        free(HexAESOrigin);

        unsigned char plain_text[32];

        //需要将加密区块长度填充为16字节整数倍，此处使用zero-padding，即末尾全用0填充
        memcpy(plain_text, "AES encrypt in openssl demo", 27);
        memset(plain_text + 27, 0, 5);

        unsigned char* enpwd = AES_128_Encode(strAESKey, ivec, (unsigned char*)plain_text);
        unsigned char* depwd = AES_128_Decode(strAESKey, ivec, (unsigned char*)enpwd);
        printf("pwd:%s\n", depwd);
    }
    
}

int main() {

    EnAndDecrpytDemo();

    DeCodeWebLogic();


    int nVersion = 0;
    std::vector<unsigned char> salt;
    std::vector<unsigned char> EncryptionKey;
    std::vector<unsigned char> pass;

    ParseDatFile("SerializedSystemIni.dat", nVersion, salt, EncryptionKey, pass);
    std::string strAES = "{AES}tV6KX+O4kjVAtXrnlkR6hcyGA2iAneG2nKG61AVrQBw=";

    unsigned char *AESKey;

    AESKey = (unsigned char*)malloc(EncryptionKey.size()+1);
    std::copy(EncryptionKey.begin(), EncryptionKey.end(), AESKey);

    char* plainEncry = hex2str(AESKey);

    printf("plainEncry:%s", plainEncry);

   // DecrypyTest(buffer, strAES);
    free(AESKey);
    // {AES}tV6KX+O4kjVAtXrnlkR6hcyGA2iAneG2nKG61AVrQBw=
    std::string strResult = "";
    // std::cin >> strAES;

    std::vector<unsigned char> Result(strAES.c_str(), strAES.c_str() + strAES.length() + 1);

    if (nVersion >= 2)
    {
        strResult = InvokeDecryptAES(salt, EncryptionKey, pass, Result);
    }

    return 0;
}



std::string InvokeWebLogicPasswordDecryptor(const std::string& serializedSystemIni, const std::string& cipherText) {
    std::ifstream file(serializedSystemIni, std::ios::binary);
    unsigned char numberOfBytes;

    file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
    std::vector<unsigned char> salt = ReadBytesFromFile(serializedSystemIni, numberOfBytes);

    unsigned char version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));

    file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
    std::vector<unsigned char> encryptionKey = ReadBytesFromFile(serializedSystemIni, numberOfBytes);

    std::string clearText;

    if (version >= 2) {
        file.read(reinterpret_cast<char*>(&numberOfBytes), sizeof(numberOfBytes));
        encryptionKey = ReadBytesFromFile(serializedSystemIni, numberOfBytes);

        // 调用 Invoke-DecryptAES 函数
        // clearText = InvokeDecryptAES(salt, encryptionKey, /* pass 和 decodedCipherText 参数 */);
    }
    else {
        // 调用 Invoke-Decrypt3DES 函数
        // clearText = InvokeDecrypt3DES(salt, encryptionKey, /* pass 和 decodedCipherText 参数 */);
    }

    file.close();

    return clearText;
}