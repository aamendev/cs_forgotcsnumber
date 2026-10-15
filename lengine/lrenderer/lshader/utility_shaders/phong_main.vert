lshader_frag_pos = lshader_model_view.model * in_position;
lshader_frag_normal = inverse(transpose(mat3(lshader_model_view.model))) * normalize(lshader_in_normal);
lshader_cam_pos = vec4(
        lshader_model_view.view[0].w,
        lshader_model_view.view[1].w,
        lshader_model_view.view[2].w,
        lshader_model_view.view[3].w,
        )
