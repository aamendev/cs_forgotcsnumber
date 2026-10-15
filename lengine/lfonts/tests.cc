#include "tests.h"
#include "font.h"
namespace lina { namespace font { namespace tests {
    void load_file()
    {
        BEGIN_TEST("load otf");
        font_reader fr;
        fr.open_file("test_otfs/NotoSansMedium.otf");
        END_TEST("load otf");
    }
    void all()
    {
        load_file();
    }
}}}
