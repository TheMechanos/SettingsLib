#pragma once

#include <Setting.hpp>


namespace Settings {

class Numerical : public Setting {
public:
    Numerical(Manager* manager, const char* name, int32_t def, int32_t min = INT32_MIN, int32_t max = INT32_MAX)
        : Setting(manager, name)
        , min(min)
        , max(max)
        , def(def)
        , value(def) {
        if (min > max) {
            int32_t newMin = max;
            max = min;
            min = newMin;
        }
    }

    ~Numerical() { }

    int32_t get() {
        lock();
        int32_t v = value;
        unlock();
        return v;
    }

    bool set(int32_t value) {
        if (value >= getMin() && value <= getMax()) {
            lock();
            this->value = value;
            unlock();
            return true;
        }
        return false;
    }

    int32_t getMax() {
        lock();
        int32_t v = max;
        unlock();
        return v;
    };
    int32_t getMin() {
        lock();
        int32_t v = min;
        unlock();
        return v;
    };
    int32_t getDef() {
        lock();
        int32_t v = def;
        unlock();
        return v;
    };

    operator int32_t() { return get(); }

    Numerical& operator=(int32_t v) {
        set(v);
        return *this;
    }



private:
    int32_t min;
    int32_t max;
    int32_t def;

    int32_t value;


    virtual bool fromJsonInternal(JsonVariant& var) override {
        if (var.is<int32_t>()) {
            set(var.as<int32_t>());
            return true;
        }
        return false;
    }
    virtual void toJsonInternal(JsonVariant& var) override { var.set(get()); }


    virtual size_t printInTableInternal(char* buffer, size_t bufferSize) override {
        return snprintf(buffer, bufferSize, "| %15s = %-32" PRId32 " | %6" PRId32 " < x < %-6" PRId32 " |\n", name, get(), getMin(), getMax());
    }
    virtual size_t printInLineInternal(char* buffer, size_t bufferSize) override {
        return snprintf(buffer, bufferSize, "%s = %" PRId32 " [%" PRId32 " : %" PRId32 "]", name, get(), getMin(), getMax());
    }


    virtual void restoreDefaultInternal() override { set(getDef()); };

    virtual void saveInternal() override { storage->save(name, get()); }
    virtual void loadInternal() override { set(storage->load(name, get())); }
};


}