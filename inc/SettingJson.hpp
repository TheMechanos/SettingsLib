#pragma once

#include <Setting.hpp>

#if __has_include(<ArduinoJson.h>)

namespace Settings {


#ifdef __cpp_concepts
template <typename T>
concept Printable = requires(T obj, char* buffer, size_t maxBuffer) {
    { obj.printValue(buffer, maxBuffer) } -> std::same_as<size_t>;
    { obj.printConstrains(buffer, maxBuffer) } -> std::same_as<size_t>;
};

template <Printable E>
#else
template <typename E>
#endif
class Json : public Setting {
public:
    Json(Manager* manager, const char* name, E def = E())
        : Setting(manager, name)
        , def(def)
        , value(def) { }

    ~Json() { }

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

    Json& operator=(E v) {
        set(v);
        return *this;
    }


private:
    E def;
    E value;


    virtual bool fromJsonInternal(JsonVariant& v) override {
        if (v.is<E>()) {
            set(v.as<E>());
            return true;
        }
        return false;
    }

    virtual void toJsonInternal(JsonVariant& v) override { v.set(get()); }



#ifdef __cpp_concepts
    virtual size_t printInTableInternal(char* buffer, size_t bufferSize) override {
        auto v = get();

        char summary[32];
        v.printValue(summary, 32);

        char constrains[19];
        v.printConstrains(constrains, 19);

        return snprintf(buffer, bufferSize, "| %15s = %-32s | %-19s |\n", name, summary, constrains);
    }
#else
    virtual size_t printInTable(char* buffer, size_t bufferSize) override {
        return snprintf(buffer, bufferSize, "| %15s = %-32s | %-19s |\n", name, "<OBJECT>", "");
    }
#endif

    virtual size_t printInLineInternal(char* buffer, size_t bufferSize) override {
        JsonDocument doc;
        doc.set(get());
        size_t s = measureJsonPretty(doc);
        char b[s];
        serializeJsonPretty(doc, b, s);

        return snprintf(buffer, bufferSize, "%s = \n%s\n", name, b);
    }




    virtual void restoreDefaultInternal() override { set(getDef()); };

    virtual void saveInternal() override {
        JsonDocument doc;
        doc.set(get());

        size_t s = measureMsgPack(doc);
        char b[s];
        serializeMsgPack(doc, b, s);

        storage->saveBuffer(name, b, s);
    }
    virtual void loadInternal() override {

        size_t s = storage->getBufferSize(name);
        if (s == 0)
            return;

        char b[s];
        storage->loadBuffer(name, b, s);

        JsonDocument doc;
        auto err = deserializeMsgPack(doc, b, s);

        if (err)
            return;


        if (doc.is<E>())
            set(doc.as<E>());
    }
};

}

#endif