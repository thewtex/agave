from agave_pyclient import AgaveRenderer

# imgplot = plt.imshow(numpy.zeros((1024, 768)))
if __name__ == "__main__":
    filename = (
        "https://animatedcell-test-data.s3.us-west-2.amazonaws.com/variance/1.zarr"
    )
    r = AgaveRenderer()
    r.load_data(filename, 0, 0, 0)
    r.set_resolution(512, 512)
    r.set_voxel_scale(1.0, -1.0, 2.9)
    r.render_iterations(128)
    r.set_clip_region(0, 1, 0, 1, 0, 1)
    r.camera_projection(1, 55)
    r.frame_scene()
    r.exposure(0.8714)
    r.density(100)
    r.aperture(0)
    r.focaldist(0.75)
    r.enable_channel(0, 0)
    r.mat_diffuse(0, 1, 0, 1, 1.0)
    r.mat_specular(0, 0, 0, 0, 0.0)
    r.mat_emissive(0, 0, 0, 0, 0.0)
    r.mat_glossiness(0, 0)
    r.set_window_level(0, 1, 0.758)
    r.enable_channel(1, 1),
    r.mat_diffuse(1, 1, 1, 1, 1.0)
    r.mat_specular(1, 0, 0, 0, 0.0)
    r.mat_emissive(1, 0, 0, 0, 0.0)
    r.mat_glossiness(1, 0)
    r.set_window_level(1, 1, 0.811)
    r.enable_channel(2, 1)
    r.mat_diffuse(2, 0, 1, 1, 1.0)
    r.mat_specular(2, 0, 0, 0, 0.0)
    r.mat_emissive(2, 0, 0, 0, 0.0)
    r.mat_glossiness(2, 0)
    r.set_window_level(2, 0.9922, 0.7704)
    r.skylight_top_color(0.5, 0.5, 0.5)
    r.skylight_middle_color(0.5, 0.5, 0.5)
    r.skylight_bottom_color(0.5, 0.5, 0.5)
    r.light_pos(0, 10.1663, 1.1607, 0.5324)
    r.light_color(0, 122.926, 122.926, 125.999)
    r.light_size(0, 1, 1)
    r.session("out.png")
    r.redraw()
    # r.batch_render_rocker(number_of_frames=40, angle=30, output_name="rocker")
    # r.batch_render_turntable(number_of_frames=90, output_name="turntable")
