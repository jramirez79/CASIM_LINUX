#extension GL_ARB_texture_rectangle : enable

#define BlendOverlayf(base, blend) 		(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define Blend(base, blend, funcf) 		vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)

uniform sampler2D pattern_tex;		// 0
uniform samplerRect coordinate_tex; // 1
uniform sampler2D wrinkle_tex;		// 2

void main ()
{
	
	float patternColorId = texture2D (pattern_tex, gl_TexCoord[0].st).r;

	// para female_coordinate_tex:
	// s = 0 es el renglon de la tabla de colores
	// s = 1 es el renglon de combinaciones 
	// t = 0 es la columna que da el color de la tabla de colores
	// t = 1 es la columna que da el color de las combinaciones
	// el segundo 1 es el id de la combinacion
	vec4 outfitCoordinate = texture2DRect (coordinate_tex, vec2(1.0, 3.0)) * 255.0; // expanding the range from [0,1] -> [0, 255]

	vec4 colorCoordinate1 = texture2DRect (coordinate_tex, vec2(0.0, outfitCoordinate.r));
	vec4 colorCoordinate2 = texture2DRect (coordinate_tex, vec2(0.0, outfitCoordinate.g));
	vec4 colorCoordinate3 = texture2DRect (coordinate_tex, vec2(0.0, outfitCoordinate.b));

	//vec4 finalColor = texture2DRect (coordinate_tex, vec2(0.0, patternColorId));

	// arrugas de la ropa
	vec4 foldingColor = texture2D (wrinkle_tex, gl_TexCoord[0].st);
	//foldingColor.a = 0.0;
	
	gl_FragColor = vec4(1.0,0.0,1.0,1.0);
/*
	if (patternColorId == 1.0)
	{
		gl_FragColor = colorCoordinate1 ;
		gl_FragColor.a = 1.0;
	}
	if (patternColorId == 2.0)
	{
		gl_FragColor = colorCoordinate2 ;
		gl_FragColor.a = 1.0;
	}
	if (patternColorId == 3.0)
	{
		gl_FragColor = colorCoordinate3 ;
		gl_FragColor.a = 1.0;
	}
*/

	gl_FragColor = vec4(BlendOverlay(gl_FragColor, foldingColor),gl_FragColor.a);
	
	gl_FragColor = vec4(foldingColor.a);
	//gl_FragColor = vec4 (1,0,0,1);
	//gl_FragColor = foldingColor;
}

