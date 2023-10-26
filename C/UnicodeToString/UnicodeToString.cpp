#include <iostream>
#include <string>
#include <codecvt>
#include <atlstr.h>
#include <atlconv.h>

using namespace std;

int main() {

    string jsonStr = "{\"share_names\":[\"\u4e2d\u6587\u8def\u5f84\"]}";

    cout << jsonStr << endl;

    cout << "≡ is the same as \u2261" << endl;

    string s("≡ is the same as \u2261");

    cout << s << endl;
}

string Unescape(const string& input) {
    wstring wresult;
    for (size_t i = 0; i < input.length(); ) {
        if (input[i] == '\\' && input[i + 1] == 'u') {
            string code = input.substr(i + 2, 4);
            wchar_t unicode = stoi(code, nullptr, 16);
            wresult += unicode;
            i += 6;
        }
        else {
            wresult += input[i++];
        }
    }
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    string result = conv.to_bytes(wresult);
    return result;
}

CString Unescape(const CString& csInput) {
    string input = csInput.GetString();
    wstring wresult;
	for (size_t i = 0; i < input.length(); ) {
		if (input[i] == '\\' && input[i + 1] == 'u') {
			string code = input.substr(i + 2, 4);
			wchar_t unicode = stoi(code, nullptr, 16);
			wresult += unicode;
			i += 6;
		}
		else {
			wresult += input[i++];
		}
	}
	CString csResult;
	csResult.Format("%s", wresult);
	return csResult;
}