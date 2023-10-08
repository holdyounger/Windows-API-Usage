#include"UriHelper.h"
#include "include/curl/curl.h"

string UriHelper::Encode(const string& source)
{
	CURL* curl = curl_easy_init();
	char* cres = curl_escape(source.c_str(), source.length());
	string res(cres);
	curl_free(cres);
	curl_easy_cleanup(curl);
	return res;
}

string UriHelper::Decode(const string& encoded)
{
	CURL* curl = curl_easy_init();
	int outlength;
	char* cres = curl_easy_unescape(curl, encoded.c_str(), encoded.length(), &outlength);
	string res(cres, cres + outlength);
	curl_free(cres);
	curl_easy_cleanup(curl);

	//if it's UTF-8, convert it to ISO_8859-1. Based on https://stackoverflow.com/questions/11156473/is-there-a-way-to-convert-from-utf8-to-iso-8859-1/11156490#11156490
	// iconv_t cd = iconv_open("ISO_8859-1", "UTF-8");

	const char* in_buf = res.c_str();
	size_t in_left = res.length();

	char* output = new char[res.length() + 1];
	std::fill(output, output + res.length() + 1, '\\0');
	char* out_buf = &output[0];
	size_t out_left = res.length();

#if 0
	do {
		if (iconv(cd, &in_buf, &in_left, &out_buf, &out_left) == (size_t)-1) {
			//failed to convert, just return the value received from curl
			delete[] output;
			iconv_close(cd);
			return res;
		}
	} while (in_left > 0 && out_left > 0);

	string outputString(output);
	delete[] output;
	iconv_close(cd);
#endif
	return "";
	// return outputString;
}