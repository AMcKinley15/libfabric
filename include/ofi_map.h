/*
 * Copyright (c) 2019 Intel Corporation, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <ofi.h>
#include <ofi_tree.h>
#include <ofi_mem.h>

struct ofi_map_attr {
	size_t entry_size;
	void (*insert)(void *data, void *key);
	void (*remove)(void *data);
	int  (*compare)(struct ofi_rbmap *map, void *key, void *data);
};

struct ofi_map {
	struct ofi_rbmap *rbmap;
	struct ofi_bufpool *pool;
	void (*insert)(void *data, void *key);
	void (*remove)(void *data);
};

int ofi_map_init(struct ofi_map *map, struct ofi_map_attr *attr);
void *ofi_map_insert(struct ofi_map *map, void *key);
void *ofi_map_find(struct ofi_map *map, void *key);
int ofi_map_remove(struct ofi_map *map, void *key);
void ofi_map_free(struct ofi_map *map);
