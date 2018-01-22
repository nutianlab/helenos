/*
 * Copyright (c) 2011 Jiri Zarevucky
 * Copyright (c) 2011 Petr Koupy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup libposix
 * @{
 */
/** @file Standard buffered input/output.
 */

#define LIBPOSIX_INTERNAL
#define __POSIX_DEF__(x) posix_##x

#include "internal/common.h"
#include "posix/stdio.h"

#include <assert.h>

#include <errno.h>

#include "posix/stdlib.h"
#include "posix/string.h"
#include "posix/sys/types.h"
#include "posix/unistd.h"

#include "libc/stdio.h"
#include "libc/io/printf_core.h"
#include "libc/str.h"
#include "libc/malloc.h"
#include "libc/adt/list.h"

/** Clears the stream's error and end-of-file indicators.
 *
 * @param stream Stream whose indicators shall be cleared.
 */
void posix_clearerr(FILE *stream)
{
	clearerr(stream);
}

/**
 * Generate a pathname for the controlling terminal.
 *
 * @param s Allocated buffer to which the pathname shall be put.
 * @return Either s or static location filled with the requested pathname.
 */
char *posix_ctermid(char *s)
{
	/* Currently always returns an error value (empty string). */
	// TODO: return a real terminal path

	static char dummy_path[L_ctermid] = {'\0'};

	if (s == NULL) {
		return dummy_path;
	}

	s[0] = '\0';
	return s;
}

/**
 * Put a string on the stream.
 * 
 * @param s String to be written.
 * @param stream Output stream.
 * @return Non-negative on success, EOF on failure.
 */
int posix_fputs(const char *restrict s, FILE *restrict stream)
{
	return fputs(s, stream);
}

/**
 * Push byte back into input stream.
 * 
 * @param c Byte to be pushed back.
 * @param stream Stream to where the byte shall be pushed.
 * @return Provided byte on success or EOF if not possible.
 */
int posix_ungetc(int c, FILE *stream)
{
	return ungetc(c, stream);
}

/**
 * Read a stream until the delimiter (or EOF) is encountered.
 *
 * @param lineptr Pointer to the output buffer in which there will be stored
 *     nul-terminated string together with the delimiter (if encountered).
 *     Will be resized if necessary.
 * @param n Pointer to the size of the output buffer. Will be increased if
 *     necessary.
 * @param delimiter Delimiter on which to finish reading the stream.
 * @param stream Input stream.
 * @return Number of fetched characters (including delimiter if encountered)
 *     or -1 on error (set in errno).
 */
ssize_t posix_getdelim(char **restrict lineptr, size_t *restrict n,
    int delimiter, FILE *restrict stream)
{
	/* Check arguments for sanity. */
	if (!lineptr || !n) {
		errno = EINVAL;
		return -1;
	}

	size_t alloc_step = 80; /* Buffer size gain during reallocation. */
	char *pos = *lineptr; /* Next free byte of the output buffer. */
	size_t cnt = 0; /* Number of fetched characters. */
	int c = fgetc(stream); /* Current input character. Might be EOF. */

	do {
		/* Mask EOF as NUL to terminate string. */
		if (c == EOF) {
			c = '\0';
		}

		/* Ensure there is still space left in the buffer. */
		if (pos == *lineptr + *n) {
			*lineptr = realloc(*lineptr, *n + alloc_step);
			if (*lineptr) {
				pos = *lineptr + *n;
				*n += alloc_step;
			} else {
				errno = ENOMEM;
				return -1;
			}
		}

		/* Store the fetched character. */
		*pos = c;

		/* Fetch the next character according to the current character. */
		if (c != '\0') {
			++pos;
			++cnt;
			if (c == delimiter) {
				/* Delimiter was just stored. Provide EOF as the next
				 * character - it will be masked as NUL and output string
				 * will be properly terminated. */
				c = EOF;
			} else {
				/* Neither delimiter nor EOF were encountered. Just fetch
				 * the next character from the stream. */
				c = fgetc(stream);
			}
		}
	} while (c != '\0');

	if (errno == EOK && cnt > 0) {
		return cnt;
	} else {
		/* Either some error occured or the stream was already at EOF. */
		return -1;
	}
}

/**
 * Read a stream until the newline (or EOF) is encountered.
 * 
 * @param lineptr Pointer to the output buffer in which there will be stored
 *     nul-terminated string together with the delimiter (if encountered).
 *     Will be resized if necessary.
 * @param n Pointer to the size of the output buffer. Will be increased if
 *     necessary.
 * @param stream Input stream.
 * @return Number of fetched characters (including newline if encountered)
 *     or -1 on error (set in errno).
 */
ssize_t posix_getline(char **restrict lineptr, size_t *restrict n,
    FILE *restrict stream)
{
	return posix_getdelim(lineptr, n, '\n', stream);
}

/**
 * Reopen a file stream.
 * 
 * @param filename Pathname of a file to be reopened or NULL for changing
 *     the mode of the stream.
 * @param mode Mode to be used for reopening the file or changing current
 *     mode of the stream.
 * @param stream Current stream associated with the opened file.
 * @return On success, either a stream of the reopened file or the provided
 *     stream with a changed mode. NULL otherwise.
 */
FILE *posix_freopen(const char *restrict filename, 
    const char *restrict mode, FILE *restrict stream)
{
	return freopen(filename, mode, stream);
}

/**
 * Write error messages to standard error.
 *
 * @param s Error message.
 */
void posix_perror(const char *s)
{
	if (s == NULL || s[0] == '\0') {
		fprintf(stderr, "%s\n", posix_strerror(errno));
	} else {
		fprintf(stderr, "%s: %s\n", s, posix_strerror(errno));
	}
}

/** Restores stream a to position previously saved with fgetpos().
 *
 * @param stream Stream to restore
 * @param pos Position to restore
 * @return Zero on success, non-zero (with errno set) on failure
 */
int posix_fsetpos(FILE *stream, const posix_fpos_t *pos)
{
	return fseek64(stream, pos->offset, SEEK_SET);
}

/** Saves the stream's position for later use by fsetpos().
 *
 * @param stream Stream to save
 * @param pos Place to store the position
 * @return Zero on success, non-zero (with errno set) on failure
 */
int posix_fgetpos(FILE *restrict stream, posix_fpos_t *restrict pos)
{
	off64_t ret = ftell64(stream);
	if (ret != -1) {
		pos->offset = ret;
		return 0;
	} else {
		return -1;
	}
}

/**
 * Reposition a file-position indicator in a stream.
 * 
 * @param stream Stream to seek in.
 * @param offset Direction and amount of bytes to seek.
 * @param whence From where to seek.
 * @return Zero on success, -1 otherwise.
 */
int posix_fseek(FILE *stream, long offset, int whence)
{
	return fseek(stream, offset, whence);
}

/**
 * Reposition a file-position indicator in a stream.
 * 
 * @param stream Stream to seek in.
 * @param offset Direction and amount of bytes to seek.
 * @param whence From where to seek.
 * @return Zero on success, -1 otherwise.
 */
int posix_fseeko(FILE *stream, posix_off_t offset, int whence)
{
	return fseek64(stream, offset, whence);
}

/**
 * Discover current file offset in a stream.
 * 
 * @param stream Stream for which the offset shall be retrieved.
 * @return Current offset or -1 if not possible.
 */
long posix_ftell(FILE *stream)
{
	return ftell(stream);
}

/**
 * Discover current file offset in a stream.
 * 
 * @param stream Stream for which the offset shall be retrieved.
 * @return Current offset or -1 if not possible.
 */
posix_off_t posix_ftello(FILE *stream)
{
	return ftell64(stream);
}

/**
 * Discard prefetched data or write unwritten data.
 * 
 * @param stream Stream that shall be flushed.
 * @return Zero on success, EOF on failure.
 */
int posix_fflush(FILE *stream)
{
	return fflush(stream);
}

/**
 * Print formatted output to the opened file.
 *
 * @param fildes File descriptor of the opened file.
 * @param format Format description.
 * @return Either the number of printed characters or negative value on error.
 */
int posix_dprintf(int fildes, const char *restrict format, ...)
{
	va_list list;
	va_start(list, format);
	int result = posix_vdprintf(fildes, format, list);
	va_end(list);
	return result;
}

/**
 * Write ordinary string to the opened file.
 *
 * @param str String to be written.
 * @param size Size of the string (in bytes)..
 * @param fd File descriptor of the opened file.
 * @return The number of written characters.
 */
static int _dprintf_str_write(const char *str, size_t size, void *fd)
{
	const int fildes = *(int *) fd;
	size_t wr;
	if (failed(vfs_write(fildes, &posix_pos[fildes], str, size, &wr)))
		return -1;
	return str_nlength(str, wr);
}

/**
 * Write wide string to the opened file.
 * 
 * @param str String to be written.
 * @param size Size of the string (in bytes).
 * @param fd File descriptor of the opened file.
 * @return The number of written characters.
 */
static int _dprintf_wstr_write(const wchar_t *str, size_t size, void *fd)
{
	size_t offset = 0;
	size_t chars = 0;
	size_t sz;
	char buf[4];
	
	while (offset < size) {
		sz = 0;
		if (chr_encode(str[chars], buf, &sz, sizeof(buf)) != EOK) {
			break;
		}
		
		const int fildes = *(int *) fd;
		size_t nwr;
		if (vfs_write(fildes, &posix_pos[fildes], buf, sz, &nwr) != EOK)
			break;
		
		chars++;
		offset += sizeof(wchar_t);
	}
	
	return chars;
}

/**
 * Print formatted output to the opened file.
 * 
 * @param fildes File descriptor of the opened file.
 * @param format Format description.
 * @param ap Print arguments.
 * @return Either the number of printed characters or negative value on error.
 */
int posix_vdprintf(int fildes, const char *restrict format, va_list ap)
{
	printf_spec_t spec = {
		.str_write = _dprintf_str_write,
		.wstr_write = _dprintf_wstr_write,
		.data = &fildes
	};
	
	return printf_core(format, &spec, ap);
}

/**
 * Print formatted output to the string.
 * 
 * @param s Output string.
 * @param format Format description.
 * @return Either the number of printed characters (excluding null byte) or
 *     negative value on error.
 */
int posix_sprintf(char *s, const char *restrict format, ...)
{
	va_list list;
	va_start(list, format);
	int result = posix_vsprintf(s, format, list);
	va_end(list);
	return result;
}

/**
 * Print formatted output to the string.
 * 
 * @param s Output string.
 * @param format Format description.
 * @param ap Print arguments.
 * @return Either the number of printed characters (excluding null byte) or
 *     negative value on error.
 */
int posix_vsprintf(char *s, const char *restrict format, va_list ap)
{
	return vsnprintf(s, STR_NO_LIMIT, format, ap);
}

/**
 * Convert formatted input from the stream.
 * 
 * @param stream Input stream.
 * @param format Format description.
 * @return The number of converted output items or EOF on failure.
 */
int posix_fscanf(FILE *restrict stream, const char *restrict format, ...)
{
	va_list list;
	va_start(list, format);
	int result = posix_vfscanf(stream, format, list);
	va_end(list);
	return result;
}

/**
 * Convert formatted input from the standard input.
 * 
 * @param format Format description.
 * @return The number of converted output items or EOF on failure.
 */
int posix_scanf(const char *restrict format, ...)
{
	va_list list;
	va_start(list, format);
	int result = posix_vscanf(format, list);
	va_end(list);
	return result;
}

/**
 * Convert formatted input from the standard input.
 * 
 * @param format Format description.
 * @param arg Output items.
 * @return The number of converted output items or EOF on failure.
 */
int posix_vscanf(const char *restrict format, va_list arg)
{
	return posix_vfscanf(stdin, format, arg);
}

/**
 * Convert formatted input from the string.
 * 
 * @param s Input string.
 * @param format Format description.
 * @return The number of converted output items or EOF on failure.
 */
int posix_sscanf(const char *restrict s, const char *restrict format, ...)
{
	va_list list;
	va_start(list, format);
	int result = posix_vsscanf(s, format, list);
	va_end(list);
	return result;
}

/**
 * Acquire file stream for the thread.
 *
 * @param file File stream to lock.
 */
void posix_flockfile(FILE *file)
{
	/* dummy */
}

/**
 * Acquire file stream for the thread (non-blocking).
 *
 * @param file File stream to lock.
 * @return Zero for success and non-zero if the lock cannot be acquired.
 */
int posix_ftrylockfile(FILE *file)
{
	/* dummy */
	return 0;
}

/**
 * Relinquish the ownership of the locked file stream.
 *
 * @param file File stream to unlock.
 */
void posix_funlockfile(FILE *file)
{
	/* dummy */
}

/**
 * Get a byte from a stream (thread-unsafe).
 *
 * @param stream Input file stream.
 * @return Either read byte or EOF.
 */
int posix_getc_unlocked(FILE *stream)
{
	return getc(stream);
}

/**
 * Get a byte from the standard input stream (thread-unsafe).
 *
 * @return Either read byte or EOF.
 */
int posix_getchar_unlocked(void)
{
	return getchar();
}

/**
 * Put a byte on a stream (thread-unsafe).
 *
 * @param c Byte to output.
 * @param stream Output file stream.
 * @return Either written byte or EOF.
 */
int posix_putc_unlocked(int c, FILE *stream)
{
	return putc(c, stream);
}

/**
 * Put a byte on the standard output stream (thread-unsafe).
 * 
 * @param c Byte to output.
 * @return Either written byte or EOF.
 */
int posix_putchar_unlocked(int c)
{
	return putchar(c);
}

/**
 * Remove a file or directory.
 *
 * @param path Pathname of the file that shall be removed.
 * @return Zero on success, -1 (with errno set) otherwise.
 */
int posix_remove(const char *path)
{
	if (failed(vfs_unlink_path(path)))
		return -1;
	else
		return 0;
}

/**
 * Rename a file or directory.
 *
 * @param old Old pathname.
 * @param new New pathname.
 * @return Zero on success, -1 (with errno set) otherwise.
 */
int posix_rename(const char *old, const char *new)
{
	if (failed(vfs_rename_path(old, new)))
		return -1;
	else
		return 0;
}

/**
 * Get a unique temporary file name (obsolete).
 *
 * @param s Buffer for the file name. Must be at least L_tmpnam bytes long.
 * @return The value of s on success, NULL on failure.
 */
char *posix_tmpnam(char *s)
{
	assert(L_tmpnam >= posix_strlen("/tmp/tnXXXXXX"));
	
	static char buffer[L_tmpnam + 1];
	if (s == NULL) {
		s = buffer;
	}
	
	posix_strcpy(s, "/tmp/tnXXXXXX");
	posix_mktemp(s);
	
	if (*s == '\0') {
		/* Errno set by mktemp(). */
		return NULL;
	}
	
	return s;
}

/**
 * Get an unique temporary file name with additional constraints (obsolete).
 *
 * @param dir Path to directory, where the file should be created.
 * @param pfx Optional prefix up to 5 characters long.
 * @return Newly allocated unique path for temporary file. NULL on failure.
 */
char *posix_tempnam(const char *dir, const char *pfx)
{
	/* Sequence number of the filename. */
	static int seq = 0;
	
	size_t dir_len = posix_strlen(dir);
	if (dir[dir_len - 1] == '/') {
		dir_len--;
	}
	
	size_t pfx_len = posix_strlen(pfx);
	if (pfx_len > 5) {
		pfx_len = 5;
	}
	
	char *result = malloc(dir_len + /* slash*/ 1 +
	    pfx_len + /* three-digit seq */ 3 + /* .tmp */ 4 + /* nul */ 1);
	
	if (result == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	
	char *res_ptr = result;
	posix_strncpy(res_ptr, dir, dir_len);
	res_ptr += dir_len;
	posix_strncpy(res_ptr, pfx, pfx_len);
	res_ptr += pfx_len;
	
	for (; seq < 1000; ++seq) {
		snprintf(res_ptr, 8, "%03d.tmp", seq);
		
		int orig_errno = errno;
		errno = EOK;
		/* Check if the file exists. */
		if (posix_access(result, F_OK) == -1) {
			if (errno == ENOENT) {
				errno = orig_errno;
				break;
			} else {
				/* errno set by access() */
				return NULL;
			}
		}
	}
	
	if (seq == 1000) {
		free(result);
		errno = EINVAL;
		return NULL;
	}
	
	return result;
}

/**
 * Create and open an unique temporary file.
 * The file is automatically removed when the stream is closed.
 *
 * @param dir Path to directory, where the file should be created.
 * @param pfx Optional prefix up to 5 characters long.
 * @return Newly allocated unique path for temporary file. NULL on failure.
 */
FILE *posix_tmpfile(void)
{
	char filename[] = "/tmp/tfXXXXXX";
	int fd = posix_mkstemp(filename);
	if (fd == -1) {
		/* errno set by mkstemp(). */
		return NULL;
	}
	
	/* Unlink the created file, so that it's removed on close(). */
	posix_unlink(filename);
	return fdopen(fd, "w+");
}

/** @}
 */