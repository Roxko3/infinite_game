#include "test_application.h"

TestApplication::TestApplication() {
    Renderer::initialize();
    GUI::initialize();
    scene = Ref<Scene>(memnew(TestScene));
}
TestApplication::~TestApplication() {
    Renderer::destroy();
    GUI::destroy();
}
