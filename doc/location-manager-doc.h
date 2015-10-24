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


#ifndef __TIZEN_LOCATION_LOCATION_MANAGER_DOC_H__
#define __TIZEN_LOCATION_LOCATION_MANAGER_DOC_H__

/**
 * @ingroup CAPI_LOCATION_FRAMEWORK
 * @defgroup CAPI_LOCATION_MANAGER_MODULE Location Manager
 * @brief This Location Manager API provides functions for obtaining information related to geographical location.
 * This API provides functions to acquire information related to the current position. Notifications on events like service becoming
 * enabled or disabled, new position data being available and others can also be acquired.
 *
 * @section CAPI_LOCATION_MANAGER_MODULE_HEADER Required Header
 *   \#include <locations.h>
 *
 * @section CAPI_LOCATION_MANAGER_MODULE_OVERVIEW Overview
 * The Location Manager API provides functions to acquire information about geographical location, including both as accurate a current position as possible,
 *  and receiving notification of changes in position, crossing boundary "fences", and velocity changes detected by the device.
 *
 * The related Geolocation API can be used to convert the location to a physical address.
 *
 * Notifications can be received about the following events:
 * - Change in service status (enabled / disabled)
 * - New position and velocity information becoming available
 * - Given area being entered or left (geofencing)
 *
 * Information on GPS satellites is provided by the GPS Status & Satellite API.
 *
 * The main component of the Location Service is the location manager. A location manager serves as a gateway so the application does not have
 * to manage the details of the back-end connection, which might be GPS, WiFi or others, although it may instruct the location manager
 * as to a preference. The location manager also manages the callback functions which it invokes when a given event takes place.
 *
 * The location manager has the following properties:
 * - Method
 * - Boundary
 * - 'Position updated' callback
 * - 'Service state change' callback
 * - 'Velocity updated' callback
 * - 'Boundary event' callback
 *
 * The method property of a location manager (#location_method_e) determines the source of location information. It can be set only
 * once - when the location manager is instantiated. The method can be one specific location method, or hybrid.
 *
 * A location manager set to the hybrid method will automatically choose
 * the best method available at the moment, choosing between GPS, WPS
 * and WPS. Note that the method may change any time, due to signals
 * coming in/out of range, and so on. If more than one method is available,
 * priorities are as follows:
 *
 * 1. GPS\n
 * 2. WPS\n
 *
 * If no methods are available, the service state is changed to disabled
 * and the appropriate callback is called. See location_manager_set_service_state_changed_cb() and location_service_state_changed_cb().
 *
 * The boundary property describes a border enclosing an area. The area
 * can be circular - defined by its center in geographic coordinates a radius, or rectangular - defined by the geographic coordinates of its
 * top left and bottom right corner.
 * The location manager will use the selected method to track if the area has been entered or exited.
 * The location manager will send asynchronous notifications by invoking
 * previously registered callback functions on such change events.
 *
 * There may be multiple location managers. Callback functions to receive
 * notifications are registered to a specific location manager, not to the location service as a whole.
 * So it is, for example, possible to define several different geographic areas or fences, using different location methods,
 * each being tracked by a different location manager and leading to distinct callbacks.
 *
 * Setting and unsetting callbacks for the manager can be done at any time, however before callbacks set for a given location manager can
 * be invoked, the location service must be started for that location manager. This constraint holds for managers with GPS, WPS and
 * Hybrid methods. See the Location manager life cycle section below. Note the start/stop functions apply only to
 * the individual location manager and do not affect any other location managers that may be running.
 *
 * Only one callback can be set for a given event within the scope of a single location manager. Successive callback setting will result in
 * the new callback replacing the old one. If an 'unset callback' function is called when there is no callback set, no error is returned.
 * Callbacks can be set and unset any number of times.
 *
 * @subsection CAPI_LOCATION_MANAGER_MODULE_MANAGER_LIFECYCLE Location manager life cycle
 *
 * 1. Create location manager ( location_manager_create() )\n
 * 2. Set callbacks and other required information\n
 * 3. Start service with location_manager_start()\n
 * 4. Wait until state change callback ( location_service_state_changed_cb() ) is called with #LOCATIONS_SERVICE_ENABLED as first argument\n
 * 5. Other callbacks are working now, wait for events and process\n
 * 6. Stop service with location_manager_stop()\n
 * 7. Wait until state change callback ( location_service_state_changed_cb() ) is called with #LOCATIONS_SERVICE_DISABLED as first argument\n
 * 8. Destroy location manager (location_manager_destroy())\n
 *
 * After stopping the location service for a given location manager,
 * the location manager can still be used. So some functions can be called if they do not require a location service running on your device.
 * The location service can be re-started later.
 *
 * If a location manager is no longer needed and is going to be destroyed, it is not mandatory to call location_manager_stop(),
 * as location_manager_destroy() will call it automatically.
 *
 * Note that after a location manager has been destroyed, the handle (#location_manager_h) can be reused - location_manager_create() can
 * be called again for this handle. In other words, the state before creation and after destruction can be treated as the same state.
 *
 * @section CAPI_LOCATION_MANAGER_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *	- http://tizen.org/feature/location\n
 *	- http://tizen.org/feature/location.gps\n
 *	- http://tizen.org/feature/location.wps\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a devrice supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
 */

/**
 * @ingroup CAPI_LOCATION_MANAGER_MODULE
 * @defgroup CAPI_LOCATION_GPS_STATUS_MODULE GPS Status & Satellite
 * @brief The GPS Status & Satellite API provides functions to acquire information about GPS satellites in range and in use.
 *
 * @section CAPI_LOCATION_GPS_STATUS_MODULE_HEADER Required Header
 *   \#include <locations.h>
 *
 * @section CAPI_LOCATION_GPS_STATUS_MODULE_OVERVIEW Overview
 *  The GPS Status & Satellite API provides functions to acquire data on satellites that are currently visible to the device. Information
 *  like azimuth and elevation of each satellite is available, along with number of satellites in range and in use.
 *
 * @par System Info note:
 * All of the functions require a started location manager with the #LOCATIONS_METHOD_GPS method set. See the Location Manager API for details.
 *
 * The gps_status_get_nmea() function gets NMEA data from the GPS system.  NMEA 0183 (or NMEA for short) is a combined electrical and data
 * specification for communication between electronic devices. NMEA standard uses a simple ASCII, serial communications protocol that
 * defines how data is transmitted in a "sentence" from one "talker"  to multiple "listeners" at a time. For more details, see <a href = "http://en.wikipedia.org/wiki/NMEA_0183">NMEA_0183</a>.
 *
 * The gps_status_get_satellite() method returns a count of satellites in view. The gps_status_foreach_satellites_in_view() method is an
 * iterator which will invoke a callback function for each satellite in view. The  gps_status_get_satellite() method returns a count of satellites in use.
 *
 * @section CAPI_LOCATION_GPS_STATUS_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *	- http://tizen.org/feature/location\n
 *	- http://tizen.org/feature/location.gps\n
 *	- http://tizen.org/feature/location.gps.satellite\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
*/

/**
 * @ingroup CAPI_LOCATION_MANAGER_MODULE
 * @defgroup CAPI_LOCATION_BOUNDS_MODULE Location Bounds
 * @brief The Location Bounds APIs provides functions related to geographic bounds information.
 *
 *
 * @section CAPI_LOCATION_BOUNDS_MODULE_HEADER Required Header
 *   \#include <locations.h>
 *
 * @section CAPI_LOCATION_BOUNDS_MODULE_OVERVIEW Overview
 *
 * Boundary defines geographical boundary. It is same as geo-fence which is a virtual perimeter for a real-world geographic area.
 * If you create a boundary, you can trigger some activities when a device enters(or exits) the boundaries defined by you.
 *
 * @section CAPI_LOCATION_BOUNDS_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *	- http://tizen.org/feature/location\n
 *	- http://tizen.org/feature/location.gps\n
 *	- http://tizen.org/feature/location.wps\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 *
 */

/**
 * @internal
 * @{
 * @ingroup CAPI_LOCATION_FRAMEWORK
 * @defgroup CAPI_LOCATION_PREF_MODULE Location Preference
 * @brief The Location Preference APIs provides functions related to Maps
 * Service.
 *
 *
 * @section CAPI_LOCATION_PREF_MODULE_HEADER Required Header
 *   \#include <location_preference_product.h>
 *
 * @section CAPI_LOCATION_PREF_MODULE_OVERVIEW Overview
 *
 * You can set some options with Location Preference when you use Maps Service(geocoder, poi, and route) module.
 *
 * @section CAPI_LOCATION_PREF_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *	- http://tizen.org/feature/location\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature"><b>Feature Element</b>.</a>
 * @}
 *
 */

#endif  /* __TIZEN_LOCATION_LOCATION_MANAGER_DOC_H__ */
