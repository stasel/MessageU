#include "Base64.h"
#include <base64.h> // cryptopp
#include <boost/algorithm/string.hpp>

String Base64::encode(const String& str)
{
	String encoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(encoded)
		) // Base64Encoder
	); // StringSource

    boost::trim_right(encoded);
    return encoded;
}

String Base64::encode(const Bytes& bytes) {
    let str = bytesToString(bytes);
    return encode(str);
}

String Base64::decode(const String& str)
{
	String decoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded)
		) // Base64Decoder
	); // StringSource

	return decoded;
}

Bytes Base64::decodeToBytes(const String& str) {
    let decodedString = decode(str);
    return stringToBytes(decodedString);
}
