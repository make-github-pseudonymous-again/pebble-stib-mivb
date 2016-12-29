function State ( dflt ) {
	this.dflt = dflt ;
	this.data = dflt;
}

State.prototype.freeze = function ( ) {
	console.log('freeze');
  var raw = JSON.stringify(this.data);
	localStorage.setItem('state', raw);
} ;

State.prototype.thaw = function ( ) {
  try{
    var raw = localStorage.getItem('state');
    this.data = JSON.parse(raw);
  }
  catch(e){
    this.data = this.dflt;
  }
	console.log('thaw');
} ;

function create ( dflt ) {
	return new State(dflt) ;
}

module.exports.create = create ;