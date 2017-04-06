#include "num2words.h"
#include "string.h"
#include "stdio.h"

static const char* const ONES[] = {
  "ДВАНАЙСЕТ",
  "ЕДИН",
  "ДВА",
  "ТРИ",
  "ЧЕТИРИ",
  "ПЕТ",
  "ШЕСТ",
  "СЕДЕМ",
  "ОСЕМ",
  "ДЕВЕТ"
};

static const char* const TEENS[] ={
  "",
  "ЕДИНАЙСЕТ",
  "ДВАНАЙСЕТ",
  "ТРИНАЙСЕТ",
  "ЧЕТИРИНАЙСЕТ",
  "ПЕТНАЙСЕТ",
  "ШЕСТНАЙСЕТ",
  "СЕДЕМНАЙСЕТ",
  "ОСЕМНАЙСЕТ",
  "ДЕВЕТНАЙСЕТ"
};

static const char* const TENS[] = {
  "",
  "ДЕСЕТ",
  "ДВАЙСЕТ",
  "ТРИЙСЕТ",
  "ЧЕТИРЕСЕТ",
  "ПЕТДЕСЕТ",
  "ШЕЙСЕТ",
  "СЕДЕМДЕСЕТ",
  "ОСЕМДЕСЕТ",
  "ДЕВЕТДЕСЕТ"
};

static const char* const HOUR_WORDS[] = {
	"ДВАНАЙСЕТ",
	"ЕДИН",
	"ДВА",
	"ТРИ",
	"ЧЕТИРИ",
	"ПЕТ",
	"ШЕСТ",
	"СЕДЕМ",
	"ОСЕМ",
	"ДЕВЕТ",
	"ДЕСЕТ",
	"ЕДИНАЙСЕТ",
	"ДВАНАЙСЕТ"
};

static const char* const MINUTE_WORDS[] = {
	"ТОЧНО",
	"ЕДНА",
	"ДВЕ",
	"ТРИ",
	"ЧЕТИРИ",
	"ПЕТ",
	"ШЕСТ",
	"СЕДЕМ",
	"ОСЕМ",
	"ДЕВЕТ",
	"ДЕСЕТ",
	"ЕДИНАЙСЕТ",
	"ДВАНАЙСЕТ",
	"ТРИНАЙСЕТ",
	"ЧЕТИРИНАЙ\u00AD\nСЕТ",
	"ПЕТНАЙСЕТ",
	"ШЕСТНАЙСЕТ",
	"СЕДЕМНАЙ\u00AD\nСЕТ",
	"ОСЕМНАЙСЕТ",
	"ДЕВЕТНАЙСЕТ",
	"ДВАЙСЕТ",
	"ДВАЙСЕТ И ЕДНА",
	"ДВАЙСЕТ И ДВЕ",
	"ДВАЙСЕТ И ТРИ",
	"ДВАЙСЕТ И ЧЕТИРИ",
	"ДВАЙСЕТ И ПЕТ",
	"ДВАЙСЕТ И ШЕСТ",
	"ДВАЙСЕТ И СЕДЕМ",
	"ДВАЙСЕТ И ОСЕМ",
	"ДВАЙСЕТ И ДЕВЕТ",
	"ПОЛОВИНА"
};

static const char* const WEEKDAY_SHORT[] = {
	"НЕД",
	"ПОН",
	"ВТО",
	"СРЯ",
	"ЧЕТ",
	"ПЕТ",
	"СЪБ"
};

static const char* const WEEKDAY[] = {
	"НЕДЕЛЯ",
	"ПОНЕДЕЛНИК",
	"ВТОРНИК",
	"СРЯДА",
	"ЧЕТВЪРТЪК",
	"ПЕТЪК",
	"СЪБОТА"
};

static const char* MINUTE_SINGLE_WORD = "МИНУТА";
static const char* MINUTE_PLURAL_WORD = "МИНУТИ";
static const char* HOUR_SINGLE_WORD   = "ЧАСА";

static const char* STR_HOUR = "ЧАСА";
static const char* STR_WITHOUT = "БЕЗ";
static const char* STR_AND = "И";
static const char* STR_HALF = "ПОЛОВИНА";

static void min2str( char* buffer, int min ) {
	if ( min == 0 || min == 30 ) {
		strcat( buffer, " " );
		strcat( buffer, MINUTE_WORDS[min] );
	} else if ( min == 1 ) {
		strcat( buffer, " " );
		strcat( buffer, MINUTE_WORDS[min] );
		strcat( buffer, " " );
		strcat( buffer, MINUTE_SINGLE_WORD );
	} else {
		strcat( buffer, " " );
		strcat( buffer, MINUTE_WORDS[min] );
		strcat( buffer, " " );
		strcat( buffer, MINUTE_PLURAL_WORD );
	}
}

static void hour2str( char* buffer, int hour, int add_hour ) {
	if ( add_hour == 1 ) {
		strcat( buffer, HOUR_WORDS[hour] );
		strcat( buffer, " " );
		strcat( buffer, HOUR_SINGLE_WORD );
	} else {
		strcat( buffer, HOUR_WORDS[hour] );
	}
}

void date_to_words( struct tm* t, char *buffer, const size_t buffer_len, char *date_format) {
	int month = t->tm_mon + 1;
	if ( strcmp( date_format, "off" ) == 0 ) {
		snprintf( buffer, buffer_len, " " );
	} else if ( strcmp( date_format, "on" ) == 0 ) {
		snprintf( buffer, buffer_len, "\n%02d/%02d", t->tm_mday, month );
	} else if ( strcmp( date_format, "full" ) == 0 ) {
		snprintf( buffer, buffer_len, "\n%s %02d/%02d", WEEKDAY_SHORT[t->tm_wday], t->tm_mday, month );
	} else if ( strcmp( date_format, "weekday" ) == 0 ) {
		snprintf( buffer, buffer_len, "\n%s", WEEKDAY[t->tm_wday]);
	} else {
		if ( month < 3 || ( month == 3 && t->tm_mday < 21 ) ) {
			snprintf( buffer, buffer_len, "\nЗИМА" );
		} else if ( month < 6 || ( month == 6 && t->tm_mday < 21 ) ) {
			snprintf( buffer, buffer_len, "\nПРОЛЕТ" );
		} else if ( month < 9 || ( month == 9 && t->tm_mday < 23 ) ) {
			snprintf( buffer, buffer_len, "\nЛЯТО" );
		} else if ( month < 12 || ( month == 12 && t->tm_mday < 21 ) ) {
			snprintf( buffer, buffer_len, "\nЕСЕН" );
		} else {
			snprintf( buffer, buffer_len, "\nЗИМА" );
		}
	}
}

void time_to_words( struct tm* t, char *buffer, const size_t buffer_len, const char *time_format) {
	int hours, minutes;

	memset( buffer, 0, buffer_len );
	
	hours = t->tm_hour;
	minutes = t->tm_min;

	if ( strcmp( time_format, "fuzzy" ) == 0 ) {
		minutes = ((minutes + 2) / 5) * 5;
	} 
	if ( minutes >= 60 ) {
		minutes = 0;
		hours++;
	} else if ( minutes > 37 && strcmp( time_format, "fuzziest" ) == 0 ) {
		hours++;
	} else if ( minutes > 32 && strcmp( time_format, "fuzzy" ) == 0 ) {
		hours++;
	} else if ( minutes > 30 && strcmp( time_format, "normal" ) == 0 ) {
		hours++;
	}
	if ( strcmp( time_format, "fuzziest" ) == 0 ) {
		hour2str( buffer, hours > 12 ? hours % 12 : hours, 0 );
		if ( minutes <= 7 || minutes >= 53 ) {
			/* no addition, hour only */
			strcat( buffer, " ЧАСА" );
		} else {
			if ( minutes <= 37 ) {
				strcat( buffer, " И" );
			} else {
				strcat( buffer, " БЕЗ" );
			}
			if ( minutes >= 23 && minutes <= 37 ) {
				strcat( buffer, " ПОЛОВИНА" );
			} else {
				strcat( buffer, " НЕЩО" );
			}
		}
	} else {
		hour2str( buffer, hours > 12 ? hours % 12 : hours, 1 );
		if ( minutes == 0 ) {
		} else if ( minutes <= 30 ) {
			strcat( buffer, " И" );
			min2str( buffer, minutes );
		} else {
			strcat( buffer, " БЕЗ" );
			min2str( buffer, 60 - minutes );
		}
	}
	if ( hours < 12 ) {
		strcat( buffer, " СУТРИНТА" );
	} else if ( hours < 18 ) {
		strcat( buffer, " СЛЕДОБЯД" );
	} else {
		strcat( buffer, " ВЕЧЕРТА" );
	}

}

static size_t append_number(char* words, int num) {
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;

  size_t len = 0;

  if (tens_val > 0) {
    if (tens_val == 1 && num != 10) {
      strcat(words, TEENS[ones_val]);
      return strlen(TEENS[ones_val]);
    }
    strcat(words, TENS[tens_val]);
    len += strlen(TENS[tens_val]);
    if (ones_val > 0) {
      strcat(words, " И ");
      len += 1;
    }
  }

  if (ones_val > 0 || num == 0) {
    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  return len;
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);

  size_t written = strlen(str);
  return (length > written) ? written : length;
}

void fuzzy_time_to_words(int hours, int minutes, int mday, int month, int year, char* words, size_t length) {
  int fuzzy_hours = hours;
  int fuzzy_minutes = ((minutes + 2) / 5) * 5;
  month += 1;
  year += 1900;

  // Handle hour & minute roll-over.
  if (fuzzy_minutes > 55) {
    fuzzy_minutes = 0;
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }

  size_t remaining = length;
  memset(words, 0, length);


  if ( fuzzy_minutes > 30 ) {
    remaining -= append_number(words, (fuzzy_hours + 1)% 12);
  } else {
    remaining -= append_number(words, fuzzy_hours % 12);
  }
  remaining -= append_string(words, remaining, "\n");
  if ( fuzzy_minutes == 30 ) {
    remaining -= append_string(words, remaining, STR_AND);
    remaining -= append_string(words, remaining, "\n");
    remaining -= append_string(words, remaining, STR_HALF);
  } else if ( fuzzy_minutes == 0 ) {
    remaining -= append_string(words, remaining, STR_HOUR);
    remaining -= append_string(words, remaining, "\n");
  } else if ( fuzzy_minutes < 30 ) {
    remaining -= append_string(words, remaining, STR_AND);
    remaining -= append_string(words, remaining, "\n");
    remaining -= append_number(words, fuzzy_minutes);
  } else {
    remaining -= append_string(words, remaining, STR_WITHOUT);
    remaining -= append_string(words, remaining, "\n");
    remaining -= append_number(words, 60 - fuzzy_minutes);
  }
	  
  remaining -= append_string(words, remaining, "\n");
  if ( fuzzy_hours > 0 && fuzzy_hours < 11 ) {
    remaining -= append_string(words, remaining, "СУТРИНТА\n");
  } else if ( fuzzy_hours == 11 && fuzzy_minutes < 30 ) {
    remaining -= append_string(words, remaining, "СУТРИНТА\n");
  } else if ( fuzzy_hours > 0 && fuzzy_hours < 17 ) {
    remaining -= append_string(words, remaining, "СЛЕДОБЯД\n");
  } else if ( fuzzy_hours == 17 && fuzzy_minutes < 17 ) {
    remaining -= append_string(words, remaining, "СЛЕДОБЯД\n");
  } else {
    remaining -= append_string(words, remaining, "ВЕЧЕРТА\n");
  }

  if ( fuzzy_minutes != 25 && fuzzy_minutes != 35 ) {
	  remaining -= append_string(words, remaining, "\n");
  }
  if ( month < 3 || ( month == 3 && mday < 21 ) ) {
    remaining -= append_string(words, remaining, "ЗИМА");
  } else if ( month < 6 || ( month == 6 && mday < 21 ) ) {
    remaining -= append_string(words, remaining, "ПРОЛЕТ");
  } else if ( month < 9 || ( month == 9 && mday < 23 ) ) {
    remaining -= append_string(words, remaining, "ЛЯТО");
  } else if ( month < 12 || ( month == 12 && mday < 21 ) ) {
    remaining -= append_string(words, remaining, "ЕСЕН");
  } else {
    remaining -= append_string(words, remaining, "ЗИМА");
  }

  snprintf( words, length, "%s %d", words, year );

}

