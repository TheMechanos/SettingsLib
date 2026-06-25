#pragma once

#include <Setting.hpp>

namespace Settings {

class Bool : public Setting {
public:
    Bool(Manager* manager, const char* name, bool def)
        : Setting(manager, name)
        , value(def)
        , def(def) { }

    ~Bool() { }

    bool get() {
        lock();
        bool v = value;
        unlock();
        return v;
    }

    void set(bool value) {
        lock();
        this->value = value;
        unlock();
    }

    bool getDef() {
        lock();
        bool v = def;
        unlock();
        return v;
    };


    operator bool() { return get(); }

    Bool& operator=(int32_t v) {
        set(v);
        return *this;
    }

private:
    bool value;
    bool def;


    virtual bool fromJsonInternal(JsonVariant& v) override {
        if (v.is<bool>()) {
            set(v.as<bool>());
            return true;
        }
        return false;
    }
    virtual void toJsonInternal(JsonVariant& v) override { v.set(get()); }


    virtual size_t printInTableInternal(char* buffer, size_t bufferSize) override {
        return snprintf(buffer, bufferSize, "| %15s = %-32s | %-19s |\n", name, get() ? "True" : "False", "");
    }
    virtual size_t printInLineInternal(char* buffer, size_t bufferSize) override {
        return snprintf(buffer, bufferSize, "%s = %s", name, get() ? "True" : "False");
    }



    virtual void restoreDefaultInternal() override { set(getDef()); };
    virtual void saveInternal() override { storage->save(name, get()); }
    virtual void loadInternal() override { set(storage->load(name, get())); }
};

}