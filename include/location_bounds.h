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

#ifndef __TIZEN_LOCATION_BOUNDS_H__
#define __TIZEN_LOCATION_BOUNDS_H__

#include <tizen_type.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOCATION_BOUNDS_ERROR_CLASS			TIZEN_ERROR_LOCATION_MANAGER | 0x20

/**
 * @addtogroup CAPI_LOCATION_BOUNDS_MODULE
 * @{
 */

/**
 * @brief The structure type to represent coordinates with latitude and longitude.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct {
	double latitude;	/**< The latitude [-90.0 ~ 90.0] (degrees) */
	double longitude;	/**< The longitude [-180.0 ~ 180.0] (degrees) */
} location_coords_s;

/**
 * @brief Enumeration for error code for Location manager.
 */
typedef enum {
	LOCATION_BOUNDS_ERROR_NONE = TIZEN_ERROR_NONE,								/**< Successful */
	LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,			/**< Out of memory */
	LOCATION_BOUNDS_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,	/**< Invalid parameter */
	LOCATION_BOUNDS_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED,			/**< Not supported */
	LOCATION_BOUNDS_ERROR_INCORRECT_TYPE = LOCATION_BOUNDS_ERROR_CLASS | 0x01,	/**< Incorrect bounds type for a given call */
	LOCATION_BOUNDS_ERROR_IS_ADDED = LOCATION_BOUNDS_ERROR_CLASS | 0x02,		/**< Cannot remove bounds handle from location manager */
} location_bound_error_e;

/**
 * @brief Enumeration for Location boundary type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATION_BOUNDS_RECT = 1,		/**< Rectangular geographical area type. */
	LOCATION_BOUNDS_CIRCLE,			/**< Circle geographical area type.. */
	LOCATION_BOUNDS_POLYGON,		/**< Polygon geographical area type.. */
} location_bounds_type_e;

/**
 * @brief Enumeration for the boundary state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	LOCATIONS_BOUNDARY_IN,			/**< Boundary In (Zone In) */
	LOCATIONS_BOUNDARY_OUT			/**< Boundary Out (Zone Out) */
} location_boundary_state_e;

/**
 * @brief The location boundary handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct location_bounds_s *location_bounds_h;

/**
 * @brief Gets called iteratively to notify you of coordinates of a polygon.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] coords		The coordinates
 * @param[in] user_data		The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre location_bounds_foreach_polygon_coords() will invoke this callback.
 * @see location_bounds_foreach_polygon_coords()
 */
typedef bool (*polygon_coords_cb)(location_coords_s coords, void *user_data);

/**
 * @brief Called when the given boundary is entered or exited.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] state			The boundary state
 * @pre location_manager_start() will invoke this callback if you register this callback using location_bounds_set_state_changed_cb()
 * @see #location_boundary_state_e
 * @see location_manager_start()
 * @see location_bounds_set_state_changed_cb()
 */
typedef void (*location_bounds_state_changed_cb)(location_boundary_state_e state, void *user_data);

/**
 * @brief Creates a rect type of new location bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a bounds using location_bounds_destroy().
 * @param[in] top_left		The top left position
 * @param[in] bottom_right	The bottom right position
 * @param[out] bounds		The location bounds handle that is newly created
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_get_rect_coords()
 * @see location_bounds_destroy()
 */
int location_bounds_create_rect(location_coords_s top_left, location_coords_s bottom_right, location_bounds_h *bounds);

/**
 * @brief Creates a circle type of new location bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a bounds using location_bounds_destroy().
 * @param[in] center		The center position
 * @param[in] radius		The radius of a circle (meters)
 * @param[out] bounds		The location bounds handle that is newly created
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_get_circle_coords()
 * @see location_bounds_destroy()
 */
int location_bounds_create_circle(location_coords_s center, double radius, location_bounds_h *bounds);

/**
 * @brief Creates a polygon type of new location bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a bounds using location_bounds_destroy().
 * @remarks @a length should be more than @c 3 to represent polygon.
 * @param[in] coords_list	The list of coordinates
 * @param[in] length		The length of the coordinates list
 * @param[out] bounds		The location bounds handle that is newly created on success
 * @return @c 0 on success, otherwise a negative error value
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_foreach_polygon_coords()
 * @see location_bounds_destroy()
 */
int location_bounds_create_polygon(location_coords_s *coords_list, int length, location_bounds_h *bounds);

/**
 * @brief Checks whether the bounds contains the specified coordinates.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remark The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] bounds		The location bounds handle
 * @param[in] coords		The coordinates
 * @return @c true if the bounds contains the specified coordinates, otherwise else @c false
 * @exception #LOCATION_BOUNDS_ERROR_NONE Successful
 * @exception #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @exception #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @exception #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_create_rect()
 * @see location_bounds_create_circle()
 * @see location_bounds_create_polygon()
 */
bool location_bounds_contains_coordinates(location_bounds_h bounds, location_coords_s coords);

/**
 * @brief Gets the type of location bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @param[out] type			The type of location bounds
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_create_rect()
 * @see location_bounds_create_circle()
 * @see location_bounds_create_polygon()
 */
int location_bounds_get_type(location_bounds_h bounds, location_bounds_type_e *type);

/**
 * @brief Gets the center position and radius of circle bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds			The location bounds handle
 * @param[out] top_left			The top left position
 * @param[out] bottom_right		The bottom right position
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @retval #LOCATION_BOUNDS_ERROR_INCORRECT_TYPE Incorrect bounds type
 * @see location_bounds_create_rect()
 */
int location_bounds_get_rect_coords(location_bounds_h bounds, location_coords_s *top_left, location_coords_s *bottom_right);

/**
 * @brief Gets the center position and radius of circle bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @param[out] center		The center position of the circle
 * @param[out] radius		The radius of the circle
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @retval #LOCATION_BOUNDS_ERROR_INCORRECT_TYPE Incorrect bounds type
 * @see location_bounds_create_circle()
 */
int location_bounds_get_circle_coords(location_bounds_h bounds, location_coords_s *center, double *radius);

/**
 * @brief Get the coordinates of a polygon.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @param[in] callback		The iteration callback
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @retval #LOCATION_BOUNDS_ERROR_INCORRECT_TYPE Incorrect bounds type
 * @post It invokes polygon_coords_cb() to get coordinates of polygon.
 * @see polygon_coords_cb()
 * @see location_bounds_create_polygon()
 */
int location_bounds_foreach_polygon_coords(location_bounds_h bounds, polygon_coords_cb callback, void *user_data);

/**
 * @brief Releases the location bounds.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_create_rect()
 * @see location_bounds_create_circle()
 * @see location_bounds_create_polygon()
*/
int location_bounds_destroy(location_bounds_h bounds);

/**
 * @brief Registers a callback function to be invoked when the boundary area is entered or exited.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @param[in] callback		The callback function to register
 * @param[in] user_data		The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @post location_bounds_state_changed_cb() will be invoked
 * @see location_bounds_unset_state_changed_cb()
 * @see location_bounds_state_changed_cb()
 */
int location_bounds_set_state_changed_cb(location_bounds_h bounds, location_bounds_state_changed_cb callback, void *user_data);

/**
 * @brief	Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] bounds		The location bounds handle
 * @return @c 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_NOT_SUPPORTED	Not supported
 * @see location_bounds_set_state_changed_cb()
 */
int location_bounds_unset_state_changed_cb(location_bounds_h bounds);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_LOCATION_BOUNDS_H__ */

