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
#include <glib.h>
#include <locations.h>
#include <location_bounds.h>
#include <location_batch.h>

static location_manager_h manager;
static GMainLoop *g_mainloop = NULL;
static int menu;

static int repeat_count;
static void location_cleanup();
static int location_test();
static guint test_timer;

static gboolean exit_program(gpointer data)
{
	g_main_loop_quit(g_mainloop);

	return FALSE;
}

static gboolean wait_test()
{
	if (test_timer) {
		g_source_remove(test_timer);
		test_timer = 0;
	}

	location_cleanup();
	location_test();

	return FALSE;
}

#if 0
void zone_event_cb(location_boundary_state_e state, double latitude, double longitude, double altitude, time_t timestamp,
                   void *user_data)
{
	if (state == LOCATIONS_BOUNDARY_IN) {
		fprintf(stderr, "Entering zone\n");
	} else {		/* state == LOCATIONS_BOUNDARY_OUT */
		fprintf(stderr, "Leaving zone\n");
	}

	fprintf(stderr, "Latitude: %lf, longitude: %lf, altitude: %lf\n", latitude, longitude, altitude);

	fprintf(stderr, "Time: %s\n", ctime(&timestamp));
}

static bool last_satellites_foreach_cb(unsigned int azimuth, unsigned int elevation, unsigned int prn, int snr, bool is_in_use,
                                       void *user_data)
{
	fprintf(stderr, "[Last Satellite information]	azimuth	: %d, elevation: %d, prn: %d, snr: %d, used: %d\n", azimuth, elevation,
	       prn, snr, is_in_use);
	return true;
}

static bool __poly_coords_cb(location_coords_s coords, void *user_data)
{
	fprintf(stderr, "location_bounds_foreach_rect_coords(latitude: %lf, longitude: %lf) \n", coords.latitude, coords.longitude);
	return TRUE;
}

static bool __location_bounds_cb(location_bounds_h bounds, void *user_data)
{
	if (bounds == NULL)
		fprintf(stderr, "bounds ==NULL\n");
	else {
		location_bounds_type_e type;
		location_bounds_get_type(bounds, &type);
		if (type == LOCATION_BOUNDS_CIRCLE) {
			location_coords_s center;
			double radius;
			location_bounds_get_circle_coords(bounds, &center, &radius);
			fprintf(stderr, "location_bounds_get_circle_coords(center: %lf, %lf, radius: %lf) \n", center.latitude,
			       center.longitude, radius);

		} else if (type == LOCATION_BOUNDS_RECT) {
			location_coords_s left_top;
			location_coords_s right_bottom;
			location_bounds_get_rect_coords(bounds, &left_top, &right_bottom);
			fprintf(stderr, "location_bounds_get_rect_coords(left_top: %lf, %lf - right_bottom: %lf, %lf) \n",
			       left_top.latitude, left_top.longitude, right_bottom.latitude, right_bottom.longitude);
		} else if (type == LOCATION_BOUNDS_POLYGON) {
			location_bounds_foreach_polygon_coords(bounds, __poly_coords_cb, NULL);
		}
	}
	return TRUE;
}

void location_bounds_test()
{
	int ret;
	location_coords_s center;
	center.latitude = 37.258;
	center.longitude = 127.056;
	double radius = 30;
	location_bounds_h bounds_circle;
	ret = location_bounds_create_circle(center, radius, &bounds_circle);
	if (ret != LOCATION_BOUNDS_ERROR_NONE) {
		fprintf(stderr, "location_bounds_create_circle() failed\n");
	} else
		fprintf(stderr, "Bounds(circle) has been created successfully.\n");

	ret = location_manager_add_boundary(manager, bounds_circle);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "Setting boundary failed\n");
	} else
		fprintf(stderr, "Boundary set\n");

	location_coords_s center2;
	double radius2;
	ret = location_bounds_get_circle_coords(bounds_circle, &center2, &radius2);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "location_bounds_get_circle_coords() failed\n");
	} else
		fprintf(stderr, "location_bounds_get_circle_coords(center: %lf, %lf, radius: %lf) \n", center2.latitude,
		       center2.longitude, radius2);

	/*Add the rect bounds */
	location_coords_s left_top;
	left_top.latitude = 30;
	left_top.longitude = 30;

	location_coords_s right_bottom;
	right_bottom.latitude = 10;
	right_bottom.longitude = 50;

	location_bounds_h bounds_rect;
	ret = location_bounds_create_rect(left_top, right_bottom, &bounds_rect);
	if (ret != LOCATION_BOUNDS_ERROR_NONE) {
		fprintf(stderr, "location_bounds_create_rect() failed\n");
	} else
		fprintf(stderr, "Bounds(rect) has been created successfully.\n");

	ret = location_manager_add_boundary(manager, bounds_rect);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "Setting boundary failed\n");
	} else
		fprintf(stderr, "Boundary set\n");

	location_coords_s left_top2;
	location_coords_s right_bottom2;

	ret = location_bounds_get_rect_coords(bounds_rect, &left_top2, &right_bottom2);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "location_bounds_get_rect_coords() failed\n");
	} else
		fprintf(stderr, "location_bounds_get_rect_coords(left_top: %lf, %lf - right_bottom: %lf, %lf) \n", left_top2.latitude,
		       left_top2.longitude, right_bottom2.latitude, right_bottom2.longitude);

	/*Add the polygon bounds */

	int poly_size = 3;
	location_coords_s coord_list[poly_size];

	coord_list[0].latitude = 10;
	coord_list[0].longitude = 10;
	coord_list[1].latitude = 20;
	coord_list[1].longitude = 20;
	coord_list[2].latitude = 30;
	coord_list[2].longitude = 30;

	location_bounds_h bounds_poly;
	ret = location_bounds_create_polygon(coord_list, poly_size, &bounds_poly);
	if (ret != LOCATION_BOUNDS_ERROR_NONE) {
		fprintf(stderr, "location_bounds_create_polygon() failed\n");
	} else
		fprintf(stderr, "Bounds(polygon) has been created successfully.\n");

	ret = location_manager_add_boundary(manager, bounds_poly);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "Setting boundary failed\n");
	} else
		fprintf(stderr, "Boundary set\n");

	ret = location_bounds_foreach_polygon_coords(bounds_poly, __poly_coords_cb, NULL);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "location_bounds_get_rect_coords() failed\n");
	}

	location_coords_s test_coords;
	test_coords.latitude = 12;
	test_coords.longitude = 12;

	if (location_bounds_contains_coordinates(bounds_poly, test_coords))
		fprintf(stderr, "location_bounds_contains_coordinates() retrun TRUE \n");
	else
		fprintf(stderr, "location_bounds_contains_coordinates() retrun FALSE \n");

	/*print current bounds */
	ret = location_manager_foreach_boundary(manager, __location_bounds_cb, (void *)manager);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "location_manager_foreach_boundary() failed\n");
	}

}

void location_get_last_information_test()
{
	int ret;
	double altitude, latitude, longitude;
	double climb, direction, speed;
	double horizontal, vertical;
	location_accuracy_level_e level;
	time_t timestamp;
	int num_of_inview, num_of_active;

	ret = location_manager_get_last_position(manager, &altitude, &latitude, &longitude, &timestamp);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, " Fail: location_manager_get_last_position ---> %d \n", ret);
	} else {
		fprintf(stderr, "[%ld] alt: %g, lat: %g, long: %g\n", timestamp, altitude, latitude, longitude);
	}

	ret = location_manager_get_last_velocity(manager, &climb, &direction, &speed, &timestamp);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, " Fail: location_manager_get_last_velocity ---> %d \n", ret);
	} else {
		fprintf(stderr, "climb: %f, direction: %f, speed: %f\n", climb, direction, speed);
	}

	ret = location_manager_get_last_accuracy(manager, &level, &horizontal, &vertical);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, " Fail: location_manager_get_last_accuracy ---> %d \n", ret);
	} else {
		fprintf(stderr, "Level: %d, horizontal: %g, vertical: %g\n", level, horizontal, vertical);
	}

	ret = gps_status_get_last_satellite(manager, &num_of_active, &num_of_inview, &timestamp);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, " Fail: gps_status_get_last_satellite_count_in_view ---> %d \n", ret);
	} else {
		fprintf(stderr, "[%ld] Satellite number of active: %d, in view: %d\n", timestamp, num_of_active, num_of_inview);
	}

	ret = gps_status_foreach_last_satellites_in_view(manager, last_satellites_foreach_cb, NULL);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, " Fail: gps_status_foreach_last_satellites_in_view ---> %d \n", ret);
	}
}
#endif

static void _state_change_cb(location_service_state_e state, void *user_data)
{
	fprintf(stderr, "--------------------------state change: %d---------\n", state);
	location_manager_h lm = (location_manager_h) user_data;
	if (state == LOCATIONS_SERVICE_ENABLED) {
		int ret;
		double altitude;
		double latitude;
		double longitude;
		time_t timestamp;
		ret = location_manager_get_position(lm, &altitude, &latitude, &longitude, &timestamp);
		if (ret != LOCATIONS_ERROR_NONE) {
			fprintf(stderr, " Fail: location_manager_get_position ---> %d \n", ret);
		} else {
			fprintf(stderr, "[%ld] alt: %g, lat %g, long %g\n", timestamp, altitude, latitude, longitude);
		}

		location_accuracy_level_e level;
		double horizontal;
		double vertical;
		ret = location_manager_get_accuracy(lm, &level, &horizontal, &vertical);
		if (ret != LOCATIONS_ERROR_NONE) {
			fprintf(stderr, " Fail: location_manager_get_accuracy ---> %d \n", ret);
		} else {
			fprintf(stderr, "Level: %d, horizontal: %g, vertical %g\n", level, horizontal, vertical);
		}
#if 0
		char *nmea;
		ret = gps_status_get_nmea(lm, &nmea);
		if (ret != LOCATIONS_ERROR_NONE) {
			fprintf(stderr, " Fail: gps_status_get_nmea ---> %d \n", ret);
		} else {
			fprintf(stderr, "NMEA: %s\n", nmea);
			free(nmea);
		}
#endif
	}

}

void _position_updated_cb(double latitude, double longitude, double altitude, time_t timestamp, void *user_data)
{
	fprintf(stderr, "-------------------------- position updated --------------------------\n");
	fprintf(stderr, "[%ld] lat[%f] lon[%f] alt[%f]\n", timestamp, latitude, longitude, altitude);

	location_manager_h lm = (location_manager_h) user_data;
	location_accuracy_level_e level;
	double horizontal;
	double vertical;
	int ret = location_manager_get_accuracy(lm, &level, &horizontal, &vertical);
	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, " Fail: location_manager_get_accuracy ---> %d \n", ret);
	} else {
		fprintf(stderr, "Level: %d, horizontal: %g, vertical %g\n", level, horizontal, vertical);
	}

	repeat_count++;

	if (repeat_count > 2) {
		test_timer = g_timeout_add_seconds(1, wait_test, NULL);
	}
}

void _velocity_updated_cb(double speed, double direction, double climb, time_t timestamp, void *user_data)
{
	fprintf(stderr, "-------------------------- velocity updated --------------------------\n");
	fprintf(stderr, "[%ld] speed[%f] direction[%f] climb[%f]\n", timestamp, speed, direction, climb);
}

void _location_cb(int error, double latitude, double longitude, double altitude, time_t timestamp, double speed, double climb, double direction, void *user_data)
{
	fprintf(stderr, "error[%d]\n", error);
	fprintf(stderr, "location_cb: lat[%f] lon[%f] alt[%f]\n", latitude, longitude, altitude);
	fprintf(stderr, "speed[%f] climb[%f] direction[%f]\n", speed, climb, direction);

	test_timer = g_timeout_add_seconds(1, wait_test, NULL);
}

void _location_changed_cb(double latitude, double longitude, double altitude, double speed, double direction, double horizontal_accuracy, time_t timestamp, void *user_data)
{
	fprintf(stderr, "-------------------------- location changed --------------------------\n");
	fprintf(stderr, "[%ld] lat[%f] lon[%f] alt[%f] speed[%lf] direction[%lf], horizontal_accuracy[%lf]\n", timestamp, latitude, longitude, altitude, speed, direction, horizontal_accuracy);

	repeat_count++;

	if (repeat_count > 2) {
		test_timer = g_timeout_add_seconds(1, wait_test, NULL);
	}
}

bool _get_location_cb(double latitude, double longitude, double altitude, double speed, double direction, double horizontal, double vertical, time_t timestamp, void *user_data)
{
	fprintf(stderr, "-------------------------- batch: get location --------------------------\n");
	fprintf(stderr, "[%ld] lat[%f] lon[%f] alt[%f] speed[%lf] direction[%lf], horizontal_accuracy[%lf]\n", timestamp, latitude, longitude, altitude, speed, direction, horizontal);
	return TRUE;
}

void _location_batch_cb(int num_of_location, void *user_data)
{
	fprintf(stderr, "-------------------------- location batch --------------------------\n");
	fprintf(stderr, "num_of_location: [%d]\n", num_of_location);

	location_manager_h manager = user_data;

	if (num_of_location > 0) {
		location_manager_foreach_location_batch(manager, _get_location_cb, user_data);
	}
	repeat_count++;

	if (repeat_count > 1) {
		test_timer = g_timeout_add_seconds(1, wait_test, NULL);
	}
}

static void __setting_cb(location_method_e method, bool enable, void *user_data)
{
	fprintf(stderr, "method[%d], enable[%d]\n", method, enable);
}

static void print_location_status()
{
	fprintf(stderr, "==== LOCATION Setting state =====\n");
	bool is_enabled = FALSE;
	location_manager_is_enabled_method(LOCATIONS_METHOD_HYBRID, &is_enabled);
	fprintf(stderr, "hybrid: %d, ", is_enabled);

	location_manager_is_enabled_method(LOCATIONS_METHOD_GPS, &is_enabled);
	fprintf(stderr, "gps: %d, ", is_enabled);

	location_manager_is_enabled_method(LOCATIONS_METHOD_WPS, &is_enabled);
	fprintf(stderr, "wps: %d\n", is_enabled);
}

static int enable_method(location_method_e method, bool enable)
{
	int ret = 0;
	fprintf(stderr, "==== LOCATION Setting changed =====\n");

	location_manager_set_setting_changed_cb(LOCATIONS_METHOD_HYBRID, __setting_cb, NULL);

	fprintf(stderr, "method[%d], enable[%d]\n", method, enable);
	ret = location_manager_enable_method(method, enable);

	location_manager_unset_setting_changed_cb(LOCATIONS_METHOD_HYBRID);
	return ret;
}

static void print_menu()
{
	fprintf(stderr, "============= LOCATION TEST =============\n");
	fprintf(stderr, "[1] Get location: LOCATIONS_METHOD_HYBRID\n");
	fprintf(stderr, "[2] Get location: LOCATIONS_METHOD_GPS\n");
	fprintf(stderr, "[3] Get location: LOCATIONS_METHOD_WPS\n");
	fprintf(stderr, "[4] Single location: LOCATIONS_METHOD_HYBRID\n");
	fprintf(stderr, "[5] Single location: LOCATIONS_METHOD_GPS\n");
	fprintf(stderr, "[6] Single location: LOCATIONS_METHOD_WPS\n\n");
	fprintf(stderr, "[11] Change update interval: LOCATIONS_METHOD_HYBRID\n");
	fprintf(stderr, "[12] Change update interval: LOCATIONS_METHOD_GPS\n\n");
	fprintf(stderr, "[21] Distance based location update: LOCATIONS_METHOD_HYBRID\n");
	fprintf(stderr, "[22] Distance based location update: LOCATIONS_METHOD_GPS\n");
	fprintf(stderr, "[23] Distance based location update: LOCATIONS_METHOD_WPS\n\n");
	fprintf(stderr, "[31] Location batch update: LOCATIONS_METHOD_GPS\n\n");
	fprintf(stderr, "[41] Turn on/off method: LOCATIONS_METHOD_HYBRID\n");
	fprintf(stderr, "[42] Turn on/off method: LOCATIONS_METHOD_GPS\n");
	fprintf(stderr, "[43] Turn on/off method: LOCATIONS_METHOD_WPS\n\n");
	fprintf(stderr, "[51] Boundary Test\n\n");
	fprintf(stderr, "[0] Exit!!!\n\n");
	fprintf(stderr, "Select menu: ");

	if (scanf("%d", &menu) < 0) {
		fprintf(stderr, "Can't read menu !!!\n");
	}
}

static int location_test()
{
	int ret = LOCATIONS_ERROR_NONE;
	int basic = 0;
	int interval = 1;
	repeat_count = 0;

	print_location_status();
	print_menu();

	switch (menu) {
		case 1:
		case 2:
		case 3: {
			basic = 1;

			int method = menu - 1;
			ret = location_manager_create(method, &manager);
			fprintf(stderr, "location_manager_create (method: %d): %d\n", method, ret);
			ret = location_manager_start(manager);
			fprintf(stderr, "start: %d\n", ret);
			break;
			}
		case 4:
		case 5:
		case 6: {
			int timeout = 30;

			fprintf(stderr, "\n	Input timeout ==> ");
			ret = scanf("%d", &timeout);

			int method = menu - 4;
			ret = location_manager_create(method, &manager);
			ret = location_manager_request_single_location(manager, timeout, _location_cb, manager);
			fprintf(stderr, "request single_location (method: %d): %d\n", method, ret);
			break;
			}
		case 11:
		case 12:
		case 13: {
			int interval = 1;

			fprintf(stderr, "\n	Input position interval ==> ");
			ret = scanf("%d", &interval);

			int method = menu - 11;
			ret = location_manager_create(method, &manager);
			fprintf(stderr, "location_manager_create (method: %d): %d\n", method, ret);

			ret = location_manager_set_position_updated_cb(manager, _position_updated_cb, interval, (void *)manager);
			fprintf(stderr, "set_position_updated_cb: %d\n", ret);

			/*
			ret = location_manager_set_velocity_updated_cb(manager, _velocity_updated_cb, interval*2, (void *)manager);
			fprintf(stderr, "set_velocity_updated_cb: %d\n", ret);
			*/

			ret = location_manager_set_location_changed_cb(manager, _location_changed_cb, interval * 2, (void *)manager);
			fprintf(stderr, "set_location_changed_cb: %d\n", ret);

			ret = location_manager_start(manager);
			fprintf(stderr, "start: %d\n", ret);
			break;
			}
		case 21:
		case 22:
		case 23: {
			int interval = 1;
			int method = menu - 21;

			fprintf(stderr, "\n	Input position interval ==> ");
			ret = scanf("%d", &interval);

			ret = location_manager_create(method, &manager);
			fprintf(stderr, "location_manager_create (method : %d)", method);

			/*ret = location_manager_set_position_updated_cb(manager, _position_updated_cb, interval, (void *)manager); */
			/*fprintf(stderr, "set position changed callback: %d\n", ret); */

			ret = location_manager_set_distance_based_location_changed_cb(manager, _location_changed_cb, interval, 30, (void *)manager);
			fprintf(stderr, "set_distance_based_location_changed_cb: %d\n", ret);

			ret = location_manager_start(manager);
			fprintf(stderr, "start: %d\n", ret);
			break;
			}
		case 31: {
			int interval = 1;
			fprintf(stderr, "\n	Input batch interval ==> ");
			ret = scanf("%d", &interval);

			int period = 60;
			fprintf(stderr, "	Input batch period ==> ");
			ret = scanf("%d", &period);


			ret = location_manager_create(LOCATIONS_METHOD_GPS, &manager);
			fprintf(stderr, "location_manager_create (method : %d)\n", LOCATIONS_METHOD_GPS);

			ret = location_manager_set_location_batch_cb(manager, _location_batch_cb, interval, period, (void *)manager);
			fprintf(stderr, "set_location_batch_cb: %d\n", ret);

			ret = location_manager_start_batch(manager);
			fprintf(stderr, "start_batch: %d\n", ret);
			break;
			}
		case 41:
		case 42:
		case 43: {
			int method = menu - 41;
			int onoff = 1;

			fprintf(stderr, "\n	Input ON: 1 or OFF: 0 ==> ");
			ret = scanf("%d", &onoff);

			ret = enable_method(method, onoff);
			fprintf(stderr, "Enabling method: [%d], ret=%d\n", method, ret);
			break;
			}

		case 51: {
			location_bounds_h hPolyLocationBound = NULL;
			bool bIsContained = false;
			int nPolySize = 3;
			location_coords_s location_coord_list[nPolySize];
			location_coord_list[0].latitude = 10;
			location_coord_list[0].longitude = 10;
			location_coord_list[1].latitude = 20;
			location_coord_list[1].longitude = 20;
			location_coord_list[2].latitude = 30;
			location_coord_list[2].longitude = 10;

			//Checking coordinates in location boundary
			location_coords_s testLocationCoordinates;
			testLocationCoordinates.latitude = 20;
			testLocationCoordinates.longitude = 12;
			location_bound_error_e nRet = location_bounds_create_polygon(location_coord_list, nPolySize, &hPolyLocationBound);

			fprintf(stderr, "location_bounds_create_polygon: %d\n", nRet);

			bIsContained = location_bounds_contains_coordinates(hPolyLocationBound, testLocationCoordinates);// Target API
			fprintf(stderr, "bIsContained: %d\n", bIsContained);

			location_bounds_destroy(hPolyLocationBound);
			break;
			}
		case 0:
			g_timeout_add_seconds(1, exit_program, NULL);
			return 0;
		default:
			fprintf(stderr, "Exit!!! Input: %d\n", menu);
			g_timeout_add_seconds(1, exit_program, NULL);
			return 0;
	}

	if (ret != LOCATIONS_ERROR_NONE) {
		fprintf(stderr, "Test Failed!!! [%d]\n", ret);
		g_timeout_add_seconds(1, exit_program, NULL);
		return 0;
	}

	if (menu < 40) {
		ret = location_manager_set_service_state_changed_cb(manager, _state_change_cb, (void *)manager);
		fprintf(stderr, "set_service_state_changed_cb: %d\n", ret);

		if (basic) {
			ret = location_manager_set_position_updated_cb(manager, _position_updated_cb, interval, (void *)manager);
			fprintf(stderr, "set_position_updated_cb: %d\n", ret);
		}
	}
	else {
		test_timer = g_timeout_add_seconds(1, wait_test, NULL);
	}

	return 0;
}

static void location_cleanup()
{
	int ret = 0;
	if (manager != NULL) {
		ret = location_manager_stop(manager);
		fprintf(stderr, "stop: %d\n", ret);

		ret = location_manager_unset_service_state_changed_cb(manager);
		fprintf(stderr, "unset_service_state_changed_cb: %d\n", ret);

		ret = location_manager_unset_position_updated_cb(manager);
		fprintf(stderr, "unset_position_updated_cb: %d\n", ret);

		ret = location_manager_destroy(manager);
		fprintf(stderr, "destroy: %d\n", ret);
		manager = NULL;
	}
}

int main(int argc, char **argv)
{
	g_mainloop = g_main_loop_new(NULL, 0);
	location_test();
	g_main_loop_run(g_mainloop);
	location_cleanup();
	return 0;
}
