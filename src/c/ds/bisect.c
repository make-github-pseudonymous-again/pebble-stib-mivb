#include <pebble.h>
#include "bisect.h"

//size_t ds_bisect_left_uint32 ( uint32_t* pt , size_t hi , uint32_t x ) {

//	size_t lo = 0;
	
//  while (lo < hi) {

//    const size_t mid = (lo + hi) >> 1 ;

//    if (x > *(pt + mid)) lo = mid + 1 ;

//    else hi = mid ;

//  }

//  return lo ;

//}

//size_t ds_bisect_right_uint32 ( uint32_t* pt , size_t hi , uint32_t x ) {
	
//	size_t lo = 0;

//  while (lo < hi) {

//    const size_t mid = (lo + hi) >> 1 ;

//    if ( x < *(pt + mid) ) hi = mid ;

//    else lo = mid + 1 ;

//  }

//  return lo ;

//}