/*
 * Copyright (c) 2021 Alexadru Ardelean.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

/**
 * @file
 * @brief JSON Patch (RFC 6902) implementation for manipulating JSON objects
 */
#ifndef _json_patch_h_
#define _json_patch_h_

#include "json_pointer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Apply the JSON patch to the base object.
 * The patch object must be formatted as per RFC 6902.
 * If the patch is not correctly formatted, an error will
 * be returned.
 *
 * The original `base` object will first be copied, and then
 * the patch will be applied.
 * If anything fails during patching, the `res` object will be
 * NULL and the function will return a negative result.
 *
 * @param base the JSON object which to patch
 * @param patch the JSON object that describes the patch to be applied
 * @param the resulting patched JSON object
 *
 * @return negative if an error (or not found), or 0 if succeeded
 */
JSON_EXPORT int json_patch_apply(struct json_object *base, struct json_object *patch,
                                 struct json_object **res);

#ifdef __cplusplus
}
#endif

#endif
