#pragma once

#include <string>
#include "../Common.h"

class Base64
{
public:
    static String encode(const Bytes& bytes);
	static String encode(const String& str);
	static String decode(const String& str);
    static Bytes decodeToBytes(const String& str);
};
