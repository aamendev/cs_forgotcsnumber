#include "font.h"
#include "layers/simple_layer.h"
#include "layers/ui_layer.h"
#include "lengine/src/editor/editor.h"
#include "glfw_vulkan_window.h"
#include "src/tests/tests.h"
#define DBG (0)
#include "lrenderer/renderer.h"
void run_engine()
{
    lina::renderer::lrenderer<GraphicsBackend::Vulkan>* rend = new lina::renderer::lrenderer<GraphicsBackend::Vulkan>;
    lina::engine::editor e(rend);

    f32 w = 0.5;
    f32 h = 0.5;
    f64 currx = 0.;

    lina::layers::simple* sl = new lina::layers::simple(
            rend, 
            currx, 0,
            1-currx, 1.);
    e.add_layer(sl);

    lina::layers::ui* ul = new lina::layers::ui(rend, 
            0., 0., 
            currx += 0.25, 1.
            );
    e.add_layer(ul);


    using wsettings = typename lina::renderer::backend::window<GraphicsBackend::Vulkan>::settings;
    wsettings s;
    s.name = "lina engine";
    s.width = 600;
    s.height = 600;
    s.resizable = true;
    s.events = {};
    s.resized = false;
    auto window = new lina::renderer::backend::glfw_vulkan_window;
    window->create(s);
    e.init(window);
    e.run();
    /*
    lina::font::font_reader fr;
    fr.open_file("fonts/NotoSansMedium.otf");
    std::vector<u64> cps;
    for (int i = 0; i < 9; i++)
    {
        cps.push_back(i + '0');
    }
    fr.add_characters(cps);
    fr.write_files("./fonts", "test_font");
    */
}
int main()
{
#if DBG
       std::cerr << "beginning engine\n";
       lina::tests::run_graphics();
       std::cerr << "ending engine\n";
#else
    run_engine();
#endif

}
