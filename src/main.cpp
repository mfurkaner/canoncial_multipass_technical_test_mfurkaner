#include <iostream>
#include <string>
#include <vector>
#include "ubuntu_cloud_image_fetcher.h"

void PrintHelp() {
    std::cout << "Ubuntu Cloud Image Fetcher CLI\n"
              << "Usage:\n"
              << "  --help                 Show this help message\n"
              << "  --list-releases        List currently supported releases\n"
              << "  --current-lts          Show current LTS version\n"
              << "  --sha256-uri <path>    Get SHA256 by version path\n"
              << "  --sha256-pubname <name> Get SHA256 by publication name\n"
              << "  --url <url>            Custom Simplestreams URL\n"
              << "  --clean                Minimal output (machine-readable)\n";
}

int main(int argc, char* argv[]) {
    bool clean_output = false;
    UbuntuCloudImageFetcher fetcher;
    std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json";
    
    enum class Command {
        None,
        ListReleases,
        CurrentLTS,
        Sha256Uri,
        Sha256Pubname
    } command = Command::None;
    
    std::string argument;
    std::vector<std::string> args(argv, argv + argc);

    // Parse command line arguments
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--help") {
            PrintHelp();
            return 0;
        }
        else if (args[i] == "--clean") {
            clean_output = true;
        }
        else if (args[i] == "--list-releases") {
            command = Command::ListReleases;
        }
        else if (args[i] == "--current-lts") {
            command = Command::CurrentLTS;
        }
        else if (args[i] == "--sha256-uri") {
            if (i + 1 >= args.size()) {
                std::cerr << "Error: Missing argument for --sha256-uri\n";
                return 1;
            }
            command = Command::Sha256Uri;
            argument = args[++i];
        }
        else if (args[i] == "--sha256-pubname") {
            if (i + 1 >= args.size()) {
                std::cerr << "Error: Missing argument for --sha256-pubname\n";
                return 1;
            }
            command = Command::Sha256Pubname;
            argument = args[++i];
        }
        else if (args[i] == "--url") {
            if (i + 1 >= args.size()) {
                std::cerr << "Error: Missing argument for --url\n";
                return 1;
            }
            url = args[++i];
        }
        else {
            std::cerr << "Error: Unknown option " << args[i] << "\n";
            PrintHelp();
            return 1;
        }
    }

    if (command == Command::None) {
        if (!clean_output) {
            std::cerr << "Error: No command specified\n";
            PrintHelp();
        }
        return 1;
    }

    // Fetch data
    auto err = fetcher.FetchLatestImageInfo(url);
    
    if(err != FetchError::NoError) {
        if (!clean_output) {
            std::cerr << "Error: ";
            switch(err) {
                case FetchError::FetchFailed:
                    std::cerr << "Failed to fetch data from URL\n";
                    break;
                case FetchError::JsonParseFailed:
                    std::cerr << "Failed to parse JSON data\n";
                    break;
                default:
                    std::cerr << "Unknown error\n";
            }
        }
        return 1;
    }

    // Execute command
    switch(command) {
        case Command::ListReleases: {
            auto result = fetcher.GetCurrentlySupportedReleases();
            if (std::holds_alternative<APIError>(result)) {
                if (!clean_output) {
                    auto error = std::get<APIError>(result);
                    std::cerr << "Error: ";
                    switch(error) {
                        case APIError::NotFetched:
                            std::cerr << "Data not fetched - try again\n";
                            break;
                        default:
                            std::cerr << "Unknown error\n";
                    }
                }
                return 1;
            }

            auto releases = std::get<const std::vector<UbuntuCloudImageSimplestreamsProduct>>(result);
            if (!clean_output) {
                std::cout << "Currently supported Ubuntu Cloud releases (amd64):\n";
            }
            for(const auto& release : releases) {
                if (clean_output) {
                    std::cout << release.release_title << "\n";
                } else {
                    std::cout << " - " << release.release_title 
                              << " (" << release.release_codename << ")\n";
                }
            }
            break;
        }
        
        case Command::CurrentLTS: {
            auto result = fetcher.GetCurrentLTSVersion();
            if (std::holds_alternative<APIError>(result)) {
                if (!clean_output) {
                    auto error = std::get<APIError>(result);
                    std::cerr << "Error: ";
                    switch(error) {
                        case APIError::NotFetched:
                            std::cerr << "Data not fetched - try again\n";
                            break;
                        case APIError::NotFound:
                            std::cerr << "LTS version not found in data\n";
                            break;
                        default:
                            std::cerr << "Unknown error\n";
                    }
                }
                return 1;
            }

            auto lts = std::get<const UbuntuCloudImageSimplestreamsProduct>(result);
            if (clean_output) {
                std::cout << lts.release_title << "\n";
            } else {
                std::cout << "Current LTS Version:\n"
                          << " - " << lts.release_title 
                          << " (" << lts.release_codename << ")\n";
            }
            break;
        }
        
        case Command::Sha256Uri: {
            auto res = fetcher.GetSHA256ofDisk1ImgByURI(argument);
            if(std::holds_alternative<APIError>(res)) {
                if (!clean_output) {
                    auto error = std::get<APIError>(res);
                    std::cerr << "Error: ";
                    switch(error) {
                        case APIError::InvalidVersionFormat:
                            std::cerr << "Invalid version format\n";
                            break;
                        case APIError::InvalidSubversionFormat:
                            std::cerr << "Invalid subversion format\n";
                            break;
                        case APIError::NotFound:
                            std::cerr << "Version not found\n";
                            break;
                        case APIError::NotFetched:
                            std::cerr << "Data not fetched - try again\n";
                            break;
                        default:
                            std::cerr << "Unknown error\n";
                    }
                }
                return 1;
            }
            if(!clean_output){
                std::cout << "SHA256 of disk1.img for " << argument << " : ";
            }
            std::cout << std::get<const std::string>(res) << "\n";
            break;
        }
        
        case Command::Sha256Pubname: {
            auto res = fetcher.GetSHA256ofDisk1ImgByPubname(argument);
            if(std::holds_alternative<APIError>(res)) {
                if (!clean_output) {
                    auto error = std::get<APIError>(res);
                    std::cerr << "Error: ";
                    switch(error) {
                        case APIError::InvalidPubnameFormat:
                            std::cerr << "Invalid pubname format\n";
                            break;
                        case APIError::NotFound:
                            std::cerr << "Publication name not found\n";
                            break;
                        case APIError::NotFetched:
                            std::cerr << "Data not fetched - try again\n";
                            break;
                        default:
                            std::cerr << "Unknown error\n";
                    }
                }
                return 1;
            }
            if(!clean_output){
                std::cout << "SHA256 of disk1.img for " << argument << " : ";
            }
            std::cout << std::get<const std::string>(res) << "\n";
            break;
        }
        
        default:
            if (!clean_output) {
                std::cerr << "Error: Invalid command\n";
                PrintHelp();
            }
            return 1;
    }

    return 0;
}