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

#include <ofi_map.h>

static void ofi_map_delete(struct ofi_map *map, struct ofi_rbnode *node)
{
	map->remove(node->data);
	ofi_buf_free(node->data);
	ofi_rbmap_delete(map->rbmap, node);
}

int ofi_map_init(struct ofi_map *map, struct ofi_map_attr *attr)
{
	int ret;
	map = malloc(sizeof(*map));
	if (!map)
		return -FI_ENOMEM;

	map->insert = attr->insert;
	map->remove = attr->remove;

	map->rbmap = ofi_rbmap_create(attr->compare);
	if (!(map->rbmap)) {
		free(map);
		return -FI_ENOMEM;
	}

	ret = ofi_bufpool_create(&(map->pool), attr->entry_size, 16, 0, 0, 0);
	if (ret) {
        ofi_rbmap_destroy(map->rbmap);
        free(map);
	}

	return ret;
}

void *ofi_map_insert(struct ofi_map *map, void *key)
{
	struct ofi_rbnode *node;
	void *data;
	int ret;

	ret = ofi_rbmap_insert(map->rbmap, key, NULL, &node);
	if (!ret) {
		data = ofi_buf_alloc(map->pool);
		map->insert(key, data);
		node->data = data; 
	} else if (ret != -FI_EALREADY) {
		return NULL;
	}

	return node->data;
}

void *ofi_map_find(struct ofi_map *map, void *key)
{
	return ofi_rbmap_find(map->rbmap, key)->data;
}

int ofi_map_remove(struct ofi_map *map, void *key)
{
	struct ofi_rbnode *node;
	node = ofi_map_find(map, key);
	if (!node)
		return -FI_ENODATA;

	ofi_map_delete(map, node);

	return 0;
}

void ofi_map_free(struct ofi_map *map)
{
	struct ofi_rbnode *node;
	while (!ofi_rbmap_empty(map->rbmap)) {
		node = map->rbmap->root;
	   	ofi_map_delete(map, node);
	}

	ofi_bufpool_destroy(map->pool);
	ofi_rbmap_cleanup(map->rbmap);
	free(map);
}
