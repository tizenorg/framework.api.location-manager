/*
 * Copyright (c) 2011-2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <location_bounds.h>
#include <locations_private.h>


#define LOCATIONS_NULL_ARG_CHECK(arg)	\
	LOCATIONS_CHECK_CONDITION((arg != NULL),LOCATION_BOUNDS_ERROR_INVALID_PARAMETER,"LOCATION_BOUNDS_ERROR_INVALID_PARAMETER") \

static void __free_position_list(gpointer data)
{
	if (data == NULL)
		return;

	LocationPosition *position = (LocationPosition *) data;
	location_position_free(position);
}

static location_bounds_type_e __convert_bounds_type(LocationBoundaryType type)
{
	location_bounds_type_e ret;
	switch (type) {
	case LOCATION_BOUNDARY_CIRCLE:
		ret = LOCATION_BOUNDS_CIRCLE;
		break;
	case LOCATION_BOUNDARY_POLYGON:
		ret = LOCATION_BOUNDS_POLYGON;
		break;
	case LOCATION_BOUNDARY_NONE:
	case LOCATION_BOUNDARY_RECT:
	default:
		ret = LOCATION_BOUNDS_RECT;
		break;
	}
	return ret;
}

EXPORT_API int location_bounds_create_rect(location_coords_s top_left, location_coords_s bottom_right, location_bounds_h * bounds)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_CHECK_CONDITION(top_left.latitude>=-90 && top_left.latitude<=90,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER,"LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(top_left.longitude>=-180 && top_left.longitude<=180,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER, "LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(bottom_right.latitude>=-90 && bottom_right.latitude<=90,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER, "LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(bottom_right.longitude>=-180 && bottom_right.longitude<=180,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER, "LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");

	LocationPosition *lt = location_position_new(0, top_left.latitude, top_left.longitude, 0, LOCATION_STATUS_2D_FIX);
	if (lt == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_position_new", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}

	LocationPosition *rb = location_position_new(0, bottom_right.latitude, bottom_right.longitude, 0, LOCATION_STATUS_2D_FIX);
	if (rb == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_position_new", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		location_position_free(lt);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}

	location_bounds_s *handle = (location_bounds_s *) malloc(sizeof(location_bounds_s));
	if (handle == NULL) {
		LOCATIONS_LOGE("OUT_OF_MEMORY(0x%08x)", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		location_position_free(rb);
		location_position_free(lt);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0, sizeof(location_bounds_s));

	handle->is_added = FALSE;
	handle->boundary = location_boundary_new_for_rect(lt, rb);
	location_position_free(rb);
	location_position_free(lt);
	if (handle->boundary == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_boundary_new_for_rect", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		free(handle);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}
	handle->user_cb = NULL;
	handle->user_data = NULL;

	*bounds = (location_bounds_h) handle;
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_create_circle(location_coords_s center, double radius, location_bounds_h * bounds)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_CHECK_CONDITION(radius>=0,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER,"LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(center.latitude>=-90 && center.latitude<=90,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER,"LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(center.longitude>=-180 && center.longitude<=180,LOCATION_BOUNDS_ERROR_INVALID_PARAMETER, "LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");

	LocationPosition *ct = location_position_new(0, center.latitude, center.longitude, 0, LOCATION_STATUS_2D_FIX);
	if (ct == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_position_new", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}

	location_bounds_s *handle = (location_bounds_s *) malloc(sizeof(location_bounds_s));
	if (handle == NULL) {
		LOCATIONS_LOGE("OUT_OF_MEMORY(0x%08x)", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		location_position_free(ct);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0, sizeof(location_bounds_s));

	handle->is_added = FALSE;
	handle->boundary = location_boundary_new_for_circle(ct, radius);
	location_position_free(ct);
	if (handle->boundary == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_boundary_new_for_circle", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		free(handle);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}
	handle->user_cb = NULL;
	handle->user_data = NULL;

	*bounds = (location_bounds_h) handle;
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_create_polygon(location_coords_s * coords_list, int length, location_bounds_h * bounds)
{
	LOCATIONS_NULL_ARG_CHECK(coords_list);
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_CHECK_CONDITION(length>=3, LOCATION_BOUNDS_ERROR_INVALID_PARAMETER, "LOCATION_BOUNDS_ERROR_INVALID_PARAMETER");

	int i;
	GList *position_list = NULL;
	LocationPosition *position = NULL;
	bool isValid;

	for (i = 0; i < length; i++) {
		if (coords_list[i].latitude < -90 || coords_list[i].latitude > 90 || coords_list[i].longitude < -180 || coords_list[i].longitude > 180) {
			LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_INVALID_PARAMETER(0x%08x)", LOCATION_BOUNDS_ERROR_INVALID_PARAMETER);
			isValid = FALSE;
			break;
		}
		position = location_position_new(0, coords_list[i].latitude, coords_list[i].longitude, 0.0, LOCATION_STATUS_2D_FIX);
		position_list = g_list_append(position_list, position);
		location_position_free(position);
		isValid = TRUE;
	}

	if (!isValid) {
		g_list_free_full(position_list, (GDestroyNotify) __free_position_list);
		return LOCATION_BOUNDS_ERROR_INVALID_PARAMETER;
	}

	if (position_list == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_position_new", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		g_list_free_full(position_list, (GDestroyNotify) __free_position_list);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}

	location_bounds_s *handle = (location_bounds_s *) malloc(sizeof(location_bounds_s));
	if (handle == NULL) {
		LOCATIONS_LOGE("OUT_OF_MEMORY(0x%08x)", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0, sizeof(location_bounds_s));

	handle->is_added = FALSE;
	handle->boundary = location_boundary_new_for_polygon(position_list);
	if (handle->boundary == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_boundary_new_for_polygon", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		free(handle);
		return LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY;
	}
	handle->user_cb = NULL;
	handle->user_data = NULL;

	*bounds = (location_bounds_h) handle;
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API bool location_bounds_contains_coordinates(location_bounds_h bounds, location_coords_s coords)
{
	if (!bounds)
		return FALSE;

	if (coords.latitude < -90 || coords.latitude > 90 || coords.longitude < -180 || coords.longitude > 180)
		return FALSE;

	LocationPosition *pos = location_position_new(0, coords.latitude, coords.longitude, 0, LOCATION_STATUS_2D_FIX);
	if (pos == NULL) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY(0x%08x) : fail to location_position_new", LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY);
		return FALSE;
	}
	location_bounds_s *handle = (location_bounds_s *) bounds;
	gboolean is_inside = location_boundary_if_inside(handle->boundary, pos);
	location_position_free(pos);
	bool result = is_inside ? TRUE : FALSE;
	return result;
}

EXPORT_API int location_bounds_get_type(location_bounds_h bounds, location_bounds_type_e * type)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_NULL_ARG_CHECK(type);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	*type = __convert_bounds_type(handle->boundary->type);
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_get_rect_coords(location_bounds_h bounds, location_coords_s * top_left, location_coords_s * bottom_right)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_NULL_ARG_CHECK(top_left);
	LOCATIONS_NULL_ARG_CHECK(bottom_right);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	if (__convert_bounds_type(handle->boundary->type) != LOCATION_BOUNDS_RECT) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_INCORRECT_TYPE(0x%08x)", LOCATION_BOUNDS_ERROR_INCORRECT_TYPE);
		return LOCATION_BOUNDS_ERROR_INCORRECT_TYPE;
	}

	top_left->latitude = handle->boundary->rect.left_top->latitude;
	top_left->longitude = handle->boundary->rect.left_top->longitude;
	bottom_right->latitude = handle->boundary->rect.right_bottom->latitude;
	bottom_right->longitude = handle->boundary->rect.right_bottom->longitude;
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_get_circle_coords(location_bounds_h bounds, location_coords_s * center, double *radius)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_NULL_ARG_CHECK(center);
	LOCATIONS_NULL_ARG_CHECK(radius);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	if (__convert_bounds_type(handle->boundary->type) != LOCATION_BOUNDS_CIRCLE) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_INCORRECT_TYPE(0x%08x)", LOCATION_BOUNDS_ERROR_INCORRECT_TYPE);
		return LOCATION_BOUNDS_ERROR_INCORRECT_TYPE;
	}

	center->latitude = handle->boundary->circle.center->latitude;
	center->longitude = handle->boundary->circle.center->longitude;
	*radius = handle->boundary->circle.radius;
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_foreach_polygon_coords(location_bounds_h bounds, polygon_coords_cb callback, void *user_data)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_NULL_ARG_CHECK(callback);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	if (__convert_bounds_type(handle->boundary->type) != LOCATION_BOUNDS_POLYGON) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_INCORRECT_TYPE(0x%08x)", LOCATION_BOUNDS_ERROR_INCORRECT_TYPE);
		return LOCATION_BOUNDS_ERROR_INCORRECT_TYPE;
	}

	GList *list = handle->boundary->polygon.position_list;
	while (list) {
		LocationPosition *pos = list->data;
		location_coords_s coords;
		coords.latitude = pos->latitude;
		coords.longitude = pos->longitude;

		if (callback(coords, user_data) != TRUE) {
			LOCATIONS_LOGI("User quit the loop ");
			break;
		}
		list = g_list_next(list);
	}
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_destroy(location_bounds_h bounds)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	if (handle->is_added) {
		LOCATIONS_LOGE("LOCATION_BOUNDS_ERROR_IS_ADDED(0x%08x)", LOCATION_BOUNDS_ERROR_IS_ADDED);
		return LOCATION_BOUNDS_ERROR_IS_ADDED;
	}
	location_boundary_free(handle->boundary);
	handle->user_cb = NULL;
	handle->user_data = NULL;
	free(handle);
	LOCATIONS_LOGE("");
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_set_state_changed_cb(location_bounds_h bounds, location_bounds_state_changed_cb callback, void *user_data)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	LOCATIONS_NULL_ARG_CHECK(callback);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	handle->user_cb = callback;
	handle->user_data = user_data;
	return LOCATION_BOUNDS_ERROR_NONE;
}

EXPORT_API int location_bounds_unset_state_changed_cb(location_bounds_h bounds)
{
	LOCATIONS_NULL_ARG_CHECK(bounds);
	location_bounds_s *handle = (location_bounds_s *) bounds;
	handle->user_cb = NULL;
	handle->user_data = NULL;
	return LOCATION_BOUNDS_ERROR_NONE;
}
