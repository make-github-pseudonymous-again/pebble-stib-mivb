#include <pebble.h>
#include "translate_error.h"

// https://stackoverflow.com/a/21172222/1582182
char *pebble_translate_app_message_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

char *pebble_translate_status_error(status_t status) {
  switch (status) { 
    case S_SUCCESS: return "S_SUCCESS";
    case E_ERROR: return "E_ERROR";
    case E_UNKNOWN: return "E_UNKNOWN";
    case E_INTERNAL: return "E_INTERNAL";
    case E_INVALID_ARGUMENT: return "E_INVALID_ARGUMENT";
    case E_OUT_OF_MEMORY: return "E_OUT_OF_MEMORY";
    case E_OUT_OF_STORAGE: return "E_OUT_OF_STORAGE";
    case E_OUT_OF_RESOURCES: return "E_OUT_OF_RESOURCES";
    case E_RANGE: return "E_RANGE";
    case E_DOES_NOT_EXIST: return "E_DOES_NOT_EXIST";
    case E_INVALID_OPERATION: return "E_INVALID_OPERATION";
    case E_BUSY: return "E_BUSY";
    case E_AGAIN: return "E_AGAIN";
    case S_TRUE: return "S_TRUE";
    // case S_FALSE: return "S_FALSE"; // apparently both S_SUCCESS and S_FALSE equal 0
    case S_NO_MORE_ITEMS: return "S_NO_MORE_ITEMS";
    case S_NO_ACTION_REQUIRED: return "S_NO_ACTION_REQUIRED";
    default: return "UNKNOWN ERROR";
  }
}