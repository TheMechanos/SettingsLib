#pragma once

#include "Setting.hpp"


namespace Settings {

class Text : public Setting {
public:
    Text(Manager* manager, const char* name, size_t maxLen, const char* defVal = "")
        : Setting(manager, name)
        , maxLen(maxLen)
        , defVal(defVal) {

        value = new char[maxLen + 1];
        memset(value, 0x00, maxLen + 1);

        set(defVal);
    }

    ~Text() { delete[] value; }

    size_t get(char* buffer, size_t bufferSize) {
        if (!buffer || bufferSize == 0)
            return 0;

        lock();
        size_t len = strnlen(value, maxLen);
        size_t copyLen = (len < (bufferSize - 1)) ? len : (bufferSize - 1);
        memcpy(buffer, value, copyLen);
        buffer[copyLen] = '\0';
        unlock();

        return copyLen;
    }

    bool set(const char* newValue, size_t len = 0) {
        if (!newValue)
            return false;

        if (len == 0)
            len = strnlen(newValue, maxLen);
        if (len > maxLen)
            len = maxLen;

        lock();
        memcpy(value, newValue, len);
        value[len] = '\0';
        unlock();

        return true;
    }

    size_t getMaxSize() const { return maxLen; }

private:
    char* value;
    size_t maxLen;
    const char* defVal;

    virtual bool fromJsonInternal(JsonVariant& v) override {
        if (v.is<const char*>()) {
            set(v.as<const char*>()); // set() samo lockuje
            return true;
        }
        return false;
    }

    virtual void toJsonInternal(JsonVariant& v) override {
        lock();
        v.set(value);
        unlock();
    }

    virtual size_t printInTableInternal(char* buffer, size_t bufferSize) override {
        lock();
        size_t n = snprintf(buffer, bufferSize, "| %15s = %-32s | MaxLen: %-11zu |\n", name, value, maxLen);
        unlock();
        return n;
    }

    virtual size_t printInLineInternal(char* buffer, size_t bufferSize) override {
        lock();
        size_t n = snprintf(buffer, bufferSize, "%s = '%s' [max %zu chars]", name, value, maxLen);
        unlock();
        return n;
    }

    virtual void restoreDefaultInternal() override {
        set(defVal); // set() samo lockuje
    }

    virtual void saveInternal() override {
        lock();
        storage->saveBuffer(name, value, strnlen(value, maxLen));
        unlock();
    }

    virtual void loadInternal() override {
        char* buf = new char[maxLen + 1];
        size_t s = storage->loadBuffer(name, buf, maxLen);
        if (s > maxLen)
            s = maxLen;
        buf[s] = '\0';
        if (s)
            set(buf, s); // set() samo lockuje
        delete[] buf;
    }
};
}