vec3 normal = normalize(lshader_frag_normal);
vec4 illumination = vec4(0, 0, 0, 1);
vec4 diffuse_colour = kd * material.diffuse_col;
illumination += material.ambient_col * ka;
float att = 1.0f;
vec3 light_dir;
float dotted = 0.0f;
for (int i = 0; i < light_count; i++)
{
    switch(lights[i].type)
    {
        case DIRECTIONAL:
            {
                light_dir = normalize(lights[i].dir);
                dotted = dot(lights[i].dir, -normal);
                if (dotted <= 0.0f) 
                {
                    continue;
                }
            }
            break;
        case POINT:
            {
                light_dir = -normalize(lights[i].pos - lshader_frag_pos);
                dotted = dot(light_dir, -normal);
                if (!(dotted > 0)) continue;
                att = min(1.0f, 
                        1.0f/(lights[i].attenuation_func.x + 
                            lights[i].attenuation_func.y *length(lights[i].pos - lshader_frag_pos)  
                            + lights[i].attenuation_func.z * 
                            dot(lights[i].pos - lshader_frag_pos, lights[i].pos-lshader_frag_pos)));
            }
            break;
        case SPOT:
            {
                light_dir = normalize(-lights[i].pos + (lshader_frag_pos));
                dotted = dot(light_dir, -normal);
                if (!(dotted > 0)) continue;

                if (acos(dot(light_dir, normalize(lights[i].dir)))
                        > lights[i].angle)
                {
                    continue; // out of spot region.
                }
                else 
                {
                    float falloff = 1.0f;
                    float curr_theta = acos(dot(light_dir, normalize(lights[i].dir)));
                    float umbra = (lights[i].angle - lights[i].penumbra);
                    float temp = (curr_theta - umbra)/lights[i].penumbra;
                    if (curr_theta > umbra)
                    {
                        falloff = (-2.0f * temp * temp * temp + 3.0f * temp * temp);
                        falloff = 1.0f - falloff;
                    }
                    att = falloff * 
                        min(1.0f, 
                                1.0f/(lights[i].attenuation_func.x + 
                                    lights[i].attenuation_func.y *length(lights[i].pos - lshader_frag_pos)  
                                    + lights[i].attenuation_func.z * 
                                    dot(lights[i].pos - lshader_frag_pos, lights[i].pos-lshader_frag_pos)));
                }
            }
            break;
    }

    dotted = clamp(dotted, 0.0f, 1.0f);
    illumination.xyz += (lights[i].color * diffuse_colour.xyz) * (dotted * att);

    if (material.shin > 0)
    {
        vec3 reflected_dir = normalize(light_dir - normal * (2 * dot(light_dir, normal)));
        dotted = dot(reflected_dir, normalize(lshader_cam_pos - lshader_frag_pos));
        if (dotted > 0)
        {
            dotted = clamp(dotted, 0.0f, 1.0f);
            illumination.xyz += (lights[i].color * material.specular_col.xyz) 
                * (att * max(0.0f, (ks * pow(dotted, material.shin))));
        }
    }
}
illumination.x = clamp(illumination.x, 0.0f, 1.0f);
illumination.y = clamp(illumination.y, 0.0f, 1.0f);
illumination.z = clamp(illumination.z, 0.0f, 1.0f);
frag_col = vec4(illumination.xyz, 1.0);
