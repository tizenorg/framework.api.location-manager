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

#include <location_preference.h>
#include <location-map-service.h>
#include <stdlib.h>
#include <string.h>
#include <locations_private.h>

#define LOCATION_PREFERENCE_NULL_ARG_CHECK(arg)  \
    LOCATIONS_CHECK_CONDITION((arg != NULL), LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER")

#define LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service) *((LocationMapObject**)service)

/*
* Internal Implementation
*/

static int __convert_error_code(int code)
{
	int ret = LOCATION_PREFERENCE_ERROR_NONE;
	char* msg = "LOCATION_PREFERENCE_ERROR_NONE";
	switch(code)
	{
		case LOCATION_ERROR_NONE:
			ret = LOCATION_PREFERENCE_ERROR_NONE;
			msg = "LOCATION_PREFERENCE_ERROR_NONE";
			break;
		case LOCATION_ERROR_NETWORK_NOT_CONNECTED:
			ret = LOCATION_PREFERENCE_ERROR_NETWORK_FAILED;
			msg = "LOCATION_PREFERENCE_ERROR_NETWORK_FAILED";
			break;
		case LOCATION_ERROR_PARAMETER:
			ret = LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER;
			msg = "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER";
			break;
		case LOCATION_ERROR_NOT_AVAILABLE:
			msg = "LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND";
			ret = LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND;
	}
	LOCATIONS_LOGE("%s(0x%08x)", msg, ret);
	return ret;
}


/*
 * Public Implementation
 */
EXPORT_API int location_preference_foreach_available_property_keys(location_service_h service, location_preference_available_property_key_cb callback, void* user_data)
{
	LocationMapObject* object = NULL;
	GList* keys = NULL;
	char* key = NULL;
	int ret = 0;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(callback);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	ret = location_map_get_provider_capability_key(object, MAP_SERVICE_PREF_PROPERTY, &keys);
	if(ret != LOCATION_ERROR_NONE)
	{
		return __convert_error_code(ret);
	}
	else
	{
		while(keys) {
			key = keys->data;
			if(!callback(key, user_data))
				break;
			keys = keys->next;
		}

		return LOCATION_PREFERENCE_ERROR_NONE;
	}
}

EXPORT_API int location_preference_foreach_available_property_values(location_service_h service, const char* key, location_preference_available_property_value_cb callback, void* user_data)
{
	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(key);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(callback);
	return LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND;
}

EXPORT_API int location_preference_foreach_available_languages(location_service_h service, location_preference_available_language_cb callback, void* user_data)
{
	LocationMapObject* object = NULL;
	GList* keys = NULL;
	char* key = NULL;
	int ret = 0;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(callback);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	ret = location_map_get_provider_capability_key(object, MAP_SERVICE_PREF_LANGUAGE, &keys);
	if(ret != LOCATION_ERROR_NONE)
	{
		return __convert_error_code(ret);
	}
	else
	{
		while(keys) {
			key = keys->data;
			if(!callback(key, user_data))
				break;
			keys = keys->next;
		}

		return LOCATION_PREFERENCE_ERROR_NONE;
	}
}

EXPORT_API int location_preference_foreach_available_country_codes(location_service_h service, location_preference_available_country_code_cb callback, void* user_data)
{
	LocationMapObject* object = NULL;
	GList* keys = NULL;
	char* key = NULL;
	int ret = 0;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(callback);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	ret = location_map_get_provider_capability_key(object, MAP_SERVICE_PREF_COUNTRY, &keys);
	if(ret != LOCATION_ERROR_NONE)
	{
		return __convert_error_code(ret);
	}
	else
	{
		while(keys) {
			key = keys->data;
			if(!callback(key, user_data))
				break;
			keys = keys->next;
		}
	}

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_foreach_properties(location_service_h service, location_preference_property_cb callback, void* user_data)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	GList* keys = NULL;
	char* key = NULL;
	char* value = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(callback);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	keys = location_map_pref_get_property_key(pref);
	while(keys) {
		key = keys->data;
		value = (char*)location_map_pref_get_property(pref, key);
		if(!callback(key, value, user_data))
			break;
		keys = keys->next;
	}

	location_map_pref_free(pref);
	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_set(location_service_h service, const char* key, const char* value)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(key);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(value);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	location_map_pref_set_property(pref, (gconstpointer)key, (gconstpointer)value);
	location_map_set_service_pref(object, pref);
	location_map_pref_free(pref);

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get(location_service_h service, const char* key, char** value)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* ret = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(key);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(value);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = (char*)location_map_pref_get_property(pref, (gconstpointer)key);
	if(ret != NULL)
	{
		*value = strdup(ret);
		location_map_pref_free(pref);
	}
	else
	{
		*value = NULL;
		location_map_pref_free(pref);
		LOCATIONS_PRINT_ERROR_CODE(LOCATION_PREFERENCE_ERROR_INVALID_KEY, "LOCATION_PREFERENCE_ERROR_INVALID_KEY");
	}

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_set_provider(location_service_h service, char* provider)
{
	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(provider);

	LocationMapObject *object = NULL;
	LocationMapPref *pref = NULL;
	gboolean ret = FALSE;

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = location_map_pref_set_provider_name(pref, provider);
	if (!ret) {
		location_map_pref_free(pref);
		LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_KEY, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");
	}
	ret = location_map_set_service_pref(object, pref);
	if (!ret) {
		location_map_pref_free(pref);
		LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_KEY, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");
	}

	location_map_pref_free(pref);
	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get_provider(location_service_h service, char** provider)
{
	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(provider);

	LocationMapObject *object = NULL;
	LocationMapPref *pref = NULL;
	gchar* current_provider = NULL;

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	current_provider = location_map_pref_get_provider_name(pref);
	if (!current_provider) {
		location_map_pref_free(pref);
		LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_KEY, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");
	}

	*provider = g_strdup (current_provider);
	location_map_pref_free(pref);

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get_default_provider(location_service_h service, char** provider)
{
	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(provider);

	LocationMapObject *object = NULL;
	gchar *current_provider = NULL;
	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	current_provider = location_map_get_default_provider(object);
	LOCATIONS_CHECK_CONDITION(current_provider != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	*provider = g_strdup(current_provider);
	g_free(current_provider);

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_foreach_supported_provider(location_service_h service, location_preference_supported_provider_cb callback , void *user_data)
{
	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(callback);

	LocationMapObject *object = NULL;
	GList *providers = NULL;
	gchar *provider = NULL;

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	providers = location_map_get_supported_providers(object);
	LOCATIONS_CHECK_CONDITION(providers != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");
	while(providers) {
		provider = providers->data;
		if(!callback(provider, user_data))
			break;
		providers = providers->next;
	}

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get_provider_name(location_service_h service, char** provider)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* ret = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(provider);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = location_map_pref_get_provider_name(pref);
	if(ret != NULL)
		*provider = strdup(ret);
	else
		*provider = NULL;

	location_map_pref_free(pref);
	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get_distance_unit(location_service_h service, location_preference_distance_unit_e* unit)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* ret = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(unit);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = location_map_pref_get_distance_unit(pref);
	if(ret != NULL)
	{
		switch(ret[0]) {
			case 'F' :
				*unit = LOCATION_PREFERENCE_DISTANCE_UNIT_FT;
				break;
			case 'K' :
				*unit = LOCATION_PREFERENCE_DISTANCE_UNIT_KM;
				break;
			case 'Y' :
				*unit = LOCATION_PREFERENCE_DISTANCE_UNIT_YD;
				break;
			case 'M' :
				if(ret[1] == 'I')
					*unit = LOCATION_PREFERENCE_DISTANCE_UNIT_MI;
				else
					*unit = LOCATION_PREFERENCE_DISTANCE_UNIT_M;
				break;
		}
		location_map_pref_free(pref);
		return LOCATION_PREFERENCE_ERROR_NONE;
	}
	else
	{
		location_map_pref_free(pref);
		LOCATIONS_PRINT_ERROR_CODE(LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND, "LOCATION_PREFERENCE_ERROR_RESULT_NOT_FOUND");
	}
}

EXPORT_API int location_preference_set_distance_unit(location_service_h service, location_preference_distance_unit_e unit)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* distance = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	switch(unit) {
		case LOCATION_PREFERENCE_DISTANCE_UNIT_FT :
			distance = "FT";
			break;
		case LOCATION_PREFERENCE_DISTANCE_UNIT_KM :
			distance = "KM";
			break;
		case LOCATION_PREFERENCE_DISTANCE_UNIT_YD :
			distance = "YD";
			break;
		case LOCATION_PREFERENCE_DISTANCE_UNIT_MI :
			distance = "MI";
			break;
		case LOCATION_PREFERENCE_DISTANCE_UNIT_M :
			distance = "M";
			break;
		default :
			LOCATIONS_PRINT_ERROR_CODE(LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");
	}

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	location_map_pref_set_distance_unit(pref, distance);
	location_map_set_service_pref(object, pref);
	location_map_pref_free(pref);

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get_language(location_service_h service, char** language)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* ret = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(language);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = location_map_pref_get_language(pref);
	if(ret != NULL)
		*language = strdup(ret);
	else
		*language = NULL;

	location_map_pref_free(pref);
	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_set_language(location_service_h service, const char* language)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(language);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	location_map_pref_set_language(pref, language);
	location_map_set_service_pref(object, pref);
	location_map_pref_free(pref);

	return LOCATION_PREFERENCE_ERROR_NONE;
}


EXPORT_API int location_preference_get_country_code(location_service_h service, char** country_code)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* ret = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(country_code);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = location_map_pref_get_country(pref);
	if(ret != NULL)
		*country_code = strdup(ret);
	else
		*country_code = NULL;

	location_map_pref_free(pref);
	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_set_country_code(location_service_h service, const char* country_code)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(country_code);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	location_map_pref_set_country(pref, country_code);
	location_map_set_service_pref(object, pref);
	location_map_pref_free(pref);

	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_set_maps_key (location_service_h service, const char* maps_key)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(maps_key);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	location_map_pref_set_maps_key(pref, maps_key);
	location_map_set_service_pref(object, pref);
	location_map_pref_free(pref);
	
	return LOCATION_PREFERENCE_ERROR_NONE;
}

EXPORT_API int location_preference_get_maps_key (location_service_h service, char** maps_key)
{
	LocationMapPref* pref = NULL;
	LocationMapObject* object = NULL;
	char* ret = NULL;

	LOCATION_PREFERENCE_NULL_ARG_CHECK(service);
	LOCATION_PREFERENCE_NULL_ARG_CHECK(maps_key);

	object = LOCATION_PREFERENCE_GET_LOCATION_OBJECT(service);
	pref = location_map_get_service_pref(object);
	LOCATIONS_CHECK_CONDITION(pref != NULL, LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER, "LOCATION_PREFERENCE_ERROR_INVALID_PARAMETER");

	ret = location_map_pref_get_maps_key(pref);
	if(ret != NULL)
		*maps_key = strdup(ret);
	else
		*maps_key = NULL;

	location_map_pref_free(pref);
	return LOCATION_PREFERENCE_ERROR_NONE;

}