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

    _parseJson(j);

    std::cout << "Currently supported versions of Ubuntu Cloud in amd64 architecture : \n";
    for(auto const& release : _fetched_sample.products){
        if(release.arch == "amd64" && release.supported){
            std::cout << "   - " << release.release_title << " (" << release.release_codename << ")\n";
        }
    }


}

FetchError UbuntuCloudImageFetcher::FetchLatestImageInfo(const std::string& url) {
    _fetched_sample.Clear();
    auto json_data = _fetchJson(url);
    if (!std::holds_alternative<json>(json_data)) return FetchError::FetchFailed;
    return _parseJson(std::get<json>(json_data));
}


const std::vector<const UbuntuCloudImageSimplestreamsProduct> UbuntuCloudImageFetcher::GetCurrentlySupportedReleases() const{
    std::vector<const UbuntuCloudImageSimplestreamsProduct> supported_releases;
    for(auto const& release : _fetched_sample.products){
        if(release.arch == "amd64" && release.supported){
            supported_releases.push_back(release);
        }
    }

    return supported_releases;
}


const UbuntuCloudImageSimplestreamsProduct UbuntuCloudImageFetcher::GetCurrentLTSVersion() const{
    auto supported_releases = GetCurrentlySupportedReleases();


    double latest_version = 0.0;
    UbuntuCloudImageSimplestreamsProduct latest;
    for(auto const& release : supported_releases){
        if (release.release_title.find("LTS") == std::string::npos) continue;


        double version = std::stod(release.version);

        if(version > latest_version){
            latest_version = version;
            latest = release;
        }
    }

    return latest;
}

const std::variant<std::string, APIError>  UbuntuCloudImageFetcher::GetSHA256ofDisk1Img(const std::string& uri) const {
    std::string sha_res;

    std::string version_name;
    std::string subversion_name;


    size_t slash_pos = uri.find('/');

    if (slash_pos == std::string::npos || slash_pos + 1 >= uri.size()) {
        // Only one part
        version_name = uri;
    } else {
        // Two parts
        version_name = uri.substr(0, slash_pos);
        subversion_name = uri.substr(slash_pos + 1);
    }

    size_t dot_pos = version_name.find('.');
    // The version name is not correct format
    if(version_name.empty() || dot_pos == std::string::npos || dot_pos == 0 || dot_pos == version_name.size() - 1) {
        return APIError::InvalidVersionFormat;
    };

    // The subversion name is not correct format
    int dot_count = 0;
    for(const char& c : subversion_name) {
        if(c == '.') dot_count++;

        if(std::isdigit(c) == false || dot_count > 1) {
            return APIError::InvalidSubversionFormat;
        }
    }
    
    for(auto product : _fetched_sample.products){
        if(product.version == version_name){
            // We found the requested version, now lets check the subversion if present

            // if the user provided a subversion
            if(subversion_name.empty() == false){
                for(auto subversion : product.versions){
                    if(subversion.json_name == subversion_name){
                        for(auto item : subversion.items){
                            if(item.json_name == "disk1.img") {
                                sha_res = item.sha256;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            // No subversion provided return the latest subversion
            else{
                int latest = 0;
                for(auto subversion : product.versions){
                    int name_i = std::stoi(subversion.json_name);
                    if(name_i > latest){
                        for(auto item : subversion.items){
                            if(item.json_name == "disk1.img"){
                                sha_res = item.sha256;
                                // dont change the latest if no disk1.img is present
                                latest = name_i;
                                break;
                            }
                        }
                    }
                }
            } 
            break;
        }
    }

    return sha_res;
}