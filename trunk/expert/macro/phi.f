
	function phi(x)
	real x
	common/pawpar/par(2)
	phi = par(1)/100*sin(-x+par(2)/180*3.141592)
*	phi = par(1)*sin(-x+par(2))
	end
