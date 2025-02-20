
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
    const std::vector<const UbuntuCloudImageSimplestreamsProduct> GetCurrentlySupportedReleases() const;

    const UbuntuCloudImageSimplestreamsProduct GetCurrentLTSVersion() const;

    const std::variant<std::string, APIError> GetSHA256ofDisk1Img(const std::string& uri) const;

    void Test(const std::string& url);
};

#endif // UBUNTU_CLOUD_IMAGE_FETCHER_H