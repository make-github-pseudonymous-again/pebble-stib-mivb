#include <pebble.h>
#include "inbox.h"
#include "pebble/translate_error.h"

static const uint32_t VAL_TYPE_STATE = 0;
static const uint32_t VAL_TYPE_REALTIME_TIME = 1;
static const uint32_t VAL_TYPE_REALTIME_STOP = 2;
static const uint32_t VAL_TYPE_REALTIME_END = 3;

static const uint32_t VAL_STATE_LOADING = 0;
static const uint32_t VAL_STATE_LOADED = 0;
static const uint32_t VAL_STATE_LOADED_GEOERROR = 0;
static const uint32_t VAL_STATE_LOADED_ERROR = 0;

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "[inbox] Message dropped: %i - %s", reason, pebble_translate_error(reason));
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  int size = (int)iterator->end - (int)iterator->dictionary;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[inbox] Received %d bytes", size);

  switch (dict_find(iterator, MESSAGE_KEY_TYPE)->value->uint32) {
    case VAL_TYPE_REALTIME_STOP: {
      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
      //const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;

      if ( uuid_run < s_uuid_run || ( uuid_run == s_uuid_run && uuid_send_realtime < s_uuid_send_realtime ) ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "received old realtime message");
	break;
      }

      if ( uuid_run > s_uuid_run || uuid_send_realtime > s_uuid_send_realtime ) {
	// we are receiving new realtime data, drop old
	APP_LOG(APP_LOG_LEVEL_DEBUG, "receiving new realtime data");
	s_uuid_run = uuid_run;
	s_uuid_send_realtime = uuid_send_realtime;
	Stops_clear(&s_stops_recv);
      }

      const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
      const char *stop_name = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_NAME)->value->cstring;
      const uint32_t stop_error = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ERROR)->value->uint32;
      const char *stop_message = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_MESSAGE)->value->cstring;

      Stop* stop = Stop_create(stop_id, stop_name, stop_error, stop_message);
      ds_DynamicArray_push(&s_stops_recv, stop);

      break;

    }
    case VAL_TYPE_REALTIME_REALTIME: {

      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;
      //const uint32_t uuid_send_realtime_item = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME_ITEM)->value->uint32;

      if ( uuid_run != UUID_RUN || uuid_send_realtime != UUID_SEND_REALTIME ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "received wrong realtime uuid");
	break;
      }

      const uint32_t stop_id = dict_find(iterator, MESSAGE_KEY_REALTIME_STOP_ID)->value->uint32;
      const char *line_name = dict_find(iterator, MESSAGE_KEY_REALTIME_LINE_NAME)->value->cstring;
      const char *destination_name = dict_find(iterator, MESSAGE_KEY_REALTIME_DESTINATION_NAME)->value->cstring;
      const uint32_t foreground_color = dict_find(iterator, MESSAGE_KEY_REALTIME_FOREGROUND_COLOR)->value->uint32;
      const uint32_t background_color = dict_find(iterator, MESSAGE_KEY_REALTIME_BACKGROUND_COLOR)->value->uint32;
      const uint32_t utc = dict_find(iterator, MESSAGE_KEY_REALTIME_UTC)->value->uint32;

      Realtime *realtime = Realtime_create(
	stop_id,
	line_name,
	destination_name,
	foreground_color,
	background_color,
	utc
      );

      Stop *stop = get_stop(&s_stops_recv, stop_id);

      if ( stop == NULL ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "stop object missing");
	break;
      }

      ds_DynamicArray_push(&stop->realtime, realtime);
      break;
    }
    case VAL_TYPE_REALTIME_END: {

      const uint32_t uuid_run = dict_find(iterator, MESSAGE_KEY_UUID_RUN)->value->uint32;
      const uint32_t uuid_send_realtime = dict_find(iterator, MESSAGE_KEY_UUID_SEND_REALTIME)->value->uint32;

      if ( uuid_run != UUID_RUN || uuid_send_realtime != UUID_SEND_REALTIME ) {
	// this should  never happen
	APP_LOG(APP_LOG_LEVEL_ERROR, "received wrong realtime uuid");
	break;
      }

      // time to update the screen
      ds_DynamicArray_swap(&s_stops, &s_stops_recv);
      clear();
      draw();
      Stops_clear(&s_stops_recv);
      break;
    }
    case VAL_TYPE_STATE: {

      switch(dict_find(iterator, MESSAGE_KEY_STATE)->value->uint32){
	case VAL_STATE_LOADING:{
	  status_bar_layer_set_colors(s_status_bar, BLO, FLO);
	  break;
        }
	case VAL_STATE_LOADED:{
	  status_bar_layer_set_colors(s_status_bar, BOK, FOK);
	  break;
        }
	case VAL_STATE_LOADED_GEOERROR:{
	  status_bar_layer_set_colors(s_status_bar, BNG, FNG);
	  break;
        }
	case VAL_STATE_LOADED_ERROR:{
	  status_bar_layer_set_colors(s_status_bar, BKO, FKO);
	  break;
        }
      }
      break;
    }
  }
}
