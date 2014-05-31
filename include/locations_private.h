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

#ifndef __TIZEN_LOCATION_LOCATIONS_PRIVATE_H__
#define	__TIZEN_LOCATION_LOCATIONS_PRIVATE_H__

#include <location/location.h>
#include <locations.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_LOCATION_MANAGER"

/*
* Internal Macros
*/
#define LOCATIONS_LOGD(fmt,args...)  LOGD(fmt, ##args)
#define LOCATIONS_LOGW(fmt,args...)  LOGW(fmt, ##args)
#define LOCATIONS_LOGI(fmt,args...)  LOGI(fmt, ##args)
#define LOCATIONS_LOGE(fmt,args...)  LOGE(fmt, ##args)

#define LOCATIONS_CHECK_CONDITION(condition, error, msg)	\
		do { \
			if (condition) { \
			} else { \
				LOCATIONS_LOGE("%s(0x%08x)", msg, error); \
				return error; \
			} \
		} while (0)


#define LOCATIONS_PRINT_ERROR_CODE(error, msg)	\
		do { \
			LOCATIONS_LOGE("%s(0x%08x)", msg, error); \
			return error; \
		} while (0)


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	_LOCATIONS_EVENT_TYPE_SERVICE_STATE,
	_LOCATIONS_EVENT_TYPE_POSITION,
	_LOCATIONS_EVENT_TYPE_VELOCITY,
	_LOCATIONS_EVENT_TYPE_BOUNDARY,
	_LOCATIONS_EVENT_TYPE_SATELLITE,
	_LOCATIONS_EVENT_TYPE_FOREACH_BOUNDS,
	_LOCATIONS_EVENT_TYPE_NUM
} _location_event_e;

typedef enum {
	_LOCATION_SIGNAL_SERVICE_ENABLED,
	_LOCATION_SIGNAL_SERVICE_DISABLED,
	_LOCATION_SIGNAL_SERVICE_UPDATED,
	_LOCATION_SIGNAL_ZONE_IN,
	_LOCATION_SIGNAL_ZONE_OUT,
	_LOCATION_SIGNAL_NUM
} _location_signal_e;

typedef struct _location_manager_s {
	LocationObject* object;
	const void* user_cb[_LOCATIONS_EVENT_TYPE_NUM];
	void* user_data[_LOCATIONS_EVENT_TYPE_NUM];
	location_method_e method;
	bool is_continue_foreach_bounds;
	GList *bounds_list;
	gulong sig_id[_LOCATION_SIGNAL_NUM];
} location_manager_s;

typedef struct _location_bounds_s {
	LocationBoundary* boundary;
	const void* user_cb;
	void* user_data;
	bool is_added;
} location_bounds_s;
#ifdef __cplusplus
}
#endif

#endif //__TIZEN_LOCATION_LOCATIONS_PRIVATE_H__
