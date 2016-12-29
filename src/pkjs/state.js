function State ( dflt ) {
	this.dflt = dflt ;
	this.data = dflt;
}

State.prototype.freeze = function ( ) {
	console.log('freeze', JSON.stringify(this.data));
	localStorage.setItem('state', this.data);
} ;

State.prototype.thaw = function ( ) {
	//this.data = localStorage.getItem('state') || this.dflt ;
	//console.log('thaw', JSON.stringify(this.data));
} ;

function create ( dflt ) {
	return new State(dflt) ;
}

module.exports.create = create ;