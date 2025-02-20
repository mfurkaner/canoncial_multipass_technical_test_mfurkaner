#include <iostream>
#include "ubuntu_cloud_image_fetcher.h"

int main() {
    UbuntuCloudImageFetcher fetcher;
    const std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json";
    
    auto err = fetcher.FetchLatestImageInfo(url);


    if( err == FetchError::FetchFailed ){

    }
    else if (err == FetchError::JsonParseFailed)
    {
        /* code */
    }
    else if (err == FetchError::NoError)
    {
        auto current_supported_releases = fetcher.GetCurrentlySupportedReleases();

        std::cout << "Currently supported versions of Ubuntu Cloud in amd64 architecture : \n";
        for(auto const& release : current_supported_releases){
            std::cout << "   - " << release.release_title << " (" << release.release_codename << ")\n";
        }


        std::cout << "Currently LTS version : \n";
        auto current_lts = fetcher.GetCurrentLTSVersion();

        std::cout << "   - " << current_lts.release_title << " (" << current_lts.release_codename << ")\n";

        std::cout << "SHA256 of image 13.04/20140111 : \n";
        auto res = fetcher.GetSHA256ofDisk1ImgByURI("13.04/20140111");

        if (std::holds_alternative<APIError>(res)) {
            auto err = std::get<APIError>(res);

            switch(err){
                case APIError::InvalidVersionFormat:
                    std::cerr << "Invalid version format\n";
                    break;
                case APIError::InvalidSubversionFormat:
                    std::cerr << "Invalid subversion format\n";
                    break;
                case APIError::NotFound:
                    std::cerr << "Version not found\n";
                    break;
            }
        }
        else if (std::holds_alternative<std::string>(res)){
            auto sha256 = std::get<std::string>(res);
            std::cout << "SHA256: " << sha256 << "\n";
        }
        else{
            std::cerr << "How did you get here?\n"; 
        }

        std::cout << "SHA256 of image ubuntu-trusty-14.04-amd64-server-20150227.2 : \n";
        auto res2 = fetcher.GetSHA256ofDisk1ImgByPubname("ubuntu-trusty-14.04-amd64-server-20150227.2");

        if (std::holds_alternative<APIError>(res2)) {
            auto err = std::get<APIError>(res2);

            switch(err){
                case APIError::InvalidPubnameFormat:
                    std::cerr << "Invalid pubname format\n";
                    break;
                case APIError::NotFound:
                    std::cerr << "Version not found\n";
                    break;
            }
        }
        else if (std::holds_alternative<std::string>(res2)){
            auto sha256 = std::get<std::string>(res2);
            std::cout << "SHA256: " << sha256 << "\n";
        }
        else{
            std::cerr << "How did you get here?\n"; 
        }
    }
    

    return 0;
}