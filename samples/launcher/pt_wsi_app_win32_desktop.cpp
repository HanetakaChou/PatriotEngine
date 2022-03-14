//
// Copyright (C) YuqiaoZhang(HanetakaChou)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <pt_wsi_main.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_mcrt_string.h>
#include "pt_wsi_app_base.h"
#include <assert.h>

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN 1
#define NOGDICAPMASKS 1
#define NOVIRTUALKEYCODES 1
#define NOWINMESSAGES 1
#define NOWINSTYLES 1
#define NOSYSMETRICS 1
#define NOMENUS 1
#define NOICONS 1
#define NOKEYSTATES 1
#define NOSYSCOMMANDS 1
#define NORASTEROPS 1
#define NOSHOWWINDOW 1
#define NOATOM 1
#define NOCLIPBOARD 1
#define NOCOLOR 1
#define NOCTLMGR 1
#define NODRAWTEXT 1
#define NOGDI 1
#define NOKERNEL 1
#define NOUSER 1
#define NONLS 1
#define NOMB 1
#define NOMEMMGR 1
#define NOMETAFILE 1
#define NOMINMAX 1
#define NOMSG 1
#define NOOPENFILE 1
#define NOSCROLL 1
#define NOSERVICE 1
#define NOSOUND 1
#define NOTEXTMETRIC 1
#define NOWH 1
#define NOWINOFFSETS 1
#define NOCOMM 1
#define NOKANJI 1
#define NOHELP 1
#define NOPROFILER 1
#define NODEFERWINDOWPOS 1
#define NOMCX 1
#include <Windows.h>

static mcrt_wstring g_wsi_app_win32_desktop_executable_path;

static pt_wsi_app_ref PT_PTR launcher_app_init(int argc, char *argv[], pt_gfx_connection_ref gfx_connection);
static int PT_PTR launcher_app_main(pt_wsi_app_ref wsi_app);

static pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename);
static int PT_CALL cache_input_stream_stat_size_callback(pt_gfx_input_stream_ref cache_input_stream, int64_t *size);
static intptr_t PT_PTR cache_input_stream_read_callback(pt_gfx_input_stream_ref cache_input_stream, void *data, size_t size);
static void PT_PTR cache_input_stream_destroy_callback(pt_gfx_input_stream_ref cache_input_stream);
static pt_gfx_output_stream_ref PT_PTR cache_output_stream_init_callback(char const *initial_filename);
static intptr_t PT_PTR cache_output_stream_write_callback(pt_gfx_output_stream_ref cache_output_stream, void *data, size_t size);
static void PT_PTR cache_output_stream_destroy_callback(pt_gfx_output_stream_ref cache_output_stream);

extern pt_gfx_input_stream_ref PT_PTR asset_input_stream_init_callback(char const *);
extern intptr_t PT_PTR asset_input_stream_read_callback(pt_gfx_input_stream_ref, void *, size_t);
extern int64_t PT_PTR asset_input_stream_seek_callback(pt_gfx_input_stream_ref, int64_t, int);
extern void PT_PTR asset_input_stream_destroy_callback(pt_gfx_input_stream_ref);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR cmd_line, int cmd_show)
{
	// Win32 Desktop Path
	{
		WCHAR file_name[4096];
		GetModuleFileNameW(hInstance, file_name, 4096);

		(*wcsrchr(file_name, L'\\')) = L'\0';

		g_wsi_app_win32_desktop_executable_path.assign(file_name);
	}

	// Lunarg Vulkan SDK
	{
		mcrt_wstring layer_path = g_wsi_app_win32_desktop_executable_path;
#if defined(PT_X64)
		layer_path += L"\\..\\..\\..\\third_party\\vulkan_sdk\\lib\\win32_desktop_x64";
#elif defined(PT_X86)
		layer_path += L"\\..\\..\\..\\third_party\\vulkan_sdk\\lib\\win32_desktop_x86";
#else
#error Unknown Architecture
#endif
		SetEnvironmentVariableW(L"VK_LAYER_PATH", layer_path.c_str());
	}

	return pt_wsi_main(
		cmd_line, cmd_show,
		cache_input_stream_init_callback, cache_input_stream_stat_size_callback, cache_input_stream_read_callback, cache_input_stream_destroy_callback,
		cache_output_stream_init_callback, cache_output_stream_write_callback, cache_output_stream_destroy_callback,
		launcher_app_init, launcher_app_main);
}

inline pt_wsi_app_ref wrap(class launcher_app *wsi_app) { return reinterpret_cast<pt_wsi_app_ref>(wsi_app); }
inline class launcher_app *unwrap(pt_wsi_app_ref wsi_app) { return reinterpret_cast<class launcher_app *>(wsi_app); }

static pt_wsi_app_ref PT_PTR launcher_app_init(int argc, char *argv[], pt_gfx_connection_ref gfx_connection)
{
	class launcher_app *wsi_app = new (mcrt_aligned_malloc(sizeof(class launcher_app), alignof(class launcher_app))) launcher_app();
	wsi_app->init(gfx_connection);
	return wrap(wsi_app);
}

static int PT_PTR launcher_app_main(pt_wsi_app_ref wsi_app)
{
	return unwrap(wsi_app)->main();
}

static inline bool internal_utf8_to_utf16(uint8_t const *pInBuf, uint32_t *pInCharsLeft, uint16_t *pOutBuf, uint32_t *pOutCharsLeft);

static pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename)
{
	mcrt_wstring full_path = g_wsi_app_win32_desktop_executable_path;
	full_path += L"\\..\\../";

	// UTF8 to UTF16
	{
		wchar_t wide_file_name[4096];
		uint32_t in_chars_left = strlen(initial_filename);
		uint32_t out_chars_left = 4096;
		bool res_internal_utf8_to_utf16 = internal_utf8_to_utf16(reinterpret_cast<uint8_t const *>(initial_filename), &in_chars_left, reinterpret_cast<uint16_t *>(wide_file_name), &out_chars_left);
		assert(res_internal_utf8_to_utf16);
		wide_file_name[4096 - out_chars_left] = L'\0';

		full_path += wide_file_name;
	}

	HANDLE hFile = CreateFileW(full_path.c_str(), FILE_READ_DATA | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	return (INVALID_HANDLE_VALUE != hFile) ? reinterpret_cast<pt_gfx_input_stream_ref>(hFile) : pt_gfx_input_stream_ref(-1);
}

static int PT_CALL cache_input_stream_stat_size_callback(pt_gfx_input_stream_ref cache_input_stream, int64_t *size)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(cache_input_stream);

	LARGE_INTEGER file_size;
	BOOL res_get_file_size_ex = GetFileSizeEx(hFile, &file_size);
	if (FALSE != res_get_file_size_ex)
	{
		(*size) = file_size.QuadPart;
		return 0;
	}
	else
	{
		(*size) = -1;
		return -1;
	}
}

static intptr_t PT_PTR cache_input_stream_read_callback(pt_gfx_input_stream_ref cache_input_stream, void *data, size_t size)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(cache_input_stream);

	DWORD number_of_bytes_to_read;
	BOOL res_read_file = ReadFile(hFile, data, static_cast<DWORD>(size), &number_of_bytes_to_read, NULL);
	if (FALSE != res_read_file)
	{
		return number_of_bytes_to_read;
	}
	else
	{
		return -1;
	}
}

static void PT_PTR cache_input_stream_destroy_callback(pt_gfx_input_stream_ref cache_input_stream)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(cache_input_stream);

	BOOL res_close_handle = CloseHandle(hFile);
	assert(FALSE != res_close_handle);
}

static pt_gfx_output_stream_ref PT_PTR cache_output_stream_init_callback(char const *initial_filename)
{
	mcrt_wstring full_path = g_wsi_app_win32_desktop_executable_path;
	full_path += L"\\..\\../";

	// UTF8 to UTF16
	{
		wchar_t wide_file_name[4096];
		uint32_t in_chars_left = strlen(initial_filename);
		uint32_t out_chars_left = 4096;
		bool res_internal_utf8_to_utf16 = internal_utf8_to_utf16(reinterpret_cast<uint8_t const *>(initial_filename), &in_chars_left, reinterpret_cast<uint16_t *>(wide_file_name), &out_chars_left);
		assert(res_internal_utf8_to_utf16);
		wide_file_name[4096 - out_chars_left] = L'\0';

		full_path += wide_file_name;
	}

	HANDLE hFile = CreateFileW(full_path.c_str(), FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES, 0U, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	return (INVALID_HANDLE_VALUE != hFile) ? reinterpret_cast<pt_gfx_output_stream_ref>(hFile) : pt_gfx_output_stream_ref(-1);
}

static intptr_t PT_PTR cache_output_stream_write_callback(pt_gfx_output_stream_ref cache_output_stream, void *data, size_t size)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(cache_output_stream);

	DWORD number_of_bytes_to_write;
	BOOL res_write_file = WriteFile(hFile, data, static_cast<DWORD>(size), &number_of_bytes_to_write, NULL);
	if (FALSE != res_write_file)
	{
		return number_of_bytes_to_write;
	}
	else
	{
		return -1;
	}
}

static void PT_PTR cache_output_stream_destroy_callback(pt_gfx_output_stream_ref cache_output_stream)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(cache_output_stream);

	BOOL res_close_handle = CloseHandle(hFile);
	assert(FALSE != res_close_handle);
}

pt_gfx_input_stream_ref PT_PTR asset_input_stream_init_callback(char const *initial_filename)
{
	mcrt_wstring full_path = g_wsi_app_win32_desktop_executable_path;
	full_path += L"\\..\\..\\..\\assets/";

	// UTF8 to UTF16
	{
		wchar_t wide_file_name[4096];
		uint32_t in_chars_left = strlen(initial_filename);
		uint32_t out_chars_left = 4096;
		bool res_internal_utf8_to_utf16 = internal_utf8_to_utf16(reinterpret_cast<uint8_t const *>(initial_filename), &in_chars_left, reinterpret_cast<uint16_t *>(wide_file_name), &out_chars_left);
		assert(res_internal_utf8_to_utf16);
		wide_file_name[4096 - out_chars_left] = L'\0';

		full_path += wide_file_name;
	}

	HANDLE hFile = CreateFileW(full_path.c_str(), FILE_READ_DATA | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	return (INVALID_HANDLE_VALUE != hFile) ? reinterpret_cast<pt_gfx_input_stream_ref>(hFile) : pt_gfx_input_stream_ref(-1);
}

intptr_t PT_PTR asset_input_stream_read_callback(pt_gfx_input_stream_ref asset_input_stream, void *data, size_t size)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(asset_input_stream);

	DWORD number_of_bytes_to_read;
	BOOL res_read_file = ReadFile(hFile, data, static_cast<DWORD>(size), &number_of_bytes_to_read, NULL);
	if (FALSE != res_read_file)
	{
		return number_of_bytes_to_read;
	}
	else
	{
		return -1;
	}
}

int64_t PT_PTR asset_input_stream_seek_callback(pt_gfx_input_stream_ref asset_input_stream, int64_t offset, int whence)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(asset_input_stream);

	static_assert(FILE_BEGIN == PT_GFX_INPUT_STREAM_SEEK_SET, "");
	static_assert(FILE_CURRENT == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
	static_assert(FILE_END == PT_GFX_INPUT_STREAM_SEEK_END, "");

	LARGE_INTEGER distance_to_move;
	distance_to_move.QuadPart = offset;
	LARGE_INTEGER new_file_pointer;
	BOOL res_set_file_pointer_ex = SetFilePointerEx(hFile, distance_to_move, &new_file_pointer, whence);
	if (FALSE != res_set_file_pointer_ex)
	{
		return new_file_pointer.QuadPart;
	}
	else
	{
		return -1;
	}
}

void PT_PTR asset_input_stream_destroy_callback(pt_gfx_input_stream_ref asset_input_stream)
{
	HANDLE hFile = reinterpret_cast<HANDLE>(asset_input_stream);

	BOOL res_close_handle = CloseHandle(hFile);
	assert(FALSE != res_close_handle);
}

static inline bool internal_utf8_to_utf16(uint8_t const *pInBuf, uint32_t *pInCharsLeft, uint16_t *pOutBuf, uint32_t *pOutCharsLeft)
{
	while ((*pInCharsLeft) >= 1)
	{
		uint32_t ucs4code = 0; // Accumulator

		// UTF-8 To UCS-4
		if ((*pInBuf) < 128U) // 0XXX XXXX
		{
			ucs4code += (*pInBuf); // Accumulate

			++pInBuf;
			--(*pInCharsLeft);
		}
		else if ((*pInBuf) < 192U) // 10XX XXXX
		{
			return false;
		}
		else if ((*pInBuf) < 224U) // 110X XXXX 10XX XXXX
		{

			if ((*pInCharsLeft) >= 2)
			{
				ucs4code += (((*pInBuf) - 192U) << 6U); // Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U) // 10XX XXXX
				{
					ucs4code += ((*pInBuf) - 128U); // Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else if ((*pInBuf) < 240U) // 1110 XXXX 10XX XXXX 10XX XXXX
		{
			if ((*pInCharsLeft) >= 3)
			{
				ucs4code += (((*pInBuf) - 224U) << 12U); // Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U) // 10XX XXXX
				{
					ucs4code += (((*pInBuf) - 128U) << 6U); // Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return false;
				}

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U) // 10XX XXXX
				{
					ucs4code += ((*pInBuf) - 128U); // Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else // ((*pInBuf) >= 240U) 1111 0XXX 10XX XXXX 10XX XXXX 10XX XXXX
		{
			if ((*pInCharsLeft) >= 4)
			{
				ucs4code += (((*pInBuf) - 240U) << 18U); // Accumulate

				++pInBuf;
				--(*pInCharsLeft);

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U) // 10XX XXXX
				{
					ucs4code += (((*pInBuf) - 128U) << 12U); // Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return false;
				}

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U) // 10XX XXXX
				{
					ucs4code += (((*pInBuf) - 128U) << 6U); // Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return false;
				}

				if ((*pInBuf) >= 128U && (*pInBuf) < 192U) // 10XX XXXX
				{
					ucs4code += ((*pInBuf) - 128U); // Accumulate

					++pInBuf;
					--(*pInCharsLeft);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		// UCS-4 To UTF-16
		if (ucs4code < 0X10000U)
		{
			if ((*pOutCharsLeft) >= 1)
			{
				(*pOutBuf) = static_cast<uint16_t>(ucs4code);

				++pOutBuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return false;
			}
		}
		else // ucs4code >= 0X10000U
		{
			if ((*pOutCharsLeft) >= 2)
			{
				(*pOutBuf) = static_cast<uint16_t>((((ucs4code - 65536U) & (0XFFC00U)) >> 10U) + 0XD800U); // 110110xxxxxxxxxx

				++pOutBuf;
				--(*pOutCharsLeft);

				(*pOutBuf) = static_cast<uint16_t>(((ucs4code - 65536U) & (0X3FFU)) + 0XDC00U); // 110111xxxxxxxxxx

				++pOutBuf;
				--(*pOutCharsLeft);
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}
