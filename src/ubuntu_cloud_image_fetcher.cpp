#include "ubuntu_cloud_image_fetcher.h"
#include "httplib.h"
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iostream> 
#include <string>

using json = nlohmann::json;


JsonResult UbuntuCloudImageFetcher::_fetchJson(const std::string& url) {
    // Parse the URL into host and path
    std::string host, path;
    size_t host_start = url.find("://");
    if (host_start == std::string::npos) {
        return FetchError::FetchFailed;
    }
    host_start += 3; // Skip "://"
    size_t path_start = url.find('/', host_start);
    if (path_start == std::string::npos) {
        host = url.substr(host_start);
        path = "/";
    } else {
        host = url.substr(host_start, path_start - host_start);
        path = url.substr(path_start);
    }

    httplib::Client cli(host.c_str());

    auto res = cli.Get(path.c_str());

    // Check for errors
    if (!res || res->status != 200) {
        return FetchError::FetchFailed;
    }

    // Parse the JSON response
    try {
        return json::parse(res->body);
    } catch (const json::parse_error&) {
        return FetchError::FetchFailed;
    }
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

FetchError UbuntuCloudImageFetcher::FetchLatestImageInfo(const std::string& url) {
    // Clearup the previous data
    _fetched_sample.Clear();
    // Get the JSON data
    auto json_data = _fetchJson(url);
    // If there is an error, abort
    if (!std::holds_alternative<json>(json_data)) return FetchError::FetchFailed;

    // Parse the JSON data
    auto result = _parseJson(std::get<json>(json_data));

    // If there is no error, update _fetched
    if ( result == FetchError::NoError ) _fetched = true;

    return result;
}


std::variant<const std::vector<UbuntuCloudImageSimplestreamsProduct>, APIError> UbuntuCloudImageFetcher::GetCurrentlySupportedReleases() const{
    // if not fetched, no reason to do calculation
    if(_fetched == false) return APIError::NotFetched;
    
    std::vector<UbuntuCloudImageSimplestreamsProduct> supported_releases;
    for(auto const& release : _fetched_sample.products){
        if(release.arch == "amd64" && release.supported){
            supported_releases.push_back(release);
        }
    }

    return supported_releases;
}


std::variant<const UbuntuCloudImageSimplestreamsProduct, APIError> UbuntuCloudImageFetcher::GetCurrentLTSVersion() const{
    // if not fetched, no reason to do calculation
    if(_fetched == false) return APIError::NotFetched;

    auto supported_releases_err = GetCurrentlySupportedReleases();

    if (std::holds_alternative<APIError>(supported_releases_err)) return std::get<APIError>(supported_releases_err);

    auto supported_releases = std::get<const std::vector<UbuntuCloudImageSimplestreamsProduct>>(supported_releases_err);

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
    if ( latest_version == 0.0 ) return APIError::NotFound;
    return latest;
}

std::variant<const std::string, APIError>  UbuntuCloudImageFetcher::GetSHA256ofDisk1ImgByURI(const std::string& uri) const {
    // if not fetched, no reason to do calculation
    if(_fetched == false) return APIError::NotFetched;
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
    if(sha_res.empty()) return APIError::NotFound;
    return sha_res;
}


std::variant<const std::string, APIError>  UbuntuCloudImageFetcher::GetSHA256ofDisk1ImgByPubname(const std::string& pubname) const {
    // if not fetched, no reason to do calculation
    if(_fetched == false) return APIError::NotFetched;

    // Pubname contains 5 dashes
    int dash_count = 0;
    for(const char& c : pubname)
        if(c == '-') dash_count++;

    if(dash_count != 5) return APIError::InvalidPubnameFormat;

    // Split the pubname into parts
    std::vector<std::string> pubname_parts;
    std::string remaining = pubname;
    for(int i = 0; i < dash_count; i++){
        size_t dash_pos = remaining.find('-');
        if (dash_pos != std::string::npos)
        {
            pubname_parts.push_back(remaining.substr(0,  dash_pos));
            remaining = remaining.substr(dash_pos + 1);
        }
    }
    pubname_parts.push_back(remaining);

    // There should be 6 parts
    if(pubname_parts.size() != 6) return APIError::InvalidPubnameFormat;
    
    std::string sha_res;

    // Get the important parts
    std::string version_name = pubname_parts[2];
    std::string subversion_name = pubname_parts[5];

    size_t dot_pos = version_name.find('.');
    // The version name is not correct format
    if(version_name.empty() || dot_pos == std::string::npos || dot_pos == 0 || dot_pos == version_name.size() - 1) {
        return APIError::InvalidPubnameFormat;
    };


    // The subversion name is not correct format
    int dot_count = 0;
    for(const char& c : subversion_name) {
        if(c == '.') dot_count++;

        if((std::isdigit(c) == false && c != '.') || dot_count > 1) {
            return APIError::InvalidPubnameFormat;
        }
    }
    
    // Find the requested version and its 
    for(auto product : _fetched_sample.products){
        if(product.version == version_name){
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
            break;
        }
    }
    if(sha_res.empty()) return APIError::NotFound;
    return sha_res;
}