#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 255
#define TUP_TIME_FORMAT 0
#define TUP_DATE_FORMAT 1	 

#define STORAGE_KEY_TIME_FORMAT 0
#define STORAGE_KEY_DATE_FORMAT 1


#define MyTupletCString(_key, _cstring) \
((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = strlen(_cstring) + 1 }})

typedef enum {
  AppKeyTimeFormat = 0,
  AppKeyDateFormat = 1,
} AppKey;

char time_format[40] = "fuzzy";
char date_format[40] = "season";

	char time_str[BUFFER_SIZE] = " ";
	char date_str[BUFFER_SIZE] = " ";

static AppSync sync;
static uint8_t sync_buffer[255];

static struct CommonWordsData {
	Layer *ac_layer;
	Window *window;
	char buffer[BUFFER_SIZE];
} s_data;

static GFont font;

static void update_time(struct tm* t) {
//	t->tm_hour = 16;
//	t->tm_min = 33;
//	if ( strcmp( time_format, "fuzzy" ) == 0 && strcmp( date_format, "season" ) == 0 ) {
//		fuzzy_time_to_words(t->tm_hour, t->tm_min, t->tm_mday, t->tm_mon, t->tm_year, s_data.buffer, BUFFER_SIZE);
//	} else {
		if ( strcmp( time_format, "weekend" ) == 0 && (t->tm_wday == 0 || t->tm_wday == 6)) {
			time_to_words( t, time_str, BUFFER_SIZE, "fuzziest" );
		} else if ( strcmp( time_format, "weekend" ) == 0 && t->tm_wday > 0 && t->tm_wday < 6) {
			time_to_words( t, time_str, BUFFER_SIZE, "normal" );
		} else {
			time_to_words( t, time_str, BUFFER_SIZE, time_format );
		}
		if ( strcmp( date_format, "weekend" ) == 0 && (t->tm_wday == 0 || t->tm_wday == 6)) {
			date_to_words( t, date_str, BUFFER_SIZE, "weekday" );
		} else if ( strcmp( date_format, "weekend" ) == 0 && t->tm_wday > 0 && t->tm_wday < 6) {
			date_to_words( t, date_str, BUFFER_SIZE, "full" );
		} else {
			date_to_words( t, date_str, BUFFER_SIZE, date_format );
		}

		snprintf( s_data.buffer, BUFFER_SIZE, "%s %s", time_str, date_str );
//	}
//	snprintf( s_data.buffer, BUFFER_SIZE, "ДВАНАЙСЕТ БЕЗ ПЕТНАЙСЕТ" );
//	text_layer_set_text(s_data.label, s_data.buffer);
	layer_mark_dirty( s_data.ac_layer );
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	update_time(tick_time);
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	switch (key) {
		case TUP_TIME_FORMAT:
			snprintf( time_format, 40, "%s", new_tuple->value->cstring );
			persist_write_string( STORAGE_KEY_TIME_FORMAT, time_format );
			break;
		case TUP_DATE_FORMAT:
			snprintf( date_format, 40, "%s", new_tuple->value->cstring );
			persist_write_string( STORAGE_KEY_DATE_FORMAT, date_format );
			break;
	}

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	update_time(t);
}

static void text_layer_update(struct Layer *layer, GContext *ctx) {

	GRect frame = layer_get_frame(layer);
	GSize size = graphics_text_layout_get_content_size( s_data.buffer, font, GRect(0, 0, frame.size.w, frame.size.h), 
			GTextOverflowModeWordWrap, GTextAlignmentCenter ); 	
	graphics_draw_text( ctx, s_data.buffer, font, GRect(0, (frame.size.h - size.h) / 2, frame.size.w, frame.size.h),
			GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL ); 	
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *time_format_tp = dict_find(iter, AppKeyTimeFormat);
  if(time_format_tp) {
    char *time_format_tpv = time_format_tp->value->cstring;
    strncpy(time_format, time_format_tpv, sizeof(time_format));
  }
  Tuple *date_format_tp = dict_find(iter, AppKeyDateFormat);
  if(date_format_tp) {
    char *date_format_tpv = date_format_tp->value->cstring;
    strncpy(date_format, date_format_tpv, sizeof(date_format));
  }
  
	persist_write_string( STORAGE_KEY_TIME_FORMAT, time_format );
	persist_write_string( STORAGE_KEY_DATE_FORMAT, date_format );
  
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	update_time(t);
}



static void do_init(void) {
	s_data.window = window_create();
	const bool animated = true;
  #ifdef PBL_SDK_2
	window_set_fullscreen( s_data.window, true );
  #endif
	window_stack_push(s_data.window, animated);

	window_set_background_color(s_data.window, GColorBlack);

	Layer *root_layer = window_get_root_layer(s_data.window);
	GRect frame = layer_get_frame(root_layer);

	s_data.ac_layer = layer_create( frame );
	layer_set_update_proc( s_data.ac_layer, text_layer_update );
	layer_add_child(root_layer, s_data.ac_layer);

	font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_CONDENSED_24));

	if ( persist_exists( STORAGE_KEY_TIME_FORMAT ) ) {
		persist_read_string(STORAGE_KEY_TIME_FORMAT, time_format, 40);
	}
	if ( persist_exists( STORAGE_KEY_DATE_FORMAT ) ) {
		persist_read_string( STORAGE_KEY_DATE_FORMAT, date_format, 40);
	}

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	update_time(t);

	const int inbound_size = 255;
	const int outbound_size = 255;
	app_message_open(inbound_size, outbound_size);

	Tuplet initial_values[] = {
		MyTupletCString(TUP_TIME_FORMAT, time_format),
		MyTupletCString(TUP_DATE_FORMAT, date_format),
	};

  app_message_register_inbox_received(inbox_received_handler);
  
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}


static void do_deinit(void) {
	tick_timer_service_unsubscribe();
	app_sync_deinit(&sync);
	window_destroy(s_data.window);
	layer_destroy(s_data.ac_layer);
}

int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}
