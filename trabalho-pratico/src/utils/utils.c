#define _GNU_SOURCE
#include "utils/utils.h"

#include <glib.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* calculate_age(char* birth_date_str) {
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

int calculate_delay(char* scheduled_date, char* actual_date) {
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

void sort_by_date(char** flight_ids, char** flight_dates, char** flight_types,
                  int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (strcmp(flight_dates[i], flight_dates[j]) > 0) {
        char* temp = flight_dates[i];
        char* temp2 = flight_ids[i];
        char* temp3 = flight_types[i];
        flight_dates[i] = flight_dates[j];
        flight_dates[j] = temp;
        flight_ids[i] = flight_ids[j];
        flight_ids[j] = temp2;
        flight_types[i] = flight_types[j];
        flight_types[j] = temp3;
      } else if (strcmp(flight_dates[i], flight_dates[j]) == 0) {
        if (strcmp(flight_ids[i], flight_ids[j]) < 0) {
          char* temp = flight_dates[i];
          char* temp2 = flight_ids[i];
          char* temp3 = flight_types[i];
          flight_dates[i] = flight_dates[j];
          flight_dates[j] = temp;
          flight_ids[i] = flight_ids[j];
          flight_ids[j] = temp2;
          flight_types[i] = flight_types[j];
          flight_types[j] = temp3;
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

char* remove_quotation_marks(char* str) {
  static char lineWithoutQuotes[100];
  int i, j;
  for (i = j = 0; i < strlen(str); i++) {
    if (str[i] == '"') {
      if (i == strlen(str) - 1)
        break;
      else
        i++;
    }
    lineWithoutQuotes[j] = str[i];
    j++;
  }
  lineWithoutQuotes[j] = '\0';
  return lineWithoutQuotes;
}

char* create_prefix(char** parameters, int N) {
  static char prefix[100];
  strcpy(prefix, parameters[0]);
  if (N == 1) return prefix;
  for (int i = 1; i < N; i++) {
    if (strcmp(parameters[i], prefix) == 0) break;
    strcat(prefix, " ");
    strcat(prefix, parameters[i]);
  }
  if (strchr(prefix, '"') != NULL)
    return remove_quotation_marks(prefix);
  else
    return prefix;
}

struct query4_result_helper {
  char* reservation_id;
  char* begin_date;
  char* end_date;
  char* user_id;
  int rating;
  float total_price;
};

struct query4_result {
  GArray* query4_result;
};

int compare_query4_result(const void* a, const void* b) {
  setlocale(LC_COLLATE, "en_US.UTF-8");
  const QUERY4_RESULT_HELPER* query4_result_a = (const QUERY4_RESULT_HELPER*)a;
  const QUERY4_RESULT_HELPER* query4_result_b = (const QUERY4_RESULT_HELPER*)b;
  int dateComparison =
      strcoll((*query4_result_b)->begin_date, (*query4_result_a)->begin_date);

  if (dateComparison != 0) {
    return dateComparison;
  } else {
    return strcoll((*query4_result_a)->reservation_id,
                   (*query4_result_b)->reservation_id);
  }
}

void sort_by_date_and_value(void* result, int N) {
  QUERY4_RESULT* query_result = (QUERY4_RESULT*)result;

  qsort((*query_result)->query4_result, N, sizeof(QUERY4_RESULT),
        compare_query4_result);
}

struct user_info {
  char* user_id;
  char* user_name;
};

int compare(const void* a, const void* b) {
  const UserInfoStats* user_a = *(const UserInfoStats**)a;
  const UserInfoStats* user_b = *(const UserInfoStats**)b;

  // Ensure null termination
  g_assert(user_a->user_name[strlen(user_a->user_name)] == '\0');
  g_assert(user_b->user_name[strlen(user_b->user_name)] == '\0');
  g_assert(user_a->user_id[strlen(user_a->user_id)] == '\0');
  g_assert(user_b->user_id[strlen(user_b->user_id)] == '\0');

  int nameComparison = strcmp(user_a->user_name, user_b->user_name);
  if (nameComparison != 0) {
    return nameComparison;
  } else {
    return strcmp(user_a->user_id, user_b->user_id);
  }
}

int check_prefix(const void* a, const void* b) {
  const UserInfoStats* user = *(const UserInfoStats**)a;
  const char* prefix = b;

  return strncmp(user->user_name, prefix, strlen(prefix));
}

int compare_respond(const void* a, const void* b) {
  setlocale(LC_COLLATE, "en_US.UTF-8");
  const UserInfoStats* user_a = *(const UserInfoStats**)a;
  const UserInfoStats* user_b = *(const UserInfoStats**)b;

  // Ensure null termination
  g_assert(user_a->user_name[strlen(user_a->user_name)] == '\0');
  g_assert(user_b->user_name[strlen(user_b->user_name)] == '\0');
  g_assert(user_a->user_id[strlen(user_a->user_id)] == '\0');
  g_assert(user_b->user_id[strlen(user_b->user_id)] == '\0');

  int nameComparison = strcoll(user_a->user_name, user_b->user_name);
  if (nameComparison != 0) {
    return nameComparison;
  } else {
    return strcoll(user_a->user_id, user_b->user_id);
  }
}