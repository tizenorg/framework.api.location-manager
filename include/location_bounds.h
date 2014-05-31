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

#define LOCATION_BOUNDS_ERROR_CLASS          TIZEN_ERROR_LOCATION_CLASS | 0x20

/**
 * @addtogroup CAPI_LOCATION_BOUNDS_MODULE
 * @{
 */

/**
 * @brief Represents a coordinates with latitude and longitude.
 */
typedef struct
{
	double latitude;	/**< The latitude [-90.0 ~ 90.0] (degrees) */
	double longitude;	/**< The longitude [-180.0 ~ 180.0] (degrees) */
} location_coords_s;

/**
 * @brief Enumerations of error code for Location manager.
 */
typedef enum
{
    LOCATION_BOUNDS_ERROR_NONE = TIZEN_ERROR_NONE,					/**< Successful */
    LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,			/**< Out of memory */
    LOCATION_BOUNDS_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,		/**< Invalid parameter */
    LOCATION_BOUNDS_ERROR_INCORRECT_TYPE = LOCATION_BOUNDS_ERROR_CLASS | 0x01,		/**< Incorrect bounds type for a given call */
    LOCATION_BOUNDS_ERROR_IS_ADDED = LOCATION_BOUNDS_ERROR_CLASS | 0x02,		/**< Cannot remove bounds handle from location manager */
} location_bound_error_e;

/**
 * @brief Location boundary type.
 */
typedef enum
{
    LOCATION_BOUNDS_RECT = 1,	/**< Rectangular geographical area type. */
    LOCATION_BOUNDS_CIRCLE,	/**< Circle geographical area type.. */
    LOCATION_BOUNDS_POLYGON,	/**< Polygon geographical area type.. */
} location_bounds_type_e;

/**
 * @brief Enumerations of the boundary state.
 */
typedef enum
{
    LOCATIONS_BOUNDARY_IN,  /**< Boundary In (Zone In) */
    LOCATIONS_BOUNDARY_OUT  /**< Boundary Out (Zone Out) */
} location_boundary_state_e;

/**
 * @brief The location boundary handle.
 */
typedef struct location_bounds_s *location_bounds_h;

/**
 * @brief Gets called iteratively to notify you of coordinates of polygon.
 * @param[in] coords   The coordinates
 * @param[in] user_data	The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre location_bounds_foreach_polygon_coords() will invoke this callback.
 * @see location_bounds_foreach_polygon_coords()
 */
typedef bool (*polygon_coords_cb)(location_coords_s coords, void *user_data);

/**
 * @brief Called when the given boundary is entered or exited.
 * @param[in] state  The boundary state
 * @pre location_manager_start() will invoke this callback if you register this callback using location_bounds_set_state_changed_cb()
 * @see #location_boundary_state_e
 * @see location_manager_start()
 * @see location_bounds_set_state_changed_cb()
 */
typedef void (*location_bounds_state_changed_cb)(location_boundary_state_e state, void *user_data);

/**
 * @brief Creates a rect type of new location bounds.
 * @remarks @a bounds must be released location_bounds_destroy() by you.
 * @param[in] top_left  The top left position
 * @param[in] bottom_right  The bottom right position
 * @param[out] bounds  A location bounds handle to be newly created on success
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see location_bounds_get_rect_coords()
 * @see location_bounds_destroy()
 */
int location_bounds_create_rect(location_coords_s top_left, location_coords_s bottom_right, location_bounds_h* bounds);

/**
 * @brief Creates a circle type of new location bounds.
 * @remarks @a bounds must be released location_bounds_destroy() by you.
 * @param[in] center  The center  position
 * @param[in] radius  The radius of a circle (meters)
 * @param[out] bounds  A location bounds handle to be newly created on success
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see location_bounds_get_circle_coords()
 * @see location_bounds_destroy()
 */
int location_bounds_create_circle(location_coords_s center, double radius, location_bounds_h* bounds);

/**
 * @brief Creates a polygon type of new location bounds.
 * @remarks @a bounds must be released location_bounds_destroy() by you.
 * @remarks @a length sholud be over than 3 to represent polygon.
 * @param[in] coords_list  The list of coordinates
 * @param[in] length  The length of the coordinates list
 * @param[out] bounds  A location bounds handle to be newly created on success
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see location_bounds_foreach_polygon_coords()
 * @see location_bounds_destroy()
 */
int location_bounds_create_polygon(location_coords_s* coords_list, int length, location_bounds_h* bounds);

/**
 * @brief Check if the bounds contains the specified coordinates.
 * @param[in]   bounds  The location bounds handle
 * @param[in]   coords   The coordinates
 * @param[out]   contained  The result indicating whether the boundary contains the specified coordinate (@c true = contained, @c false = not contained )
 * @return @c true if the bouns contains the specified coordinates. \n else @c false
 * @see location_bounds_create_rect()
 * @see location_bounds_create_circle()
 * @see location_bounds_create_polygon()
 */
bool location_bounds_contains_coordinates(location_bounds_h bounds, location_coords_s coords);

/**
 * @brief Get the type of location bounds.
 * @param[in] bounds  The location bounds handle
 * @param[out] type The type of location bounds
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see location_bounds_create_rect()
 * @see location_bounds_create_circle()
 * @see location_bounds_create_polygon()
 */
int location_bounds_get_type(location_bounds_h bounds, location_bounds_type_e *type);

/**
 * @brief Get the center position and radius of circle bounds.
 * @param[in] bounds  The location bounds handle
 * @param[out] top_left  The top left position
 * @param[out] bottom_right  The bottom right position
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_INCORRECT_TYPE Incorrect bounds type
 * @see location_bounds_create_rect()
 */
int location_bounds_get_rect_coords(location_bounds_h bounds, location_coords_s  *top_left, location_coords_s  *bottom_right);

/**
 * @brief Get the center position and radius of circle bounds.
 * @param[in] bounds  The location bounds handle
 * @param[out] center The center position of circle
 * @param[radius] center The radius of circle
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_INCORRECT_TYPE Incorrect bounds type
 * @see location_bounds_create_circle()
 */
int location_bounds_get_circle_coords(location_bounds_h bounds, location_coords_s  *center, double *radius);

/**
 * @brief Get the coordinates of polygon.
 * @param[in] bounds  The location bounds handle
 * @param[in] callback	The geocoder get positions callback function
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_BOUNDS_ERROR_INCORRECT_TYPE Incorrect bounds type
 * @post It invokes polygon_coords_cb() to get coordinates of polygon.
 * @see polygon_coords_cb()
 * @see location_bounds_create_polygon()
 */
int location_bounds_foreach_polygon_coords(location_bounds_h bounds, polygon_coords_cb callback, void *user_data);

/**
 * @brief Releases the location bounds.
 * @param[in] bounds The location bounds handle
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see location_bounds_create_rect()
 * @see location_bounds_create_circle()
 * @see location_bounds_create_polygon()
*/
int location_bounds_destroy(location_bounds_h bounds);

/**
 * @brief Registers a callback function to be invoked when boundary area is entered or exited.
 * @param[in] bounds The location bounds handle
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return  0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
 * @post location_bounds_state_changed_cb() will be invoked
 * @see location_bounds_unset_state_changed_cb()
 * @see location_bounds_state_changed_cb()
 */
int location_bounds_set_state_changed_cb(location_bounds_h bounds, location_bounds_state_changed_cb callback, void* user_data);

/**
 * @brief	Unregisters the callback function.
 * @param[in] bounds The location bounds handle
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_BOUNDS_ERROR_NONE Successful
 * @retval #LOCATION_BOUNDS_ERROR_INVALID_PARAMETER	Invalid parameter
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

