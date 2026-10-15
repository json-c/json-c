/*
 * Regression test: json_tokener_parse_ex() must not leak the value object it
 * is about to insert into a parent container when json_object_object_add() or
 * json_object_array_add() fails (for example under memory pressure).
 *
 * Historically the tokener detached the completed value from its parse level
 * and then, if the insertion into the parent failed, jumped to the error exit
 * without releasing the value's last reference, leaking it.
 *
 * The test injects an allocation failure at every allocation step of a parse
 * and verifies that nothing is left allocated afterwards.  To make allocation
 * failures deterministic it provides its own malloc/calloc/realloc/free/
 * strdup/vasprintf, so it is not built on platforms where replacing the C
 * runtime allocator is unreliable (Windows, macOS).
 */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "json_tokener.h"

/*
 * Counting bump allocator.  Freed memory is not reused (these tests allocate
 * a few kilobytes per run), but every allocation and release is accounted so
 * leaks are visible as a non-zero live count.  Each allocation carries a size
 * header so realloc() can copy the previous contents.
 */
#define ARENA_SIZE (4 * 1024 * 1024)

static union {
	long double align;
	void *ptr;
	char bytes[ARENA_SIZE];
} arena_u;

static size_t arena_off;
static size_t live_count;
static size_t alloc_seq;
static size_t fail_at;

static void *oom_alloc(size_t size)
{
	/* keep payloads 16-byte aligned, with a size header in front */
	size_t header = (sizeof(size_t) + 15) & ~(size_t)15;
	size_t need = header + size;

	alloc_seq++;
	if (fail_at && alloc_seq == fail_at)
		return NULL;
	if (arena_off + need > ARENA_SIZE)
	{
		fputs("test_tokener_oom: arena exhausted\n", stderr);
		abort();
	}
	{
		unsigned char *base = (unsigned char *)arena_u.bytes + arena_off;
		arena_off += need;
		*(size_t *)base = size;
		live_count++;
		return base + header;
	}
}

static void oom_release(void *ptr)
{
	if (!ptr)
		return;
	assert(live_count > 0);
	live_count--;
	/* bump allocator: memory is not reused */
}

static size_t oom_size_of(void *ptr)
{
	unsigned char *base = (unsigned char *)ptr;
	size_t header = (sizeof(size_t) + 15) & ~(size_t)15;
	return *(size_t *)(base - header);
}

void *malloc(size_t size)
{
	if (size == 0)
		size = 1;
	return oom_alloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
	size_t total;
	void *p;

	if (nmemb != 0 && size > (size_t)-1 / nmemb)
		return NULL;
	total = nmemb * size;
	if (total == 0)
		total = 1;
	p = oom_alloc(total);
	if (p)
		memset(p, 0, total);
	return p;
}

void *realloc(void *ptr, size_t size)
{
	void *np;
	size_t old_size;

	if (!ptr)
		return malloc(size);
	if (size == 0)
	{
		oom_release(ptr);
		return NULL;
	}
	np = oom_alloc(size);
	if (!np)
		return NULL; /* original pointer stays valid */
	old_size = oom_size_of(ptr);
	memcpy(np, ptr, old_size < size ? old_size : size);
	oom_release(ptr);
	return np;
}

void free(void *ptr)
{
	oom_release(ptr);
}

char *strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *p = (char *)oom_alloc(len);

	if (!p)
		return NULL;
	memcpy(p, s, len);
	return p;
}

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	va_list ap2;
	int len;
	char *p;

	va_copy(ap2, ap);
	len = vsnprintf(NULL, 0, fmt, ap2);
	va_end(ap2);
	if (len < 0)
	{
		*strp = NULL;
		return -1;
	}
	p = (char *)oom_alloc((size_t)len + 1);
	if (!p)
	{
		*strp = NULL;
		return -1;
	}
	vsnprintf(p, (size_t)len + 1, fmt, ap);
	*strp = p;
	return len;
}

static const char *test_docs[] = {
	/* values are attached to objects and arrays through both failing paths */
	"{\"a\":1,\"b\":[true,false,null],\"c\":{\"d\":\"str\",\"e\":123}}",
	"[[1,2],[3,[4,5]],{\"x\":\"y\"}]",
	"{\"k\":{\"nested\":{\"deeper\":[\"s\",1.5,-2,true]}}}",
	"[\"a string that is long enough to need a heap buffer\","
	"\"another one\",{\"k\":9}]",
	"{\"u\":\"\\u00e9\\u0041\",\"n\":-42,\"big\":18446744073709551615}",
};

int main(void)
{
	size_t doc_i;

	for (doc_i = 0; doc_i < sizeof(test_docs) / sizeof(test_docs[0]); doc_i++)
	{
		const char *doc = test_docs[doc_i];
		int doc_len = (int)strlen(doc);
		size_t n;

		for (n = 0; n < 400; n++)
		{
			struct json_tokener *tok;
			struct json_object *obj;

			arena_off = 0;
			live_count = 0;
			alloc_seq = 0;
			fail_at = n;

			tok = json_tokener_new_ex(JSON_TOKENER_DEFAULT_DEPTH);
			if (!tok)
				continue; /* allocation failed as injected */
			obj = json_tokener_parse_ex(tok, doc, doc_len);
			json_object_put(obj);
			json_tokener_free(tok);

			/* No allocation from the attempt may remain. */
			if (live_count != 0)
			{
				fprintf(stderr,
				        "LEAK: doc %zu, failing allocation %zu: "
				        "%zu allocation(s) left\n",
				        doc_i, n, live_count);
				return 1;
			}
		}
	}

	/* Sanity: the documents must parse when no failure is injected. */
	fail_at = 0;
	for (doc_i = 0; doc_i < sizeof(test_docs) / sizeof(test_docs[0]); doc_i++)
	{
		struct json_tokener *tok = json_tokener_new();
		struct json_object *obj;

		assert(tok != NULL);
		obj = json_tokener_parse_ex(tok, test_docs[doc_i],
		                            (int)strlen(test_docs[doc_i]));
		assert(obj != NULL);
		assert(json_tokener_get_error(tok) == json_tokener_success);
		json_object_put(obj);
		json_tokener_free(tok);
	}

	puts("PASS");
	return 0;
}
