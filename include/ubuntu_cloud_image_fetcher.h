
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

    const UbuntuCloudImageSimplestreamsProduct GetCurrentLTSVersion() const;

    const std::variant<std::string, APIError> GetSHA256ofDisk1ImgByURI(const std::string& uri) const;
    const std::variant<std::string, APIError> GetSHA256ofDisk1ImgByPubname(const std::string& pubname) const;

    void Test(const std::string& url);
};

#endif // UBUNTU_CLOUD_IMAGE_FETCHER_H