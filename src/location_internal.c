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

int __convert_error_code(int code)
{
	int ret;
	const char *msg = NULL;
	switch (code) {
		case LOCATION_ERROR_NONE:
			ret = LOCATIONS_ERROR_NONE;
			msg = "LOCATIONS_ERROR_NONE";
			break;
		case LOCATION_ERROR_PARAMETER:
			ret = LOCATIONS_ERROR_INVALID_PARAMETER;
			msg = "LOCATIONS_ERROR_INVALID_PARAMETER";
			break;
		case LOCATION_ERROR_NOT_ALLOWED:
			ret = LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED;
			msg = "LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED";
			break;
		case LOCATION_ERROR_NOT_SUPPORTED:
			ret = LOCATIONS_ERROR_NOT_SUPPORTED;
			msg = "LOCATIONS_ERROR_NOT_SUPPORTED";
			break;
		case LOCATION_ERROR_NETWORK_FAILED:
		case LOCATION_ERROR_NETWORK_NOT_CONNECTED:
			ret = LOCATIONS_ERROR_NETWORK_FAILED;
			msg = "LOCATIONS_ERROR_NETWORK_FAILED";
			break;
		case LOCATION_ERROR_SETTING_OFF:
			ret = LOCATIONS_ERROR_GPS_SETTING_OFF;
			msg = "LOCATIONS_ERROR_GPS_SETTING_OFF";
			break;
		case LOCATION_ERROR_SECURITY_DENIED:
			ret = LOCATIONS_ERROR_SECURITY_RESTRICTED;
			msg = "LOCATIONS_ERROR_SECURITY_RESTRICTED";
			break;
		case LOCATION_ERROR_NOT_AVAILABLE:
		case LOCATION_ERROR_CONFIGURATION:
		case LOCATION_ERROR_UNKNOWN:
		default:
			msg = "LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE";
			ret = LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE;
	}

	if (ret != LOCATIONS_ERROR_NONE) {
		LOCATIONS_LOGE("%s(0x%08x) : core fw error(0x%x)", msg, ret, code);
	}
	return ret;
}

int __is_gps_supported(void)
{
	bool is_supported = false;
	int retval = 0;

	retval = system_info_get_platform_bool("http://tizen.org/feature/location.gps", &is_supported);
	if (retval != SYSTEM_INFO_ERROR_NONE) {
		LOCATIONS_LOGW("system_info_get_platform_bool failed: retval = %d", retval);
	}
	if (is_supported == false) {
		return LOCATIONS_ERROR_NOT_SUPPORTED;
	}
	return LOCATIONS_ERROR_NONE;
}

int __is_gps_satellite_supported(void)
{
	bool is_supported = false;
	int retval = 0;

	retval = system_info_get_platform_bool("http://tizen.org/feature/location.gps.satellite", &is_supported);
	if (retval != SYSTEM_INFO_ERROR_NONE) {
		LOCATIONS_LOGW("system_info_get_platform_bool failed: retval = %d", retval);
	}
	if (is_supported == false) {
		return LOCATIONS_ERROR_NOT_SUPPORTED;
	}
	return LOCATIONS_ERROR_NONE;
}

int __is_wps_supported(void)
{
	bool is_supported = false;
	int retval = 0;

	retval = system_info_get_platform_bool("http://tizen.org/feature/location.wps", &is_supported);
	if (retval != SYSTEM_INFO_ERROR_NONE) {
		LOCATIONS_LOGW("system_info_get_platform_bool failed: retval = %d", retval);
	}
	if (is_supported == false) {
		return LOCATIONS_ERROR_NOT_SUPPORTED;
	}
	return LOCATIONS_ERROR_NONE;
}

int __is_location_supported(void)
{
	if (__is_gps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
		if (__is_wps_supported() == LOCATIONS_ERROR_NOT_SUPPORTED) {
			return LOCATIONS_ERROR_NOT_SUPPORTED;
		} else {
			return LOCATIONS_ERROR_NONE;
		}
	}

	return LOCATIONS_ERROR_NONE;
}

int __set_callback(_location_event_e type, location_manager_h manager, void *callback, void *user_data)
{
	LOCATIONS_NULL_ARG_CHECK(manager);
	LOCATIONS_NULL_ARG_CHECK(callback);
	location_manager_s *handle = (location_manager_s *) manager;
	handle->user_cb[type] = callback;
	handle->user_data[type] = user_data;
	LOCATIONS_LOGD("event type : %d", type);
	return LOCATIONS_ERROR_NONE;
}

int __unset_callback(_location_event_e type, location_manager_h manager)
{
	LOCATIONS_NULL_ARG_CHECK(manager);
	location_manager_s *handle = (location_manager_s *) manager;
	handle->user_cb[type] = NULL;
	handle->user_data[type] = NULL;
	LOCATIONS_LOGD("event type : %d. ", type);
	return LOCATIONS_ERROR_NONE;
}

