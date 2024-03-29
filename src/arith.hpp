#pragma once

#include "cint.hpp"

template <typename T>
T min(const T &a, const T &b)
{
	return a < b ? a : b;
}

template <typename T, size_t Size>
constexpr size_t array_size(T (&)[Size])
{
	return Size;
}

template <typename T>
size_t store(char *dst, const T &src)
{
	auto csrc = reinterpret_cast<const char*>(&src);
	for (size_t i = 0; i < sizeof(T); i++)
		*dst++ = *csrc++;
	return sizeof(T);
}

template <typename T>
T load(const char *src)
{
	T res;
	auto cres = reinterpret_cast<char*>(&res);
	for (size_t i = 0; i < sizeof(T); i++)
		*cres++ = *src++;
	return res;
}

template <typename T>
size_t store_u16(uint16_t *dst, const T &src)
{
	static_assert(sizeof(T) % 2 == 0, "T size must be multiple of two");
	auto csrc = reinterpret_cast<const uint16_t*>(&src);
	for (size_t i = 0; i < sizeof(T) / 2; i++)
		*dst++ = *csrc++;
	return sizeof(T);
}

template <typename T>
T load_u16(const uint16_t *src)
{
	static_assert(sizeof(T) % 2 == 0, "T size must be multiple of two");
	T res;
	auto cres = reinterpret_cast<uint16_t*>(&res);
	for (size_t i = 0; i < sizeof(T) / 2; i++)
		*cres++ = *src++;
	return res;
}

template <size_t Size, typename T>
size_t store_part(char *dst, const T &src)
{
	static_assert(Size < sizeof(T), "Size must be less than sizeof(T)");
	auto csrc = reinterpret_cast<const char*>(&src);
#ifndef CINT_HOST	// big-endian
	csrc += sizeof(T) - Size;
#endif
	for (size_t i = 0; i < Size; i++)
		*dst++ = *csrc++;
	return Size;
}

template <typename T>
size_t store_part(char *dst, size_t size, const T &src)
{
	auto csrc = reinterpret_cast<const char*>(&src);
#ifndef CINT_HOST	// big-endian
	csrc += sizeof(T) - size;
#endif
	for (size_t i = 0; i < size; i++)
		*dst++ = *csrc++;
	return size;
}

template <size_t Size, typename T>
T load_part(const char *src)
{
	static_assert(Size < sizeof(T), "Size must be less than sizeof(T)");
	T res = 0;
	auto cres = reinterpret_cast<char*>(&res);
#ifndef CINT_HOST	// big-endian
	cres += sizeof(T) - Size;
#endif
	for (size_t i = 0; i < Size; i++)
		*cres++ = *src++;
	return res;
}

template <typename T, size_t Size>
struct carray
{
	operator const T*(void) const
	{
		return data;
	}
	static constexpr auto size(void) { return Size; }
	T data[Size];
};

#define cpy_chunk(dst_id, base, end) char dst_id[(end) - (base)];	\
	{								\
		size_t size = (end) - (base);				\
		for (size_t i = 0; i < size; i++)			\
			dst_id[i] = (base)[i];				\
	}								\

#define load_chunk(dst_id, base_off, end_off) cpy_chunk(dst_id, m_buffer + (base_off), m_buffer + (end_off))

#define cpy_chunk_back(dst, src)			\
	[&]() -> size_t {				\
		size_t size = sizeof(src);		\
		for (size_t i = 0; i < size; i++)	\
			(dst)[i] = (src)[i];		\
		return sizeof(src);			\
	}()						\

#define store_chunk(src) m_size += cpy_chunk_back(m_buffer + m_size, src);