#pragma once

#include <Setting.hpp>

namespace Settings {

template <typename E> class Enum : public Setting {
public:
    using PrintFn = const char* (*)(E);

    Enum(Manager* manager, const char* name, E def, PrintFn printFn = nullptr)
        : Setting(manager, name)
        , def(def)
        , value(def)
        , printFn(printFn) { }

    ~Enum() { }

    E get() {
        lock();
        E v = value;
        unlock();
        return v;
    }

    void set(E value) {
        lock();
        this->value = value;
        unlock();
    }

    E getDef() {
        lock();
        E v = def;
        unlock();
        return v;
    };



    operator E() { return get(); }

    Enum& operator=(E v) {
        set(v);
        return *this;
    }


private:
    E def;
    E value;
    PrintFn printFn;

    virtual bool fromJsonInternal(JsonVariant& v) override {
        if (v.is<int32_t>()) {
            set((E)v.as<int32_t>());
            return true;
        }
        return false;
    }
    virtual void toJsonInternal(JsonVariant& v) override { v.set((int32_t)get()); }



    virtual size_t printInTableInternal(char* buffer, size_t bufferSize) override {
        if (printFn != nullptr)
            return snprintf(buffer, bufferSize, "| %15s = %-32s |                     |\n", name, printFn(get()));
        else
            return snprintf(buffer, bufferSize, "| %15s = %-32" PRId32 " |                     |\n", name, (int32_t)get());
    }
    virtual size_t printInLineInternal(char* buffer, size_t bufferSize) override {
        if (printFn != nullptr)
            return snprintf(buffer, bufferSize, "%s = %s [def %s]", name, printFn(get()), printFn(getDef()));
        else
            return snprintf(buffer, bufferSize, "%s = %" PRId32 " [def %" PRId32 "]", name, (int32_t)get(), (int32_t)getDef());
    }


    virtual void restoreDefaultInternal() override { set(getDef()); };

    virtual void saveInternal() override { storage->save(name, (int32_t)get()); }
    virtual void loadInternal() override { set((E)storage->load(name, (int32_t)get())); }
};

}