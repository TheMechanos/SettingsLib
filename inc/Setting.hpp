#pragma once


// Platform include
#if defined(ARDUINO)
#include <Arduino.h>

#elif defined(ESP_PLATFORM)
#include <inttypes.h>
#include <stdint.h>

#elif defined(USE_HAL_DRIVER)
#include <inttypes.h>
#include <stdint.h>

#elif defined(TI_PLATFORM)
#include <inttypes.h>
#include <stdint.h>

#endif


// RTOS INCLUDE
#if __has_include("freertos/FreeRTOS.h")
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#define SETTING_HAS_FREERTOS

#elif __has_include("FreeRTOS.h")
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#define SETTING_HAS_FREERTOS

#endif


#include <ArduinoJson.h>
#include <SKVector.h>


namespace Settings {

class Storage {
public:
    virtual void init() = 0;

    virtual int32_t load(const char* name, int32_t defVal) = 0;
    virtual bool save(const char* name, int32_t value) = 0;

    virtual bool load(const char* name, bool defVal) = 0;
    virtual bool save(const char* name, bool value) = 0;

    virtual size_t getBufferSize(const char* name) = 0;
    virtual size_t loadBuffer(const char* name, void* buffer, size_t bufferMaxSize) = 0;
    virtual size_t saveBuffer(const char* name, void* buffer, size_t bufferLen) = 0;
};

class Manager;

class Setting {
public:
    Setting(Manager* saver, const char* name);
    ~Setting();

    Setting(const Setting&) = delete;
    Setting& operator=(const Setting&) = delete;

    const char* getName() { return name; }

    size_t printInLine(char* buffer, size_t bufferSize) { return printInLineInternal(buffer, bufferSize); }
    size_t printInTable(char* buffer, size_t bufferSize) { return printInTableInternal(buffer, bufferSize); }

    void restoreDefault() { restoreDefaultInternal(); }
    void save() { saveInternal(); }
    void load() { loadInternal(); }

    bool fromJson(JsonVariant& var) { return fromJsonInternal(var); }
    void toJson(JsonVariant& var) { toJsonInternal(var); }



protected:
    Storage* storage;
    const char* name;

#ifdef SETTING_HAS_FREERTOS
    inline void lock() { xSemaphoreTake(mutex, portMAX_DELAY); }
    inline void unlock() { xSemaphoreGive(mutex); }
#else
    inline void lock() { }
    inline void unlock() { }
#endif

private:
#ifdef SETTING_HAS_FREERTOS
    SemaphoreHandle_t mutex;
#endif

    virtual bool fromJsonInternal(JsonVariant& var) = 0;
    virtual void toJsonInternal(JsonVariant& var) = 0;

    virtual size_t printInTableInternal(char* buffer, size_t bufferSize) = 0;
    virtual size_t printInLineInternal(char* buffer, size_t bufferSize) = 0;

    virtual void restoreDefaultInternal() = 0;
    virtual void saveInternal() = 0;
    virtual void loadInternal() = 0;
};

class Manager {
public:
    Manager(Storage* storage, size_t maxMembersCount = 32)
        : storage(storage)
        , members(maxMembersCount) { }

    ~Manager() { }

    Storage* getStorage() { return storage; }

    void registerMember(Setting* member) { members.push_back(member); }

    void save() {
        for (size_t q = 0; q < members.size(); q++)
            members[q]->save();
    }

    void load() {
        for (size_t q = 0; q < members.size(); q++)
            members[q]->load();
    }


    void fromJson(JsonDocument& doc) {
        for (size_t q = 0; q < members.size(); q++) {
            JsonVariant v = doc[members[q]->getName()];
            members[q]->fromJson(v);
        }
    }

    void toJson(JsonDocument& doc) {
        for (size_t q = 0; q < members.size(); q++) {
            JsonVariant v = doc[members[q]->getName()].to<JsonVariant>();
            members[q]->toJson(v);
        }
    }

    void restoreDefault() {
        for (size_t q = 0; q < members.size(); q++)
            members[q]->restoreDefault();
    }


    size_t printTable(char* buffer, size_t bufferSize) {

        size_t pos = 0;
        pos += snprintf(buffer + pos, bufferSize - pos, "\n");

        pos += snprintf(buffer + pos, bufferSize - pos, "                           ----- PARAMETERS -----\n");
        pos += snprintf(buffer + pos, bufferSize - pos, "+----------------------------------------------------+---------------------+\n");
        pos += snprintf(buffer + pos, bufferSize - pos, "| %15s = %-32s | %-19s |\n", "Name", "Value", "Constrains");
        pos += snprintf(buffer + pos, bufferSize - pos, "+----------------------------------------------------+---------------------+\n");

        for (size_t q = 0; q < members.size(); q++)
            pos += members[q]->printInTable(buffer + pos, bufferSize - pos);

        pos += snprintf(buffer + pos, bufferSize - pos, "+----------------------------------------------------+---------------------+");
        return pos;
    }






private:
    Storage* storage;
    Vector<Setting*> members;
};
}