var MAX_REQUESTS = 10 ;
var NCLOSEST = 10 ;

function url ( lat , lon ) {
	var n = NCLOSEST ;
	var m = MAX_REQUESTS ;
	return 'https://stib-mivb-api.herokuapp.com/realtime/nclosest/' + n + '/' + lat + '/' + lon + '?max_requests=' + m ;
}

module.exports.url = url ;