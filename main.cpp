#include "WebUI.hpp"

int main(int argc, char const *argv[])
{

    srand((unsigned)time(NULL));
    WebUI w;
    w.init();
    w.start();
    return 0;
}
