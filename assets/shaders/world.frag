#version 330 core
in vec3 vWorld;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCamera;
uniform vec3 uLight0Pos;
uniform vec3 uLight0Color;
uniform float uLight0Radius;
uniform float uLight0Intensity;
uniform vec3 uLight1Pos;
uniform vec3 uLight1Color;
uniform float uLight1Radius;
uniform float uLight1Intensity;
uniform vec3 uFlashPos;
uniform vec3 uFlashDir;
uniform float uFlashOn;
uniform float uTime;
uniform int uMaterial;

float hash(vec2 p){
    p=fract(p*vec2(127.1,311.7));p+=dot(p,p+34.5);
    return fract(p.x*p.y);
}
float noise(vec2 p){
    vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
    float a=hash(i),b=hash(i+vec2(1,0)),c=hash(i+vec2(0,1)),d=hash(i+vec2(1,1));
    return mix(mix(a,b,f.x),mix(c,d,f.x),f.y);
}
float fbm(vec2 p){
    float n=0.0,a=.5;
    for(int i=0;i<4;i++){n+=noise(p)*a;p=p*2.03+17.1;a*=.5;}
    return n;
}
vec3 pointLight(vec3 pos,vec3 col,float radius,float intensity,vec3 normal){
    vec3 to=pos-vWorld;float d=length(to);vec3 dir=to/max(d,.001);
    float diffuse=max(dot(normal,dir),0.0);
    float fall=1.0-clamp(d/radius,0.0,1.0);fall*=fall;
    return col*diffuse*fall*intensity;
}
void main(){
    vec3 normal=normalize(cross(dFdx(vWorld),dFdy(vWorld)));
    if(dot(normal,uCamera-vWorld)<0.0)normal=-normal;

    vec3 lighting=vec3(.008,.011,.014);
    lighting+=uColor*pointLight(uLight0Pos,uLight0Color,uLight0Radius,uLight0Intensity,normal);
    lighting+=uColor*pointLight(uLight1Pos,uLight1Color,uLight1Radius,uLight1Intensity,normal);

    vec3 toPixel=vWorld-uFlashPos;
    float dist=length(toPixel);
    vec3 ray=toPixel/max(dist,.001);
    float cone=dot(normalize(uFlashDir),ray);
    float beam=smoothstep(.62,.91,cone);
    float hotspot=smoothstep(.88,.995,cone);
    beam=mix(beam*.38,hotspot,.58);
    float fall=1.0/(1.0+.055*dist+.018*dist*dist);
    float diffuse=max(dot(normal,-ray),0.0);
    lighting+=uColor*beam*fall*diffuse*8.5*uFlashOn;

    float n=fbm(vWorld.xz*.32+vec2(uTime*.006,0));
    float micro=noise(vWorld.xz*5.5);
    if(uMaterial==0){
        float wet=.5+.5*noise(vWorld.xz*1.8);
        lighting*=.52+n*.34;
        lighting*=.78+micro*.28;
        lighting+=vec3(.018,.023,.022)*wet;
    }else if(uMaterial==1){
        lighting*=.72+.28*noise(vWorld.xy*5.0);
        lighting*=.82+.18*sin(vWorld.y*11.0);
    }else if(uMaterial==2){
        float leaves=.62+.38*fbm(vWorld.xz*1.7);
        lighting*=leaves;
        lighting+=vec3(.004,.018,.007)*leaves;
    }else if(uMaterial==3){
        lighting*=.58+.42*fbm(vWorld.xz*2.2);
    }else if(uMaterial==4){
        float grain=sin(vWorld.y*13.0+noise(vWorld.xz*2.0)*4.0);
        lighting*=.72+.28*grain*.5;
    }else if(uMaterial==5){
        float metal=noise(vWorld.xz*8.0);
        lighting*=.5+.5*metal;
    }else if(uMaterial==6){
        float flicker=.94+.045*sin(uTime*2.3)+.02*sin(uTime*7.7);
        lighting=uColor*(1.65+flicker*.75);
    }else if(uMaterial==8){
        lighting*=.045;
    }else if(uMaterial==9){
        float ripple=sin(vWorld.x*5.0+uTime*.7)*sin(vWorld.z*3.0-uTime*.4);
        lighting*=.24+.12*noise(vWorld.xz*2.0);
        lighting+=vec3(.008,.022,.028)*(ripple*.5+.5);
    }

    float d=length(vWorld-uCamera);
    float fogNoise=fbm(vWorld.xz*.08+vec2(0,uTime*.012));
    float fogNear=smoothstep(18.0,46.0,d);
    float groundFog=(1.0-smoothstep(0.3,3.5,vWorld.y))*.28;
    float fog=clamp(fogNear+groundFog*.72+fogNoise*.08,0.0,.94);

    vec3 fogColor=mix(vec3(.008,.014,.020),vec3(.020,.032,.041),smoothstep(0.,1.,fogNoise));
    vec3 color=mix(lighting,fogColor,fog);

    float edge=length(gl_FragCoord.xy/vec2(1280.0,720.0)-vec2(.5));
    color*=1.0-smoothstep(.28,.78,edge)*.42;

    float grain=(hash(gl_FragCoord.xy+uTime*13.0)-.5)*.012;
    color+=grain;

    FragColor=vec4(max(color,vec3(0)),1);
}
