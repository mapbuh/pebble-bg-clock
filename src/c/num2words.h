#pragma once

#include "string.h"
#include "pebble.h"

void fuzzy_time_to_words(int hours, int minutes, int mday, int month, int year, char* words, size_t length);
void time_to_words( struct tm* t, char *buffer, const size_t buffer_len, const char *time_format);
void date_to_words( struct tm* t, char *buffer, const size_t buffer_len, char *date_format);
