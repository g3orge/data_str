/**
 * This file is part of the Data Structures project (CEID)
 *
 * Copyright 2013 George "papanikge" Papanikolaou
 *
 * This is experimental software. Beware of Dragons.
 *
 * This project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with these files; If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * Implementing the database actions such as file parsing and
 * creating book structs
 */

#include "core.h"

/* create and return a book struct */
static Book* create_book(char* name, short year, char* pub)
{
	Book *b = smalloc(sizeof(Book));
	b->id = idSum;
	idSum++;
	/* the rest */
	strcpy(b->title, name);
	strcpy(b->publisher, pub);
	b->yearPublished = year;
	b->numberOfAuthors = 0;
	return b;
}

/* dynamically change (and returns) the authors array in a given book struct */
static Author* add_author(Book* b, char* f, char* l)
{
	Author *a;
	unsigned int index = b->numberOfAuthors;
	/* creating or reallocating the proper array */
	if (index == 0) {
		a = smalloc(sizeof(Author));
	} else {
		a = realloc(b->authors, (sizeof(Author)*(index+1)) );
		if (!a) fatal("while reallocating to add author");
	}
	/* now add the names and point back */
	strncpy(a[index].first, f, 56);
	strncpy(a[index].last,  l, 56);
	/* increment the counter */
	b->numberOfAuthors++;
	return a;
}

/* load and initialize db from file first menu option function */
void init_db(const char *file)
{
	/* temp buffers and descriptors */
	FILE *fd;
	unsigned int i;   /* for the temp index */
	unsigned int j;   /* for the line index */
	char line[256+56+56+4+40];
	char book_name[256];
	char writer[56*2];
	char first_name[56];
	char last_name[56];
	char the_year[4];
	char pub_name[40];
	idSum = 0;
	fd = fopen(file, "r");
	if (!fd)
		fatal("while opening data file");
	/* get the total number of books */
	if (!fgets(line, 8, fd))
		fatal("while reading data file");
	long n = atol(line);
	/* allocate the main database */
	db.arr = smalloc(sizeof(Book)*n);
	db.numberOfBooks = n;
	/* start parsing the file, delimiter is ';' */
	memset(line, 0, sizeof(line));
	printf("Loading file...\n");
	while (fgets(line, sizeof(Book), fd) != NULL) {
		/* erase the previous data */
		memset(book_name, 0, sizeof(book_name));
		memset(writer, 0, sizeof(writer));
		memset(first_name, 0, sizeof(first_name));
		memset(last_name, 0, sizeof(last_name));
		memset(the_year, 0, sizeof(the_year));
		memset(pub_name, 0, sizeof(pub_name));
		j = 0;
		/* first the name */
		i = 0;
		while (line[j] != ';' && j < 256) {
			/* don't wrap my strings in "" */
			if (line[j] == '"') {
				j++;
				continue;
			}
			book_name[i] = line[j];
			i++;
			j++;
		}
		j++;
		/* second the author */
		i = 0;
		while (line[j] != ';' && j < (56*2)) {
			if (line[j] == '"') {
				j++;
				continue;
			}
			writer[i] = line[j];
			i++;
			j++;
		}
		j++;
		/* next the year */
		i = 0;
		while (line[j] != ';' && j < 4) {
			if (line[j] == '"') {
				j++;
				continue;
			}
			the_year[i] = line[j];
			i++;
			j++;
		}
		j++;
		/* and last one is the publisher */
		i = 0;
		while (line[j] != '\n' && j < 40) {
			if (line[j] == '"') {
				j++;
				continue;
			}
			pub_name[i] = line[j];
			i++;
			j++;
		}
		/* done. final work */
		Book *B;
		B = create_book(book_name, atoi(the_year), pub_name);
		/* splitting names. (Multiple authors separated with commas) */
		j = 0;  /* using `i` and `j` as indexes again to save memory */
next:
		/**
		 * this is the entry point for every author loop, implemented with
		 * a goto label to save multiple nested while loops
		 */
		i = 0;
		while (writer[j] != '\0' && writer[j] != ',' && writer[j] != ' ' && j < 56) {
			first_name[i] = writer[j];
			j++;
			i++;
		}
		/* if we got a space, we go for last name */
		if (writer[j] == ' ') {
			i = 0;
			j++;
			while (writer[j] != '\0' && writer[j] != ',' && j < 56) {
				last_name[i] = writer[j];
				j++;
				i++;
			}
		}
		/* add him */
		B->authors = add_author(B, first_name, last_name);
		if (writer[j] == ',') {
			/* with comma, we go for second author */
			j++;
			goto next;
		}

		/* put it in the actual database */
		/* the id is incremented inside create_book() */
		db.arr[B->id] = *B;
		memset(line, 0, sizeof(line));
	}
	fclose(fd);
}

/* write the dynamic db back to the provided file */
void save_db(const char *file)
{
	FILE *fd;
	long i;
	int n;
	int j;
	char *name;

	fd = fopen(file, "w");
	if (!fd)
		fatal("while opening data file for writing");
	/* first we write the total number of books */
	fprintf(fd, "%d\n", (int)idSum);
	/* iterate over the db and add to file */
	for (i=0; i < idSum; i++) {
		/* concatenate the first and last name of every author */
		n = db.arr[i].numberOfAuthors;
		name = (char *)smalloc(sizeof(char) * 56 * 2 * n);
		for (j=1; j <= n; j++) {
			strncpy(name, db.arr[i].authors[j-1].first, 56);
			strncpy(name, db.arr[i].authors[j-1].last,  56);
		}
		fprintf(fd, "%s;%s;%d;%s\n",
					db.arr[i].title,
					name,
					db.arr[i].yearPublished,
					db.arr[i].publisher);
	}
	fclose(fd);
}
