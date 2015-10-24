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
#include "location_internal.h"
#include "location_batch.h"


/*
* Internal Implementation
*/

static void __cb_batch_updated(GObject *self, guint num_of_location, gpointer userdata)
{
	LOCATIONS_LOGD("Batch callback function has been invoked.");
	location_manager_s *handle = (location_manager_s *) userdata;

	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_BATCH]) {
		((location_batch_cb) handle->user_cb[_LOCATIONS_EVENT_TYPE_BATCH])(num_of_location, handle->user_data[_LOCATIONS_EVENT_TYPE_BATCH]);
	}
}

/*/////////////////////////////////////// */
/* Location Manager */
/*////////////////////////////////////// */

EXPORT_API int location_manager_set_location_batch_cb(location_manager_h manager, location_batch_cb callback, int batch_interval, int batch_period, void *user_data)
{
	LOCATIONS_LOGD("location_manager_set_location_batch_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());

	LOCATIONS_CHECK_CONDITION(batch_interval >= 1 && batch_interval <= 120, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_CHECK_CONDITION(batch_period >= 120 && batch_period <= 600, LOCATIONS_ERROR_INVALID_PARAMETER, "LOCATIONS_ERROR_INVALID_PARAMETER");
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);
	location_manager_s *handle = (location_manager_s *) manager;
	g_object_set(handle->object, "batch-period", batch_period, NULL);
	g_object_set(handle->object, "batch-interval", batch_interval, NULL);
	return __set_callback(_LOCATIONS_EVENT_TYPE_BATCH, manager, callback, user_data);
}

EXPORT_API int location_manager_unset_location_batch_cb(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_unset_location_batch_cb");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	return __unset_callback(_LOCATIONS_EVENT_TYPE_BATCH, manager);
}

EXPORT_API int location_manager_start_batch(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_start_batch");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;

	if (LOCATIONS_METHOD_GPS == handle->method) {
		if (!handle->sig_id[_LOCATION_SIGNAL_BATCH_UPDATED]) {
			handle->sig_id[_LOCATION_SIGNAL_BATCH_UPDATED] = g_signal_connect(handle->object, "batch-updated", G_CALLBACK(__cb_batch_updated), handle);
		}
	} else {
		LOCATIONS_LOGE("method is not GPS");
	}

	if (handle->user_cb[_LOCATIONS_EVENT_TYPE_BATCH] != NULL) {
		LOCATIONS_LOGI("batch status set : Start");
	}

	int ret = location_start_batch(handle->object);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_stop_batch(location_manager_h manager)
{
	LOCATIONS_LOGD("location_manager_stop_batch");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;

	if (LOCATIONS_METHOD_GPS == handle->method) {
		if (handle->sig_id[_LOCATION_SIGNAL_BATCH_UPDATED]) {
			g_signal_handler_disconnect(handle->object, handle->sig_id[_LOCATION_SIGNAL_BATCH_UPDATED]);
			handle->sig_id[_LOCATION_SIGNAL_BATCH_UPDATED] = 0;
		}
	} else {
		LOCATIONS_LOGE("method is not GPS");
	}

	int ret = location_stop_batch(handle->object);
	if (ret != LOCATION_ERROR_NONE) {
		return __convert_error_code(ret);
	}

	return LOCATIONS_ERROR_NONE;
}

EXPORT_API int location_manager_foreach_location_batch(location_manager_h manager, location_batch_get_location_cb callback, void *user_data)
{
	LOCATIONS_LOGD("location_manager_foreach_location_batch");
	LOCATIONS_NOT_SUPPORTED_CHECK(__is_location_supported());
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);
	location_manager_s *handle = (location_manager_s *) manager;
	LocationBatch *batch = NULL;

	int ret = location_get_batch(handle->object, &batch);
	if (ret != LOCATION_ERROR_NONE || batch == NULL) {
		if (ret == LOCATION_ERROR_NOT_SUPPORTED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_INCORRECT_METHOD(0x%08x) : method - %d", LOCATIONS_ERROR_INCORRECT_METHOD, handle->method);
			return LOCATIONS_ERROR_INCORRECT_METHOD;
		} else if (ret == LOCATION_ERROR_NOT_ALLOWED) {
			LOCATIONS_LOGE("LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED");
			return LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED;
		}

		LOCATIONS_LOGE("LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE(0x%08x) : batch is NULL ",
		               LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE);
		return LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	int i;
	for (i = 0; i < batch->num_of_location; i++) {
		gdouble latitude;
		gdouble longitude;
		gdouble altitude;
		gdouble speed;
		gdouble direction;
		gdouble h_accuracy;
		gdouble v_accuracy;
		guint timestamp;

		location_get_batch_details(batch, i, &latitude, &longitude, &altitude, &speed, &direction, &h_accuracy, &v_accuracy, &timestamp);
		if (callback(latitude, longitude, altitude, speed, direction, h_accuracy, v_accuracy, timestamp, user_data) != TRUE) {
			break;
		}
	}
	location_batch_free(batch);
	batch = NULL;
	return LOCATIONS_ERROR_NONE;
}
