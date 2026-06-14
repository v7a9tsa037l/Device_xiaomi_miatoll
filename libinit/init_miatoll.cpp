/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

using android::base::GetProperty;

std::vector<std::string> ro_props_default_source_order = {
    "",
    "odm.",
    "odm_dlkm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
    "vendor_dlkm.",
};

void property_override(char const prop[], char const value[], bool add = true)
{
    prop_info *pi;

    pi = (prop_info *) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_dalvik_heap() {
    struct sysinfo sys;

    sysinfo(&sys);
    if (sys.totalram < 4096ull * 1024 * 1024) {
        property_override("dalvik.vm.heapstartsize", "8m");
        property_override("dalvik.vm.heapgrowthlimit", "192m");
        property_override("dalvik.vm.heapsize", "512m");
	    property_override("dalvik.vm.heaptargetutilization", "0.6");
        property_override("dalvik.vm.heapminfree", "8m");
        property_override("dalvik.vm.heapmaxfree", "16m");
    }
}

void set_ro_build_prop(const std::string &prop, const std::string &value) {
    for (const auto &source : ro_props_default_source_order) {
        auto prop_name = "ro." + source + "build." + prop;
        if (source == "")
            property_override(prop_name.c_str(), value.c_str());
        else
            property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void set_ro_product_prop(const std::string &prop, const std::string &value) {
    for (const auto &source : ro_props_default_source_order) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void vendor_load_properties() {
    std::string region;
    std::string hwname;
    region = GetProperty("ro.boot.hwc", "");
    hwname = GetProperty("ro.boot.hwname", "");

    std::string brand;
    std::string model;
    std::string device;
    std::string fingerprint;

    if (region == "India") {
        if (hwname == "curtana") {
            brand = "Redmi";
            model = "Redmi Note 10 Lite";
            device = "curtana";
            fingerprint = "Redmi/curtana_in1/curtana:12/RKQ1.211019.001/V14.0.5.0.SJWINRF:user/release-keys";
        } else if (hwname == "gram"){
            brand = "POCO";
            model = "POCO M2 Pro";
            device = "gram";
            fingerprint = "POCO/gram_in/gram:12/RKQ1.211019.001/V14.0.5.0.SJPINXM:user/release-keys";
        } else if (hwname == "excalibur") {
            brand = "Redmi";
            model = "Redmi Note 9 Pro Max";
            device = "excalibur";
            fingerprint = "Redmi/excalibur_in/excalibur:12/RKQ1.211019.001/V14.0.2.0.SJXINXM:user/release-keys";
        }
    } else {
        if (hwname == "curtana") {
            brand = "Redmi";
            model = "Redmi Note 9S";
            device = "curtana";
            fingerprint = "Redmi/curtana_global/curtana:12/RKQ1.211019.001/V14.0.4.0.SJWMIXM:user/release-keys";
        } else if (hwname == "joyeuse") {
            brand = "Redmi";
            model = "Redmi Note 9 Pro";
            device = "joyeuse";
            fingerprint = "Redmi/joyeuse_global/joyeuse:12/RKQ1.211019.001/V14.0.3.0.SJZMIXM:user/release-keys";
        }
    }

    set_ro_build_prop("fingerprint", fingerprint);
    set_ro_product_prop("brand", brand);
    set_ro_product_prop("device", device);
    set_ro_product_prop("model", model);

    set_dalvik_heap();
}
