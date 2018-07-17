#pragma once
#ifndef _SM_ENUM_H
#define _SM_ENUM_H
#include <type_traits>

enum class SM_ATTRIBS : unsigned char {
	SM_POSITION = 0x01,
	SM_NORMAL = 0x02,
	SM_TEXCOORD = 0x04,
	SM_TANGENT = 0x08,
	SM_BITANGENT = 0x10
};
using T = std::underlying_type_t <SM_ATTRIBS>;

// Overload BITWISE-OR
inline SM_ATTRIBS operator | (SM_ATTRIBS lhs, SM_ATTRIBS rhs)

{
	return (SM_ATTRIBS)(static_cast<T>(lhs) | static_cast<T>(rhs));
}
// Overload BITWISE-OR EQUAL
inline SM_ATTRIBS& operator |= (SM_ATTRIBS& lhs, SM_ATTRIBS rhs)
{
	lhs = (SM_ATTRIBS)(static_cast<T>(lhs) | static_cast<T>(rhs));
	return lhs;
}

// Overload BITWISE-AND
inline SM_ATTRIBS operator & (SM_ATTRIBS lhs, SM_ATTRIBS rhs)

{
	return (SM_ATTRIBS)(static_cast<T>(lhs) & static_cast<T>(rhs));
}
// Overload BITWISE-AND EQUAL
inline SM_ATTRIBS& operator &= (SM_ATTRIBS& lhs, SM_ATTRIBS rhs)
{
	lhs = (SM_ATTRIBS)(static_cast<T>(lhs) & static_cast<T>(rhs));
	return lhs;
}


#endif // !_SM_ENUM_H
