
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

using JsonResult = std::variant<nlohmann::json, FetchError>;


class UbuntuCloudImageFetcher {
private:
    UbuntuCloudImageSimplestreamsFetch _fetched_sample;


    JsonResult _fetchJson(const std::string& url); 
    FetchError _parseJson(const nlohmann::json& json);

public:
    void Test(const std::string& url);
};

#endif // UBUNTU_CLOUD_IMAGE_FETCHER_H