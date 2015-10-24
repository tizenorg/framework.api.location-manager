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

#ifndef __TIZEN_LOCATION_BATCH_H__
#define __TIZEN_LOCATION_BATCH_H__

#include <locations.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @internal
 * @{
 * @addtogroup CAPI_LOCATION_MANAGER_MODULE
 * @{
 */

/**
 * @brief Called when the batch interval is expired.
 * @param[in] num_of_location	The number of location batch data
 * @param[in] user_data			The user data passed from the callback registration function
 */
typedef void(*location_batch_cb)(int num_of_location, void *user_data);

/**
 * @brief Gets called iteratively to receive location batch data.
 * @param[in] latitude		The updated latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude		The updated longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] altitude		The updated altitude (meters)
 * @param[in] speed			The updated speed (km/h)
 * @param[in] direction		The updated direction (in degrees from the north)
 * @param[in] horizontal	The horizontal accuracy (meters)
 * @param[in] vertical		The vertical accuracy (meters)
 * @param[in] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_batch_get_location_cb() will invoke this callback if you register this callback using location_manager_foreach_location_batch()
 */
typedef bool(*location_batch_get_location_cb)(double latitude, double longitude, double altitude, double speed, double direction, double horizontal, double vertical, time_t timestamp, void *user_data);

/**
 * @brief Registers a callback function to be invoked when batch_period is expired.
 * @param[in] manager			The location manager handle
 * @param[in] callback			The callback function to register
 * @param[in] batch_interval	The batch smpling interval
 * @param[in] batch_period		The batch period
 * @post location_batch_cb() will be invoked
 */
int location_manager_set_location_batch_cb(location_manager_h manager, location_batch_cb callback, int batch_interval, int batch_period, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @param[in] manager			The location manager handle
 */
int location_manager_unset_location_batch_cb(location_manager_h manager);

/**
 * @brief Start the location batch mode.
 * @param[in] manager			The location manager handle
 */
int location_manager_start_batch(location_manager_h manager);

/**
 * @brief Stop the location batch mode.
 * @param[in] manager		The location manager handle
 */
int location_manager_stop_batch(location_manager_h manager);

/**
 * @brief Retrieves all location information by invoking a specific callback for each location data.
 * @param[in] manager		The location manager handle
 * @param[in] callback		The iteration callback function
 * @param[in] user_data		The user data passed from the callback registration function
 * @post location_batch_get_location_cb() will be invoked
 */
int location_manager_foreach_location_batch(location_manager_h manager, location_batch_get_location_cb callback, void *user_data);

/**
 * @}
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_LOCATION_BATCH_H__ */
