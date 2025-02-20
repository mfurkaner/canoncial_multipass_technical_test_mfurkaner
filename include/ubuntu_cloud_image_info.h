#ifndef UBUNTU_CLOUD_IMAGE_INFO_H
#define UBUNTU_CLOUD_IMAGE_INFO_H

#include <string>
#include <vector>

struct UbuntuCloudImageSimplestreamsProductVersionItem{
    std::string json_name;

    std::string ftype;
    std::string md5;
    std::string path;
    std::string sha256;
    uint64_t size;
};

struct UbuntuCloudImageSimplestreamsProductVersion{
    std::string json_name;

    std::vector<UbuntuCloudImageSimplestreamsProductVersionItem> items;
    std::string label;
    std::string pubname;
};


struct UbuntuCloudImageSimplestreamsProduct{
    std::string json_name;

    std::string aliases;
    std::string arch;
    std::string os;
    std::string release;
    std::string release_codename;
    std::string release_title;
    std::string support_eol;
    bool supported;
    std::string version;
    std::vector<UbuntuCloudImageSimplestreamsProductVersion> versions;
};

struct UbuntuCloudImageSimplestreamsFetch{
    std::string content_id;
    std::string creator;
    std::string datatype;
    std::string format;
    std::string license;
    std::vector<UbuntuCloudImageSimplestreamsProduct> products;
    std::string updated;

    // Clear all the data
    void Clear(){
        content_id.clear();
        creator.clear();
        datatype.clear();
        format.clear();
        license.clear();
        products.clear();
        updated.clear();
    }
};


#endif // UBUNTU_CLOUD_IMAGE_INFO_H