#version 120

varying vec3 PosW;

uniform vec3 lbf;
uniform vec3 rtb;

void main (void)  
{  
	vec3 x, xdash;
	vec4 col;
	x = PosW;
	
	xdash = (x - lbf) / (rtb - lbf);
	
	col = vec4(		xdash.x/2.0f + 0.5f,
					xdash.y/2.0f + 0.5f,
					xdash.z/2.0f + 0.5f,
					1.0		);;    
					
	bool isInside = all(greaterThanEqual(xdash,vec3(0,0,0)));
	
	isInside = isInside && all(lessThanEqual(xdash,vec3(1,1,1)));
	
	gl_FragColor = (isInside ? col : vec4(0.3, 0.3, 0.3, 1.0));

}