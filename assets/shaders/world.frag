#version 330 core
in vec3 vWorld;
in vec2 vUV;
out vec4 FragColor;
uniform vec3 uColor,uCamera,uLight0Pos,uLight0Color,uLight1Pos,uLight1Color,uLight2Pos,uLight2Color,uFlashPos,uFlashDir;
uniform float uLight0Radius,uLight0Intensity,uLight1Radius,uLight1Intensity,uLight2Radius,uLight2Intensity,uFlashOn,uTime;
uniform int uMaterial,uUseTexture,uAlphaCutout;
uniform sampler2D uTexture;
float hash(vec2 p){p=fract(p*vec2(127.1,311.7));p+=dot(p,p+34.5);return fract(p.x*p.y);}
float noise(vec2 p){vec2 i=floor(p),f=fract(p);f=f*f*(3.-2.*f);float a=hash(i),b=hash(i+vec2(1,0)),c=hash(i+vec2(0,1)),d=hash(i+vec2(1,1));return mix(mix(a,b,f.x),mix(c,d,f.x),f.y);}
float fbm(vec2 p){float n=0.,a=.5;for(int i=0;i<5;i++){n+=noise(p)*a;p=p*2.02+17.1;a*=.5;}return n;}
vec3 pointLight(vec3 pos,vec3 col,float radius,float intensity,vec3 normal){vec3 to=pos-vWorld;float d=length(to),fall=1.-clamp(d/radius,0.,1.);return col*max(dot(normal,to/max(d,.001)),0.)*fall*fall*intensity;}
void main(){
 vec4 tex=uUseTexture==1?texture(uTexture,vUV):vec4(1.);
 if(uAlphaCutout==1&&tex.a<.35)discard;
 vec3 albedo=tex.rgb*uColor;
 vec3 normal=normalize(cross(dFdx(vWorld),dFdy(vWorld)));if(dot(normal,uCamera-vWorld)<0.)normal=-normal;
 vec3 lighting=vec3(.006,.009,.012);
 lighting+=pointLight(uLight0Pos,uLight0Color,uLight0Radius,uLight0Intensity,normal);
 lighting+=pointLight(uLight1Pos,uLight1Color,uLight1Radius,uLight1Intensity,normal);
 lighting+=pointLight(uLight2Pos,uLight2Color,uLight2Radius,uLight2Intensity,normal);
 vec3 toPixel=vWorld-uFlashPos;float dist=length(toPixel),cone=dot(normalize(uFlashDir),toPixel/max(dist,.001));
 float beam=smoothstep(.70,.94,cone)*(1.-smoothstep(8.,30.,dist));
 float fall=1./(1.+.075*dist+.028*dist*dist);
 lighting+=vec3(1.0,.94,.78)*beam*fall*max(dot(normal,-normalize(toPixel)),0.)*5.0*uFlashOn;
 float n=fbm(vWorld.xz*.16+vec2(uTime*.004,0));
 if(uMaterial==0){lighting*=.50+n*.28;lighting*=.80+.20*noise(vWorld.xz*5.);}
 else if(uMaterial==2){lighting*=.64+.36*fbm(vWorld.xz*1.5);lighting+=vec3(.003,.012,.004);}
 else if(uMaterial==3){lighting*=.55+.45*fbm(vWorld.xz*2.1);}
 else if(uMaterial==4){lighting*=.72+.28*sin(vWorld.y*12.+noise(vWorld.xz*2.)*3.)*.5;}
 else if(uMaterial==8){lighting*=.025;}
 vec3 color=albedo*lighting;
 float d=length(vWorld-uCamera);
 float fog=clamp(smoothstep(16.,44.,d)+(1.-smoothstep(.5,4.,vWorld.y))*.20+fbm(vWorld.xz*.06+vec2(0,uTime*.008))*.055,0.,.88);
 color=mix(color,vec3(.007,.012,.017),fog);
 color=color/(color+vec3(.7));color=pow(max(color,vec3(0)),vec3(.92));
 float edge=length(gl_FragCoord.xy/vec2(1280.,720.)-vec2(.5));color*=1.-smoothstep(.26,.78,edge)*.48;
 color+=(hash(gl_FragCoord.xy+uTime*13.)-.5)*.008;
 FragColor=vec4(color,tex.a);
}
