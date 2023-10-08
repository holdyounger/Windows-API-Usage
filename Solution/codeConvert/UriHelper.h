#pragma once

using namespace std;
#include <string>
using std::string;

static class UriHelper
{
public:
	static string Encode(const string& source);
	static string Decode(const string& encoded);
};
