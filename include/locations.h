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

#ifndef __TIZEN_LOCATION_LOCATIONS_H__
#define __TIZEN_LOCATION_LOCATIONS_H__

#include <tizen_type.h>
#include <tizen_error.h>
#include <time.h>
#include <location_bounds.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_LOCATION_MANAGER_MODULE
 * @{
 */

/**
 * @brief Enumeration for error code for Location manager.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATIONS_ERROR_NONE						= TIZEN_ERROR_NONE,						/**< Successful */
	LOCATIONS_ERROR_OUT_OF_MEMORY				= TIZEN_ERROR_OUT_OF_MEMORY,			/**< Out of memory */
	LOCATIONS_ERROR_INVALID_PARAMETER			= TIZEN_ERROR_INVALID_PARAMETER,		/**< Invalid parameter */
	LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED	= TIZEN_ERROR_PERMISSION_DENIED,		/**< Permission denied */
	LOCATIONS_ERROR_NOT_SUPPORTED				= TIZEN_ERROR_NOT_SUPPORTED,			/**< Not supported */
	LOCATIONS_ERROR_INCORRECT_METHOD			= TIZEN_ERROR_LOCATION_MANAGER | 0x01,	/**< Location manager contains incorrect method for a given call */
	LOCATIONS_ERROR_NETWORK_FAILED				= TIZEN_ERROR_LOCATION_MANAGER | 0x02,	/**< Network unavailable */
	LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE		= TIZEN_ERROR_LOCATION_MANAGER | 0x03,	/**< Location service is not available */
	LOCATIONS_ERROR_GPS_SETTING_OFF				= TIZEN_ERROR_LOCATION_MANAGER | 0x04,	/**< GPS/WPS setting is not enabled */
	LOCATIONS_ERROR_SECURITY_RESTRICTED			= TIZEN_ERROR_LOCATION_MANAGER | 0x05,	/**< Restricted by security system policy */
} location_error_e;


/**
 * @brief Enumeration for Location method type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATIONS_METHOD_NONE = -1,	/**< Undefined method */
	LOCATIONS_METHOD_HYBRID,	/**< This method selects the best method available at the moment */
	LOCATIONS_METHOD_GPS,		/**< This method uses Global Positioning System */
	LOCATIONS_METHOD_WPS,		/**< This method uses WiFi Positioning System */
} location_method_e;

/**
 * @brief Enumeration for Approximate accuracy level of given information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATIONS_ACCURACY_NONE = 0,	/**< Invalid data */
	LOCATIONS_ACCURACY_COUNTRY,		/**< Country accuracy level */
	LOCATIONS_ACCURACY_REGION,		/**< Regional accuracy level */
	LOCATIONS_ACCURACY_LOCALITY,	/**< Local accuracy level */
	LOCATIONS_ACCURACY_POSTALCODE,	/**< Postal accuracy level */
	LOCATIONS_ACCURACY_STREET,		/**< Street accuracy level */
	LOCATIONS_ACCURACY_DETAILED,	/**< Detailed accuracy level */
} location_accuracy_level_e;

/**
 * @brief Enumeration for the state of the location service.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATIONS_SERVICE_DISABLED,			/**< Service is disabled */
	LOCATIONS_SERVICE_ENABLED,			/**< Service is enabled */
} location_service_state_e;

/**
 * @brief Enumeration for the location service accessibility state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATIONS_ACCESS_STATE_NONE,		/**< Access state is not determined */
	LOCATIONS_ACCESS_STATE_DENIED,		/**< Access denied */
	LOCATIONS_ACCESS_STATE_ALLOWED,		/**< Access authorized */
} location_accessibility_state_e;

/**
 * @brief The location manager handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct location_manager_s *location_manager_h;

/**
 * @}
 */
/*
 * Location Manager
*/
/**
 * @addtogroup CAPI_LOCATION_MANAGER_MODULE
 * @{
 */
/**
 * @brief Called at defined interval with updated position information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] latitude		The updated latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude		The updated longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] altitude		The updated altitude (meters)
 * @param[in] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] user_data		The user data passed from the call registration function
 * @pre location_manager_start() will invoke this callback if you register this callback using location_manager_set_position_updated_cb().
 * @see location_manager_start()
 * @see location_manager_set_position_updated_cb()
 */
typedef void(*location_position_updated_cb)(double latitude, double longitude, double altitude, time_t timestamp, void *user_data);

/**
 * @brief Called at defined interval with updated velocity information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] speed			The updated speed (km/h)
 * @param[in] direction		The updated direction (in degrees from the north)
 * @param[in] climb			The updated climb (km/h)
 * @param[in] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_manager_start() will invoke this callback if you register this callback using location_manager_set_velocity_updated_cb().
 * @see location_manager_start()
 * @see location_manager_set_velocity_updated_cb()
 */
typedef void(*location_velocity_updated_cb)(double speed, double direction, double climb, time_t timestamp, void *user_data);

/**
 * @brief Called at defined interval with updated location information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] error			#LOCATIONS_ERROR_NONE Successful
 * @param[in] latitude		The updated latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude		The updated longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] altitude		The updated altitude (meters)
 * @param[in] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] speed			The updated speed (km/h)
 * @param[in] direction		The updated direction (in degrees from the north)
 * @param[in] climb			The updated climb (km/h)
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_manager_request_single_location() will invoke this callback.
 * @see location_manager_request_single_location()
 */
typedef void(*location_updated_cb)(location_error_e error, double latitude, double longitude, double altitude, time_t timestamp, double speed, double direction, double climb, void *user_data);

/**
 * @brief Called when the state of location service is changed from enabled to disabled or vice versa.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] state			The service state
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre Either location_manager_start() or location_manager_stop() will invoke this callback if you register this callback using location_manager_set_service_state_changed_cb()
 * @see location_manager_start()
 * @see location_manager_stop()
 * @see location_manager_set_service_state_changed_cb()
 * @see #location_service_state_e
 */
typedef void(*location_service_state_changed_cb)(location_service_state_e state, void *user_data);

/**
 * @brief Called when the user defined zones are entered or exited.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] state			The boundary state
 * @param[in] latitude		The updated latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude		The updated longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] altitude		The updated altitude (meters)
 * @param[in] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_manager_start() will invoke this callback if you register this callback using location_manager_set_zone_changed_cb().
 * @see #location_boundary_state_e
 * @see location_manager_start()
 * @see location_manager_set_zone_changed_cb()
 */
typedef void(*location_zone_changed_cb)(location_boundary_state_e state, double latitude, double longitude, double altitude, time_t timestamp, void *user_data);

/**
 * @brief Called when the state of location method is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] method		The method changed on setting
 * @param[in] enable		The setting value changed
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_setting_changed_cb() will invoke this callback if you register this callback using location_manager_set_setting_changed_cb()
 * @see location_manager_set_setting_changed_cb()
 * @see location_manager_unset_setting_changed_cb()
 */
typedef void(*location_setting_changed_cb)(location_method_e method, bool enable, void *user_data);

/**
 * @brief Called once for each location bound.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_manager_foreach_boundary() will invoke this callback.
 * @see location_manager_foreach_boundary()
 */
typedef bool(*location_bounds_cb)(location_bounds_h bounds, void *user_data);

/**
 * @brief Called at defined interval with updated location information.
 * @since_tizen 2.4
 * @param[in] latitude		The updated latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] longitude		The updated longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] altitude		The updated altitude (meters)
 * @param[in] speed			The updated speed (km/h)
 * @param[in] direction		The updated direction (in degrees from the north)
 * @param[in] horizontal_accuracy		The horizontal accuracy (meters)
 * @param[in] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] user_data		The user data passed from the callback registration function
 * @pre location_manager_start() will invoke this callback if you register this callback using location_manager_set_position_updated_cb()
 * @see location_manager_start()
 * @see location_manager_set_distance_based_location_changed_cb()
 * @see location_manager_set_location_changed_cb()
 */
typedef void(*location_changed_cb)(double latitude, double longitude, double altitude, double speed, double direction, double horizontal_accuracy, time_t timestamp, void *user_data);

/**
 * @brief Checks whether the given location method is available.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remark The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] method The location method to be checked
 * @return @c true if the specified location method is supported, otherwise @c false if it is not available
 * @retval #LOCATIONS_ERROR_NONE Successful
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED Not supported
 * @retval #LOCATIONS_ERROR_INCORRECT_METHOD Incorrect method
 * @see	location_manager_create()
 * @see location_manager_get_method()
 */
bool location_manager_is_supported_method(location_method_e method);

/**
 * @brief Checks whether the given location method is enabled or not on setting.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] method		The location method to be checked
 * @param[out] enable		The result value of checking the given location method
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE Successful
 * @retval #LOCATIONS_ERROR_INCORRECT_METHOD Incorrect method
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_manager_create()
 * @see location_manager_set_setting_changed_cb()
 * @see location_manager_unset_setting_changed_cb()
 */
int location_manager_is_enabled_method(location_method_e method, bool *enable);

/**
 * @platform
 * @brief Enable the given location method.
 * @since_tizen 2.3.1
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/location.enable
 * @param[in] method		The location method to be checked
 * @param[in] enable		The value to set
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE Successful
 * @retval #LOCATIONS_ERROR_INCORRECT_METHOD Incorrect method
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED Permission denied
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_manager_is_enabled_method()
 * @see location_manager_create()
 */
int location_manager_enable_method(const location_method_e method, const bool enable);

/**
 * @brief Creates a new location manager.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a manager using location_manager_destroy().
 * @param[in] method		The location method
 * @param[out] manager		The location manager handle that is newly created
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_OUT_OF_MEMORY		Out of memory
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE Service not available
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_manager_destroy()
 */
int location_manager_create(location_method_e method, location_manager_h *manager);

/**
 * @brief Releases the location manager.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_manager_create()
*/
int location_manager_destroy(location_manager_h manager);

/**
 * @brief Request to update current location once.
 * @remarks Do not request to start the location service using the same manager handler
 * Calling this function invokes a location service event. When the location service is updated, location_updated_cb
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[in] timeout		Timeout to stop requesting single location after
 * @param[in] callback		The location callback function to register
 * @param[in] user_data		The user data to be passed to the callback function
 * @retval #LOCATIONS_ERROR_NONE						Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER			Invalid parameter
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE		Service not available
 * @retval #LOCATIONS_ERROR_NETWORK_FAILED				Network failed
 * @retval #LOCATIONS_ERROR_GPS_SETTING_OFF				GPS is not enabled
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED	The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED				Not supported
 * @post It invokes location_updated_cb().
 */
int location_manager_request_single_location(location_manager_h manager, int timeout, location_updated_cb callback, void *user_data);

/**
 * @brief Starts the location service.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 *
 * @remarks There is no limit on number of location managers for which this function was called.
 *
 * @remarks Calling this function invokes a location service event. When the location service is enabled, the service state change callback
 *		(set using location_manager_set_service_state_changed_cb()) notifies the user with #LOCATIONS_SERVICE_ENABLED as
 *		the first argument, and the service starts.
 *
 * @remarks The started service is a requirement for calling these functions:
 *		location_manager_get_position(), location_manager_get_velocity(), location_manager_get_accuracy(),
 *		gps_status_get_nmea(), gps_status_get_satellite(), gps_status_foreach_satellites_in_view().
 *
 * @remarks Once you stop the service using location_manager_stop(), you can no longer call the functions listed above.
 *
 * @remarks Starting and stopping the service is in the scope of the given location manager only (if there is more than one manager,
 *		starting and stopping should be executed for each of them separately).
 *
 * @param[in] manager		The location manager handle
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid parameter
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_NETWORK_FAILED			Network failed
 * @retval #LOCATIONS_ERROR_GPS_SETTING_OFF			GPS is not enabled
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @post It invokes location_position_updated_cb(), location_velocity_updated_cb(), location_zone_changed_cb(), and location_service_state_changed_cb().
 * @see location_manager_stop()
 * @see location_manager_get_position()
 * @see location_manager_get_velocity()
 * @see location_manager_get_accuracy()
 * @see location_manager_set_service_state_changed_cb()
 * @see location_manager_set_position_updated_cb()
 * @see location_position_updated_cb()
 * @see location_manager_set_velocity_updated_cb()
 * @see location_velocity_updated_cb()
 * @see location_manager_set_zone_changed_cb()
 * @see location_zone_changed_cb()
 */
int location_manager_start(location_manager_h manager);

/**
 * @brief Stops the location service.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This function initiates the process of stopping the service. When the process is finished, callback set using
 * #location_manager_set_service_state_changed_cb() will be called, with #LOCATIONS_SERVICE_DISABLED as first argument.
 * When that happens, the service is stopped and the user is notified.
 *
 * @remarks You can stop and start the location manager as needed.
 *
 * @param[in] manager		The location manager handle
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE						Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER			Invalid parameter
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE		Service not available
 * @retval #LOCATIONS_ERROR_NETWORK_FAILED				Network failed
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED				Not supported
 * @see location_manager_start()
 * @see	location_manager_set_service_state_changed_cb()
 * @see	location_service_state_changed_cb()
 */
int location_manager_stop(location_manager_h manager);

/**
 * @brief Adds a bounds for a given location manager.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] bounds		The location bounds handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER			Invalid parameter
 * @retval #LOCATIONS_ERROR_OUT_OF_MEMORY				Out of memory
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED				Not supported
 * @post It invokes location_manager_set_zone_changed_cb() when a boundary is entered or exited, if you set a callback with location_manager_set_zone_changed_cb().
 * @see location_manager_remove_boundary()
 * @see location_manager_set_zone_changed_cb()
 */
int location_manager_add_boundary(location_manager_h manager, const location_bounds_h bounds);

/**
 * @brief Deletes a bounds for a given location manager.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] bounds		The location bounds handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE						Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER			Invalid parameter
 * @retval #LOCATIONS_ERROR_OUT_OF_MEMORY				Out of memory
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED				Not supported
 * @see location_manager_add_boundary()
 */
int location_manager_remove_boundary(location_manager_h manager, const location_bounds_h bounds);

/**
 * @brief Retrieves all location bounds by invoking a specific callback for each location bounds
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] callback		The iteration callback
 * @param[in] user_data		The user data to be passed to the callback function
 * @return	@c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE						Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER			Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED				Not supported
 * @post location_bounds_cb() will be invoked
 * @see location_manager_add_boundary()
 * @see location_manager_remove_boundary()
 * @see location_bounds_cb()
 */
int location_manager_foreach_boundary(location_manager_h manager, location_bounds_cb callback, void *user_data);

/**
 * @brief Gets the given location manager's method.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[out] method		The location method
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_create()
 */
int location_manager_get_method(location_manager_h manager, location_method_e *method);

/**
 * @brief Gets the current position information.
 * @details The result contains the current altitude, latitude, and longitude, with a measurement timestamp.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] altitude		The current altitude (meters)
 * @param[out] latitude		The current latitude [-90.0 ~ 90.0] (degrees)
 * @param[out] longitude	The current longitude [-180.0 ~ 180.0] (degrees)
 * @param[out] timestamp	The timestamp (time when measurement took place or @c 0 if valid)
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_GPS_SETTING_OFF			GPS is not enabled
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 */
int location_manager_get_position(location_manager_h manager, double *altitude, double *latitude, double *longitude, time_t *timestamp);

/**
 * @brief Gets the current position information.
 * @details The result contains the current altitude, latitude, longitude, climb, direction, speed, level, horizontal and vertical with a measurement timestamp.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] altitude		The current altitude (meters)
 * @param[out] latitude		The current latitude [-90.0 ~ 90.0] (degrees)
 * @param[out] longitude	The current longitude [-180.0 ~ 180.0] (degrees)
 * @param[out] climb		The climb (km/h)
 * @param[out] direction	The direction, degrees from the north
 * @param[out] speed		The speed (km/h)
 * @param[out] level		The accuracy level
 * @param[out] horizontal	The horizontal accuracy (meters)
 * @param[out] vertical		The vertical accuracy (meters)
 * @param[out] timestamp	The timestamp (time when measurement took place or @c 0 if valid)
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_GPS_SETTING_OFF			GPS is not enabled
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 */
int location_manager_get_location(location_manager_h manager, double *altitude, double *latitude, double *longitude, double *climb, double *direction, double *speed, location_accuracy_level_e *level, double *horizontal, double *vertical, time_t *timestamp);

/**
 * @brief Gets the current velocity information.
 * @details The result contains the current climb, direction, and speed, with a measurement timestamp.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] climb		The climb (km/h)
 * @param[out] direction	The direction, degrees from the north
 * @param[out] speed		The speed (km/h)
 * @param[out] timestamp	The timestamp (time when measurement took place or @c 0 if valid)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_GPS_SETTING_OFF			GPS is not enabled
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 */
int location_manager_get_velocity(location_manager_h manager, double *climb, double *direction, double *speed, time_t *timestamp);

/**
 * @brief Gets the current accuracy information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] level		The accuracy level
 * @param[out] horizontal	The horizontal accuracy (meters)
 * @param[out] vertical		The vertical accuracy (meters)
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_GPS_SETTING_OFF			GPS is not enabled
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 */
int location_manager_get_accuracy(location_manager_h manager, location_accuracy_level_e *level, double *horizontal, double *vertical);

/**
 * @brief Gets the last position information which is recorded.
 * @details The @a altitude, @a latitude, @a longitude, and @c timestamp values should be 0, if there is no record of any previous position information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] altitude		The last altitude (meters)
 * @param[out] latitude		The last latitude [-90.0 ~ 90.0] (degrees)
 * @param[out] longitude	The last longitude [-180.0 ~ 180.0] (degrees)
 * @param[out] timestamp	The timestamp (time when measurement took place or @c 0 if valid)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid argument
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @pre The location manager handle must be created by location_manager_create()
 */
int location_manager_get_last_position(location_manager_h manager, double *altitude, double *latitude, double *longitude, time_t *timestamp);

/**
 * @brief Gets the last location information.
 * @details The @a altitude, @a latitude, @a longitude, @a climb, @a direction, @a speed and @c timestamp values should be 0, if there is no record of any previous position information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] altitude		The current altitude (meters)
 * @param[out] latitude		The current latitude [-90.0 ~ 90.0] (degrees)
 * @param[out] longitude	The current longitude [-180.0 ~ 180.0] (degrees)
 * @param[out] climb		The climb (km/h)
 * @param[out] direction	The direction, degrees from the north
 * @param[out] speed		The speed (km/h)
 * @param[out] level		The accuracy level
 * @param[out] horizontal	The horizontal accuracy (meters)
 * @param[out] vertical		The vertical accuracy (meters)
 * @param[out] timestamp	The timestamp (time when measurement took place or @c 0 if valid)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid argument
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @pre The location manager handle must be created by location_manager_create()
 */
int location_manager_get_last_location(location_manager_h manager, double *altitude, double *latitude, double *longitude, double *climb, double *direction, double *speed, location_accuracy_level_e *level, double *horizontal, double *vertical, time_t *timestamp);


/**
 * @brief Gets the last velocity information which is recorded.
 * @details The @a climb, @a direction and @a speed values should be @c 0, if there is no record of any previous velocity information.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] climb		The last climb (km/h)
 * @param[out] direction	The last direction, degrees from the north
 * @param[out] speed		The last speed (km/h)
 * @param[out] timestamp	The timestamp (time when measurement took place or @c 0 if valid)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid argument
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @pre The location manager handle must be created by location_manager_create()
 */
int location_manager_get_last_velocity(location_manager_h manager, double *climb, double *direction, double *speed, time_t *timestamp);

/**
 * @brief Gets the last accuracy information which is recorded.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[out] level		The last accuracy level
 * @param[out] horizontal	The last horizontal accuracy (meters)
 * @param[out] vertical		The last vertical accuracy (meters)
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid argument
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED	Not supported
 * @pre The location manager handle must be created by location_manager_create()
 */
int location_manager_get_last_accuracy(location_manager_h manager, location_accuracy_level_e *level, double *horizontal, double *vertical);

/**
 * @brief Gets the current application's location accessibility status.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[out] state		The current location service accessibility status.
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 */
int location_manager_get_accessibility_state(location_accessibility_state_e *state);

/**
 * @brief Registers a callback function to be invoked at defined interval with updated position information.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] interval		The interval [1 ~ 120] (seconds)
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post location_position_updated_cb() will be invoked
 * @see location_manager_unset_position_updated_cb()
 * @see location_position_updated_cb()
 */
int location_manager_set_position_updated_cb(location_manager_h manager, location_position_updated_cb callback, int interval, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_position_updated_cb()
 */
int location_manager_unset_position_updated_cb(location_manager_h manager);

/**
 * @brief Registers a callback function to be invoked at defined interval with updated velocity information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] interval		The interval [1 ~ 120] (seconds)
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post location_velocity_updated_cb() will be invoked
 * @see location_manager_unset_velocity_updated_cb()
 * @see location_velocity_updated_cb()
 */
int location_manager_set_velocity_updated_cb(location_manager_h manager, location_velocity_updated_cb callback, int interval, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_velocity_updated_cb()
 */
int location_manager_unset_velocity_updated_cb(location_manager_h manager);

/**
 * @brief Registers a callback function to be invoked when the location service state is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post location_service_state_changed_cb() will be invoked
 * @see location_manager_unset_service_state_changed_cb()
 * @see location_service_state_changed_cb()
 * @see location_manager_start()
 * @see location_manager_stop()
 * @see #location_service_state_e
*/
int location_manager_set_service_state_changed_cb(location_manager_h manager, location_service_state_changed_cb callback, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_service_state_changed_cb()
 */
int location_manager_unset_service_state_changed_cb(location_manager_h manager);

/**
 * @brief Registers a callback function to be invoked when the previously set boundary area is entered or left.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @pre		location_manager_add_boundary() is called before.
 * @post location_zone_changed_cb() will be invoked
 * @see location_manager_unset_zone_changed_cb()
 * @see location_zone_changed_cb()
 */
int location_manager_set_zone_changed_cb(location_manager_h manager, location_zone_changed_cb callback, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] manager		The location manager handle
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_zone_changed_cb()
 */
int location_manager_unset_zone_changed_cb(location_manager_h manager);

/**
 * @brief Registers a callback function to be invoked when the location setting is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] method		The method to observe
 * @param[in] callback		The callback function to register
 * @param[in] user_data		The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post location_setting_changed_cb() will be invoked
 * @see location_manager_unset_setting_changed_cb()
 * @see location_setting_changed_cb()
 */
int location_manager_set_setting_changed_cb(location_method_e method, location_setting_changed_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] method		The method to observe
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval	#LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_setting_changed_cb()
 */
int location_manager_unset_setting_changed_cb(location_method_e method);

/**
 * @brief Gets the distance in meters between two locations.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] start_latitude		The starting latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] start_longitude		The starting longitude [-180.0 ~ 180.0] (degrees)
 * @param[in] end_latitude			The ending latitude [-90.0 ~ 90.0] (degrees)
 * @param[in] end_longitude			The ending longitude [-180.0 ~ 180.0] (degrees)
 * @param[out] distance				The distance between two locations (meters)
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid argument
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 */
int location_manager_get_distance(double start_latitude, double start_longitude, double end_latitude, double end_longitude, double *distance);

/**
 * @brief Registers a callback function to be invoked at minimum interval or minimum distance with updated position information.
 * @since_tizen 2.4
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] interval		The minimum interval between position updates [1 ~ 120] (seconds)
 * @param[in] distance		The minimum distance between position updates [1 ~ 120] (meters)
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post location_changed_cb() will be invoked
 * @see location_manager_unset_distance_based_location_changed_cb()
 * @see location_changed_cb()
 */
int location_manager_set_distance_based_location_changed_cb(location_manager_h manager, location_changed_cb callback, int interval, double distance, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen 2.4
 * @param[in] manager		The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_distance_based_location_changed_cb()
 */
int location_manager_unset_distance_based_location_changed_cb(location_manager_h manager);

/**
 * @brief Registers a callback function to be invoked at defined interval with updated location information.
 * @since_tizen 2.4
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] interval		The interval [1 ~ 120] (seconds)
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post location_changed_cb() will be invoked
 * @see location_manager_unset_location_changed_cb()
 * @see location_changed_cb()
 */
int location_manager_set_location_changed_cb(location_manager_h manager, location_changed_cb callback, int interval, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen 2.4
 * @param[in] manager		The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see location_manager_set_location_changed_cb()
 */
int location_manager_unset_location_changed_cb(location_manager_h manager);

/**
 * @}
 */

/*
 * GPS Status & Satellites
 */

/**
 * @addtogroup CAPI_LOCATION_GPS_STATUS_MODULE
 * @{
 */

/**
 * @brief Called once for each satellite in range.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] azimuth		The azimuth of the satellite (degrees)
 * @param[in] elevation		The elevation of the satellite (meters)
 * @param[in] prn			The PRN of the satellite
 * @param[in] snr			The SNR of the satellite [dB]
 * @param[in] is_active		The flag signaling if satellite is in use
 * @param[in] user_data		The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre gps_status_foreach_satellites_in_view() will invoke this callback.
 * @pre gps_status_foreach_last_satellites_in_view() will invoke this callback.
 * @see gps_status_foreach_satellites_in_view()
 */
typedef bool(*gps_status_get_satellites_cb)(unsigned int azimuth, unsigned int elevation, unsigned int prn, int snr, bool is_active, void *user_data);

/**
 * @brief Called at defined interval with updated satellite information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[out] num_of_active	The last number of active satellites
 * @param[out] num_of_inview	The last number of satellites in view
 * @param[out] timestamp		The last timestamp (time when measurement took place or @c 0 if valid)
 * @param[in] user_data			The user data passed from the call registration function
 * @pre location_manager_start() will invoke this callback if you register this callback using location_manager_set_position_updated_cb()
 * @see location_manager_start()
 * @see location_manager_set_position_updated_cb()
 */
typedef void(*gps_status_satellite_updated_cb)(int num_of_active, int num_of_inview, time_t timestamp, void *user_data);

/**
 * @brief Gets the GPS NMEA data.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This call is valid only for location managers with #LOCATIONS_METHOD_GPS method.
 *		 You must release @a nmea using @c free().
 * @param[in] manager	The location manager handle
 * @param[out] nmea		The NMEA data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 * @see location_manager_start()
 */
int gps_status_get_nmea(location_manager_h manager, char **nmea);

/**
 * @brief Gets the information of satellites.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks This call is valid only for location managers with #LOCATIONS_METHOD_GPS method.
 * @param[in] manager			The location manager handle
 * @param[out] num_of_active	The number of active satellites
 * @param[out] num_of_inview	The number of satellites in view
 * @param[out] timestamp		The timestamp (time when measurement took place or @c 0 if valid)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 * @pre The gps_status_satellite_updated_cb must be set with gps_status_set_satellite_updated_cb()
 * @see	gps_status_foreach_satellites_in_view()
 */
int gps_status_get_satellite(location_manager_h manager, int *num_of_active, int *num_of_inview, time_t *timestamp);

/**
 * @brief Registers a callback function to be invoked at defined interval with updated satellite information.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @param[in] callback		The callback function to register
 * @param[in] interval		The interval [1 ~ 120] (seconds)
 * @param[in] user_data		The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @post gps_status_satellite_updated_cb() will be invoked
 * @see gps_status_unset_satellite_updated_cb()
 * @see gps_status_satellite_updated_cb()
 * @see gps_status_get_satellite()
 * @see gps_status_foreach_satellites_in_view()
 * @see gps_status_get_last_satellite()
 * @see gps_status_foreach_last_satellites_in_view()
 */
int gps_status_set_satellite_updated_cb(location_manager_h manager, gps_status_satellite_updated_cb callback, int interval, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @param[in] manager		The location manager handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE				Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED		Not supported
 * @see gps_status_set_satellite_updated_cb()
 */
int gps_status_unset_satellite_updated_cb(location_manager_h manager);

/**
 * @brief Invokes the callback function for each satellite.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks This function is valid only for location managers with the #LOCATIONS_METHOD_GPS method.
 * @param[in] manager		The location manager handle
 * @param[in] callback		The iteration callback function
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_INCORRECT_METHOD		Incorrect method
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 * @pre The gps_status_satellite_updated_cb must be set with gps_status_set_satellite_updated_cb()
 * @post It invokes gps_status_get_satellites_cb().
 * @see	gps_status_get_satellite()
 * @see	gps_status_get_satellites_cb()
 */
int gps_status_foreach_satellites_in_view(location_manager_h manager, gps_status_get_satellites_cb callback, void *user_data);

/**
 * @brief Gets the last information of satellites.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks This call is valid only for location managers with #LOCATIONS_METHOD_GPS method.
 * @param[in] manager			The location manager handle
 * @param[out] num_of_active	The last number of active satellites
 * @param[out] num_of_inview	The last number of satellites in view
 * @param[out] timestamp		The last timestamp (time when last measurement took place or @c 0 if valid)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_INCORRECT_METHOD		Incorrect method
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 * @pre The gps_status_satellite_updated_cb must be set with gps_status_set_satellite_updated_cb()
 * @see	gps_status_foreach_satellites_in_view()
 */
int gps_status_get_last_satellite(location_manager_h manager, int *num_of_active, int *num_of_inview, time_t *timestamp);

/**
 * @brief Invokes the callback function for each last satellite which is recorded.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/location
 * @remarks This function is valid only for location managers with the #LOCATIONS_METHOD_GPS method.
 * @param[in] manager		The location manager handle
 * @param[in] callback		The iteration callback function
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATIONS_ERROR_NONE					Successful
 * @retval #LOCATIONS_ERROR_INVALID_PARAMETER		Invalid argument
 * @retval #LOCATIONS_ERROR_INCORRECT_METHOD		Incorrect method
 * @retval #LOCATIONS_ERROR_SERVICE_NOT_AVAILABLE	Service not available
 * @retval #LOCATIONS_ERROR_ACCESSIBILITY_NOT_ALLOWED The application does not have the privilege to call this method
 * @retval #LOCATIONS_ERROR_NOT_SUPPORTED			Not supported
 * @pre The location service state must be #LOCATIONS_SERVICE_ENABLED with location_manager_start()
 * @pre The gps_status_satellite_updated_cb must be set with gps_status_set_satellite_updated_cb()
 * @post It invokes gps_status_get_satellites_cb().
 * @see	gps_status_get_last_satellite()
 * @see	gps_status_get_satellites_cb()
 */
int gps_status_foreach_last_satellites_in_view(location_manager_h manager, gps_status_get_satellites_cb callback, void *user_data);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_LOCATION_LOCATIONS_H__ */
