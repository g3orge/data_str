/**
 * Books data structures CEID project Basic header file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******* Base Structs *******/

/* author type for names */
typedef struct {
	char first[56];
	char last[56];
} Author;

/* a single unit type */
/* more than one author possible */
typedef struct {
	long   id;
	char   title[256];
	char   publisher[40];
	short  yearPublished;
	int    numberOfAuthors;
	Author *authors;
} Book;

/* the wrapper around the dynamic array of the managed books */
typedef struct {
	Book  *arr;
	long  numberOfBooks;
} Data;

/******* Global function prototypes *******/

/* global main in-memory dynamic database */
extern Data db;
/* for the individual ids of book structs */
extern long idSum;

void init_db(const char *file);
void save_db(const char *file);
inline void fatal(char *message);
void* smalloc(const unsigned int size);