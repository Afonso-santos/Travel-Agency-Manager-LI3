#include "utils/utils.h"

#include <ctype.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int calculate_delay(char* scheduled_date, char* actual_date) {
  static char result[20];

  int scheduled_year, scheduled_month, scheduled_day, scheduled_hour,
      scheduled_minute, scheduled_second;
  int actual_year, actual_month, actual_day, actual_hour, actual_minute,
      actual_second;

  sscanf(scheduled_date, "%d/%d/%d %d:%d:%d", &scheduled_year, &scheduled_month,
         &scheduled_day, &scheduled_hour, &scheduled_minute, &scheduled_second);
  sscanf(actual_date, "%d/%d/%d %d:%d:%d", &actual_year, &actual_month,
         &actual_day, &actual_hour, &actual_minute, &actual_second);

  int time_difference_seconds =
      ((actual_year - scheduled_year) * 365 * 24 * 60 * 60) +
      ((actual_month - scheduled_month) * 30 * 24 * 60 * 60) +
      ((actual_day - scheduled_day) * 24 * 60 * 60) +
      ((actual_hour - scheduled_hour) * 60 * 60) +
      ((actual_minute - scheduled_minute) * 60) +
      (actual_second - scheduled_second);

  return time_difference_seconds;
}

double calculate_total_price(int num_nights, int price_per_night,
                             int city_tax) {
  double total_price =
      num_nights * price_per_night +
      ((double)(num_nights * price_per_night) / 100) * city_tax;

  return total_price;
}

int calculate_number_of_nights(const char* begin_date_str,
                               const char* end_date_str) {
  if (strcmp(begin_date_str, end_date_str) == 0) return 1;
  int begin_year, begin_month, begin_day;
  int end_year, end_month, end_day;

  sscanf(begin_date_str, "%d/%d/%d", &begin_year, &begin_month, &begin_day);
  sscanf(end_date_str, "%d/%d/%d", &end_year, &end_month, &end_day);

  int begin_date = begin_year * 10000 + begin_month * 100 + begin_day;
  int end_date = end_year * 10000 + end_month * 100 + end_day;
  int nights = end_date - begin_date;
  return nights;
}

char* calculate_age(const char* birth_date_str) {
  static char result[3];

  int birth_date_year, birth_date_month, birth_date_day;
  int master_date_year, master_date_month, master_date_day;

  sscanf(birth_date_str, "%d/%d/%d", &birth_date_year, &birth_date_month,
         &birth_date_day);
  sscanf(MASTER_DATE, "%d/%d/%d", &master_date_year, &master_date_month,
         &master_date_day);

  int age = master_date_year - birth_date_year;

  if (master_date_month < birth_date_month ||
      (master_date_month == birth_date_month &&
       master_date_day < birth_date_day)) {
    age--;
  }

  sprintf(result, "%d", age);

  return result;
}

int extract_number(char* input) {
  while (*input && !isdigit(*input)) input++;
  return strtol(input, NULL, 10);
}

char* get_error_as_string(ERRORS error) {
  switch (error) {
    case ERR_OPENING_FLIGHTS_FILE:
      return "Error opening flights file!";

    case ERR_OPENING_PASSENGERS_FILE:
      return "Error opening passengers file!";

    case ERR_OPENING_RESERVATIONS_FILE:
      return "Error opening reservations file!";

    case ERR_OPENING_USERS_FILE:
      return "Error opening users file!";

    case ERR_CREATING_DIRECTORY:
      return "Error creating directory!";

    case ERR_OPENING_QUERIES_FILE:
      return "Error opening queries file!";

    case ERR_OPENING_OUTPUT_FILE:
      return "Error opening output file!";

    case ERROR_WRITING_TO_ERRORS_FILE:
      return "Error writing in errors file!";

    default:
      return "Unknown error.";
  }
}

int print_errors_to_file(char* line[], int size_of_line, const char* file_name,
                         int num_lines) {
  FILE* file = fopen(file_name, "a");

  if (file == NULL) {
    return ERROR_WRITING_TO_ERRORS_FILE;
  }

  for (int i = 0; i < size_of_line; i++) {
    fprintf(file, "%s", line[i]);
    if (i < size_of_line - 1) {
      fprintf(file, ";");
    }
  }

  fprintf(file, "\n");

  fclose(file);
  return num_lines++;
}

char* create_filename(char* folder, const char* string) {
  char* filename = malloc(sizeof(char) * (strlen(folder) + strlen(string) + 1));
  strcpy(filename, folder);
  strcat(filename, string);

  return filename;
}

int create_directory(char* folder) {
  int ret = g_mkdir_with_parents(folder, 0777);

  if (ret == -1) {
    return ERR_CREATING_DIRECTORY;
  }

  return 0;
}

FILE* create_output_file(int queries_counter) {
  char* filename = malloc(sizeof(char) * 256);
  sprintf(filename, "Resultados/command%d_output.txt", queries_counter);
  FILE* output_file = fopen(filename, "w");
  free(filename);

  return output_file;
}

char* standardize_account_status(char* account_status) {
  if (account_status != NULL) {
    for (int i = 0; account_status[i]; i++) {
      account_status[i] = tolower(account_status[i]);
    }

    if (strcmp(account_status, "active") == 0) {
      return "ACTIVE";
    } else if (strcmp(account_status, "inactive") == 0) {
      return "INACTIVE";
    }
  }

  return "NO STATUS";
}

char* standardize_includes_breakfast(char* includes_breakfast) {
  if (includes_breakfast == NULL) return "FALSE";

  for (int i = 0; includes_breakfast[i]; i++) {
    includes_breakfast[i] = tolower(includes_breakfast[i]);
  }

  if ((strcmp(includes_breakfast, "t") == 0) ||
      (strcmp(includes_breakfast, "true") == 0) ||
      (strcmp(includes_breakfast, "1") == 0))
    return "TRUE";
  else if (strcmp(includes_breakfast, "false") == 0 ||
           strcmp(includes_breakfast, "f") == 0 ||
           strcmp(includes_breakfast, "0") == 0)
    return "FALSE";
}

char* standardize_airport_name(char* airport_name) {
  if (airport_name != NULL) {
    for (int i = 0; airport_name[i]; i++) {
      airport_name[i] = tolower(airport_name[i]);
    }
  }

  return airport_name;
}

bool validate_date_format_without_time(char* date) {
  if (strlen(date) != 10) return false;

  for (int i = 0; i < 10; i++) {
    if (i == 4 || i == 7) {
      if (date[i] != '/') return false;
    } else {
      if (!isdigit(date[i])) return false;
    }
  }

  int year, month, day;
  sscanf(date, "%4d/%2d/%2d", &year, &month, &day);

  if (month < 1 || month > 12 || day < 1 || day > 31) return false;

  return true;
}

bool validate_date_format_with_time(char* date) {
  if (strlen(date) != 19) return false;

  for (int i = 0; i < 19; i++) {
    if (i == 4 || i == 7) {
      if (date[i] != '/') return false;
    } else if (i == 10) {
      if (date[i] != ' ') return false;
    } else if (i == 13 || i == 16) {
      if (date[i] != ':') return false;
    } else {
      if (!isdigit(date[i])) return false;
    }
  }

  int year, month, day, hour, minutes, seconds;
  sscanf(date, "%4d/%2d/%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minutes,
         &seconds);

  if (month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 ||
      minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59)
    return false;

  return true;
}

bool validate_email(char* email) {
  char* email_validate = malloc(sizeof(char) * (strlen(email) + 1));
  strcpy(email_validate, email);
  char* username = strtok(email_validate, "@");
  if (username == NULL || strlen(username) < 1) return false;

  char* domain = strtok(NULL, ".");
  char* tld = strtok(NULL, ".");
  if (domain == NULL || strlen(domain) < 1 || tld == NULL || strlen(tld) < 2) {
    free(email_validate);
    return false;
  }
  free(email_validate);
  return true;
}

bool validate_country_code(char* country_code) {
  char* country_code_validation =
      malloc(sizeof(char) * (strlen(country_code) + 1));
  strcpy(country_code_validation, country_code);
  if (strlen(country_code_validation) != 2) {
    free(country_code_validation);
    return false;
  }

  for (int i = 0; i < 2; i++) {
    if (!isalpha(country_code[i])) {
      free(country_code_validation);
      return false;
    }
  }
  free(country_code_validation);
  return true;
}

bool validate_account_status(char* account_status) {
  char* account_status_validate =
      malloc(sizeof(char) * (strlen(account_status) + 1));
  strcpy(account_status_validate, account_status);
  for (int i = 0; account_status[i]; i++) {
    account_status_validate[i] = tolower(account_status[i]);
  }

  if (strcmp(account_status_validate, "active") == 0 ||
      strcmp(account_status_validate, "inactive") == 0) {
    free(account_status_validate);
    return true;
  }
  free(account_status_validate);
  return false;
}

bool validate_total_seats(const char* total_seats) {
  char* total_seats_validate = malloc(sizeof(char) * (strlen(total_seats) + 1));
  strcpy(total_seats_validate, total_seats);
  char* endptr;  // Pointer to the first invalid character in strtol

  // Convert the string to an integer
  long int value = strtol(total_seats, &endptr, 10);

  // Check for conversion errors
  if (*endptr != '\0' || total_seats == endptr || value < 0) {
    // Not a valid integer or empty string
    free(total_seats_validate);
    return false;
  }

  free(total_seats_validate);
  return true;
}

bool validate_airports(char* airport1, char* airport2) {
  if (strlen(airport1) < 3 || strlen(airport2) < 3) return false;

  char airport1_lower[4];
  char airport2_lower[4];

  for (int i = 0; i < 3; i++) {
    airport1_lower[i] = tolower(airport1[i]);
    airport2_lower[i] = tolower(airport2[i]);
  }

  airport1_lower[3] = '\0';
  airport2_lower[3] = '\0';

  return (strcmp(airport1_lower, airport2_lower) == 0);
}

bool validate_number_of_stars(int hotel_stars) {
  return (hotel_stars >= 1 && hotel_stars <= 5);
}

bool validate_city_tax(char* city_tax) {
  char* city_tax_validate = malloc(sizeof(char) * (strlen(city_tax) + 1));
  strcpy(city_tax_validate, city_tax);
  char* endptr;
  long tax_value = strtol(city_tax_validate, &endptr, 10);

  if (*endptr == '\0' && tax_value >= 0) {
    free(city_tax_validate);
    return true;
  }
  free(city_tax_validate);
  return false;
}

bool validate_price_per_night(int price_per_night) {
  return (price_per_night > 0);
}

bool validate_includes_breakfast(char* includes_breakfast) {
  char* includes_breakfast_validate =
      malloc(sizeof(char) * (strlen(includes_breakfast) + 1));
  strcpy(includes_breakfast_validate, includes_breakfast);
  if (strlen(includes_breakfast_validate) == 0) return true;

  for (int i = 0; includes_breakfast_validate[i]; i++) {
    includes_breakfast_validate[i] = tolower(includes_breakfast_validate[i]);
  }

  if ((strcmp(includes_breakfast_validate, "t") == 0) ||
      (strcmp(includes_breakfast_validate, "true") == 0) ||
      (strcmp(includes_breakfast_validate, "1") == 0)) {
    free(includes_breakfast_validate);
    return true;
  } else if (strcmp(includes_breakfast_validate, "false") == 0 ||
             strcmp(includes_breakfast_validate, "f") == 0 ||
             strcmp(includes_breakfast_validate, "0") == 0) {
    free(includes_breakfast_validate);
    return true;
  }
  free(includes_breakfast_validate);
  return false;
}

bool validate_rating(char* rating) {
  char* rating_validate = malloc(sizeof(char) * (strlen(rating) + 1));
  strcpy(rating_validate, rating);
  char* endptr;
  double rating_value = strtod(rating, &endptr);

  if (*endptr == '\0' && rating_value >= 1 && rating_value <= 5) {
    free(rating_validate);
    return true;
  }
  free(rating_validate);
  return false;
}

bool validate_parameter_not_empty(char* parameter) {
  return (parameter != NULL && strlen(parameter) > 0);
}

int setup_catalogs_and_stats(char* folder, FLIGHTS_CATALOG flights_catalog,
                             PASSENGERS_CATALOG passengers_catalog,
                             RESERVATIONS_CATALOG reservations_catalog,
                             USERS_CATALOG users_catalog, STATS stats) {
  char* flights_filename = create_filename(folder, "/flights.csv");
  char* passengers_filename = create_filename(folder, "/passengers.csv");
  char* reservations_filename = create_filename(folder, "/reservations.csv");
  char* users_filename = create_filename(folder, "/users.csv");

  char* users_errors_filename = malloc(sizeof(char) * 256);
  char* flights_errors_filename = malloc(sizeof(char) * 256);
  char* reservations_errors_filename = malloc(sizeof(char) * 256);
  char* passengers_errors_filename = malloc(sizeof(char) * 256);

  sprintf(users_errors_filename, "Resultados/users_errors.csv");
  sprintf(flights_errors_filename, "Resultados/flights_errors.csv");
  sprintf(reservations_errors_filename, "Resultados/reservations_errors.csv");
  sprintf(passengers_errors_filename, "Resultados/passengers_errors.csv");

  if (flights_filename == NULL || passengers_filename == NULL ||
      reservations_filename == NULL || users_filename == NULL) {
    printf("Error creating filenames!\n");
    return -1;
  }

  FILE* flights_file = fopen(flights_filename, "r");
  FILE* passengers_file = fopen(passengers_filename, "r");
  FILE* reservations_file = fopen(reservations_filename, "r");
  FILE* users_file = fopen(users_filename, "r");

  FILE* users_errors_file = fopen(users_errors_filename, "w");
  FILE* flights_errors_file = fopen(flights_errors_filename, "w");
  FILE* reservations_errors_file = fopen(reservations_errors_filename, "w");
  FILE* passengers_errors_file = fopen(passengers_errors_filename, "w");

  fprintf(users_errors_file,
          "id;name;email;phone_number;birth_date;sex;passport;country_code;"
          "address;account_creation;pay_method;account_status\n");
  fprintf(flights_errors_file,
          "id;airline;plane_model;total_seats;origin;destination;schedule_"
          "departure_date;schedule_arrival_date;real_departure_date;real_"
          "arrival_date;pilot;copilot;notes\n");
  fprintf(reservations_errors_file,
          "id;user_id;hotel_id;hotel_name;hotel_stars;city_tax;address;begin_"
          "date;end_date;price_per_night;includes_breakfast;room_details;"
          "rating;comment\n");
  fprintf(passengers_errors_file, "flight_id;user_id\n");

  if (flights_file == NULL || passengers_file == NULL ||
      reservations_file == NULL || users_file == NULL) {
    printf("Error opening files!\n");
    return -1;
  }

  parse_file(flights_file, flights_catalog, build_flight, stats,
             flights_errors_file, MAX_TOKENS_FLIGHT);
  parse_file(users_file, users_catalog, build_user, stats, users_errors_file,
             MAX_TOKENS_USER);
  parse_file_reservations(reservations_file, reservations_catalog,
                          users_catalog, build_reservation, stats,
                          reservations_errors_file, MAX_TOKENS_RESERVATION);
  parse_file_passengers(passengers_file, passengers_catalog, users_catalog,
                        flights_catalog, build_passenger, stats,
                        passengers_errors_file, MAX_TOKENS_PASSENGER);

  free(flights_filename);
  free(passengers_filename);
  free(reservations_filename);
  free(users_filename);
  free(users_errors_filename);
  free(flights_errors_filename);
  free(reservations_errors_filename);
  free(passengers_errors_filename);
  return 0;
}

struct query2_result {
  char** id;
  char** date;
  char** type;
  int iterator;
  bool has_f;
};

void sort_by_date(void* result, int N) {
  QUERY2_RESULT query_result = (QUERY2_RESULT)result;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (strcmp(query_result->date[i], query_result->date[j]) > 0) {
        char* temp = query_result->date[i];
        char* temp2 = query_result->id[i];
        char* temp3 = query_result->type[i];
        query_result->date[i] = query_result->date[j];
        query_result->date[j] = temp;
        query_result->id[i] = query_result->id[j];
        query_result->id[j] = temp2;
        query_result->type[i] = query_result->type[j];
        query_result->type[j] = temp3;
      } else if (strcmp(query_result->date[i], query_result->date[j]) == 0) {
        if (strcmp(query_result->id[i], query_result->id[j]) < 0) {
          char* temp = query_result->date[i];
          char* temp2 = query_result->id[i];
          char* temp3 = query_result->type[i];
          query_result->date[i] = query_result->date[j];
          query_result->date[j] = temp;
          query_result->id[i] = query_result->id[j];
          query_result->id[j] = temp2;
          query_result->type[i] = query_result->type[j];
          query_result->type[j] = temp3;
        }
      }
    }
  }
}

char* format_date(int year, int month, int day) {
  char* new_date = malloc(sizeof(char) * 11);
  if (day < 10 && month < 10) {
    sprintf(new_date, "%d/0%d/0%d", year, month, day);
  } else if (day < 10) {
    sprintf(new_date, "%d/%d/0%d", year, month, day);
  } else if (month < 10) {
    sprintf(new_date, "%d/0%d/%d", year, month, day);
  } else {
    sprintf(new_date, "%d/%d/%d", year, month, day);
  }

  return new_date;
}

struct query4_result {
  char** reservation_id;
  char** begin_date;
  char** end_date;
  char** user_id;
  int* rating;
  float* total_price;
  int iterator;
  bool has_f;
};

void sort_by_date_and_value(void* result, int N) {
  QUERY4_RESULT query_result = (QUERY4_RESULT)result;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (strcmp(query_result->begin_date[i], query_result->begin_date[j]) >
          0) {
        char* temp = query_result->reservation_id[i];
        char* temp1 = query_result->begin_date[i];
        char* temp2 = query_result->end_date[i];
        char* temp3 = query_result->user_id[i];
        int temp4 = query_result->rating[i];
        float temp5 = query_result->total_price[i];

        query_result->reservation_id[i] = query_result->reservation_id[j];
        query_result->begin_date[i] = query_result->begin_date[j];
        query_result->end_date[i] = query_result->end_date[j];
        query_result->user_id[i] = query_result->user_id[j];
        query_result->rating[i] = query_result->rating[j];
        query_result->total_price[i] = query_result->total_price[j];

        query_result->reservation_id[j] = temp;
        query_result->begin_date[j] = temp1;
        query_result->end_date[j] = temp2;
        query_result->user_id[j] = temp3;
        query_result->rating[j] = temp4;
        query_result->total_price[j] = temp5;

      } else if (strcmp(query_result->begin_date[i],
                        query_result->begin_date[j]) == 0) {
        if (strcmp(query_result->reservation_id[i],
                   query_result->reservation_id[j]) < 0) {
          char* temp = query_result->reservation_id[i];
          char* temp1 = query_result->begin_date[i];
          char* temp2 = query_result->end_date[i];
          char* temp3 = query_result->user_id[i];
          int temp4 = query_result->rating[i];
          float temp5 = query_result->total_price[i];

          query_result->reservation_id[i] = query_result->reservation_id[j];
          query_result->begin_date[i] = query_result->begin_date[j];
          query_result->end_date[i] = query_result->end_date[j];
          query_result->user_id[i] = query_result->user_id[j];
          query_result->rating[i] = query_result->rating[j];
          query_result->total_price[i] = query_result->total_price[j];

          query_result->reservation_id[j] = temp;
          query_result->begin_date[j] = temp1;
          query_result->end_date[j] = temp2;
          query_result->user_id[j] = temp3;
          query_result->rating[j] = temp4;
          query_result->total_price[j] = temp5;
        }
      }
    }
  }
}

int compare_dates(char* date1, char* date2) {
  int year1, month1, day1, hour1, minute1, second1;
  int year2, month2, day2, hour2, minute2, second2;

  // Parse the first date
  sscanf(date1, "%d/%d/%d %d:%d:%d", &year1, &month1, &day1, &hour1, &minute1,
         &second1);

  // Parse the second date
  sscanf(date2, "%d/%d/%d %d:%d:%d", &year2, &month2, &day2, &hour2, &minute2,
         &second2);

  // Compare years
  if (year1 < year2) {
    return -1;
  } else if (year1 > year2) {
    return 1;
  }

  // Compare months
  if (month1 < month2) {
    return -1;
  } else if (month1 > month2) {
    return 1;
  }

  // Compare days
  if (day1 < day2) {
    return -1;
  } else if (day1 > day2) {
    return 1;
  }

  // Compare hours
  if (hour1 < hour2) {
    return -1;
  } else if (hour1 > hour2) {
    return 1;
  }

  // Compare minutes
  if (minute1 < minute2) {
    return -1;
  } else if (minute1 > minute2) {
    return 1;
  }

  // Compare seconds
  if (second1 < second2) {
    return -1;
  } else if (second1 > second2) {
    return 1;
  }

  return 0;
}
