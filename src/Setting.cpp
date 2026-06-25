#include <Setting.hpp>


namespace Settings {

Setting::Setting(Manager* manager, const char* name)
    : storage(manager->getStorage())
    , name(name) {
#ifdef SETTING_HAS_FREERTOS
    mutex = xSemaphoreCreateMutex();
#endif
    manager->registerMember(this);
}

Setting::~Setting() {
#ifdef SETTING_HAS_FREERTOS
    if (mutex != nullptr)
        vSemaphoreDelete(mutex);
#endif
}

}