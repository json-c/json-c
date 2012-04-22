/** 
*******************************************************************************
* @file json_object_iterator.h
* 
* Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the MIT license. See COPYING for details.
* 
* @brief  cjson forces clients to use its private data
*         structures for JSON Object iteration.  This API
*         corrects that by abstracting the private cjson
*         details.
* 
*         The intention is to add this API (and its
*         implementation) to Palm's version of the cjson
*         library, at which point it can be removed from the
*         Wireless System Framework library implementation.
* 
* API attributes:
*   * Thread-safe: NO
*   * Re-entrant: NO
* 
*******************************************************************************
*/


#ifndef JSON_OBJECT_ITERATOR_H
#define JSON_OBJECT_ITERATOR_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * Forward declaration for the opaque iterator information.
 */
struct json_object_iter_info_;

/**
 * The opaque iterator that references a name/value pair within
 * a JSON Object intance or the "end" iterator value.
 */
struct json_object_iterator {
    const void* opaque_;
};


/**
 * forward declaration of cjson's JSON value instance structure
 */
struct json_object;


/**
 * Initializes an iterator structure to a "default" value that
 * is convenient for initializing an iterator variable to a
 * default state (e.g., initialization list in a class'
 * constructor).
 * 
 * @code 
 * struct json_object_iterator iter = json_object_iter_init_default();
 * MyClass() : iter_(json_object_iter_init_default())
 * @endcode
 * 
 * @note The initialized value doesn't reference any specific
 *       pair, is considered an invalid iterator, and MUST NOT
 *       be passed to any cjson API that expects a valid
 *       iterator.
 * 
 * @note User and internal code MUST NOT make any assumptions
 *       about and dependencies on the value of the "default"
 *       iterator value.
 * 
 * @return json_object_iterator
 */
struct json_object_iterator
json_object_iter_init_default(void);

/** Retrieves an iterator to the first pair of the JSON Object.
 * 
 * @note WARNING: Any modification of the underlying pair
 *       invalidates all iterators to that pair.
 * 
 * @param obj JSON Object instance (MUST be of type
 *             json_type_object)
 *
 * @return json_object_iterator If the JSON Object has at
 *              least one pair, on return, the iterator refers
 *              to the first pair. If the JSON Object doesn't
 *              have any pairs, the returned iterator is
 *              equivalent to the "end" iterator for the same
 *              JSON Object instance.
 * 
 * @code    
 * struct json_object_iterator it;
 * struct json_object_iterator itEnd;
 * struct json_object* obj = json_tokener_parse(
 *        "{'first':'george', 'age':100}");
 * json_object_iter_begin(obj, &it);
 * json_object_iter_end(obj, &itEnd);
 * while (!json_object_iter_equal(&it, &itEnd)) {
 *     printf("%s\n",
 *            json_object_iter_peek_name(&it));
 *     json_object_iter_next(&it);
 * }
 *
 * struct json_object* obj = json_tokener_parse(
 *        "{'first':'george', 'age':100}");
 * struct json_object_iterator it;
 * bool iterable = json_object_iter_begin(&it);
 * if (iterable) {
 *     do  {
 *         printf("%s\n", json_object_iter_peek_name(&it));
 *     } while (json_object_iter_next(&it));
 * }
 * @endcode
 */
struct json_object_iterator
json_object_iter_begin(struct json_object* obj);

/** Retrieves the iterator that represents the position beyond the
 *  last pair of the given JSON Object instance.
 * 
 *  @note WARNING: Do NOT write code that assumes that the "end"
 *        iterator value is NULL, even if it is so in a
 *        particular instance of the implementation.
 * 
 *  @note The reason we do not (and MUST NOT) provide
 *        "json_object_iter_is_end(json_object_iterator* iter)"
 *        type of API is because it would limit the underlying
 *        representation of name/value containment (or force us
 *        to add additional, otherwise unnecessary, fields to
 *        the iterator structure). The "end" iterator and the
 *        equality test method, on the other hand, permit us to
 *        cleanly abstract pretty much any reasonable underlying
 *        representation without burdening the iterator
 *        structure with unnecessary data.
 *
 *  @note For performance reasons, memoize the "end" iterator prior
 *        to any loop.
 * 
 * @param obj JSON Object instance (MUST be of type
 *             json_type_object)
 *
 * @return json_object_iterator On return, the iterator refers
 *              to the "end" of the Object instance's pairs
 *              (i.e., NOT the last pair, but "beyond the last
 *              pair" value)
 */
struct json_object_iterator
json_object_iter_end(const struct json_object* obj);

/** Returns an iterator to the next pair, if any
 * 
 * @note WARNING: Any modification of the underlying pair
 *       invalidates all iterators to that pair.
 * 
 * @param iter Iterator that references a name/value pair; 
 * 
 * @param iter [IN/OUT] Pointer to iterator that references a
 *         name/value pair; MUST be a valid, non-end iterator.
 *         WARNING: bad things will happen if invalid or "end"
 *         iterator is passed. Upon return will contain the
 *         reference to the next pair if there is one; if there
 *         are no more pairs, will contain the "end" iterator
 *         value, which may be compared against the return value
 *         of json_object_iter_end() for the same JSON Object
 *         instance.
 */
void
json_object_iter_next(struct json_object_iterator* iter);


/** Returns a const pointer to the name of the pair referenced
 *  by the given iterator.
 * 
 * @param iter pointer to iterator that references a name/value
 *             pair; MUST be a valid, non-end iterator.
 *             WARNING: bad things will happen if invalid or
 *             "end" iterator is passed.
 * 
 * @return const char* Pointer to the name of the rerferenced
 *         name/value pair.  The name memory belongs to the
 *         name/value pair, will be freed when the pair is
 *         deleted or modified, and MUST NOT be modified or
 *         freed by the user.
 */
const char*
json_object_iter_peek_name(const struct json_object_iterator* iter);


/** Returns a pointer to the cjson instance representing the
 *  value of the referenced name/value pair, without altering
 *  the instance's reference count.
 * 
 * @param iter pointer to iterator that references a name/value
 *             pair; MUST be a valid, non-end iterator.
 *             WARNING: bad things will happen if invalid or
 *             "end" iterator is passed.
 * 
 * @return struct json_object* Pointer to the cjson value
 *         instance of the referenced name/value pair;  the
 *         value's reference count is not changed by this
 *         function: if you plan to hold on to this cjson node,
 *         take a look at json_object_get() and
 *         json_object_put(). IMPORTANT: cjson API represents
 *         the JSON Null value as a NULL json_object instance
 *         pointer.
 */
struct json_object*
json_object_iter_peek_value(const struct json_object_iterator* iter);


/** Tests two iterators for equality.  Typically used to test
 *  for end of iteration by comparing an iterator to the
 *  corresponding "end" iterator (that was derived from the same
 *  JSON Object instance).
 * 
 *  @note The reason we do not (and MUST NOT) provide
 *        "json_object_iter_is_end(json_object_iterator* iter)"
 *        type of API is because it would limit the underlying
 *        representation of name/value containment (or force us
 *        to add additional, otherwise unnecessary, fields to
 *        the iterator structure). The equality test method, on
 *        the other hand, permits us to cleanly abstract pretty
 *        much any reasonable underlying representation.
 * 
 * @param iter1 Pointer to first valid, non-NULL iterator
 * @param iter2 POinter to second valid, non-NULL iterator
 * 
 * @note WARNING: if a NULL iterator pointer or an uninitialized
 *       or invalid iterator, or iterators derived from
 *       different JSON Object instances are passed, bad things
 *       will happen!
 * 
 * @return bool non-zero if iterators are equal (i.e., both
 *         reference the same name/value pair or are both at
 *         "end"); zero if they are not equal.
 */
bool
json_object_iter_equal(const struct json_object_iterator* iter1,
                       const struct json_object_iterator* iter2);


#ifdef __cplusplus
}
#endif


#endif // JSON_OBJECT_ITERATOR_H
