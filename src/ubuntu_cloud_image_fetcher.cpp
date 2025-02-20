#include "ubuntu_cloud_image_fetcher.h"
#include <curl/curl.h>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iostream> 

using json = nlohmann::json;

// CURL callback function
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

JsonResult UbuntuCloudImageFetcher::_fetchJson(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if(res != CURLE_OK) {
            return FetchError::FetchFailed;
        }
        
        try {
            return json::parse(readBuffer);
        } catch (const json::parse_error&) {
            return FetchError::FetchFailed;
        }
    }
    return FetchError::FetchFailed;
}


FetchError UbuntuCloudImageFetcher::_parseJson(const json& j) {
    try {
        _fetched_sample.content_id = j.at("content_id").get<std::string>();
        _fetched_sample.creator = j.at("creator").get<std::string>();
        _fetched_sample.datatype = j.at("datatype").get<std::string>();
        _fetched_sample.format = j.at("format").get<std::string>();
        _fetched_sample.license = j.at("license").get<std::string>();
        _fetched_sample.updated = j.at("updated").get<std::string>();
        

        const auto& products = j.at("products");

        for(const auto& [name,product] : products.items()){
            UbuntuCloudImageSimplestreamsProduct product_obj;
            product_obj.json_name = name;
            product_obj.aliases = product.at("aliases").get<std::string>();
            product_obj.arch = product.at("arch").get<std::string>();
            product_obj.os = product.at("os").get<std::string>();
            product_obj.release = product.at("release").get<std::string>();
            product_obj.release_codename = product.at("release_codename").get<std::string>();
            product_obj.release_title = product.at("release_title").get<std::string>();
            product_obj.support_eol = product.at("support_eol").get<std::string>();
            product_obj.supported = product.at("supported").get<bool>();
            product_obj.version = product.at("version").get<std::string>();

            const auto& versions = product.at("versions");

            for(const auto& [version_name, version] : versions.items()){
                UbuntuCloudImageSimplestreamsProductVersion version_obj;

                version_obj.json_name = version_name;
                version_obj.label = version.at("label").get<std::string>();
                version_obj.pubname = version.at("pubname").get<std::string>();

                const auto& items = version.at("items");

                for(const auto& [item_name, item] : items.items()){
                    UbuntuCloudImageSimplestreamsProductVersionItem item_obj;

                    item_obj.json_name = item_name;
                    item_obj.ftype = item.at("ftype").get<std::string>();
                    item_obj.md5 = item.at("md5").get<std::string>();
                    item_obj.path = item.at("path").get<std::string>();
                    item_obj.sha256 = item.at("sha256").get<std::string>();
                    item_obj.size = item.at("size").get<uint64_t>();

                    version_obj.items.push_back(item_obj);
                }

                product_obj.versions.push_back(version_obj);
            }

            _fetched_sample.products.push_back(product_obj);
        }


    } catch (const json::exception& e) {
        return FetchError::FetchFailed;
    }


    return FetchError::NoError;
}

void UbuntuCloudImageFetcher::Test(const std::string& url){
    auto json_data = _fetchJson(url);
    if (!std::holds_alternative<json>(json_data)) return ;

    auto j = std::get<json>(json_data);

    auto err = _parseJson(j);

    if (err == FetchError::NoError){

        std::cout << "Currently supported versions of Ubuntu Cloud in amd64 architecture : \n";
        for(auto const& release : _fetched_sample.products){
            if(release.arch == "amd64" && release.supported){
                std::cout << "   - " << release.release_title << " (" << release.release_codename << ")\n";
            }
        }
    }

}
