#version 330 core
in vec3 vWorld;in vec3 vNormal;in vec3 vTangent;in vec2 vUV;
out vec4 FragColor;
uniform vec3 uColor,uCamera,uLight0Pos,uLight0Color,uLight1Pos,uLight1Color,uLight2Pos,uLight2Color,uFlashPos,uFlashDir;
uniform float uLight0Radius,uLight0Intensity,uLight1Radius,uLight1Intensity,uLight2Radius,uLight2Intensity,uFlashOn,uTime;
uniform int uMaterial,uUseTexture,uUseNormal,uUseRoughness;
uniform sampler2D uAlbedo,uNormalMap,uRoughnessMap;
float hash(vec2 p){p=fract(p*vec2(127.1,311.7));p+=dot(p,p+34.5);return fract(p.x*p.y);}
float noise(vec2 p){vec2 i=floor(p),f=fract(p);f=f*f*(3.-2.*f);return mix(mix(hash(i),hash(i+vec2(1,0)),f.x),mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)),f.x),f.y);}
float fbm(vec2 p){float n=0.,a=.5;for(int i=0;i<5;i++){n+=noise(p)*a;p=p*2.02+17.1;a*=.5;}return n;}
vec3 light(vec3 pos,vec3 col,float radius,float intensity,vec3 n){vec3 d=pos-vWorld;float l=length(d);float f=1.-clamp(l/radius,0.,1.);return col*max(dot(n,d/max(l,.001)),0.)*f*f*intensity;}
void main(){
 vec4 tex=uUseTexture==1?texture(uAlbedo,vUV):vec4(1);
 if(tex.a<.28&&uMaterial==2)discard;
 vec3 albedo=tex.rgb*uColor;
 vec3 N=normalize(vNormal);
 if(uUseNormal==1){
  vec3 T=normalize(vTangent-N*dot(N,vTangent));vec3 B=normalize(cross(N,T));vec3 nm=texture(uNormalMap,vUV).xyz*2.-1.;N=normalize(mat3(T,B,N)*nm);
 }
 float rough=uUseRoughness==1?texture(uRoughnessMap,vUV).r:.72;
 vec3 lighting=vec3(.004,.006,.009);
 lighting+=light(uLight0Pos,uLight0Color,uLight0Radius,uLight0Intensity,N);
 lighting+=light(uLight1Pos,uLight1Color,uLight1Radius,uLight1Intensity,N);
 lighting+=light(uLight2Pos,uLight2Color,uLight2Radius,uLight2Intensity,N);
 vec3 fp=uFlashPos-vWorld;float fd=length(fp);vec3 fdir=fp/max(fd,.001);
 float cone=smoothstep(.70,.96,dot(normalize(uFlashDir),-fdir));float ff=1./(1.+.10*fd+.035*fd*fd);
 lighting+=vec3(1.0,.93,.78)*cone*ff*max(dot(N,fdir),0.)*4.4*uFlashOn;
 float ambient=.58+.42*fbm(vWorld.xz*.16+vec2(uTime*.003,0));
 lighting*=ambient;
 float spec=pow(max(dot(reflect(-normalize(uCamera-vWorld),N),fdir),0.),mix(64.,8.,rough))*(1.-rough)*.22;
 vec3 color=albedo*(lighting+spec);
 float d=length(vWorld-uCamera);float fog=clamp(smoothstep(17.,48.,d)+(1.-smoothstep(.5,4.,vWorld.y))*.17+fbm(vWorld.xz*.055+vec2(0,uTime*.006))*.05,0.,.88);
 color=mix(color,vec3(.006,.011,.016),fog);
 color=color/(color+vec3(.75));color=pow(max(color,vec3(0)),vec3(.92));
 float edge=length(gl_FragCoord.xy/vec2(1280.,720.)-vec2(.5));color*=1.-smoothstep(.28,.78,edge)*.42;
 color+=(hash(gl_FragCoord.xy+uTime*13.)-.5)*.006;
 FragColor=vec4(color,tex.a);
}