/*
 * Copyright (c) 2021 Alexandru Ardelean.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "json_patch.h"
#include "json_object_private.h"

/**
 * JavaScript Object Notation (JSON) Patch
 *   RFC 6902 - https://tools.ietf.org/html/rfc6902
 */

static int json_patch_apply_test(struct json_object **res,
                                 struct json_object *patch_elem,
                                 const char *path)
{
	struct json_object *value1, *value2;

	if (!json_object_object_get_ex(patch_elem, "value", &value1)) {
		errno = EINVAL;
		return -1;
	}

	/* errno should be set by json_pointer_get() */
	if (json_pointer_get(*res, path, &value2))
		return -1;

	if (!json_object_equal(value1, value2)) {
		json_object_put(*res);
		*res = NULL;
		errno = ENOENT;
		return -1;
	}

	return 0;
}

static int __json_patch_apply_remove(struct json_pointer_get_result *jpres)
{
	if (json_object_is_type(jpres->parent, json_type_array)) {
		return json_object_array_del_idx(jpres->parent, jpres->id.index, 1);
	} else if (jpres->parent && jpres->id.key) {
		json_object_object_del(jpres->parent, jpres->id.key);
		return 0;
	} else {
		return json_object_put(jpres->obj);
	}
}

static int json_patch_apply_remove(struct json_object **res, const char *path)
{
	struct json_pointer_get_result jpres;

	if (json_pointer_get_internal(*res, path, &jpres))
		return -1;

	return __json_patch_apply_remove(&jpres);
}

static int json_object_array_insert_idx_cb(struct json_object *parent, size_t idx,
                                           struct json_object *value, void *priv)
{
	int *add = priv;

	if (idx > json_object_array_length(parent))
	{
		errno = EINVAL;
		return -1;
	}

	if (*add)
		return json_object_array_insert_idx(parent, idx, value);
	else
		return json_object_array_put_idx(parent, idx, value);
}

static int json_patch_apply_add_replace(struct json_object **res,
                                        struct json_object *patch_elem,
                                        const char *path, int add)
{
	struct json_object *value;
	int rc;

	if (!json_object_object_get_ex(patch_elem, "value", &value)) {
		errno = EINVAL;
		return -1;
	}
	/* if this is a replace op, then we need to make sure it exists before replacing */
	if (!add && json_pointer_get(*res, path, NULL)) {
		errno = ENOENT;
		return -1;
	}

	rc = json_pointer_set_with_array_cb(res, path, json_object_get(value),
					    json_object_array_insert_idx_cb, &add);
	if (rc)
		json_object_put(value);

	return rc;
}

static int json_object_array_move_cb(struct json_object *parent, size_t idx,
                                     struct json_object *value, void *priv)
{
	struct json_pointer_get_result *from = priv;
	size_t len = json_object_array_length(parent);

	/**
	 * If it's the same array parent, it means that we removed
	 * and element from it, so the length is temporarily reduced
	 * by 1, which means that if we try to move an element to
	 * the last position, we need to check the current length + 1
	 */
	if (parent == from->parent)
		len++;

	if (idx > len)
	{
		errno = EINVAL;
		return -1;
	}

	return json_object_array_insert_idx(parent, idx, value);
}

static int json_patch_apply_move_copy(struct json_object **res,
                                      struct json_object *patch_elem,
                                      const char *path, int move)
{
	json_pointer_array_set_cb array_set_cb;
	struct json_pointer_get_result from;
	struct json_object *jfrom;
	const char *from_s;
	size_t from_s_len;
	int rc;

	if (!json_object_object_get_ex(patch_elem, "from", &jfrom)) {
		errno = EINVAL;
		return -1;
	}

	from_s = json_object_get_string(jfrom);

	from_s_len = strlen(from_s);
	if (strncmp(from_s, path, from_s_len) == 0) {
		/**
		 * If lengths match, it's a noop, if they don't,
		 * then we're trying to move a parent under a child
		 * which is not allowed as per RFC 6902 section 4.4
		 *   The "from" location MUST NOT be a proper prefix of the "path"
		 *   location; i.e., a location cannot be moved into one of its children.
		 */
		if (from_s_len == strlen(path))
			return 0;
		errno = EINVAL;
		return -1;
	}

	rc = json_pointer_get_internal(*res, from_s, &from);
	if (rc)
		return rc;

	json_object_get(from.obj);

	if (!move) {
		array_set_cb = json_object_array_insert_idx_cb;
	} else {
		rc = __json_patch_apply_remove(&from);
		if (rc < 0) {
			json_object_put(from.obj);
			return rc;
		}
		array_set_cb = json_object_array_move_cb;
	}

	rc = json_pointer_set_with_array_cb(res, path, from.obj, array_set_cb, &from);
	if (rc)
		json_object_put(from.obj);

	return rc;
}

int json_patch_apply(struct json_object *base, struct json_object *patch,
                     struct json_object **res)
{
	size_t i;
	int rc = 0;

	if (!base || !json_object_is_type(patch, json_type_array)) {
		errno = EINVAL;
		return -1;
	}

	/* errno should be set inside json_object_deep_copy() */
	if (json_object_deep_copy(base, res, NULL) < 0)
		return -1;

	/* Go through all operations ; apply them on res */
	for (i = 0; i < json_object_array_length(patch); i++) {
		struct json_object *jop, *jpath;
		struct json_object *patch_elem = json_object_array_get_idx(patch, i);
		const char *op, *path;
		if (!json_object_object_get_ex(patch_elem, "op", &jop)) {
			errno = EINVAL;
			rc = -1;
			break;
		}
		op = json_object_get_string(jop);
		json_object_object_get_ex(patch_elem, "path", &jpath);
		path = json_object_get_string(jpath);

		if (!strcmp(op, "test"))
			rc = json_patch_apply_test(res, patch_elem, path);
		else if (!strcmp(op, "remove"))
			rc = json_patch_apply_remove(res, path);
		else if (!strcmp(op, "add"))
			rc = json_patch_apply_add_replace(res, patch_elem, path, 1);
		else if (!strcmp(op, "replace"))
			rc = json_patch_apply_add_replace(res, patch_elem, path, 0);
		else if (!strcmp(op, "move"))
			rc = json_patch_apply_move_copy(res, patch_elem, path, 1);
		else if (!strcmp(op, "copy"))
			rc = json_patch_apply_move_copy(res, patch_elem, path, 0);
		else {
			errno = EINVAL;
			rc = -1;
			break;
		}
		if (rc < 0)
			break;
	}

	if (rc < 0) {
		json_object_put(*res);
		*res = NULL;
	}

	return rc;
}
