#include "arena.h"
#include "memory.h"
#include "system.h"

void Arena_Init (Arena *arena)
{
	Assert(arena != NULL);	

	arena->buffer = NULL;
	arena->reserved = 0;
	arena->capacity = 0;
	arena->used = 0;
}

void Arena_Reserve (Arena *arena, usize bytes)
{
	Assert(arena != NULL);	
	Assert(bytes > 0);

	usize page_size = Memory_GetOSPageSize();
	bytes = (usize) Memory_AlignForward((uptr) bytes, page_size);

	arena->buffer = (void *) Memory_Reserve(bytes);
	Assert(arena->buffer != NULL);

	arena->reserved = bytes;
	arena->capacity = 0;
	arena->used = 0;
}

usize AlignSizeToArenaDefault(usize size)
{
	const usize modulo = size % ARENA_DEFAULT_ALIGNMENT_SIZE;
	if (modulo != 0)
	{
		size += ARENA_DEFAULT_ALIGNMENT_SIZE - modulo;
	}

	return size;
}

void *Arena_Push(Arena *arena, usize size)
{
    Assert(arena != NULL);
    Assert(arena->buffer != NULL);
    Assert(size > 0);

    size = AlignSizeToArenaDefault(size);

    Assert(arena->used <= arena->reserved - size);

    usize required_size = arena->used + size;
    
	byte *base = (byte *) arena->buffer;

    if (required_size > arena->capacity)
    {
        usize page = Memory_GetOSPageSize();
        usize grow = required_size - arena->capacity;
        grow = Memory_AlignForwardToPowerOfTwo(grow, page);

        bool commit_succeeded = Memory_Commit(base + arena->capacity, grow);
        Assert(commit_succeeded);

        arena->capacity += grow;
    }

    void *result = base + arena->used;
    arena->used += size;

    return result;
}

void *Arena_PushAndZero (Arena *arena, usize size)
{
    usize aligned = AlignSizeToArenaDefault(size);
    void *address = Arena_Push(arena, size);
    Assert(address != NULL);
    Memory_Zero(address, aligned);
    return address;
}

void Arena_Clear (Arena *arena)
{
	Assert(arena != NULL);
	arena->used = 0;
}

void Arena_ClearAndDecommit (Arena *arena)
{
	Assert(arena != NULL);
	Assert(arena->buffer != NULL);

	Memory_Decommit(arena->buffer, arena->capacity);
	
	arena->capacity = 0;
	arena->used = 0;
}

void Arena_Release (Arena *arena)
{
	Assert(arena != NULL);
	Assert(arena->buffer != NULL);
	Memory_Release(arena->buffer, arena->reserved);
	
	arena->buffer = NULL;
	arena->reserved = 0;
	arena->capacity = 0;
	arena->used = 0;
}
