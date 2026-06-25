#pragma once

#include <Setting.hpp>


#ifdef ARDUINO

#include <Preferences.h>

namespace Settings {



class StoragePreferences : public Storage {
public:
    StoragePreferences(const char* prefsTag = "Settings")
        : prefsTag(prefsTag) { }

    virtual void init() override { prefs.begin(prefsTag); }


    virtual int32_t load(const char* name, int32_t defVal) override { return prefs.getInt(name, defVal); }
    virtual bool save(const char* name, int32_t value) override { return 4 == prefs.putInt(name, value); }

    virtual bool load(const char* name, bool defVal) override { return prefs.getBool(name, defVal); }
    virtual bool save(const char* name, bool value) override { return 1 == prefs.putBool(name, value); }


    virtual size_t getBufferSize(const char* name) { return prefs.getBytesLength(name); }
    virtual size_t loadBuffer(const char* name, void* buffer, size_t bufferMaxSize) { return prefs.getBytes(name, buffer, bufferMaxSize); }
    virtual size_t saveBuffer(const char* name, void* buffer, size_t bufferLen) { return prefs.putBytes(name, buffer, bufferLen); }

private:
    const char* prefsTag;
    Preferences prefs;
};





}


#endif