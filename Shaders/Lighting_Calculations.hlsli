#pragma pack_matrix( row_ major )

#define MAXLIGHTS 100
#define AMBIENT 0
#define DIRECTIONAL 1
#define POINT 2
#define SPOT 3
#define SPECPOWER 64.0f

// structure used for lighting
struct LIGHT
{	
	// light's position
	// light type stored in the w (AMBIENT, DIRECTIONAL, SPOT, POINT)
	// spot light, point light = light's position, directional light = nothing
	float4 position;
	// light's color
	float4 color;
	// light's direction
	// spot light = cone direction, directional light = direction of light, point = nothing
	float4 direction;
	// light's ratio
	// spot light = x(inner), y(outer), z(radius), point light = z(radius), directional = nothing
	float4 cone_ratio;
};

float3 CalculateDirectionalLight(LIGHT light, float3 surface_normal)
{
	// calculate the light's ratio on the surface
	float dir_light_ratio = clamp(dot(-normalize(light.direction.xyz), surface_normal), 0.0f, 1.0f);
	// return the light's color by the light's ratio on the surface
	return dir_light_ratio * light.color.xyz;
};

float3 CalculatePointLight(LIGHT light, float3 surface_position, float3 surface_normal)
{
	// create a vector to the point light
	float3 to_light_vector = light.position.xyz - surface_position;
	// calculate the light's ratio on the surface
	float point_light_ratio = dot(normalize(to_light_vector), surface_normal);
	// multiply the light's color by the light's ratio on the surface
	float3 color = point_light_ratio * light.color.xyz;
	// calculate attenuation
	float attenuation = 1.0f - saturate(length(light.position.xyz - surface_position) / light.cone_ratio.z);
	// return the final color attenuated
	return color;// *attenuation;
};

float3 CalculateSpotLight(LIGHT light, float3 surface_position, float3 surface_normal)
{
	// create a vector to the spot light
    float3 to_light_vector = normalize(light.position.xyz - surface_position);
    // calculate the surface ratio
    float surface_ratio = saturate(dot(-to_light_vector, light.direction.xyz));
    // find the spot factor
    float spot_factor = (surface_ratio > light.cone_ratio.y) ? 1.0f : 0.0f;
	// calculate the spot light's ratio on the surface
	float spot_light_ratio = saturate(dot(to_light_vector, surface_normal));
	// multiply the light's color by the light's ratio on the surface
    float3 color = spot_factor * spot_light_ratio * light.color.xyz;
	// return the final color attenuated
	return color;
};

float CalculateSpecular(float3 to_light, float3 camera_position, float3 surface_position, float3 surface_normal, float spec_power, float spec_intensity)
{
    // Create a vector to the camera from the surface
    float3 toCamera = normalize(camera_position - surface_position);
    // Create a reflection vector from the light direction and the surface normal
    float3 reflectVector = normalize(reflect(-to_light, surface_normal));
    // Find the angle between the vector to the camera and the reflection vector
    float dot_result = saturate(dot(toCamera, reflectVector));
    // Raise the dot product result to a certain power
    float intensity = pow(dot_result, spec_power);
    // Find the light's ratio on the surface
    float light_ratio = saturate(dot(to_light, surface_normal));
    // Return the specular intensity and the intensity of the specular highlight
    return saturate(intensity * spec_intensity * light_ratio);
}

float CalculatePointAttenuation(LIGHT light, float3 surface_position)
{
	// calculate attenuation
	float attenuation = 1.0f - saturate(length(light.position.xyz - surface_position) / light.cone_ratio.z);
	// return the attenuation
	return attenuation;
}

float CalculateSpotAttenuation(LIGHT light, float3 surface_position)
{
	// create a vector to the spot light
	float3 to_light_vector = light.position.xyz - surface_position;
	// calculate the light's ratio on the surface
	float surface_ratio = saturate(dot(-normalize(to_light_vector), normalize(light.direction.xyz)));
	// calculate attenuation
	float attenuation = 1.0f - saturate(length(light.position.xyz - surface_position) / light.cone_ratio.z);
    attenuation *= (1.0f - saturate((light.cone_ratio.x - surface_ratio) / (light.cone_ratio.x - light.cone_ratio.y)));
	// return the attenuation
	return attenuation;
}

float3 AddDirectionalLight(LIGHT light, float3 surface_pos, float3 normal, float3 camera_pos, float spec_intensity)
{
    // temporary variables for calculations
    float3 add_me = float3(0.0f, 0.0f, 0.0f);
    float3 spec = float3(0.0f, 0.0f, 0.0f);
    // add the directional light calculation
    add_me.xyz += CalculateDirectionalLight(light, normal.xyz);
    // store the specular color
    spec.xyz = light.color.xyz;
    // multiply the specular scalar
    spec.xyz *= CalculateSpecular(
                    -light.direction.xyz,
                    camera_pos.xyz,
                    surface_pos.xyz,
                    normal.xyz,
                    SPECPOWER,
                    spec_intensity);
    // add the specular result
    add_me.xyz += spec.xyz;
    // return the result
    return saturate(add_me);
}

float3 AddPointLight(LIGHT light, float3 surface_pos, float3 normal, float3 camera_pos, float spec_intensity)
{
    // temporary variables for calculations
    float3 add_me = float3(0.0f, 0.0f, 0.0f);
    float3 spec = float3(0.0f, 0.0f, 0.0f);
    
    // add the point light calculation
    add_me.xyz += CalculatePointLight(light, surface_pos, normal.xyz);
    // store the specular color
    spec.xyz = light.color.xyz;
    // multiply the specular scalar
    spec.xyz *= CalculateSpecular(
                    normalize(float3(light.position.xyz - surface_pos.xyz)),
                    camera_pos.xyz,
                    surface_pos.xyz,
                    normal.xyz,
                    SPECPOWER,
                    spec_intensity);
    // add the specular result
    add_me.xyz += spec.xyz;
    // attenuate the point light
    add_me.xyz *= CalculatePointAttenuation(light, surface_pos.xyz);
    // return the result
    return saturate(add_me);
}

float3 AddSpotLight(LIGHT light, float3 surface_pos, float3 normal, float3 camera_pos, float spec_intensity)
{
    // temporary variables for calculations
    float3 add_me = float3(0.0f, 0.0f, 0.0f);
    float3 spec = float3(0.0f, 0.0f, 0.0f);

    // add the spot light calculation
    add_me.xyz += CalculateSpotLight(light, surface_pos.xyz, normal.xyz);
    // store the specular color
    spec.xyz = light.color.xyz;
    // multiply the specular scalar
    spec.xyz *= CalculateSpecular(
                    normalize(float3(light.position.xyz - surface_pos.xyz)),
                    camera_pos.xyz,
                    surface_pos.xyz,
                    normal.xyz,
                    SPECPOWER,
                    spec_intensity);
    // add the specular result
    add_me.xyz += spec.xyz;
    // attenuate the spot light
    add_me.xyz *= CalculateSpotAttenuation(light, surface_pos.xyz);
    // return the result
   return saturate(add_me);
}