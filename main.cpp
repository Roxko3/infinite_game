#include "sfw.h"
#include "test_application.h"

int main()
{
    Application* app = memnew(TestApplication);

    app->start_main_loop();

    memdelete(app);

    return 0;
}
