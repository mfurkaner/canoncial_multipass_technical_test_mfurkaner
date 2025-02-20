
#ifndef UBUNTU_CLOUD_IMAGE_FETCHER_H
#define UBUNTU_CLOUD_IMAGE_FETCHER_H

#include <string>
#include <variant>
#include <vector>

#include "nlohmann/json.hpp"
#include "ubuntu_cloud_image_info.h"


enum class FetchError{
    NoError,
    FetchFailed,
    JsonParseFailed
};

enum class APIError{
    InvalidVersionFormat,
    InvalidSubversionFormat,
    InvalidPubnameFormat,
    NotFound
};


using JsonResult = std::variant<nlohmann::json, FetchError>;


class UbuntuCloudImageFetcher {
private:
    UbuntuCloudImageSimplestreamsFetch _fetched_sample;


    JsonResult _fetchJson(const std::string& url); 
    FetchError _parseJson(const nlohmann::json& json);

public:
    FetchError FetchLatestImageInfo(const std::string& url);

    // Returns the currently supported releases in the previously fetched sample
    const std::vector<UbuntuCloudImageSimplestreamsProduct> GetCurrentlySupportedReleases() const;

    // Returns the currently supported LTS version in the previously fetched sample
    const UbuntuCloudImageSimplestreamsProduct GetCurrentLTSVersion() const;

    // Returns the SHA256 of disk1.img file using a URI defined as : "<version>/<subversion>"  (ex : 13.04/20140111)
    const std::variant<std::string, APIError> GetSHA256ofDisk1ImgByURI(const std::string& uri) const;

    // Returns the SHA256 of disk1.img file using a pubname. (ex : ubuntu-lucid-10.04-amd64-server-20150427)
    const std::variant<std::string, APIError> GetSHA256ofDisk1ImgByPubname(const std::string& pubname) const;

};

#endif // UBUNTU_CLOUD_IMAGE_FETCHER_H