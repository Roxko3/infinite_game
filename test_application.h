#ifndef TEST_APPLICATION_H
#define TEST_APPLICATION_H

#include "sfw.h"
#include "test_scene.h"

class TestApplication : public Application {
    SFW_OBJECT(TestApplication, Application);

public:
    TestApplication();
    ~TestApplication();
};

#endif // TEST_APPLICATION_H
