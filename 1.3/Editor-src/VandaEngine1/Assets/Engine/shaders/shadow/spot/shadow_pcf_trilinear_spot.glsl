//Copyright (C) 2014 Ehsan Kamrani
//This file is licensed and distributed under MIT license
[vert]

#version 130
varying vec3 lightDir;
varying vec3 spotDir;
varying vec4 ambient;
varying vec4 diffuse;
varying float dist;
varying vec3 viewDir;
varying vec3 normal;
//varying vec4 position;
//varying vec3 GNormal;
uniform mat4 camera_inverse_matrix;
uniform float focalDistance, focalRange;
uniform bool enableNormalMap;
uniform bool enableShadowMap;
uniform bool enableDirtMap;
varying vec4 vPos;
void main()
{
	mat4 modelMatrix = camera_inverse_matrix * gl_ModelViewMatrix;
	mat3 modelMatrix3x3 = mat3(modelMatrix); 
	
    gl_TexCoord[0] = gl_MultiTexCoord0;
    if( enableShadowMap || enableDirtMap )
		gl_TexCoord[1] = gl_MultiTexCoord3; //second uv set

    vec3 vertexPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    vPos = gl_ModelViewMatrix * gl_Vertex;

   	if( !enableNormalMap ) 
		normal = vec3( normalize( gl_NormalMatrix * gl_Normal ) );
		
	//GNormal = normalize( modelMatrix3x3 * gl_Normal );
	
    vec3 n, t, b;
   
    viewDir = -vertexPos;
  
	mat3 tbnMatrix;
    if( enableNormalMap )
    {
		n = normalize(gl_NormalMatrix * gl_Normal);
		t = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);
		b = normalize(gl_NormalMatrix * gl_MultiTexCoord2.xyz);
		
	    tbnMatrix = mat3(t.x, b.x, n.x,
                         t.y, b.y, n.y,
                         t.z, b.z, n.z);
                          
   		viewDir = tbnMatrix * viewDir;
    }
 	vec3 tempVec;
	tempVec = (gl_LightSource[0].position.xyz - vPos.xyz);
    dist = length(tempVec );
	if( enableNormalMap )
	{
		lightDir = normalize( tbnMatrix * tempVec );	
		spotDir = normalize( tbnMatrix * gl_LightSource[0].spotDirection );
	}
	else
	{
		lightDir = normalize( tempVec );
		spotDir = normalize( gl_LightSource[0].spotDirection );
	}
			
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    // fix of the clipping bug for both Nvidia and ATi
    #ifdef __GLSL_CG_DATA_TYPES
    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
    #endif

    gl_Position = gl_ProjectionMatrix * vPos;
    //position = modelMatrix * gl_Vertex;
}

[frag]

#version 130
#extension GL_EXT_texture_array : enable
varying vec3 lightDir;
varying vec3 spotDir;
varying float dist;
uniform float light_radius;
varying vec4 ambient;
varying vec4 diffuse;
varying vec3 viewDir;
varying vec3 normal;  //It's just used for per pixel lighting without normal map
//varying vec4 position;
//varying vec3 GNormal;
uniform bool enableParallaxMap;
uniform bool enableAlphaMap;
uniform bool readAlphaFromDiffuse;
uniform bool enableColorMap;
uniform bool enableNormalMap;
uniform bool enableShadowMap;
uniform bool enableDirtMap;
uniform bool enableGlossMap;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D alphaMap;
uniform sampler2D heightMap;
uniform sampler2D glossMap;
uniform sampler2D shadowMap;
uniform sampler2D dirtMap;

uniform bool firstPass;

uniform float parallaxMapScale;
uniform float parallaxMapBias;


out vec4 myVec40;
//out vec4 myVec41;
//out vec4 myVec42;
//out vec4 myVec43;
//out vec4 myVec44;
//out vec4 myVec45;
//out vec4 myVec46;
//out vec4 myVec47;
//out vec4 myVec48;
//shadow
uniform vec4 far_d;
uniform vec2 texSize; // x - size, y - 1/size
varying vec4 vPos;
uniform bool character_shadow;
uniform sampler2DArrayShadow stex;
uniform float shadow_intensity;
float shadowCoef()
{
	int index;
	float blend = 0.0;
	
	if(gl_FragCoord.z < far_d.x) {
		index = 0;
		blend = clamp( (gl_FragCoord.z - far_d.x * 0.995) * 200.0, 0.0, 1.0); 
    }
	else if(gl_FragCoord.z < far_d.y) {
		index = 1;
		blend = clamp( (gl_FragCoord.z - far_d.y * 0.995) * 200.0, 0.0, 1.0); 
	}
	else if(gl_FragCoord.z < far_d.z) {
		index = 2;
		blend = clamp( (gl_FragCoord.z - far_d.z * 0.995) * 200.0, 0.0, 1.0); 
	}
	else if( gl_FragCoord.z < far_d.w )
	{
		index = 3;
		blend = clamp( (gl_FragCoord.z - far_d.w * 0.995) * 200.0, 0.0, 1.0);
	}
	else
	{
		return 1.0;
	}
	
	// transform this fragment's position from view space to scaled light clip space
	// such that the xy coordinates are in [0;1]
	// note there is no need to divide by w for othogonal light sources
	vec4 shadow_coord = gl_TextureMatrix[index]*vPos;
	

	shadow_coord.w = shadow_coord.z;
	
	// tell glsl in which layer to do the look up
	shadow_coord.z = float(index);
	
	// get the shadow contribution
	float ret = shadow2DArray(stex, shadow_coord).x;
	
	if (blend > 0.0) {
	    shadow_coord = gl_TextureMatrix[index+1]*vPos;
	
	    shadow_coord.w = shadow_coord.z;
	    shadow_coord.z = float(index+1);
    	
	    ret = ret*(1.0-blend) + shadow2DArray(stex, shadow_coord).x*blend; 
	}
	 
	
	return ret;
}

void main()
{
	vec4 alphaColor;
	if( enableAlphaMap )
	{
	    alphaColor = texture2D(alphaMap, gl_TexCoord[0].xy);
	
	    if(readAlphaFromDiffuse && alphaColor.a<0.1)
	    	discard;
	    else if( alphaColor.r<0.1)
			discard;
	}

  	vec3 halfV,viewV,ldir, sdir, ldirForDynamicShadow;
	float NdotL;

	vec4 ambientGlobal, specular;	
  	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	vec4 color = ambientGlobal;
	float att, spotEffect;

	viewV = normalize( viewDir );

	vec2 newTexCoord;

	if( enableParallaxMap )
	{
		//parallax mapping
		float height = texture2D(normalMap, gl_TexCoord[0].st).a;
        
		height = height * parallaxMapScale + parallaxMapBias;
		newTexCoord = gl_TexCoord[0].st + (height * viewV.xy) * texture2D(normalMap, gl_TexCoord[0].st ).z;
	}
	else 
		newTexCoord = gl_TexCoord[0].st;
		
	vec3 n, dynamicShadowN;
	vec4 normalColor;
	if( enableNormalMap )
	{
		normalColor = texture2D(normalMap, newTexCoord );
        n = normalize(texture2D(normalMap, newTexCoord ).rgb * 2.0 - 1.0);
    }
	else
		n = normalize( normal );
	dynamicShadowN = normalize( normal );	
	    	 
	vec4 textureColor;
	vec4 shadowColor = vec4(0.0, 0.0, 0.0, 0.0 );
	vec4 dirtColor;
	if( enableColorMap )
	{
  		textureColor = texture2D(colorMap, newTexCoord );
  	}
	if( enableShadowMap )
	{
  		shadowColor = texture2D(shadowMap, gl_TexCoord[1].st );
  	}

	if( enableDirtMap )
	{
  		dirtColor = texture2D(dirtMap, gl_TexCoord[1].st );
  	}  	
	vec4 glossColor;
	
  	//att = 1.0 / (gl_LightSource[0].constantAttenuation +
	//		gl_LightSource[0].linearAttenuation * dist +
	//		gl_LightSource[0].quadraticAttenuation * dist * dist);

	ldir = normalize(lightDir); 
	sdir = normalize(spotDir);
	
	NdotL = max(dot(n,ldir),0.0);
	if( NdotL > 0.0 )
	{
		   spotEffect = dot(sdir, -ldir);
           if (spotEffect > gl_LightSource[0].spotCosCutoff)
           {
            spotEffect = pow(spotEffect, gl_LightSource[0].spotExponent);
            float tempAtt = ( 1 - ( dist / light_radius ) ) / gl_LightSource[0].constantAttenuation;
            att = spotEffect * tempAtt;
            color += att * (diffuse * NdotL + ambient);	
			
		 	specular = gl_FrontMaterial.specular * gl_LightSource[0].specular;
	
			float l_specular = pow(clamp(dot(reflect(-ldir, n), viewV), 0.0, 1.0), 
					 gl_FrontMaterial.shininess );
	
			if( enableGlossMap )
			{
	 			glossColor = texture2D(glossMap, newTexCoord );
				color += att * specular * l_specular * glossColor;
			}
			else
				color += att * specular * l_specular;
		  }
	}
	
	if( enableColorMap )
	{
		color *= textureColor;
	}

	if( enableDirtMap )
	{
		color *= dirtColor;
	}
	if( enableShadowMap )
	{
		color *= shadowColor;
	}
	if( firstPass )
	{
		float shadow_coef = shadowCoef();
		if( character_shadow )
		{
			shadow_coef *= NdotL;
		}
		shadow_coef +=  1.0 - shadow_intensity;
	
		shadow_coef = clamp(shadow_coef, 0.0, 1.0);
		color *= shadow_coef;
	}	
		//vec4 pos = normalize( position );
		//myVec41 = vec4(pos.xyz,0);
		//vec3 norm = normalize( GNormal );
		//myVec42 = vec4( norm.xyz,0 );
		//myVec43 =  textureColor;
		//myVec44 =  normalColor;
		//myVec45 =  glossColor;
		//myVec46 =  shadowColor;
		//myVec47 =  dirtColor;
		//myVec48 =  alphaColor;
	//}
	
	myVec40 = vec4(color.rgb, 0.0);
}


