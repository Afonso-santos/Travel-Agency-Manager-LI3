#define _DEFAULT_SOURCE
#include "services/queries_service.h"

#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// In this file it is supposed pass to the query functions ONLY THE NEEDED data
// structs, not all of them

void query1(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {
  char *id = query_parameters[0];

  if (strncmp(id, "Book", 4) == 0) {
    ReservationInfo *reservation_info =
        get_reservation_by_reservation_id(reservations_data, id);

    if (reservation_info == NULL) return;

    int price_per_night = get_price_per_night(reservation_info);
    int city_tax = get_city_tax(reservation_info);

    char *hotel_id = get_hotel_id(reservation_info);
    char *hotel_name = get_hotel_name(reservation_info);
    int hotel_stars = get_hotel_stars(reservation_info);
    char *begin_date = get_begin_date(reservation_info);
    char *end_date = get_end_date(reservation_info);
    char *breakfast = get_includes_breakfast(reservation_info);
    char *includes_breakfast =
        (strcmp(breakfast, "false") == 0) ? "False" : "True";
    int number_of_nights = calculate_number_of_nights(begin_date, end_date);
    double total_price =
        calculate_total_price(number_of_nights, price_per_night, city_tax);

    write_query1_for_reservation(
        has_f, output_file, hotel_id, hotel_name, hotel_stars, begin_date,
        end_date, includes_breakfast, number_of_nights, total_price);
    free(begin_date);
    free(end_date);
    free(hotel_id);
    free(hotel_name);
    free(breakfast);
  }

  if (isdigit(id[0]) && isdigit(id[1])) {
    FlightInfo *flight_info = get_flight_by_flight_id(flights_data, id);

    if (flight_info == NULL) return;

    char *real_departure_date = get_real_departure_date(flight_info);

    char *airline_company = get_airline(flight_info);
    char *plane_model = get_plane_model(flight_info);
    char *origin = get_origin(flight_info);
    char *destination = get_destination(flight_info);
    char *schedule_departure_date = get_schedule_departure_date(flight_info);
    char *schedule_arrival_date = get_schedule_arrival_date(flight_info);
    int passengers = get_number_of_passengers_from_flight_stats(
        get_flight_stats_by_flight_id(flights_data, id));
    int delay = calculate_delay(schedule_departure_date, real_departure_date);

    write_query1_for_flight(has_f, output_file, airline_company, plane_model,
                            origin, destination, schedule_departure_date,
                            schedule_arrival_date, passengers, delay);
    free(schedule_departure_date);
    free(schedule_arrival_date);
    free(plane_model);
    free(origin);
    free(destination);
    free(airline_company);
    free(real_departure_date);
  }

  UserInfo *user_info = get_user_by_username(users_data, id);

  if (user_info == NULL) return;

  char *account_status = get_account_status(user_info);

  if (strcmp(account_status, "ACTIVE") == 0) {
    char *name = get_name(user_info);
    char *sex = get_sex(user_info);
    char *birth_date = get_birth_date(user_info);
    char *age = calculate_age(birth_date);
    char *country_code = get_country_code(user_info);
    char *passport = get_passport(user_info);
    int number_of_flights = get_number_of_flights_from_user_stats(
        get_user_stats_by_user_id(users_data, id));
    int number_of_reservations = get_number_of_reservations_from_user_stats(
        get_user_stats_by_user_id(users_data, id));
    double total_spent = get_total_spent_from_user_stats(
        get_user_stats_by_user_id(users_data, id));

    write_query1_for_user(has_f, output_file, name, sex, age, country_code,
                          passport, number_of_flights, number_of_reservations,
                          total_spent);
    free(name);
    free(sex);
    free(country_code);
    free(passport);
    free(birth_date);
  }
  free(account_status);
}

void query2(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file, int N) {
  char *id = query_parameters[0];
  char *type = "NULL";
  if (N == 2) {
    type = query_parameters[1];
  }

  UserInfo *user = get_user_by_username(users_data, id);
  char *account_status = get_account_status(user);

  if (account_status == NULL || strlen(account_status) <= 0 ||
      strcmp(account_status, "ACTIVE") != 0) {
    free(account_status);
    return;
  }
  free(account_status);

  UserStats *user_stats = get_user_stats_by_user_id(users_data, id);

  if (strcmp(type, "flights") == 0) {
    query2_flights(has_f, user_stats, flights_data, output_file);
  } else if (strcmp(type, "reservations") == 0) {
    query2_reservations(has_f, user_stats, reservations_data, output_file);
  } else {
    query2_both(has_f, user_stats, flights_data, reservations_data,
                output_file);
  }
}

void query2_flights(bool has_f, UserStats *user_stats,
                    FlightsData *flights_data, FILE *output_file) {
  GArray *flights = get_user_flights_from_user_stats(user_stats);
  if (flights == NULL) return;

  int len = flights->len;
  char **ids = malloc(sizeof(char *) * len);
  char **dates = malloc(sizeof(char *) * len);
  char **types = malloc(sizeof(char *) * len);

  query2_seed_flights(flights_data, NULL, flights, 0, ids, dates, types, len);
  sort_by_date(ids, dates, types, len);
  write_query2(has_f, output_file, ids, dates, types, len);
  free_query2(ids, dates, types, len);
}

void query2_reservations(bool has_f, UserStats *user_stats,
                         ReservationsData *reservations_data,
                         FILE *output_file) {
  GArray *reservations = get_user_reservations_from_user_stats(user_stats);
  if (reservations == NULL) return;

  int len = reservations->len;
  char **ids = malloc(sizeof(char *) * len);
  char **dates = malloc(sizeof(char *) * len);
  char **types = malloc(sizeof(char *) * len);

  query2_seed_reservations(reservations_data, NULL, reservations, 0, 0, ids,
                           dates, types, len);
  sort_by_date(ids, dates, types, len);
  write_query2(has_f, output_file, ids, dates, types, len);
  free_query2(ids, dates, types, len);
}

void query2_both(bool has_f, UserStats *user_stats, FlightsData *flights_data,
                 ReservationsData *reservations_data, FILE *output_file) {
  GArray *flights = get_user_flights_from_user_stats(user_stats);
  GArray *reservations = get_user_reservations_from_user_stats(user_stats);

  if (flights == NULL && reservations == NULL) return;

  int max = flights->len + reservations->len;
  char **ids = malloc(sizeof(char *) * max);
  char **dates = malloc(sizeof(char *) * max);
  char **types = malloc(sizeof(char *) * max);

  query2_seed_flights(flights_data, "flight", flights, 0, ids, dates, types,
                      flights->len);
  query2_seed_reservations(reservations_data, "reservation", reservations, 0,
                           flights->len, ids, dates, types, max);

  sort_by_date(ids, dates, types, max);
  write_query2(has_f, output_file, ids, dates, types, max);
  free_query2(ids, dates, types, max);
}

void query2_seed_flights(FlightsData *flights_data, char *type, GArray *array,
                         int i, char **ids, char **dates, char **types,
                         int len) {
  while (i < len) {
    char *id = g_array_index(array, char *, i);
    FlightInfo *flight = get_flight_by_flight_id(flights_data, id);
    if (flight == NULL) return;
    char *date = get_schedule_departure_date(flight);

    int year, month, day, hour, minute, second;
    sscanf(date, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute,
           &second);
    ids[i] = id;
    dates[i] = format_date(year, month, day);
    types[i] = type;
    free(date);
    i++;
  }
}

void query2_seed_reservations(ReservationsData *reservations_data, char *type,
                              GArray *array, int i, int sum, char **ids,
                              char **dates, char **types, int len) {
  while ((i + sum) < len) {
    char *id = g_array_index(array, char *, i);
    ReservationInfo *reservation =
        get_reservation_by_reservation_id(reservations_data, id);
    if (reservation == NULL) return;
    char *date = get_begin_date(reservation);

    int year, month, day, hour, minute, second;
    sscanf(date, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute,
           &second);
    ids[i + sum] = id;
    dates[i + sum] = format_date(year, month, day);
    types[i + sum] = type;
    free(date);
    i++;
  }
}

void free_query2(char **ids, char **dates, char **types, int len) {
  free(ids);
  for (int i = 0; i < len; i++) {
    free(dates[i]);
  }
  free(dates);
  free(types);
}

void query3(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {
  char *id = query_parameters[0];

  HotelStats *hotel_stats = get_hotel_stats_by_hotel_id(reservations_data, id);

  if (hotel_stats == NULL) return;

  int total_rating = get_total_rating_from_hotel_stats(
      get_hotel_stats_by_hotel_id(reservations_data, id));

  int number_of_clients = get_number_of_clients_from_hotel_stats(
      get_hotel_stats_by_hotel_id(reservations_data, id));

  double media_of_ratings = (double)total_rating / number_of_clients;

  write_query3(has_f, output_file, media_of_ratings);
}

struct query4_result_helper {
  char *reservation_id;
  char *begin_date;
  char *end_date;
  char *user_id;
  int rating;
  float total_price;
};

struct query4_result {
  GArray *query4_result;
};

void query4(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {
  char *id = query_parameters[0];
  HotelStats *hotel_stats = get_hotel_stats_by_hotel_id(reservations_data, id);
  GArray *reservations = get_hotel_reservations(hotel_stats);
  if (reservations == NULL) return;

  QUERY4_RESULT result = malloc(sizeof(struct query4_result));
  result->query4_result =
      g_array_new(FALSE, FALSE, sizeof(QUERY4_RESULT_HELPER));

  int i = 0, j = 0;
  for (i = 0; i < reservations->len; i++) {
    char *reservation_id = g_array_index(reservations, char *, i);
    if (reservation_id != NULL) {
      QUERY4_RESULT_HELPER result_helper =
          malloc(sizeof(struct query4_result_helper));

      ReservationInfo *reservation =
          get_reservation_by_reservation_id(reservations_data, reservation_id);
      char *begin_date = get_begin_date(reservation);
      char *end_date = get_end_date(reservation);
      int rating = get_rating(reservation);
      int price_per_night = get_price_per_night(reservation);
      float total_price = calculate_total_price(
          calculate_number_of_nights(begin_date, end_date), price_per_night,
          get_city_tax(reservation));
      char *user_id = get_user_id_reservation(reservation);

      result_helper->reservation_id = strdup(reservation_id);
      result_helper->begin_date = strdup(begin_date);
      result_helper->end_date = strdup(end_date);
      result_helper->user_id = strdup(user_id);
      result_helper->rating = rating;
      result_helper->total_price = total_price;
      g_array_append_val(result->query4_result, result_helper);
      j++;
      free(begin_date);
      free(end_date);
      free(user_id);
    }
  }
  int N = j--;
  sort_by_date_and_value(result, N);
  write_query4(has_f, output_file, result->query4_result);

  g_array_free(result->query4_result, TRUE);
}

void query5(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {}

void query6(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {}

void query7(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {}

void query8(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            FILE *output_file) {
  char *id = query_parameters[0];
  char *begin_date = query_parameters[1];
  char *end_date = query_parameters[2];
  HotelStats *hotel_stats = get_hotel_stats_by_hotel_id(reservations_data, id);
  GArray *reservations = get_hotel_reservations(hotel_stats);
  if (reservations == NULL) return;

  double revenue = 0, total_price;
  for (int i = 0; i < reservations->len; i++) {
    char *reservation_id = g_array_index(reservations, char *, i);
    ReservationInfo *reservation =
        get_reservation_by_reservation_id(reservations_data, reservation_id);
    if (strncmp(reservation_id, "Book", 4) != 0) break;
    char *begin_date_reservation = get_begin_date(reservation);
    char *end_date_reservation = get_end_date(reservation);
    int price_per_night = get_price_per_night(reservation);
    if (strcmp(begin_date, end_date) == 0 &&
        (strcmp(begin_date, begin_date_reservation) == 0 ||
         strcmp(end_date, end_date_reservation) == 0)) {
      revenue += 1 * price_per_night;
    } else if (calculate_number_of_nights(begin_date, end_date) == 1 &&
               (strcmp(begin_date, begin_date_reservation) == 0 ||
                strcmp(begin_date, end_date_reservation) == 0 ||
                strcmp(end_date, end_date_reservation) == 0 ||
                strcmp(end_date, begin_date_reservation) == 0)) {
      if (strcmp(begin_date_reservation, end_date_reservation) == 0)
        revenue += 1 * price_per_night;
      else
        revenue += 2 * price_per_night;
    } else if (strcmp(begin_date, begin_date_reservation) <= 0 &&
               strcmp(end_date, end_date_reservation) >= 0) {
      revenue += calculate_number_of_nights(begin_date_reservation,
                                            end_date_reservation) *
                 price_per_night;
    }
    free(begin_date_reservation);
    free(end_date_reservation);
  }
  write_query8(has_f, output_file, revenue);
}

struct user_info {
  char *user_id;
  char *user_name;
};

void query9(bool has_f, char **query_parameters, FlightsData *flights_data,
            PassengersData *passengers_data,
            ReservationsData *reservations_data, UsersData *users_data,
            StatsUserInfo *users_stats, FILE *output_file, int num_parameters) {
  if (query_parameters[0] == NULL) return;
  char *prefix = create_prefix(query_parameters, num_parameters);

  if (prefix == NULL) return;
  GArray *stats_user = get_stats_user_info(users_stats);
  char **user_ids = malloc(sizeof(char *) * stats_user->len);
  char **user_names = malloc(sizeof(char *) * stats_user->len);
  GArray *respond = g_array_new(FALSE, FALSE, sizeof(UserInfoStats *));

  g_array_sort(stats_user, (GCompareFunc)compare);

  guint matched_index;
  gboolean aux = g_array_binary_search(
      stats_user, prefix, (GCompareFunc)check_prefix, &matched_index);

  if (aux) {
    UserInfoStats *matched_user =
        g_array_index(stats_user, UserInfoStats *, matched_index);

    for (int i = matched_index; i >= 0 && strncmp(matched_user->user_name,
                                                  prefix, strlen(prefix)) == 0;
         i--) {
      char *account_status = get_account_status(
          get_user_by_username(users_data, matched_user->user_id));

      if (strcmp(account_status, "ACTIVE") == 0) {
        g_array_prepend_val(respond,
                            g_array_index(stats_user, UserInfoStats *, i));
      }
      matched_user = g_array_index(stats_user, UserInfoStats *, i - 1);

      free(account_status);
    }

    matched_user =
        g_array_index(stats_user, UserInfoStats *, matched_index + 1);

    for (int j = matched_index + 1;
         j < stats_user->len &&
         strncmp(matched_user->user_name, prefix, strlen(prefix)) == 0;
         j++) {
      char *account_status = get_account_status(
          get_user_by_username(users_data, matched_user->user_id));
      if (strcmp(account_status, "ACTIVE") == 0) {
        g_array_append_val(respond,
                           g_array_index(stats_user, UserInfoStats *, j));
      }
      matched_user = g_array_index(stats_user, UserInfoStats *, j + 1);
      free(account_status);
    }

    g_array_sort(respond, (GCompareFunc)compare_respond);

    for (int k = 0; k < (int)respond->len; k++) {
      UserInfoStats *user =
          (UserInfoStats *)(g_array_index(respond, void *, k));

      user_ids[k] = (user->user_id);
      user_names[k] = (user->user_name);
    }
  }
  write_query9(has_f, output_file, user_ids, user_names, respond->len);
  g_array_free(respond, TRUE);
  free(user_names);
  free(user_ids);
}

void query10(bool has_f, char **query_parameters, int num_parameters,
             FlightsData *flights_data, PassengersData *passengers_data,
             ReservationsData *reservations_data, UsersData *users_data,
             FILE *output_file) {}

void query_manager(char *line, FlightsData *flights_data,
                   PassengersData *passengers_data,
                   ReservationsData *reservations_data, UsersData *users_data,
                   StatsUserInfo *users_stats, FILE *output_file) {
  int query_type;
  char modifier;
  char *parameters = malloc(sizeof(char) * 100);
  sscanf(line, "%d%c %[^\n]", &query_type, &modifier, parameters);

  bool has_f = (modifier == 'F');

  char **query_parameters = NULL;
  int num_parameters = 0;
  char *parameters_copy = parameters;

  char *token = strsep(&parameters_copy, " ");
  while (token != NULL) {
    query_parameters =
        realloc(query_parameters, (num_parameters + 1) * sizeof(char *));
    query_parameters[num_parameters] = malloc(strlen(token) + 1);
    strcpy(query_parameters[num_parameters], token);
    num_parameters++;
    token = strsep(&parameters_copy, " ");
  }
  free(parameters_copy);
  free(parameters);

  switch (query_type) {
    case 1:
      query1(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 2:
      query2(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file, num_parameters);
      break;
    case 3:
      query3(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 4:
      query4(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 5:
      query5(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 6:
      query6(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 7:
      query7(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 8:
      query8(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, output_file);
      break;
    case 9:
      query9(has_f, query_parameters, flights_data, passengers_data,
             reservations_data, users_data, users_stats, output_file,
             num_parameters);
      break;
    case 10:
      query10(has_f, query_parameters, num_parameters, flights_data,
              passengers_data, reservations_data, users_data, output_file);
      break;
    default:
      // This is not supposed to happen
      printf("Error choosing the query\n");
      break;
  }

  for (int i = 0; i < num_parameters; ++i) {
    free(query_parameters[i]);
  }
  free(query_parameters);
}
