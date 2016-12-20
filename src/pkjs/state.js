function State ( dflt ) {
	this.dflt = dflt ;
	this.data = dflt;
}

State.prototype.freeze = function ( ) {
	console.log('freeze');
	localStorage.setItem('state', this.data);
} ;

State.prototype.thaw = function ( ) {
	console.log('thaw');
	this.data = localStorage.getItem('state') || this.dflt ;
} ;

function create ( dflt ) {
	return new State(dflt) ;
}

module.exports.create = create ;