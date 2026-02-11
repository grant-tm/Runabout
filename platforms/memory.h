#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>		// sizeof
#include <stdalign.h>	// alignof, alignas

#include "types.h"

void *Memory_Reserve (usize size);
void  Memory_Release (void *address, usize size);
b32   Memory_Commit (void *address, usize size);
b32	  Memory_Decommit (void *address, usize size);

void Memory_Zero (void *address, usize size);
void Memory_Set  (void *address, byte value, usize size);
void Memory_Copy (void *destination_address, const void *source_address, usize size);
i32  Memory_Compare (const void *address_a, const void *address_b, usize size);

#define Memory_SizeOf(T) sizeof(T)
#define Memory_AlignAs(T) alignas(T)
#define Memory_AlignOf(value) alignof(value)

uptr Memory_AlignForward (uptr address, usize alignment);
uptr Memory_AlignForwardToPowerOfTwo (uptr address, uptr alignment);
uptr Memory_AlignBackwardToPowerOfTwo (uptr address, uptr alignment);
usize Memory_GetOSPageSize (void);
usize Memory_GetOSPageRange (void *address, usize size, uptr *start_page_out, uptr *end_page_out);

#endif // MEMORY_H
