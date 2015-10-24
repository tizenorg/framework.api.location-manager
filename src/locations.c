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
#include <system_info.h>
#include "locations.h"
#include "location_internal.h"

static location_setting_changed_s g_location_setting[LOCATIONS_METHOD_WPS + 1];

static location_method_e __convert_location_method_e(LocationMethod method)
{
	location_method_e _method = LOCATIONS_METHOD_NONE;
	switch (method) {
		case LOCATION_METHOD_HYBRID:
			_method = LOCATIONS_METHOD_HYBRID;
			break;
		case LOCATION_METHOD_GPS:
			_method = LOCATIONS_METHOD_GPS;
			break;
		case LOCATION_METHOD_WPS:
			_method = LOCATIONS_METHOD_WPS;
			break;
		case LOCATION_METHOD_NONE:
		default:
			break;
	}
	return _method;
}

static LocationMethod __convert_LocationMethod(location_method_e method)
{
	LocationMethod _method = LOCATION_METHOD_NONE;
	switch (method) {
		case LOCATIONS_METHOD_HYBRID:
			_method = LOCATION_METHOD_HYBRID;
			break;
		case LOCATIONS_METHOD_GPS:
			_method = LOCATION_METHOD_GPS;
			break;
		case LOCATIONS_METHOD_WPS:
			_method = LOCATION_METHOD_WPS;
			break;
		case LOCATIONS_METHOD_NONE:
		default:
			_method = LOCATION_METHOD_NONE;
			break;
	}
	return _method;
}

static void __cb_service_updated(GObject *self, guint type, gpointer data, gpointer velocity, gpointer accuracy, gpointer userdata)
{
	LOCATIONS_LOGD("Callback function has been invoked. ");
	location_manager_s *handle = (location_manager_s *) userdata;

	if (type == SATELLITE_UPDATED && handle->user_cb[_LOCATIONS_EVENT_TYPE_SATELLITE]) {
		LocationSatellite *sat = (LocationSatellite *)data;
		LOCATIONS_LOGD("Current satellite information: timestamp : %d, number of active : %d, number of inview : %d",
						sat->timestamp, sat->num_of_sat_used, sat->num_of_sat_inview);
		((gps_status_satellite_updated_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_SATELLITE])(sat->num_of_sat_used, sat->num_of_sat_inview,
				sat->timestamp, handle->user_data[_LOCATIONS_EVENT_TYPE_SATELLITE]);
	} else if (type == DISTANCE_UPDATED && handle->user_cb[_LOCATIONS_EVENT_TYPE_DISTANCE]) {
		LocationPosition *pos = (LocationPosition *) data;
		LocationVelocity *vel = (LocationVelocity *) velocity;	/* current velocity */
		((location_changed_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_DISTANCE])(0, pos->latitude, pos->longitude, pos->altitude,
																				vel->speed, vel->direction, pos->timestamp, handle->user_data[_LOCATIONS_EVENT_TYPE_DISTANCE]);
	} else {

		if (handle->user_cb[_LOCATIONS_EVENT_TYPE_POSITION] && (type & POSITION_UPDATED) != 0) {
			LocationPosition *pos = (LocationPosition *) data;
			((location_position_updated_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_POSITION])(pos->latitude, pos->longitude, pos->altitude, pos->timestamp,
					handle->user_data[_LOCATIONS_EVENT_TYPE_POSITION]);
		}

		if (handle->user_cb[_LOCATIONS_EVENT_TYPE_VELOCITY] && (type & VELOCITY_UPDATED) != 0) {
			LocationVelocity *vel = (LocationVelocity *) velocity;
			((location_velocity_updated_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_VELOCITY])(vel->speed, vel->direction, vel->climb, vel->timestamp,
					handle->user_data[_LOCATIONS_EVENT_TYPE_VELOCITY]);
		}

		if (handle->user_cb[_LOCATIONS_EVENT_TYPE_POS_VEL] && (type & LOCATION_CHANGED) != 0) {
			LocationPosition *pos = (LocationPosition *) data;
			LocationVelocity *vel = (LocationVelocity *) velocity;
			LocationAccuracy *acc = (LocationAccuracy *) accuracy;
			((location_changed_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_POS_VEL])(pos->latitude, pos->longitude, pos->altitude,
																					vel->speed, vel->direction, acc->horizontal_accuracy, pos->timestamp, handle->user_data[_LOCATIONS_EVENT_TYPE_POS_VEL]);
		}
	}
}

static void __cb_location_updated(GObject *self, int error, gpointer position, gpointer velocity, gpointer accuracy, gpointer userdata)
{
	LOCATIONS_LOGD("Callback function has been invoked. ");
	int converted_err = __convert_error_code(error);
	location_manager_s *handle = (location_manager_s *) userdata;
	LocationPosition *pos = (LocationPosition *) position;
	LocationVelocity *vel = (LocationVelocity *) velocity;

	LOCATIONS_LOGD("Current position: timestamp : %d", pos->timestamp);
	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_LOCATION]) {
		((location_updated_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_LOCATION])(converted_err, pos->latitude, pos->longitude, pos->altitude,
																				pos->timestamp, vel->speed, vel->climb, vel->direction, handle->user_data[_LOCATIONS_EVENT_TYPE_LOCATION]);
	}
}

static void __cb_service_enabled(GObject *self, guint status, gpointer userdata)
{
	LOCATIONS_LOGD("Callback function has been invoked. ");
	location_manager_s *handle = (location_manager_s *) userdata;
	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_SERVICE_STATE]) {
		((location_service_state_changed_cb)
		 handle->user_cb[_LOCATIONS_EVENT_TYPE_SERVICE_STATE])(LOCATIONS_SERVICE_ENABLED, handle->user_data[_LOCATIONS_EVENT_TYPE_SERVICE_STATE]);
	}
}

static void __cb_service_disabled(GObject *self, guint status, gpointer userdata)
{
	LOCATIONS_LOGD("Callback function has been invoked. ");
	location_manager_s *handle = (location_manager_s *) userdata;
	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_SERVICE_STATE])
		((location_service_state_changed_cb)
		 handle->user_cb[_LOCATIONS_EVENT_TYPE_SERVICE_STATE])(LOCATIONS_SERVICE_DISABLED,handle->user_data[_LOCATIONS_EVENT_TYPE_SERVICE_STATE]);
}

static int __compare_position(gconstpointer a, gconstpointer b)
{
	if (location_position_equal((LocationPosition *) a, (LocationPosition *)b) == TRUE) {
		return 0;
	}

	return -1;
}

static int __boundary_compare(LocationBoundary *bound1, LocationBoundary *bound2)
{
	int ret = -1;

	if (bound1->type == bound2->type) {
		switch (bound1->type) {
			case LOCATION_BOUNDARY_CIRCLE:
				if (location_position_equal(bound1->circle.center, bound2->circle.center) && bound1->circle.radius == bound2->circle.radius) {
					ret = 0;
				}
				break;
			case LOCATION_BOUNDARY_RECT:
				if (location_position_equal(bound1->rect.left_top, bound2->rect.left_top) && location_position_equal(bound1->rect.right_bottom, bound2->rect.right_bottom)) {
					ret = 0;
				}
				break;
			case LOCATION_BOUNDARY_POLYGON: {
					GList *boundary1_next = NULL;
					GList *boundary2_start = NULL, *boundary2_prev = NULL, *boundary2_next = NULL;
					if (g_list_length(bound1->polygon.position_list) != g_list_length(bound2->polygon.position_list)) {
						return -1;
					}

					boundary2_start = g_list_find_custom(bound2->polygon.position_list, g_list_nth_data(bound1->polygon.position_list, 0), (GCompareFunc) __compare_position);
					if (boundary2_start == NULL) return -1;

					boundary2_prev = g_list_previous(boundary2_start);
					boundary2_next = g_list_next(boundary2_start);

					if (boundary2_prev == NULL) boundary2_prev = g_list_last(bound2->polygon.position_list);
					if (boundary2_next == NULL) boundary2_next = g_list_first(bound2->polygon.position_list);

					boundary1_next = g_list_next(bound1->polygon.position_list);
					if (boundary1_next != NULL && boundary2_prev != NULL &&
						location_position_equal((LocationPosition *)boundary1_next->data, (LocationPosition *)boundary2_prev->data) == TRUE) {
						boundary1_next = g_list_next(boundary1_next);
						while (boundary1_next) {
							boundary2_prev = g_list_previous(boundary2_prev);
							if (boundary2_prev == NULL) boundary2_prev = g_list_last(bound2->polygon.position_list);
							if (location_position_equal((LocationPosition *)boundary1_next->data, (LocationPosition *) boundary2_prev->data) == FALSE) {
								return -1;
							}
							boundary1_next = g_list_next(boundary1_next);
						}
						ret = 0;
					} else if (boundary1_next != NULL && boundary2_next != NULL &&
								location_position_equal((LocationPosition *)boundary1_next->data, (LocationPosition *)boundary2_next->data) == TRUE) {
						boundary1_next = g_list_next(boundary1_next);
						while (boundary1_next) {
							boundary2_next = g_list_next(boundary2_next);
							if (boundary2_next == NULL) boundary2_next = g_list_first(bound2->polygon.position_list);
							if (location_position_equal((LocationPosition *)boundary1_next->data, (LocationPosition *) boundary2_next->data) == FALSE) {
								return -1;
							}
							boundary1_next = g_list_next(boundary1_next);
						}
						ret = 0;
					} else {
						return -1;
					}
					break;
				}
			default:
				break;
		}
	}
	return ret;
}

static void __cb_zone_in(GObject *self, gpointer boundary, gpointer position, gpointer accuracy, gpointer userdata)
{
	LOCATIONS_LOGD("ENTER >>>");
	location_manager_s *handle = (location_manager_s *) userdata;
	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_BOUNDARY]) {
		LocationPosition *pos = (LocationPosition *) position;
		((location_zone_changed_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_BOUNDARY])(LOCATIONS_BOUNDARY_IN,
																					pos->latitude, pos->longitude,
																					pos->altitude, pos->timestamp,
																					handle->user_data
																					[_LOCATIONS_EVENT_TYPE_BOUNDARY]);
	}

	location_bounds_s *bounds;
	GList *bounds_list = g_list_first(handle->bounds_list);
	while (bounds_list) {
		bounds = (location_bounds_s *)bounds_list->data;
		if (__boundary_compare(boundary, bounds->boundary) == 0) {
			LOCATIONS_LOGD("Find zone in boundary");
			if (bounds->user_cb) {
				((location_bounds_state_changed_cb) bounds->user_cb)(LOCATIONS_BOUNDARY_IN, bounds->user_data);
			}
			break;
		}
		bounds_list = g_list_next(bounds_list);
	}
	LOCATIONS_LOGD("EXIT <<<");
}

static void __cb_zone_out(GObject *self, gpointer boundary, gpointer position, gpointer accuracy, gpointer userdata)
{
	LOCATIONS_LOGD("ENTER >>>");
	location_manager_s *handle = (location_manager_s *) userdata;
	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_BOUNDARY]) {
		LocationPosition *pos = (LocationPosition *) position;
		((location_zone_changed_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_BOUNDARY])(LOCATIONS_BOUNDARY_OUT,
																					pos->latitude, pos->longitude,
																					pos->altitude, pos->timestamp,
																					handle->user_data
																					[_LOCATIONS_EVENT_TYPE_BOUNDARY]);
	}

	location_bounds_s *bounds;
	GList *bounds_list = g_list_first(handle->bounds_list);
	while (bounds_list) {
		bounds = (location_bounds_s *)bounds_list->data;
		if (__boundary_compare(boundary, bounds->boundary) == 0) {
			LOCATIONS_LOGD("Find zone out boundary");
			if (bounds->user_cb) {
				((location_bounds_state_changed_cb) bounds->user_cb)(LOCATIONS_BOUNDARY_OUT, bounds->user_data);
			}
			break;
		}
		bounds_list = g_list_next(bounds_list);
	}
	LOCATIONS_LOGD("EXIT <<<");
}

static void __foreach_boundary(LocationBoundary *boundary, void *user_data)
{
	location_manager_s *handle = (location_manager_s *) user_data;

	if (handle != NULL && boundary != NULL) {
		int ret = -1;
		location_bounds_h bounds;
		if (boundary->type == LOCATION_BOUNDARY_CIRCLE) {
			location_coords_s center;
			center.latitude = boundary->circle.center->latitude;
			center.longitude = boundary->circle.center->longitude;
			ret = location_bounds_create_circle(center, boundary->circle.radius, &bounds);
		} else if (boundary->type == LOCATION_BOUNDARY_RECT) {
			location_coords_s left_top;
			location_coords_s right_bottom;
			left_top.latitude = boundary->rect.left_top->latitude;
			left_top.longitude = boundary->rect.left_top->longitude;
			right_bottom.latitude = boundary->rect.right_bottom->latitude;
			right_bottom.longitude = boundary->rect.right_bottom->longitude;
			ret = location_bounds_create_rect(left_top, right_bottom, &bounds);
		} else if (boundary->type == LOCATION_BOUNDARY_POLYGON) {
			GList *list = boundary->polygon.position_list;
			int size = g_list_length(list);
			if (size > 0) {
				location_coords_s coords[size];
				int cnt = 0;
				while (list) {
					LocationPosition *pos = list->data;
					coords[cnt].latitude = pos->latitude;
					coords[cnt].longitude = pos->longitude;
					list = g_list_next(list);
					cnt++;
				}
				ret = location_bounds_create_polygon(coords, size, &bounds);
			}
		} else {
			LOCATIONS_LOGI("Invalid boundary type : %d", boundary->type);
		}

		if (ret != LOCATIONS_ERROR_NONE) {
			LOCATIONS_LOGI("Failed to create location_bounds : (0x%08x) ", ret);
		} else {
			if (handle->is_continue_foreach_bounds) {
				handle->is_continue_foreach_bounds =
					((location_bounds_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_FOREACH_BOUNDS])(bounds,
							handle->
							user_data
							[_LOCATIONS_EVENT_TYPE_FOREACH_BOUNDS]);
			}
			location_bounds_destroy(bounds);
		}
	} else {
		LOCATIONS_LOGD("__foreach_boundary() has been failed");
	}
}

static void __setting_changed_cb(LocationMethod method, gboolean enable, void *user_data)
{
	LOCATIONS_LOGD("method: [%d]", method);
	location_method_e _method = __convert_location_method_e(method);
	location_setting_changed_s *_setting_changed = (location_setting_changed_s *)user_data;
	if (_setting_changed == NULL) {
		LOCATIONS_LOGE("Invaild userdata\n");
		return;
	}

	if (_setting_changed[_method].callback != NULL) {
		_setting_changed[_method].callback(_method, enable, _setting_changed[_method].user_data);
	}
}

/*/////////////////////////////////////// */
/* Location Manager */
/*////////////////////////////////////// */

EXPORT_API bool location_manager_is_supported_method(location_method_e method)
{
	LOCATIONS_LOGD("location_manager_is_supported_method %d", method);
	if (__is_location_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
		set_last_result(LOCATIONS_ERROR_NOT_SUPPORTED);
		return false;
	}

	if (method == LOCATIONS_METHOD_GPS) {
		if (__is_gps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
			set_last_result(LOCATIONS_ERROR_NOT_SUPPORTED);
			return false;
		}
	} else if (method == LOCATIONS_METHOD_WPS) {
		if (__is_wps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
			set_last_result(LOCATIONS_ERROR_NOT_SUPPORTED);
			return false;
		}
	}

	LocationMethod _method = __convert_LocationMethod(method);
	if (_method == LOCATION_METHOD_NONE) {
		LOCATIONS_LOGE("Not supported method [%d]", method);
		set_last_result(LOCATIONS_ERROR_INCORRECT_METHOD);
		return false;
	}

	set_last_result(LOCATIONS_ERROR_NONE);
	return location_is_supported_method(_method);
}

EXPORT_API int location_manager_is_enabled_method(location_method_e method, bool *enable)
{
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());

	if (LOCATIONS_METHOD_HYBRID > method || LOCATIONS_METHOD_WPS < method) {
		LOCATIONS_LOGE("Not supported method [%d]", method);
		return LOCATIONS_ERROR_INCORRECT_METHOD;
	}

	LOCATIONS_LOGD("location_manager_is_enabled_method %d", method);
	LOCATIONS_NULL_ARG_CHECK(enable);
	int is_enabled_val = -1;
	LocationMethod _method = __convert_LocationMethod(method);
	int ret = location_is_enabled_method(_method, &is_enabled_val);
	if (ret != LOCATION_ERROR_NONE) {
		if (ret == LOCATION_ERROR_NOT_SUPPORTED)
			return LOCATIONS_ERROR_INCORRECT_METHOD;
		return __convert_error_code(ret);
	}
	if (is_enabled_val == -1) {
		return TIZEN_ERROR_PERMISSION_DENIED;
	}

	*enable = (is_enabled_val == 0) ? FALSE : TRUE;
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_enable_method(const location_method_e method, const bool enable)
{
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());

	if (LOCATIONS_METHOD_HYBRID > method || LOCATIONS_METHOD_WPS < method) {
		LOCATIONS_LOGE("Not supported method [%d]", method);
		return LOCATIONS_ERROR_INCORRECT_METHOD;
	}

	LOCATIONS_LOGD("method: %d, enable: %d", method, enable);

	int ret = 0;
	if (LOCATIONS_METHOD_HYBRID == method) {
		if (__is_gps_supported() == LOCATIONS_ERROR_NONE) {
			ret = location_enable_method(LOCATION_METHOD_GPS, enable);
			if (ret != LOCATIONS_ERROR_NONE) return __convert_error_code(ret);
		}
		if (__is_wps_supported() == LOCATIONS_ERROR_NONE) {
			ret = location_enable_method(LOCATION_METHOD_WPS, enable);
			return __convert_error_code(ret);
		}
		return LOCATIONS_ERROR_NONE;

	} else  {
		if ((LOCATIONS_METHOD_GPS == method) && (__is_gps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED)) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_NOT_SUPPORTED(0x%08x)", LOCATIONS_ERROR_NOT_SUPPORTED);
			return LOCATIONS_ERROR_NOT_SUPPORTED;
		}
		else if ((LOCATIONS_METHOD_WPS == method) && (__is_wps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED)) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_NOT_SUPPORTED(0x%08x)", LOCATIONS_ERROR_NOT_SUPPORTED);
			return LOCATIONS_ERROR_NOT_SUPPORTED;
		}

		LocationMethod _method = __convert_LocationMethod(method);

		int ret = location_enable_method(_method, enable);

		return __convert_error_code(ret);
	}
}


EXPORT_API int location_manager_create(location_method_e method, location_manager_h *manager)
{
	LOCATIONS_LOGD("location_manager_create (method : %d)", method);

	if (method == LOCATIONS_METHOD_HYBRID) {
		LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	} else if (method == LOCATIONS_METHOD_GPS) {
		if (__is_gps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_NOT_SUPPORTED(0x%08x) : fail to location feature", LOCATIONS_ERROR_NOT_SUPPORTED);
			return LOCATIONS_ERROR_NOT_SUPPORTED;
		}
	} else if (method == LOCATIONS_METHOD_WPS) {
		if (__is_wps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_NOT_SUPPORTED(0x%08x) : fail to location feature", LOCATIONS_ERROR_NOT_SUPPORTED);
			return LOCATIONS_ERROR_NOT_SUPPORTED;
		}
	}

	LocationMethod _method = __convert_LocationMethod(method);
	if (_method == LOCATION_METHOD_NONE) {
		LOCATIONS_LOGE("LOCATIONS_ERROR_NOT_SUPPORTED(0x%08x) : fail to location_init", LOCATIONS_ERROR_NOT_SUPPORTED);
		return LOCATIONS_ERROR_NOT_SUPPORTED;
	}
	if (!location_is_supported_method(_method)) {
		LOCATIONS_LOGE("LOCATIONS_ERROR_NOT_SUPPORTED(0x%08x) : fail to location_is_supported_method", LOCATIONS_ERROR_NOT_SUPPORTED);
		return LOCATIONS_ERROR_NOT_SUPPORTED;
	}

	/*It is moved here becasue of TCS. */
	LOCATIONS_NULL_ARG_CHECK(manager);

	if (location_init() != LOCATION_ERROR_NONE) {
		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : fail to location_init", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	location_manager_s *handle = (location_manager_s *) malloc(sizeof(location_manager_s));
	if (handle == NULL) {
		LOCATIONS_LOGE("OUT_OF_MEMORY(0x%08x)", LOCATIONS_ERROR_OUT_OF_MEMORY);
		return LOCATIONS_ERROR_OUT_OF_MEMORY;
	}

	memset(handle, 0, sizeof(location_manager_s));

	handle->object = location_new(_method);
	if (handle->object == NULL) {
		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : fail to location_new", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		free(handle);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}
	handle->method = method;
	handle->is_continue_foreach_bounds = TRUE;
	handle->bounds_list = NULL;

	if (!handle->sig_id[_LOCATION_SIGNAL_SERVICE_ENABLED])
		handle->sig_id[_LOCATION_SIGNAL_SERVICE_ENABLED] = g_signal_connect(handle->object, "service-enabled", G_CALLBACK(__cb_service_enabled), handle);

	if (!handle->sig_id[_LOCATION_SIGNAL_SERVICE_DISABLED])
		handle->sig_id[_LOCATION_SIGNAL_SERVICE_DISABLED] = g_signal_connect(handle->object, "service-disabled", G_CALLBACK(__cb_service_disabled), handle);

	*manager = (location_manager_h) handle;
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_destroy(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_destroy");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;

	if (handle->sig_id[_LOCATION_SIGNAL_SERVICE_ENABLED]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_LOCATION_SIGNAL_SERVICE_ENABLED]);
		handle->sig_id[_LOCATION_SIGNAL_SERVICE_ENABLED] = 0;
	}

	if (handle->sig_id[_LOCATION_SIGNAL_SERVICE_DISABLED]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_LOCATION_SIGNAL_SERVICE_DISABLED]);
		handle->sig_id[_LOCATION_SIGNAL_SERVICE_DISABLED] = 0;
	}

	int ret = location_free(handle->object);
	if (ret != LOCATIONS_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	free(handle);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_start(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_start");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;

	if (!handle->sig_id[_LOCATION_SIGNAL_SERVICE_UPDATED])
		handle->sig_id[_LOCATION_SIGNAL_SERVICE_UPDATED] = g_signal_connect(handle->object, "service-updated", G_CALLBACK(__cb_service_updated), handle);

	if (LOCATIONS_METHOD_HYBRID <= handle->method && LOCATIONS_METHOD_WPS >= handle->method) {
		if (!handle->sig_id[_LOCATION_SIGNAL_ZONE_IN])
			handle->sig_id[_LOCATION_SIGNAL_ZONE_IN] = g_signal_connect(handle->object, "zone-in", G_CALLBACK(__cb_zone_in), handle);

		if (!handle->sig_id[_LOCATION_SIGNAL_ZONE_OUT])
			handle->sig_id[_LOCATION_SIGNAL_ZONE_OUT] = g_signal_connect(handle->object, "zone-out", G_CALLBACK(__cb_zone_out), handle);
	} else {
		LOCATIONS_LOGI("This method [%d] is not supported zone-in, zone-out signal.", handle->method);
	}

	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_SATELLITE] != NULL) {
		LOCATIONS_LOGI("Satellite update_cb is set");
		location_set_option(handle->object, "USE_SV");
	}

	int ret = location_start(handle->object);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_request_single_location(location_manager_h manager, int timeout, location_updated_cb callback, void *user_data)
{
	LOCATIONS_LOGD("location_manager_request_single_location");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);
	if (timeout <= 0 || timeout > 120) {
		LOCATIONS_LOGE("timeout scope is incorrect(1~120) [%d]", timeout);
		return LOCATIONS_ERROR_INVALID_PARAMETER;
	}

	location_manager_s *handle = (location_manager_s *) manager;
	int ret = LOCATIONS_ERROR_NONE;

	if (!handle->sig_id[_LOCATION_SIGNAL_LOCATION_UPDATED])
		handle->sig_id[_LOCATION_SIGNAL_LOCATION_UPDATED] = g_signal_connect(handle->object, "location-updated", G_CALLBACK(__cb_location_updated), handle);

	ret = __set_callback(_LOCATIONS_EVENT_TYPE_LOCATION, manager, callback, user_data);
	if (ret != LOCATIONS_ERROR_NONE) {
		return ret;
	}

	ret = location_request_single_location(handle->object, timeout);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_stop(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_stop");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);

	location_manager_s *handle = (location_manager_s *) manager;

	if (handle->sig_id[_LOCATION_SIGNAL_SERVICE_UPDATED]) {
		g_signal_handler_disconnect(handle->object, handle->sig_id[_LOCATION_SIGNAL_SERVICE_UPDATED]);
		handle->sig_id[_LOCATION_SIGNAL_SERVICE_UPDATED] = 0;
	}

	if (LOCATIONS_METHOD_HYBRID <= handle->method && LOCATIONS_METHOD_WPS >= handle->method) {
		if (handle->sig_id[_LOCATION_SIGNAL_ZONE_IN]) {
			g_signal_handler_disconnect(handle->object, handle->sig_id[_LOCATION_SIGNAL_ZONE_IN]);
			handle->sig_id[_LOCATION_SIGNAL_ZONE_IN] = 0;
		}

		if (handle->sig_id[_LOCATION_SIGNAL_ZONE_OUT]) {
			g_signal_handler_disconnect(handle->object, handle->sig_id[_LOCATION_SIGNAL_ZONE_OUT]);
			handle->sig_id[_LOCATION_SIGNAL_ZONE_OUT] = 0;
		}
	}

	int ret = location_stop(handle->object);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_add_boundary(location_manager_h manager, location_bounds_h bounds)
{
	LOCATIONS_LOGD("location_manager_add_boundary");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(bounds);

	location_manager_s *handle = (location_manager_s *) manager;
	location_bounds_s *bound_handle = (location_bounds_s *) bounds;
	int ret = location_boundary_add(handle->object, bound_handle->boundary);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	bound_handle->is_added = TRUE;
	handle->bounds_list = g_list_append(handle->bounds_list, bound_handle);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_remove_boundary(location_manager_h manager, location_bounds_h bounds)
{
	LOCATIONS_LOGD("location_manager_remove_boundary");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(bounds);

	location_manager_s *handle = (location_manager_s *) manager;
	location_bounds_s *bound_handle = (location_bounds_s *) bounds;
	int ret = location_boundary_remove(handle->object, bound_handle->boundary);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	handle->bounds_list = g_list_remove(handle->bounds_list, bound_handle);
	bound_handle->is_added = FALSE;
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_foreach_boundary(location_manager_h manager, location_bounds_cb callback, void *user_data)
{
	LOCATIONS_LOGD("location_manager_foreach_boundary");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);

	location_manager_s *handle = (location_manager_s *) manager;
	handle->user_cb[_LOCATIONS_EVENT_TYPE_FOREACH_BOUNDS] = callback;
	handle->user_data[_LOCATIONS_EVENT_TYPE_FOREACH_BOUNDS] = user_data;
	handle->is_continue_foreach_bounds = TRUE;
	int ret = location_boundary_foreach(handle->object, __foreach_boundary, handle);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_method(location_manager_h manager, location_method_e *method)
{
	LOCATIONS_LOGD("location_manager_get_method %d", method);
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(method);

	location_manager_s *handle = (location_manager_s *) manager;
	LocationMethod _method = LOCATION_METHOD_NONE;
	g_object_get(handle->object, "method", &_method, NULL);
	switch (_method) {
		case LOCATION_METHOD_NONE:
			*method = LOCATIONS_METHOD_NONE;
			break;
		case LOCATION_METHOD_HYBRID:
			*method = LOCATIONS_METHOD_HYBRID;
			break;
		case LOCATION_METHOD_GPS:
			*method = LOCATIONS_METHOD_GPS;
			break;
		case LOCATION_METHOD_WPS:
			*method = LOCATIONS_METHOD_WPS;
			break;
		default: {
				LOCATIONS_LOGE("LOCATIONS_ERROR_INVALID_PARAMETER(0x%08x) : Out of range (location_method_e) - method : %d ",
								LOCATIONS_ERROR_INVALID_PARAMETER, method);
				return LOCATIONS_ERROR_INVALID_PARAMETER;
			}
	}
	/**method = handle->method; */
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_position(location_manager_h manager, double *altitude, double *latitude, double *longitude,
											 time_t *timestamp)
{
	LOCATIONS_LOGD("location_manager_get_position");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(altitude);
	LOCATIONS_NULL_ARG_CHECK(latitude);
	LOCATIONS_NULL_ARG_CHECK(longitude);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;
	int ret;
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	ret = location_get_position(handle->object, &pos, &acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	if (pos->status == LOCATION_STATUS_NO_FIX) {
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	} else {
		*latitude = pos->latitude;
		*longitude = pos->longitude;
		*altitude = pos->altitude;
	}
	*timestamp = pos->timestamp;
	location_position_free(pos);
	location_accuracy_free(acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_location(location_manager_h manager, double *altitude, double *latitude, double *longitude, double *climb, double *direction, double *speed, location_accuracy_level_e *level, double *horizontal, double *vertical, time_t *timestamp)
{
	LOCATIONS_LOGD("location_manager_get_location");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(altitude);
	LOCATIONS_NULL_ARG_CHECK(latitude);
	LOCATIONS_NULL_ARG_CHECK(longitude);
	LOCATIONS_NULL_ARG_CHECK(climb);
	LOCATIONS_NULL_ARG_CHECK(direction);
	LOCATIONS_NULL_ARG_CHECK(speed);
	LOCATIONS_NULL_ARG_CHECK(level);
	LOCATIONS_NULL_ARG_CHECK(horizontal);
	LOCATIONS_NULL_ARG_CHECK(vertical);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;
	int ret;
	LocationPosition *pos = NULL;
	LocationVelocity *vel = NULL;
	LocationAccuracy *acc = NULL;
	ret = location_get_position_ext(handle->object, &pos, &vel, &acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	if (pos->status == LOCATION_STATUS_NO_FIX) {
		return __convert_error_code(LOCATION_ERROR_NOT_AVAILABLE);
	} else {
		*latitude = pos->latitude;
		*longitude = pos->longitude;
		*altitude = pos->altitude;
	}
	*timestamp = pos->timestamp;
	*climb = vel->climb;
	*direction = vel->direction;
	*speed = vel->speed;
	*level = acc->level;
	*horizontal = acc->horizontal_accuracy;
	*vertical = acc->vertical_accuracy;

	location_position_free(pos);
	location_velocity_free(vel);
	location_accuracy_free(acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_velocity(location_manager_h manager, double *climb, double *direction, double *speed, time_t *timestamp)
{
	LOCATIONS_LOGD("location_manager_get_velocity");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(climb);
	LOCATIONS_NULL_ARG_CHECK(direction);
	LOCATIONS_NULL_ARG_CHECK(speed);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;
	int ret;
	LocationVelocity *vel = NULL;
	LocationAccuracy *acc = NULL;
	ret = location_get_velocity(handle->object, &vel, &acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	*climb = vel->climb;
	*direction = vel->direction;
	*speed = vel->speed;
	*timestamp = vel->timestamp;
	location_velocity_free(vel);
	location_accuracy_free(acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_accuracy(location_manager_h manager, location_accuracy_level_e *level, double *horizontal,
											 double *vertical)
{
	LOCATIONS_LOGD("location_manager_get_accuracy");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(level);
	LOCATIONS_NULL_ARG_CHECK(horizontal);
	LOCATIONS_NULL_ARG_CHECK(vertical);
	location_manager_s *handle = (location_manager_s *) manager;

	int ret;
	LocationPosition *pos = NULL;
	LocationAccuracy *acc = NULL;
	ret = location_get_position(handle->object, &pos, &acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	if (acc == NULL) {
		return __convert_error_code(LOCATION_ERROR_NOT_AVAILABLE);
	}

	*level = acc->level;
	*horizontal = acc->horizontal_accuracy;
	*vertical = acc->vertical_accuracy;
	location_position_free(pos);
	location_accuracy_free(acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_last_position(location_manager_h manager, double *altitude, double *latitude, double *longitude, time_t *timestamp)
{
	LOCATIONS_LOGD("location_manager_get_last_position");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(altitude);
	LOCATIONS_NULL_ARG_CHECK(latitude);
	LOCATIONS_NULL_ARG_CHECK(longitude);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;

	int ret;
	LocationPosition *last_pos = NULL;
	LocationAccuracy *last_acc = NULL;
	ret = location_get_last_position(handle->object, &last_pos, &last_acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	if (last_pos->status == LOCATION_STATUS_NO_FIX) {
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	} else {
		*latitude = last_pos->latitude;
		*longitude = last_pos->longitude;
		*altitude = last_pos->altitude;
	}
	*timestamp = last_pos->timestamp;
	location_position_free(last_pos);
	location_accuracy_free(last_acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_last_location(location_manager_h manager, double *altitude, double *latitude, double *longitude, double *climb, double *direction, double *speed, location_accuracy_level_e *level, double *horizontal, double *vertical, time_t *timestamp)
{
	LOCATIONS_LOGD("location_manager_get_last_location");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(altitude);
	LOCATIONS_NULL_ARG_CHECK(latitude);
	LOCATIONS_NULL_ARG_CHECK(longitude);
	LOCATIONS_NULL_ARG_CHECK(climb);
	LOCATIONS_NULL_ARG_CHECK(direction);
	LOCATIONS_NULL_ARG_CHECK(speed);
	LOCATIONS_NULL_ARG_CHECK(level);
	LOCATIONS_NULL_ARG_CHECK(horizontal);
	LOCATIONS_NULL_ARG_CHECK(vertical);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;

	int ret;
	LocationPosition *last_pos = NULL;
	LocationVelocity *last_vel = NULL;
	LocationAccuracy *last_acc = NULL;
	ret = location_get_last_position_ext(handle->object, &last_pos, &last_vel, &last_acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	if (last_pos->status == LOCATION_STATUS_NO_FIX) {
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	} else {
		*latitude = last_pos->latitude;
		*longitude = last_pos->longitude;
		*altitude = last_pos->altitude;
	}
	*timestamp = last_pos->timestamp;
	*climb = last_vel->climb;
	*direction = last_vel->direction;
	*speed = last_vel->speed;
	*level = last_acc->level;
	*horizontal = last_acc->horizontal_accuracy;
	*vertical = last_acc->vertical_accuracy;
	location_position_free(last_pos);
	location_velocity_free(last_vel);
	location_accuracy_free(last_acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_last_velocity(location_manager_h manager, double *climb, double *direction, double *speed, time_t *timestamp)
{
	LOCATIONS_LOGD("location_manager_get_last_velocity");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(climb);
	LOCATIONS_NULL_ARG_CHECK(direction);
	LOCATIONS_NULL_ARG_CHECK(speed);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;

	int ret;
	LocationVelocity *last_vel = NULL;
	LocationAccuracy *last_acc = NULL;
	ret = location_get_last_velocity(handle->object, &last_vel, &last_acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	*climb = last_vel->climb;
	*direction = last_vel->direction;
	*speed = last_vel->speed;
	*timestamp = last_vel->timestamp;
	location_velocity_free(last_vel);
	location_accuracy_free(last_acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_last_accuracy(location_manager_h manager, location_accuracy_level_e *level, double *horizontal, double *vertical)
{
	LOCATIONS_LOGD("location_manager_get_last_accuracy");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(level);
	LOCATIONS_NULL_ARG_CHECK(horizontal);
	LOCATIONS_NULL_ARG_CHECK(vertical);
	location_manager_s *handle = (location_manager_s *) manager;

	int ret;
	LocationPosition *last_pos = NULL;
	LocationAccuracy *last_acc = NULL;
	ret = location_get_last_position(handle->object, &last_pos, &last_acc);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	*level = last_acc->level;
	*horizontal = last_acc->horizontal_accuracy;
	*vertical = last_acc->vertical_accuracy;
	location_position_free(last_pos);
	location_accuracy_free(last_acc);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_accessibility_state(location_accessibility_state_e *state)
{
	LOCATIONS_LOGD("location_manager_get_accessibility_state");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(state);

	int ret = LOCATION_ERROR_NONE;
	LocationAccessState auth = LOCATION_ACCESS_NONE;
	ret = location_get_accessibility_state(&auth);
	if (ret != LOCATION_ERROR_NONE) {
		*state = LOCATIONS_ACCESS_STATE_NONE;
		return __convert_error_code(ret);
	}

	switch (auth) {
		case LOCATION_ACCESS_DENIED:
			*state = LOCATIONS_ACCESS_STATE_DENIED;
			break;
		case LOCATION_ACCESS_ALLOWED:
			*state = LOCATIONS_ACCESS_STATE_ALLOWED;
			break;
		case LOCATION_ACCESS_NONE:
		default:
			*state = LOCATIONS_ACCESS_STATE_NONE;
			break;
	}

	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_set_distance_based_location_changed_cb(location_manager_h manager, location_changed_cb callback, int interval, double distance, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_distance_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_CHECK_CONDITION(interval >= 1 && interval <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(distance > 0 && distance <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;
	g_object_set(handle->object, "min-interval", interval, NULL);
	g_object_set(handle->object, "min-distance", distance, NULL);
	return __set_callback(_LOCATIONS_EVENT_TYPE_DISTANCE, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_distance_based_location_changed_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_distance_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;
	g_object_set(handle->object, "min-interval", 0, NULL);
	g_object_set(handle->object, "min-distance", 0, NULL);
	return __unset_callback(_LOCATIONS_EVENT_TYPE_DISTANCE, manager);
}

EXPORT_API int location_manager_set_location_changed_cb(location_manager_h manager, location_changed_cb callback, int interval, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_location_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_CHECK_CONDITION(interval >= 1 && interval <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;
	g_object_set(handle->object, "loc-interval", interval, NULL);
	return __set_callback(_LOCATIONS_EVENT_TYPE_POS_VEL, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_location_changed_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_position_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_POS_VEL, manager);
}

EXPORT_API int location_manager_set_position_updated_cb(location_manager_h manager, location_position_updated_cb callback, int interval, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_position_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_CHECK_CONDITION(interval >= 1 && interval <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;
	g_object_set(handle->object, "pos-interval", interval, NULL);
	return __set_callback(_LOCATIONS_EVENT_TYPE_POSITION, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_position_updated_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_position_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_POSITION, manager);
}

EXPORT_API int location_manager_set_velocity_updated_cb(location_manager_h manager, location_velocity_updated_cb callback, int interval, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_velocity_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_CHECK_CONDITION(interval >= 1 && interval <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;
	g_object_set(handle->object, "vel-interval", interval, NULL);
	return __set_callback(_LOCATIONS_EVENT_TYPE_VELOCITY, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_velocity_updated_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_velocity_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_VELOCITY, manager);
}

EXPORT_API int location_manager_set_service_state_changed_cb(location_manager_h manager, location_service_state_changed_cb callback,
															 void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_service_state_changed_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __set_callback(_LOCATIONS_EVENT_TYPE_SERVICE_STATE, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_service_state_changed_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_service_state_changed_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_SERVICE_STATE, manager);
}

EXPORT_API int location_manager_set_zone_changed_cb(location_manager_h manager, location_zone_changed_cb callback, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_zone_changed_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __set_callback(_LOCATIONS_EVENT_TYPE_BOUNDARY, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_zone_changed_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_zone_changed_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_BOUNDARY, manager);
}

EXPORT_API int location_manager_set_setting_changed_cb(location_method_e method, location_setting_changed_cb callback, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_setting_changed_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(callback);

	LocationMethod _method = __convert_LocationMethod(method);
	int ret = LOCATION_ERROR_NONE;

	if (_method == LOCATION_METHOD_NONE) {
		return __convert_error_code(LOCATION_ERROR_PARAMETER);
	}

	g_location_setting[_method].callback = callback;
	g_location_setting[_method].user_data = user_data;

	ret = location_add_setting_notify(_method, __setting_changed_cb, &g_location_setting);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_unset_setting_changed_cb(location_method_e method)
{
	LOCATIONS_LOGD("location_manager_unset_setting_changed_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LocationMethod _method = __convert_LocationMethod(method);
	int ret = LOCATION_ERROR_NONE;

	if (_method == LOCATION_METHOD_NONE) {
		return __convert_error_code(LOCATION_ERROR_PARAMETER);
	}

	ret = location_ignore_setting_notify(_method, __setting_changed_cb);
	if (ret != LOCATION_ERROR_NONE) {
		LOCATIONS_LOGE("Fail to ignore notify. Error[%d]", ret);
		ret = __convert_error_code(ret);
	}

	g_location_setting[method].callback = NULL;
	g_location_setting[method].user_data = NULL;

	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_get_distance(double start_latitude, double start_longitude, double end_latitude, double end_longitude, double *distance)
{
	LOCATIONS_LOGD("location_manager_get_distance");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(distance);
	LOCATIONS_CHECK_CONDITION(start_latitude >= -90 && start_latitude <= 90, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(start_longitude >= -180 && start_longitude <= 180, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(end_latitude >= -90 && end_latitude <= 90, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(end_longitude >= -180 && end_longitude <= 180, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");

	int ret = LOCATION_ERROR_NONE;
	ulong u_distance;

	LocationPosition *start = location_position_new(0, start_latitude, start_longitude, 0, LOCATION_STATUS_2D_FIX);
	LocationPosition *end = location_position_new(0, end_latitude, end_longitude, 0, LOCATION_STATUS_2D_FIX);

	ret = location_get_distance(start, end, &u_distance);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	*distance = (double)u_distance;

	return LOCATIONS_ERROR_NONE;
}

/*/////////////////////////////////////// */
/* GPS Status & Satellites */
/*////////////////////////////////////// */

EXPORT_API int gps_status_get_nmea(location_manager_h manager, char **nmea)
{
	LOCATIONS_LOGD("gps_status_get_nmea");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(nmea);
	location_manager_s *handle = (location_manager_s *) manager;
	char *nmea_data;

	int ret = location_get_nmea(handle->object, &nmea_data);
	if (ret != LOCATION_ERROR_NONE || nmea == NULL) {
		if (ret == LOCATION_ERROR_NOT_ALLOWED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED");
			return LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED;
		}

		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : NMEA is NULL ", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	*nmea = g_strdup(nmea_data);
	g_free(nmea_data);

	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int gps_status_get_satellite(location_manager_h manager, int *num_of_active, int *num_of_inview, time_t *timestamp)
{
	LOCATIONS_LOGD("gps_status_get_satellite");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(num_of_active);
	LOCATIONS_NULL_ARG_CHECK(num_of_inview);
	LOCATIONS_NULL_ARG_CHECK(timestamp);
	location_manager_s *handle = (location_manager_s *) manager;
	LocationSatellite *sat = NULL;
	int ret = location_get_satellite(handle->object, &sat);
	if (ret != LOCATION_ERROR_NONE || sat == NULL) {
		if (ret == LOCATION_ERROR_NOT_ALLOWED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED");
			return LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED;
		}

		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : satellite is NULL ", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	*num_of_active = sat->num_of_sat_used;
	*num_of_inview = sat->num_of_sat_inview;
	*timestamp = sat->timestamp;
	location_satellite_free(sat);
	sat = NULL;
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int gps_status_set_satellite_updated_cb(location_manager_h manager, gps_status_satellite_updated_cb callback, int interval, void *user_data)
{
	LOCATIONS_LOGD("gps_status_set_satellite_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	LOCATIONS_CHECK_CONDITION(interval >= 1 && interval <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);

	location_manager_s *handle = (location_manager_s *) manager;
	location_set_option(handle->object, "USE_SV");
	g_object_set(handle->object, "sat-interval", interval, NULL);
	return __set_callback(_LOCATIONS_EVENT_TYPE_SATELLITE, manager, callback, user_data);
}

EXPORT_API int gps_status_unset_satellite_updated_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("gps_status_unset_satellite_updated_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_SATELLITE, manager);
}

EXPORT_API int gps_status_foreach_satellites_in_view(location_manager_h manager, gps_status_get_satellites_cb callback, void *user_data)
{
	LOCATIONS_LOGD("gps_status_foreach_satellites_in_view");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);

	location_manager_s *handle = (location_manager_s *) manager;
	LocationSatellite *sat = NULL;
	int ret = location_get_satellite(handle->object, &sat);
	if (ret != LOCATION_ERROR_NONE || sat == NULL) {
		if (ret == LOCATION_ERROR_NOT_SUPPORTED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_INCORRECT_METHOD(0x%08x) : method - %d", LOCATIONS_ERROR_INCORRECT_METHOD, handle->method);
			return LOCATIONS_ERROR_INCORRECT_METHOD;
		} else if (ret == LOCATION_ERROR_NOT_ALLOWED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED");
			return LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED;
		}

		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : satellite is NULL ", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	int i;
	for (i = 0; i < sat->num_of_sat_inview; i++) {
		guint prn;
		gboolean used;
		guint elevation;
		guint azimuth;
		gint snr;
		location_satellite_get_satellite_details(sat, i, &prn, &used, &elevation, &azimuth, &snr);
		if (callback(azimuth, elevation, prn, snr, used, user_data) != TRUE)
			break;
	}
	location_satellite_free(sat);
	sat = NULL;
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int gps_status_get_last_satellite(location_manager_h manager, int *num_of_active, int *num_of_inview, time_t *timestamp)
{
	LOCATIONS_LOGD("gps_status_get_last_satellite");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(num_of_active);
	LOCATIONS_NULL_ARG_CHECK(num_of_inview);
	LOCATIONS_NULL_ARG_CHECK(timestamp);

	location_manager_s *handle = (location_manager_s *) manager;
	int ret = LOCATION_ERROR_NONE;
	LocationSatellite *last_sat = NULL;
	ret = location_get_last_satellite(handle->object, &last_sat);
	if (ret != LOCATION_ERROR_NONE || last_sat == NULL) {
		if (ret == LOCATION_ERROR_NOT_SUPPORTED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_INCORRECT_METHOD(0x%08x) : method - %d", LOCATIONS_ERROR_INCORRECT_METHOD, handle->method);
			return LOCATIONS_ERROR_INCORRECT_METHOD;
		} else if (ret == LOCATION_ERROR_NOT_ALLOWED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED");
			return LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED;
		}

		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : satellite is NULL ", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	*num_of_active = last_sat->num_of_sat_used;
	*num_of_inview = last_sat->num_of_sat_inview;
	*timestamp = last_sat->timestamp;
	location_satellite_free(last_sat);
	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int gps_status_foreach_last_satellites_in_view(location_manager_h manager, gps_status_get_satellites_cb callback, void *user_data)
{
	LOCATIONS_LOGD("gps_status_foreach_last_satellites_in_view");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_gps_satellite_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);
	location_manager_s *handle = (location_manager_s *) manager;
	int ret;
	LocationSatellite *last_sat = NULL;
	ret = location_get_last_satellite(handle->object, &last_sat);
	if (ret != LOCATION_ERROR_NONE || last_sat == NULL) {
		if (ret == LOCATION_ERROR_NOT_SUPPORTED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_INCORRECT_METHOD(0x%08x) : method - %d", LOCATIONS_ERROR_INCORRECT_METHOD, handle->method);
			return LOCATIONS_ERROR_INCORRECT_METHOD;
		}

		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : satellite is NULL ", LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	int i;
	for (i = 0; i < last_sat->num_of_sat_inview; i++) {
		guint prn;
		gboolean used;
		guint elevation;
		guint azimuth;
		gint snr;
		location_satellite_get_satellite_details(last_sat, i, &prn, &used, &elevation, &azimuth, &snr);
		if (callback(azimuth, elevation, prn, snr, used, user_data) != TRUE) {
			break;
		}
	}
	location_satellite_free(last_sat);
	return LOCATIONS_ERROR_NONE;
}
