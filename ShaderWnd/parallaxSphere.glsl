#version 330

in vec3 fN;
in vec3 fL;
in vec3 fE;
in vec2 texCoord;

out vec4 fColor;

uniform mat4 mVM;

uniform sampler2D bump_map;
uniform sampler2D height_map;

#define scale 0.075
#define delta 0.002
#define curvature (6./3.1415926/3.)
void main()
{
   vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
   vec4 LightPosition;
   float Shininess = 100.;

   AmbientProduct = vec4(0.2, 0.2, 0.2, 1.);
   DiffuseProduct = vec4(0.4, 0.12, 0.15, 1.);
   SpecularProduct = vec4(.2, .2, .2, 1.);

   vec2 displaced_texCoord = texCoord;
   vec3 E = normalize(fE);
   float shift = sqrt(dot(E.xy, E.xy));
   if (shift != 0) {
       vec2 displacement = -E.xy  * delta;
       float depth = 0., depth_delta = E.z * delta;
       float next_depth = ( texture(height_map, displaced_texCoord)[0]) * scale;
       int i ;
       for (i = 0; i < 500; i++) {
           if (depth > next_depth)
               break;
           depth += depth_delta;
           depth_delta -= shift * delta * delta * curvature;
           if (depth < 0)
               discard;
           displaced_texCoord += vec2(displacement.x * (1. / sin(displaced_texCoord.y*3.1415926/6.)), displacement.y);
           next_depth = ( texture(height_map, displaced_texCoord)[0]) * scale;
       }

       float lambda = (depth - next_depth) / (
           ( texture(height_map, 
               displaced_texCoord - vec2(displacement.x *(1./sin(displaced_texCoord.y * 3.1415926 / 6.)), displacement.y))[0])
           * scale - next_depth + depth_delta);
       displaced_texCoord += -lambda * vec2(displacement.x* (1./sin(displaced_texCoord.y * 3.1415926 / 6.)), displacement.y);
   }

 
   vec3 N = normalize( 2.*texture(bump_map, displaced_texCoord).xyz-1.0);
   vec3 L = normalize (fL);

   vec3 H = normalize(L + E);

	// Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;
    float Kd = max(dot(L, N), 0.0);
    vec4  diffuse = Kd*DiffuseProduct;
    float Ks = pow(max(dot(N, H), 0.0), Shininess);
    vec4  specular = Ks * SpecularProduct;
    if(dot(L, N) < 0.0) 
        specular = vec4(0.0, 0.0, 0.0, 1.0);

    fColor = clamp(ambient + diffuse + specular, 0., 1.);
    fColor.a = DiffuseProduct.a;
}