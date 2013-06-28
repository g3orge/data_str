/*
 * George 'papanikge' Papanikolaou CEID Data Structures 2013
 * Simple Trie Implementation
 * See other files about licensing
 */

/*
 * Tries are represented by the TrieNode which have a value and a (latin
 * alphabet size) array of pointers to children (not compressed).
 * Functions work by traversing the string one by one character
 */

#include "core.h"
#include "trie.h"
#include <ctype.h>

/* this is used so we can avoid wrapper functions of all the exported ones
 * we need the setup_string() function to be called once */
static int first_time = 1;

/*
 * convert a whole string to lowercase
 * also keeping only alpharithmetic characters
 * !!! the memory it returns needs to be free()d
 */
static char* setup_string(char *str)
{
	int i,j;
	char *p, *start;

	start = p = (char*) smalloc(strlen(str));
	for (i = j = 0; str[i] != '\0'; i++)
		if (isalpha(str[i])) {
			p[j] = str[i];
			j++;
		}
	p = start;
	while (*p) {
		*p = tolower(*p);
		p++;
	}
	return start;
}

/* helper for when closing up (recursively) */
void trie_dispose(TrieNode* t)
{
	int i;

	if (t) {
		for (i = 0; i < 26; i++)
			trie_dispose(t->edges[i]);
		free(t);
	}
	return;
}

/* memory allocation and assignments to new node */
TrieNode* trie_initialize(char key)
{
	int i;
	TrieNode *node;

	node = (TrieNode*) smalloc(sizeof(TrieNode));
	node->key   = key;
	node->value = NULL;
	/* null-ing all nodes */
	for (i = 0; i < 26; i++)
		node->edges[i] = NULL;
	return node;
}

/* Add a key to the trie and create nodes as needed (recursively) */
TrieNode* trie_insert(char *str, Book *v, TrieNode *t)
{
	int index;
	char *s;

	if (first_time) {
		s = setup_string(str);
		first_time = 0;
	} else
		s = str;

	if (*s == '\0') {
		/* reached the end */
		t->key   = '\0';
		t->value = v;
	} else {
		/* get index in 0..25 */
		index = *s - 'a';
		/* check for existence */
		if(!t->edges[index])
			t->edges[index] = trie_initialize(*s);
		/* key, for later */
		t->key = tolower(*s);
		/* one more shift for the next character */
		s++;
		t->edges[index] = trie_insert(s, v, t->edges[index]);
	}
	/* revert to true */
	first_time = 1;
	return t;
}

/* traverse the string and delete if there is nothing else (recursively) */
TrieNode* trie_delete(char *str, TrieNode *t)
{
	int i;
	char *s;
	int index;
	/* true by default */
	int all_null = 1;
	/* optimization flag in case of non-existence */
	static int no_need = 0;

	/* safety */
	if (!t || no_need)
		return NULL;

	if (first_time) {
		s = setup_string(str);
		first_time = 0;
	} else
		s = str;

	if (*s == '\0') {
		/* reached it */
		free(t);
		return NULL;
	} else {
		/* get index in 0..25 */
		index = *s - 'a';
		/* if not found, we just return */
		if(!t->edges[index]) {
			no_need = 1;
			return NULL;
		}
		/* shift the head of the string to continue recursing */
		s++;
		t->edges[index] = trie_delete(s, t->edges[index]);
		/* we need to delete all the nodes that lead to the one we removed
		 * if there are no other children */
		for (i = 0; i < 26; i++)
			if (t->edges[i] != NULL)
				all_null = 0;

		/* revert */
		first_time = 1;

		if (all_null) {
			free(t);
			return NULL;
		}
		else
			return t;
	}
}

/* traverse the trie and return the value if found (recursively) */
Book* trie_find(char *str, TrieNode *t)
{
	Book *b;
	char *s;
	int index;

	/* safety */
	if (!t)
		return NULL;

	if (first_time) {
		s = setup_string(str);
		first_time = 0;
	} else
		s = str;

	if (*s == '\0')
		/* found it */
		return t->value;
	else {
		/* get index in 0..25 */
		index = *s - 'a';
		/* if not found, we need to return NULL up the stack */
		if(!t->edges[index])
			return NULL;
		/* shift the head of the string to continue recursing */
		s++;
		b = trie_find(s, t->edges[index]);
		/* revert */
		first_time = 1;
		/* return whatever came back, Book or NULL */
		return b;
	}
}
