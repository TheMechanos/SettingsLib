#pragma once

#include <Setting.hpp>

#ifdef ESP_PLATFORM

#include <esp_log.h>
#include <nvs_flash.h>

namespace Settings {



class StorageNVS : public Storage {
public:
    StorageNVS(const char* nvsNamespace = "SettingsLib", const char* customPartition = nullptr)
        : nvsNamespace(nvsNamespace)
        , customPartition(customPartition) { }

    virtual void init() override {
        if (customPartition != nullptr) {
            ESP_ERROR_CHECK(nvs_flash_init_partition(customPartition));
            ESP_ERROR_CHECK(nvs_open_from_partition(customPartition, nvsNamespace, NVS_READWRITE, &handle));

        } else {
            ESP_ERROR_CHECK(nvs_open(nvsNamespace, NVS_READWRITE, &handle));
        }
    }


    virtual int32_t load(const char* name, int32_t defVal) override {
        int32_t value = defVal;

        esp_err_t err = nvs_get_i32(handle, name, &value);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
            return defVal;

        return value;
    }

    virtual bool save(const char* name, int32_t value) override {
        esp_err_t err = nvs_set_i32(handle, name, value);
        if (err != ESP_OK)
            return false;

        return nvs_commit(handle) == ESP_OK;
    }

    virtual bool load(const char* name, bool defVal) override {
        uint8_t value = defVal ? 1 : 0;

        esp_err_t err = nvs_get_u8(handle, name, &value);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
            return defVal;

        return value != 0;
    }

    virtual bool save(const char* name, bool value) override {
        esp_err_t err = nvs_set_u8(handle, name, value ? 1 : 0);
        if (err != ESP_OK)
            return false;

        return nvs_commit(handle) == ESP_OK;
    }

    virtual size_t getBufferSize(const char* name) override {
        size_t requiredSize = 0;

        esp_err_t err = nvs_get_blob(handle, name, nullptr, &requiredSize);
        if (err != ESP_OK)
            return 0;

        return requiredSize;
    }

    virtual size_t loadBuffer(const char* name, void* buffer, size_t bufferMaxSize) override {
        size_t requiredSize = bufferMaxSize;

        esp_err_t err = nvs_get_blob(handle, name, buffer, &requiredSize);
        if (err != ESP_OK)
            return 0;

        return requiredSize;
    }

    virtual size_t saveBuffer(const char* name, void* buffer, size_t bufferLen) override {
        esp_err_t err = nvs_set_blob(handle, name, buffer, bufferLen);
        if (err != ESP_OK)
            return 0;

        if (nvs_commit(handle) != ESP_OK)
            return 0;

        return bufferLen;
    }

private:
    constexpr static const char* TAG = "Saver NVS";
    const char* nvsNamespace;
    const char* customPartition;

    nvs_handle_t handle;
};





}


#endif