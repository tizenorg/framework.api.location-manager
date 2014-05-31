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

#ifndef __TIZEN_LOCATION_LOCATION_PREFERENCE_H__
#define __TIZEN_LOCATION_LOCATION_PREFERENCE_H__

#include <tizen_type.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup CAPI_LOCATION_PREF_MODULE
 * @{
 */

/**
 * @brief The handle of location service
 */
typedef void* location_service_h;


/**
 * @brief Gets a handle of location service from x
 */
#define GET_LOCATION_SERVICE(x) (location_service_h)(x)


/**
 * @brief Enumerations of error code for Location Preference
 */
typedef enum
{
    LOCATION_PREFERENCE_ERROR_NONE = TIZEN_ERROR_NONE,    /**< Successful */
    LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,    /**< Invalid parameter */
    LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND = TIZEN_ERROR_LOCATION_CLASS|0x0101,    /**<  Result not found */
    LOCATION_PREFERENCE_ERROR_NETWORK_FAILED = TIZEN_ERROR_LOCATION_CLASS|0x0102,    /**< Network unavailable */
    LOCATION_PREFERENCE_ERROR_INVALID_KEY = TIZEN_ERROR_LOCATION_CLASS | 0x0103,   /**< Invalid key */
} location_preference_error_e;

/**
 * @brief Enumerations of distance unit
 */
typedef enum
{
    LOCATION_PREFERENCE_DISTANCE_UNIT_M = 0,  /**< Meter */
    LOCATION_PREFERENCE_DISTANCE_UNIT_KM = 1,  /**< Kilometer */
    LOCATION_PREFERENCE_DISTANCE_UNIT_FT = 2,  /**< Feet */
    LOCATION_PREFERENCE_DISTANCE_UNIT_YD = 3,  /**< Mile */
    LOCATION_PREFERENCE_DISTANCE_UNIT_MI = 4,  /**< Mile */
} location_preference_distance_unit_e;

/**
 * @brief Called repeatedly when you get the available property keys.
 * @param[in] key The property key of location preference
 * @param[in] user_data The user data passed from foreach function
 * @pre location_preference_foreach_available_property_keys() will invoke this callback.
 * @return  @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @see	location_preference_foreach_available_property_keys()
 */
typedef bool (*location_preference_available_property_key_cb)(const char* key, void* user_data);

/**
 * @brief Called repeatedly when you get the available property values.
 * @param[in] value The property value of location preference
 * @param[in] user_data The user data passed from foreach function
 * @pre location_preference_foreach_available_property_values() will invoke this callback.
 * @return  @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @see	location_preference_foreach_available_property_values()
 */
typedef bool (*location_preference_available_property_value_cb)(const char* value, void* user_data);

/**
 * @brief Called repeatedly when you get the available languages.
 * @param[in] language The language of location preference
 * @param[in] user_data The user data passed from foreach function
 * @pre location_preference_foreach_available_languages() will invoke this callback.
 * @return  @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @see	location_preference_foreach_available_languages()
 */
typedef bool (*location_preference_available_language_cb)(const char* language, void* user_data);

/**
 * @brief Called repeatedly when you get the available country code.
 * @param[in] country_code The country code of location preference
 * @param[in] user_data The user data passed from foreach function
 * @pre location_preference_foreach_available_country_code() will invoke this callback.
 * @return  @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @see	location_preference_foreach_available_country_code()
 */
typedef bool (*location_preference_available_country_code_cb)(const char* country_code, void* user_data);

/**
 * @brief Called repeatedly when you get the properties that was set in location preference.
 * @param[in] key The property key of location preference
 * @param[in] value The property value of location preference
 * @param[in] user_data The user data passed from foreach function
 * @pre location_preference_foreach_properties() will invoke this callback.
 * @return  @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @see	location_preference_foreach_properties()
 */
typedef bool (*location_preference_property_cb)(const char* key, const char* value, void* user_data);

/**
 * @brief Called repeatedly to get each supported providers.
 * @param[in] provider The supported provider name
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre location_preference_foreach_supported_provider() will invoke this callback.
 * @see location_preference_foreach_supported_provider()
 */
typedef bool (*location_preference_supported_provider_cb)(const char* provider, void *user_data);

/**
 * @brief Retrieves the available property keys of location preference.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_PREFERENCE_ERROR_NETWORK_FAILED Network unavailable
 * @retval #LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND Result not found
 * @post location_preference_available_property_key_cb() will be invoked.
 * @see	GET_LOCATION_SERVICE()
 * @see location_preference_available_property_key_cb()
 * @see location_preference_foreach_available_property_values()
 */
int location_preference_foreach_available_property_keys(location_service_h service, location_preference_available_property_key_cb callback, void* user_data);

/**
 * @brief Retrieves the available property values of location preference.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] key The property of location preference
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_KEY Invalid key
 * @retval #LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND Result not found
 * @post location_preference_available_property_value_cb() will be invoked.
 * @see	GET_LOCATION_SERVICE()
 * @see location_preference_available_property_value_cb()
 * @see location_preference_foreach_available_property_keys()
 */
int location_preference_foreach_available_property_values(location_service_h service, const char* key, location_preference_available_property_value_cb callback, void* user_data);

/**
 * @brief Retrieves the available languages of location preference.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_PREFERENCE_ERROR_NETWORK_FAILED Network unavailable
 * @retval #LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND Result not found
 * @post location_preference_available_language_cb() will be invoked.
 * @see	GET_LOCATION_SERVICE()
 * @see location_preference_available_language_cb()
 */
int location_preference_foreach_available_languages(location_service_h service, location_preference_available_language_cb callback, void* user_data);

/**
 * @brief Retrieves the available country code of location preference.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_PREFERENCE_ERROR_NETWORK_FAILED Network unavailable
 * @retval #LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND Result not found
 * @post location_preference_available_country_code_cb() will be invoked.
 * @see	GET_LOCATION_SERVICE()
 * @see location_preference_available_country_code_cb()
 */
int location_preference_foreach_available_country_code(location_service_h service, location_preference_available_country_code_cb callback, void* user_data);

/**
 * @brief Gets the location preference that was set in location preference.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] key The property of location preference
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 */
int location_preference_foreach_properties(location_service_h service, location_preference_property_cb callback, void* user_data);

/**
 * @brief Sets the location preference value.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] property The property of location preference
 * @param[in] value The value of  location preference
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see location_preference_foreach_available_property_keys()
 * @see location_preference_foreach_available_property_values()
 * @see location_preference_get()
 */
int location_preference_set(location_service_h service, const char* key, const char* value);

/**
 * @brief Gets the location preference value.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] property The property of location preference
 * @param[out] value The value of  location preference
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_KEY Invalid key
 * @see	GET_LOCATION_SERVICE()
 * @see location_preference_foreach_available_property_keys()
 * @see location_preference_set()
 */
int location_preference_get(location_service_h service, const char* key, char** value);

__attribute__ ((deprecated)) int location_preference_get_provider_name(location_service_h service, char** provider);

/**
 * @brief Sets the provider of location service.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] provider The provider name of location service
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see GET_LOCATION_SERVICE()
 * @see location_preference_get_provider()
 */
int location_preference_set_provider(location_service_h service, char* provider);

/**
 * @brief Gets the provider of location service.
 * @remarks The @a provider must be released with free() by you.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[out] provider The provider name of location service
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see GET_LOCATION_SERVICE()
 * @see location_preference_set_provider()
 */
int location_preference_get_provider(location_service_h service, char** provider);

/**
 * @brief Gets the default provider of location service.
 * @remarks The @a provider must be released with free() by you.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[out] provider The default provider name of location service
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 */
int location_preference_get_default_provider(location_service_h service, char** provider);

/**
 * @brief Gets the provider of location service.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @post	This function invokes location_preference_supported_provider_cb() to get all supported providers.
 * @see GET_LOCATION_SERVICE()
 * @see location_preference_set_provider()
 */
int location_preference_foreach_supported_provider(location_service_h service, location_preference_supported_provider_cb callback , void *user_data);

/**
 * @brief Gets the distance unit of location service.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[out] unit The distance unit
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND Result not found
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_set_distance_unit()
 */
int location_preference_get_distance_unit(location_service_h service, location_preference_distance_unit_e* unit);

/**
 * @brief Sets the distance unit of location service.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] unit The distance unit
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_get_distance_unit()
 */
int location_preference_set_distance_unit(location_service_h service, location_preference_distance_unit_e unit);

/**
 * @brief Gets the language of location service.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[out] language The language
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_set_language()
 */
int location_preference_get_language(location_service_h service, char** language);

/**
 * @brief Sets the language of location service.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] language The language
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_get_language()
 */
int location_preference_set_language(location_service_h service, const char* language);

/**
 * @brief Sets the country code.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] country_code The country code
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_get_country_code()
 */
int location_preference_set_country_code(location_service_h service, const char* country_code);

/**
 * @brief Gets the country code.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[out] country_code The country code
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_set_country_code()
 */
int location_preference_get_country_code(location_service_h service, char** country_code);

/**
 * @brief Sets the account.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[in] maps_key The maps key
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_get_maps_key()
 */
int location_preference_set_maps_key(location_service_h service, const char* maps_key);

/**
 * @brief Gets the account.
 * @param[in] location_service The memory pointer of location service handle.
 *                 It must be converted into location_service_h by GET_LOCATION_SERVICE().
 * @param[out] maps_key The maps key
 * @return 0 on success, otherwise a negative error value.
 * @retval #LOCATION_PREFERENCE_ERROR_NONE Successful
 * @retval #LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER	Invalid parameter
 * @see	GET_LOCATION_SERVICE()
 * @see	location_preference_set_maps_key()
 */
int location_preference_get_maps_key(location_service_h service, char** maps_key);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_LOCATION_LOCATION_PREFERENCE_H__ */
