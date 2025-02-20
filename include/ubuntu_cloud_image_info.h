#ifndef UBUNTU_CLOUD_IMAGE_INFO_H
#define UBUNTU_CLOUD_IMAGE_INFO_H

#include <string>
#include <vector>

struct UbuntuImageSimplestreamsProductVersionItem{
    std::string json_name;

    std::string ftype;
    std::string md5;
    std::string path;
    std::string sha256;
    uint64_t size;
};

struct UbuntuImageSimplestreamsProductVersion{
    std::string json_name;

    std::vector<UbuntuImageSimplestreamsProductVersionItem> items;
    std::string label;
    std::string pubname;
};


struct UbuntuImageSimplestreamsProduct{
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
    std::vector<UbuntuImageSimplestreamsProductVersion> versions;
};

struct UbuntuImageSimplestreamsFetch{
    std::string content_id;
    std::string creator;
    std::string datatype;
    std::string format;
    std::string license;
    std::vector<UbuntuImageSimplestreamsProduct> products;
    std::string updated;
};


#endif // UBUNTU_CLOUD_IMAGE_INFO_H