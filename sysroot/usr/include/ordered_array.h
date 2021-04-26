
#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H


#include "type.h"
#include "common.h"
#include "fault.h"

typedef void* type_t;

typedef uint8 (*lessthan_predicate_t)(type_t, type_t);

typedef struct {
	type_t *array;
	uint32 size;
	uint32 max_size;
	lessthan_predicate_t less_than;
}ordered_array_t;

uint8 standard_lessthan_predicate_t (type_t a, type_t b);

ordered_array_t create_ordered_array (uint32 max_size, lessthan_predicate_t less_than);

ordered_array_t place_ordered_array (void *array, uint32 max_size, lessthan_predicate_t less_than);

void destroy_ordered_array (ordered_array_t *array);

void insert_ordered_array (type_t item, ordered_array_t *array);

type_t lookup_ordered_array (uint32 i, ordered_array_t *array);

void remove_ordered_array (uint32 i, ordered_array_t *array);


#endif
