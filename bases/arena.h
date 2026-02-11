#ifndef ARENA_H
#define ARENA_H

#include <types.h>

static const usize ARENA_DEFAULT_ALIGNMENT_SIZE = 32;

typedef struct Arena
{
	void *buffer;
	usize reserved;
	usize capacity;
	usize used;
} Arena;

void Arena_Init (Arena *arena);
void Arena_Reserve (Arena *arena, usize bytes);

void *Arena_Push (Arena *arena, usize size);
void *Arena_PushAndZero (Arena *arena, usize size);

#define Arena_PushArray(arena, type, count) \
    ((type *)Arena_Push((arena), sizeof(type) * (count)))

#define Arena_PushAndZeroArray(arena, type, count) \
    ((type *)Arena_PushAndZero((arena), sizeof(type) * (count)))

#define Arena_PushStruct(arena, type) \
    Arena_PushArray((arena), type, 1)

#define Arena_PushAndZeroStruct(arena, type) \
    Arena_PushAndZeroArray((arena), type, 1)

void Arena_Clear (Arena *arena);
void Arena_Release (Arena *arena);

#endif // ARENA_H
