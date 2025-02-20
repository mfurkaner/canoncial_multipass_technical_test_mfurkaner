#include <iostream>
#include "ubuntu_cloud_image_fetcher.h"

int main(){

    UbuntuCloudImageFetcher fetcher;
    const std::string url = "https://cloud-images.ubuntu.com/releases/streams/v1/com.ubuntu.cloud:released:download.json";
    
    fetcher.Test(url);
    
    return EXIT_SUCCESS;
}