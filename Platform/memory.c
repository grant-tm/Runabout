#include "memory.h"

#include <string.h> // memset, memcpy, memcmp

#if PLATFORM_MAC || PLATFORM_LINUX
	#include <sys/mman.h>
#endif

#include "platform_definitions.h"
#include "standard_includes.h"
#include "system.h"

// == ACQUISITION =================================================================================

// -- Memory_Reserve ------------------------------------------------------------------------------

#if PLATFORM_WINDOWS
void *Memory_Reserve (usize size)
{
	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

#elif PLATFORM_MAC
void *Memory_Reserve (usize size)
{
	void *addr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
	return (addr == MAP_FAILED) ? NULL : addr;
}

#elif PLATFORM_LINUX
void *Memory_Reserve (usize size)
{
	void *addr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	return (addr == MAP_FAILED) ? NULL : addr;
}

#else
#error "[memory.c] Memory_Reserve(...) is not implemented on this platform."
#endif

// -- Memory_Release ------------------------------------------------------------------------------

#if PLATFORM_WINDOWS
void Memory_Release (void *address, usize size) 
{
    IgnoreUnusedVariable(size);
	VirtualFree(address, 0, MEM_RELEASE);
}

#elif PLATFORM_MAC || PLATFORM_LINUX
void Memory_Release (void *address, usize size)
{
	munmap(address, size);
}

#else
#error "[memory.c] Memory_Release(...) is not implemented on this platform."
#endif

// -- Memory_Commit -------------------------------------------------------------------------------

#if PLATFORM_WINDOWS
b32 Memory_Commit (void *address, usize size)
{
	uptr start_page = 0;
	uptr end_page = 0;
	usize commit_size = Memory_GetOSPageRange(address, size, &start_page, &end_page);

	Assert(commit_size != 0);
	if (commit_size == 0)
	{
		return false;
	}

	void *page = VirtualAlloc((void *) start_page, (SIZE_T) commit_size, MEM_COMMIT, PAGE_READWRITE);	
	return (page != NULL);
}

#elif PLATFORM_MAC || PLATFORM_LINUX
b32 Memory_Commit (void *address, usize size)
{
	uptr start_page = 0;
	uptr end_page = 0;
	usize commit_size = Memory_GetOSPageRange(address, size, &start_page, &end_page);

	Assert(commit_size != 0);
	if (commit_size == 0)
	{
		return false;
	}

	int result = mprotect((void *) start_page, commit_size, PROT_READ | PROT_WRITE);
	return (result == 0);
}

#else
#error "[memory.c] Memory_Commit(...) is not implemented on this platform."
#endif

// -- Memory_Decommit -----------------------------------------------------------------------------

#if PLATFORM_WINDOWS
b32 Memory_Decommit (void *address, usize size)
{
	uptr start_page = 0;
	uptr end_page = 0;
	usize decommit_size = Memory_GetOSPageRange(address, size, &start_page, &end_page);
	
	Assert(decommit_size != 0);

	return VirtualFree((void *) start_page, (SIZE_T) decommit_size, MEM_DECOMMIT) != 0;
}

#elif PLATFORM_MAC || PLATFORM_LINUX
b32 Memory_Decommit (void *address, usize size)
{
	uptr start_page = 0;
	uptr end_page = 0;
	usize decommit_size = Memory_GetOSPageRange(address, size, &start_page, &end_page);

	Assert(decommit_size != 0);

	void *page = (void *) start_page;

	// Make memory inaccessible
	if (mprotect(page, decommit_size, PROT_NONE) != 0)
	{
		return false;
	}

	// Encourage the OS to reclaim physical pages / discard contents.
	// 		- MADV_DONTNEED: next touch gives you fresh zero pages (common choice; strong hint).
	//		- MADV_FREE: may keep pages until pressure (often faster). Not available everywhere.
	//		- POSIX_MADV_DONTNEED: usually available if MADV_DONTNEED isn't
	// Prefer MADV_FREE if present, otherwise fall back to DONTNEED.
	#if	defined(MADV_FREE)
		(void) madvise(page, decommit_size, MADV_FREE);
	#elif defined(MADV_DONTNEED)
		(void) madvise(page, decommit_size, MADV_DONTNEED);
	#elif defined(POSIX_MADV_DONTNEED)
		(void) posix_madvise(page, decommit_size, POSIX_MADV_DONTNEED);
	#else
		// No discard API available; decommit still works via PROT_NONE.
	#endif

	return true;
}

#else
#error "[memory.c] Memory_Decommit(...) is not implemented on this platform."
#endif

// == OPERATIONS ==================================================================================

// -- Memory_Zero ---------------------------------------------------------------------------------

void Memory_Zero (void *address, usize size)
{
	Assert(address != NULL || size == 0);
	memset(address, 0, size);
}

// -- Memory_SecureZero ---------------------------------------------------------------------------

#if PLATFORM_WINDOWS
void Memory_SecureZero (void *address, usize size)
{
	Assert(address != NULL || size == 0);
	SecureZeroMemory(address, size);
}

#else
void Memory_SecureZero (void *address, usize size)
{
	Assert(address != NULL || size == 0);
	
	volatile byte *address_iterator = (volatile byte *) address;
	while (size-- > 0)
	{
		*address_iterator++ = 0;
	}
}
#endif

// -- Memory_Copy ---------------------------------------------------------------------------------

void Memory_Copy (void *restrict destination_address, const void *restrict source_address, usize copy_size)
{
	Assert(destination_address != NULL || copy_size == 0);
	Assert(source_address != NULL || copy_size == 0);

	uptr dst = (uptr) destination_address;
	uptr src = (uptr) source_address;

	uptr dst_end = dst + copy_size;
	uptr src_end = src + copy_size;

	Assert(dst_end >= dst);
	Assert(src_end >= src);

	b32 overlap = (dst < src_end) && (src < dst_end);
	Assert(!overlap);

	memcpy(destination_address, source_address, copy_size);
}

// -- Memory_Move ---------------------------------------------------------------------------------

void Memory_Move (void *destination_address, const void *source_address, usize size)
{
	Assert(destination_address != NULL || size == 0);
	Assert(source_address != NULL || size == 0);
	memmove(destination_address, source_address, size);
}

// -- Memory_Set ----------------------------------------------------------------------------------

void Memory_Set (void *address, byte value, usize size)
{
	Assert(address != NULL || size == 0);
	memset(address, (int) value, size);
}

// -- Memory_Compare ------------------------------------------------------------------------------

i32 Memory_Compare (const void *address_a, const void *address_b, usize size)
{
	Assert(address_a != NULL || size == 0);
	Assert(address_b != NULL || size == 0);
	return memcmp(address_a, address_b, size);
}

// == ALIGNMENT ===================================================================================

// -- Memory_AlignForward -------------------------------------------------------------------------

uptr Memory_AlignForward (uptr address, usize alignment) 
{
    Assert(alignment != 0);

	// Fast case: alignment is a power of two
	if ((alignment & (alignment - 1)) == 0)
	{
		return Memory_AlignForwardToPowerOfTwo(address, alignment);
	}
	
	// General case
	usize modulo = address % alignment;
    if (modulo != 0) 
	{
		uptr delta = alignment - modulo;
		Assert(address + delta >= address);
		address += delta;
    }
    return address;
}

// -- Memory_AlignBackwardToPowerOfTwo ------------------------------------------------------------

uptr Memory_AlignBackwardToPowerOfTwo (uptr value, uptr alignment)
{
	Assert(alignment != 0);
    Assert((alignment & (alignment - 1)) == 0);
    return value & ~(alignment - 1);
}

// -- Memory_AlignForwardToPowerOfTwo -------------------------------------------------------------

uptr Memory_AlignForwardToPowerOfTwo (uptr value, uptr alignment)
{
	Assert(value + (alignment - 1) >= value);
	return Memory_AlignBackwardToPowerOfTwo(value + (alignment - 1), alignment);
}

// -- Memory_GetOSPageSize() ----------------------------------------------------------------------

#if PLATFORM_WINDOWS
usize Memory_GetOSPageSize (void)
{
	static usize cached_page_size = 0;
	
	if (cached_page_size == 0)
	{
		SYSTEM_INFO info = {0};
		GetSystemInfo(&info);
		cached_page_size = (usize) info.dwPageSize;
		
		Assert(cached_page_size != 0);
	}

	return cached_page_size;
}

#elif PLATFORM_MAC || PLATFORM_LINUX
usize Memory_GetOSPageSize (void)
{
	static usize cached_page_size = 0;

	if (cached_page_size == 0)
	{
		long ps = sysconf(_SC_PAGESIZE);
		cached_page_size = (ps > 0) ? (usize) ps : (usize) 4096;
		
		Assert(cached_page_size != 0);
	}

	return cached_page_size;
}

#else
#error "[memory.c] Memory_GetOSPageSize(...) is not implemented on this platform."	
#endif

// -- Memory_GetOSPageRange -----------------------------------------------------------------------

usize Memory_GetOSPageRange (void *address, usize size, uptr *start_page_out, uptr *end_page_out)
{
    if (start_page_out != NULL)
	{
		*start_page_out = 0; 
	}
	if (end_page_out != NULL)
	{
		*end_page_out   = 0; 
	}

	if (address == NULL || size == 0 || start_page_out == NULL || end_page_out == NULL)
	{
        return 0;
	}
	 
    uptr start_address = (uptr) address;
    uptr end_address = start_address + (uptr) size;
	if (end_address < start_address)
	{
        return 0;
	}

	usize page_size = Memory_GetOSPageSize();
    Assert((page_size & (page_size - 1)) == 0);

    uptr start_page = Memory_AlignBackwardToPowerOfTwo(start_address, (uptr) page_size);
    *start_page_out = start_page;

	uptr end_page = Memory_AlignForwardToPowerOfTwo(end_address, (uptr) page_size);
	*end_page_out = end_page;

	usize page_range = (usize) (end_page - start_page);
    return page_range;
}
