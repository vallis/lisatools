/* ezxml.h
 *
 * Copyright 2004, 2005 Aaron Voisine <aaron@voisine.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _EZXML_H
#define _EZXML_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

 /*!\var EZXML_BUFSIZE 
   \brief size of internal memory buffers*/
#define EZXML_BUFSIZE 1024
 /*!\var EZXML_NAMEM 
   \brief name is malloced*/
#define EZXML_NAMEM   0x80
 /*!\var EZXML_NAMEM
   \brief txt is malloced*/
#define EZXML_TXTM    0x40
 /*!\var EZXML_TXTM
   \brief attribute name and value are strduped*/
#define EZXML_DUP     0x20

/*!\brief XML manipulation structure*/

typedef struct ezxml *ezxml_t;
struct ezxml {
  /*!\brief Tag name. */
    char *name;      
/*!\brief Tag attributes { name, value, name, value, ... NULL }. */
    char **attr;     
/*!\brief Tag character content, empty string if none. */
    char *txt;       
/*!\brief Tag offset from start of parent tag character content. */
    size_t off;      
/*!\brief Next tag with same name in this section at this depth. */
    ezxml_t next;    
/*!\brief Next tag with different name in same section and depth. */
    ezxml_t sibling; 
/*!\brief next tag, same section and depth, in original order. */
    ezxml_t ordered; 
/*!\brief Head of sub tag list, NULL if none. */
    ezxml_t child;   
/*!\brief Parent tag, NULL if current tag is root tag. */
    ezxml_t parent;  
    /*!\var  flags
   \brief Additional information. */
    short flags; 
};

/*!\brief Given a string of xml data and its length, parses it and creates an ezxml
 structure. For efficiency, modifies the data by adding null terminators
 and decoding ampersand sequences. If you don't want this, copy the data and
 pass in the copy. Returns NULL on failure. */
ezxml_t ezxml_parse_str(char *s, size_t len);

/*!\brief A wrapper for ezxml_parse_str() that accepts a file descriptor. First
 attempts to mem map the file. Failing that, reads the file into memory.
 Returns NULL on failure. */
ezxml_t ezxml_parse_fd(int fd);

  /*!\brief a wrapper for ezxml_parse_fd() that accepts a file name */
ezxml_t ezxml_parse_file(const char *file);
    
/*!\brief Wrapper for ezxml_parse_str() that accepts a file stream. Reads the entire
 stream into memory and then parses it. For xml files, use ezxml_parse_file()
 or ezxml_parse_fd() */
ezxml_t ezxml_parse_fp(FILE *fp);

/*!\brief Returns the first child tag (one level deeper) with the given name or NULL
  if not found*/
ezxml_t ezxml_child(ezxml_t xml, const char *name);

  /*!\brief Returns the next tag of the same name in the same section and depth or NULL
    if not found*/
#define ezxml_next(xml) ((xml) ? xml->next : NULL)

/*!\brief Returns the Nth tag with the same name in the same section at the same depth
  or NULL if not found. An index of 0 returns the tag given.*/
ezxml_t ezxml_idx(ezxml_t xml, int idx);

  /*!\brief Returns the name of the given tag*/
#define ezxml_name(xml) ((xml) ? xml->name : NULL)

  /*!\brief Returns the given tag's character content or empty string if none*/
#define ezxml_txt(xml) ((xml) ? xml->txt : "")

  /*!\brief Returns the value of the requested tag attribute, or NULL if not found*/
const char *ezxml_attr(ezxml_t xml, const char *attr);

/*!\brief Traverses the ezxml sturcture to retrieve a specific subtag. 
 *
 * Takes a variable length list of tag names and indexes.
 * The argument list must be terminated by either an index of -1 or an empty string tag name.
 * Example: title = ezxml_get(library, "shelf", 0, "book", 2, "title", -1);
 * This retrieves the title of the 3rd book on the 1st shelf of library.
 * Returns NULL if not found.*/
ezxml_t ezxml_get(ezxml_t xml, ...);

/*!\brief Converts an ezxml structure back to xml. 
 * Returns a string of xml data that
 must be freed.*/
char *ezxml_toxml(ezxml_t xml);

/*!\brief Returns a NULL terminated array of processing instructions for the given
 target. */
const char **ezxml_pi(ezxml_t xml, const char *target);

  /*!\brief Frees the memory allocated for an ezxml structure. */
void ezxml_free(ezxml_t xml);
    
  /*!\brief Returns parser error message or empty string if none. */
const char *ezxml_error(ezxml_t xml);

  /*!\brief Returns a new empty ezxml structure with the given root tag name. */
ezxml_t ezxml_new(const char *name);

  /*!\brief Wrapper for ezxml_new() that strdup()s names. */
#define ezxml_new_d(name) ezxml_set_flag(ezxml_new(strdup(name)), EZXML_NAMEM)

/*!\brief Adds a child tag. off is the offset of the child tag relative to the start
 of the parent tag's character content. Returns the child tag.*/
ezxml_t ezxml_add_child(ezxml_t xml, const char *name, size_t off);

  /*!\brief Xrapper for ezxml_add_child() that strdup()s name. */
#define ezxml_add_child_d(xml, name, off) \
    ezxml_set_flag(ezxml_add_child(xml, strdup(name), off), EZXML_NAMEM)

  /*!\brief Sets the character content for the given tag and returns the tag. */
ezxml_t ezxml_set_txt(ezxml_t xml, const char *txt);

  /*!\brief Wrapper for ezxml_set_txt() that strdup()s txt. */
#define ezxml_set_txt_d(xml, txt) \
    ezxml_set_flag(ezxml_set_txt(xml, strdup(txt)), EZXML_TXTM)

/*!\brief Sets the given tag attribute or adds a new attribute if not found. A value
 of NULL will remove the specified attribute.*/
void ezxml_set_attr(ezxml_t xml, const char *name, const char *value);

  /*!\brief Wrapper for ezxml_set_attr() that strdup()s name/value. Value cannot be NULL.  */
#define ezxml_set_attr_d(xml, name, value) \
    ezxml_set_attr(ezxml_set_flag(xml, EZXML_DUP), strdup(name), strdup(value))

  /*!\brief Sets a flag for the given tag and returns the tag. */
ezxml_t ezxml_set_flag(ezxml_t xml, short flag);

  /*!\brief Removes a tag along with all its subtags. */
void ezxml_remove(ezxml_t xml);

#ifdef __cplusplus
}
#endif

#endif // _EZXML_H
