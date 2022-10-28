// ENVIRO
#include <ecalhdf5/eh5_meas.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <json.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core.hpp>

#define throw_line(message) {char err[500]="";sprintf(err,"Fatal error: %s in:" __FILE__ "line:%d\n",message,__LINE__);throw err;}
#define THROW_IF_ZERO(val) {if (!val) throw_line("Value "#val"is zero");}

using json = nlohmann::json;

// Declarations

int main(int argc, char** argv)
{
    std::string meas_path = "/home/aru/ecal_meas/2022-10-27_14-49-54.023_measurement";
    std::string channel_name = "rt/enviro_data";
    
    eCAL::eh5::HDF5Meas enviro_meas_(meas_path);

    // Check file status
    if (!enviro_meas_.IsOk()){
        std::cout << "Problem with measurement file." << std::endl;
        return 0;
    }

    // --------------------
    // Metadata
    // --------------------

    // List Channels Available
    auto channels = enviro_meas_.GetChannelNames();
    std::cout << "Channels Available: ";
    for (auto it = channels.begin(); it!=channels.end();it++)
    {
        std::cout<<std::endl<<*it;
    }
    std::cout<<std::endl;
    
    // Entry Info
    eCAL::eh5::EntryInfoSet entry_info_set;
    if (!enviro_meas_.GetEntriesInfo(channel_name, entry_info_set)){
        std::cout << "Problem retrieving entries info" << std::endl;
        return 0;
    }
    std::cout << "Entries Info Retrieved Successfully!" << std::endl;
    
    for (auto it = entry_info_set.begin(); it!=entry_info_set.end();it++)
    {   
        size_t entry_size;
        if (!enviro_meas_.GetEntryDataSize(it->ID, entry_size))
            return 0;
        uint8_t * data = new uint8_t[entry_size];
        if (!enviro_meas_.GetEntryData(it->ID, data)){
            std::cout << "Problem getting entry data: " << it->ID << std::endl;
        }
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Retrieved Data: "<< std::endl;
        std::cout << "ID: " << it->ID << std::endl;
    
        
        // Split data into correct format based on Image struct in msgs.hpp

        uint64_t size_of_frameid;

        std::string ID = "123456";

        uint32_t timestamp_sec;
        uint32_t timestamp_nanosec;

        double temperature;
        double pressure;
        double humidity;

        int ptr = 0;
        
        std::memcpy(&timestamp_sec, &data[ptr], sizeof(timestamp_sec));
        std::cout << "Timestamp secs: " <<  timestamp_sec << std::endl;

        ptr += sizeof(timestamp_sec);
        std::memcpy(&timestamp_nanosec, &data[ptr], sizeof(timestamp_nanosec));
        std::cout << "Timestamp nanosecs: " <<  timestamp_nanosec << std::endl;

        ptr += sizeof(timestamp_nanosec);
        std::memcpy(&size_of_frameid, &data[ptr], sizeof(size_of_frameid));
        //std::cout << "Size of frame id: " <<  size_of_frameid << std::endl;

        ptr += sizeof(size_of_frameid);
        uint8_t *frame_id_cstring = (uint8_t*)malloc((size_t)size_of_frameid);
        std::memcpy(&frame_id_cstring, &data[ptr], sizeof(frame_id_cstring));
        std::string s = (const char*)&frame_id_cstring;
        ID = s;
        std::cout << "Frame ID: " <<  ID << std::endl;
    
        ptr += size_of_frameid;
        std::memcpy(&temperature, &data[ptr], sizeof(temperature));
        std::cout << "Temperature: " <<  temperature << std::endl;

        ptr += sizeof(temperature);
        std::memcpy(&pressure, &data[ptr], sizeof(pressure));
        std::cout << "Pressure: " <<  pressure << std::endl;

        ptr += sizeof(pressure);
        std::memcpy(&humidity, &data[ptr], sizeof(humidity));
        std::cout << "Humidity: " <<  humidity << std::endl;

        // SAVE ENVIRO DATA IN JSON
        json j;
        j["timestamp_secs"] = timestamp_sec;
        j["timestamp_nanosecs"] = timestamp_nanosec;
        j["frame_id"] = ID;
        j["temperature"] = temperature;
        j["pressure"] = pressure;
        j["humidity"] = humidity;

        // write 
        std::ofstream o("enviro.json");
        o << std::setw(4) << j << std::endl;



        
    }

    std::cout<<std::endl;
    std::cout << "Done Processing Measurement" << std::endl;
    return 0;
} 



/* // BOSON
#include <ecalhdf5/eh5_meas.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core.hpp>

#define throw_line(message) {char err[500]="";sprintf(err,"Fatal error: %s in:" __FILE__ "line:%d\n",message,__LINE__);throw err;}
#define THROW_IF_ZERO(val) {if (!val) throw_line("Value "#val"is zero");}

//using namespace cv;
// Declarations

int main(int argc, char** argv)
{
    std::string meas_path = "/home/aru/ecal_meas/2022-10-26_13-09-34.473_measurement";
    std::string channel_name = "rt/thermal_raw";
    int height = 512, width = 640;
    
    eCAL::eh5::HDF5Meas thermal_meas_(meas_path);

    // Check file status
    if (!thermal_meas_.IsOk()){
        std::cout << "Problem with measurement file." << std::endl;
        return 0;
    }

    // --------------------
    // Metadata
    // --------------------

    // List Channels Available
    auto channels = thermal_meas_.GetChannelNames();
    std::cout << "Channels Available: ";
    for (auto it = channels.begin(); it!=channels.end();it++)
    {
        std::cout<<std::endl<<*it;
    }
    std::cout<<std::endl;
    
    // Entry Info
    eCAL::eh5::EntryInfoSet entry_info_set;
    if (!thermal_meas_.GetEntriesInfo(channel_name, entry_info_set)){
        std::cout << "Problem retrieving entries info" << std::endl;
        return 0;
    }
    std::cout << "Entries Info Retrieved Successfully!" << std::endl;
    
    for (auto it = entry_info_set.begin(); it!=entry_info_set.end();it++)
    {   
        size_t entry_size;
        if (!thermal_meas_.GetEntryDataSize(it->ID, entry_size))
            return 0;
        uint8_t * data = new uint8_t[entry_size];
        if (!thermal_meas_.GetEntryData(it->ID, data)){
            std::cout << "Problem getting entry data: " << it->ID << std::endl;
        }
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Retrieved Data: "<< std::endl;
        std::cout << "ID: " << it->ID << std::endl;
    
        
        // Split data into correct format based on Image struct in msgs.hpp

        uint64_t size_of_frameid;
        uint64_t encoding_bytes;

        std::string ID = "some";

        uint32_t timestamp_sec;
        uint32_t timestamp_nanosec;

        uint16_t height;
        uint16_t width;

        long data_size;

        int ptr = 0;
        
        std::memcpy(&timestamp_sec, &data[ptr], sizeof(timestamp_sec));
        std::cout << "Timestamp secs: " <<  timestamp_sec << std::endl;

        ptr += sizeof(timestamp_sec);
        std::memcpy(&timestamp_nanosec, &data[ptr], sizeof(timestamp_nanosec));
        std::cout << "Timestamp nanosecs: " <<  timestamp_nanosec << std::endl;

        ptr += sizeof(timestamp_nanosec);
        std::memcpy(&size_of_frameid, &data[ptr], sizeof(size_of_frameid));
        //std::cout << "Size of frame id: " <<  size_of_frameid << std::endl;

        ptr += sizeof(size_of_frameid);
        uint8_t *frame_id_cstring = (uint8_t*)malloc((size_t)size_of_frameid);
        std::memcpy(&frame_id_cstring, &data[ptr], sizeof(frame_id_cstring));
        std::string s = (const char*)&frame_id_cstring;
        ID = s;
        std::cout << "Frame ID: " <<  ID << std::endl;
    
        ptr += size_of_frameid;
        std::memcpy(&height, &data[ptr], sizeof(height));
        std::cout << "Height: " <<  height << std::endl;

        ptr += sizeof(height);
        std::memcpy(&width, &data[ptr], sizeof(width));
        std::cout << "Width: " <<  width << std::endl;
        
        ptr += sizeof(width);
        std::memcpy(&data_size, &data[ptr], sizeof(data_size));
        std::cout << "Data Size: " <<  data_size << std::endl;

        ptr += sizeof(data_size);
        uint16_t *data_tmp = (uint16_t*)malloc((size_t)(data_size));
        std::memcpy(&data_tmp[0], &data[ptr], 2*(size_t)(data_size));    
        std::cout << "SIZE: " << (size_t)(data_size) << std::endl;

        //THERMAL IMAGE PROCESSING 
        //1. Convert data to cv matrix to store as 16 bit .png image
        cv::Mat raw_16 = cv::Mat(height, width, CV_16U, data_tmp);
        std::cout << "DATA STORED IN MAT " << std::endl;
        std::string timestr = std::to_string(timestamp_sec) + "_" + std::to_string(timestamp_nanosec);
        std::string img_name_raw = "/home/aru/repos/ximea_stuff/ecal_ximea/img/out/" + ID + "_" + timestr + ".png";
        cv::imwrite(img_name_raw, raw_16);
        std::cout << "Saved Image" << std::endl;

        //2. Normalise 16_bit data to store as grayscale image .jpeg
        cv::Mat raw_8 = cv::Mat(height, width, CV_8U, data_tmp);
        cv::normalize(raw_16, raw_16, 0, 65535, cv::NORM_MINMAX);
        raw_16.convertTo(raw_8, CV_8U, 1./256.);

        std::string img_name_rgb = "/home/aru/repos/ximea_stuff/ecal_ximea/img/rgb/" + ID + "_" + timestr + ".jpeg";
        cv::imwrite(img_name_rgb, raw_8);
        std::cout << "Saved False Colour Image" << std::endl;
        
    }

    std::cout<<std::endl;
    std::cout << "Done Processing Measurement" << std::endl;
    return 0;
} 
*/

/* // XIMEA
#include <ecalhdf5/eh5_meas.h>

#include <m3api/xiApi.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core.hpp>

#define throw_line(message) {char err[500]="";sprintf(err,"Fatal error: %s in:" __FILE__ "line:%d\n",message,__LINE__);throw err;}
#define THROW_IF_ZERO(val) {if (!val) throw_line("Value "#val"is zero");}
#define XI_CARE(func) {XI_RETURN ret=func;char msg[200]="";if (XI_OK!=ret) {sprintf(msg,"Error on " #func " - expected XI_OK. Result:%d\n",ret);throw_line(msg);};}

// Declarations
void read_bin_file(std::string src_path, char* dest, int size);

int main(int argc, char** argv)
{
    std::string meas_path = "/home/aru/ecal_meas/2022-10-19_17-16-00.841_measurement";
    std::string cam_context_path = "/home/aru/repos/ximea_stuff/ecal_ximea/img/cam_context.bin";
    std::string channel_name = "rt/image_raw";
    int height = 1088, width = 2048;
    
    eCAL::eh5::HDF5Meas xi_meas_(meas_path);

    // Check file status
    if (!xi_meas_.IsOk()){
        std::cout << "Problem with measurement file." << std::endl;
        return 0;
    }

    // --------------------
    // Metadata
    // --------------------

    // List Channels Available
    auto channels = xi_meas_.GetChannelNames();
    std::cout << "Channels Available: ";
    for (auto it = channels.begin(); it!=channels.end();it++)
    {
        std::cout<<std::endl<<*it;
    }
    std::cout<<std::endl;

    // Entry Info
    eCAL::eh5::EntryInfoSet entry_info_set;
    if (!xi_meas_.GetEntriesInfo(channel_name, entry_info_set)){
        std::cout << "Problem retrieving entries info" << std::endl;
        return 0;
    }
    std::cout << "Entries Info Retrieved Successfully!" << std::endl;

    // Ximea Setup
    HANDLE camh = NULL;
    char* cam_context=NULL;
    #define SIZE_OF_CONTEXT_BUFFER (10*1024*1024) // 10MiB
    cam_context = (char*)malloc(SIZE_OF_CONTEXT_BUFFER);

    read_bin_file(cam_context_path, cam_context, SIZE_OF_CONTEXT_BUFFER);
    
    std::string s(cam_context);
    std::cout << "GOT CAM CONTEXT: " << std::endl << cam_context << std::endl;

    // Ximea offline processing
    std::cout << "Opening Ximea Offline Processing" << std::endl;
    xiProcessingHandle_t proc;
    XI_CARE(xiProcOpen(&proc));
    std::cout << "Ximea Offline Procesing Opened" << std::endl;
    int cam_context_len = (DWORD)strlen(cam_context);

    XI_CARE(xiProcSetParam(proc, XI_PRM_API_CONTEXT_LIST, cam_context, cam_context_len, xiTypeString));
    XI_IMG_FORMAT output_format = XI_RGB32;
    XI_CARE(xiProcSetParam(proc, XI_PRM_IMAGE_DATA_FORMAT, &output_format, sizeof(output_format), xiTypeInteger));
    
    for (auto it = entry_info_set.begin(); it!=entry_info_set.end();it++)
    {   
        size_t entry_size;
        if (!xi_meas_.GetEntryDataSize(it->ID, entry_size))
            return 0;
        uint8_t * data = new uint8_t[entry_size];
        if (!xi_meas_.GetEntryData(it->ID, data)){
            std::cout << "Problem getting entry data: " << it->ID << std::endl;
        }
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Retrieved Data: "<< std::endl;
        std::cout << "ID: " << it->ID << std::endl;
        //std::cout << "Sender Timestamp: " << it->SndTimestamp << std::endl;
        //std::cout << "Reciever Timestamp: " << it->RcvTimestamp << std::endl;
        //std::cout << "size = " << entry_size << " | " << 
        //        "[" << int(data[0]) << ",...," << int(data[entry_size-1]) << "]" << std::endl;
        
        int actual_size = height * width;
        int size_diff = entry_size-actual_size;
        
        // Split data into correct format based on Image struct in msgs.hpp

        uint64_t size_of_frameid;
        uint64_t encoding_bytes;

        std::string ID = "some";

        uint32_t timestamp_sec;
        uint32_t timestamp_nanosec;

        uint32_t height;
        uint32_t width;

        std::string encoding;
        uint8_t is_bigendian;
        uint32_t step;

        long data_size;
        int ptr = 0;
        std::memcpy(&timestamp_sec, &data[ptr], sizeof(timestamp_sec));
        std::cout << "Timestamp secs: " <<  timestamp_sec << std::endl;

        ptr += sizeof(timestamp_sec);
        std::memcpy(&timestamp_nanosec, &data[ptr], sizeof(timestamp_nanosec));
        std::cout << "Timestamp nanosecs: " <<  timestamp_nanosec << std::endl;

        ptr += sizeof(timestamp_nanosec);
        std::memcpy(&size_of_frameid, &data[ptr], sizeof(size_of_frameid));
        //std::cout << "Size of frame id: " <<  size_of_frameid << std::endl;

        ptr += sizeof(size_of_frameid);
        //std::cout << "PTR: " <<  ptr << std::endl;
        uint8_t *frame_id_cstring = (uint8_t*)malloc((size_t)size_of_frameid);
        std::memcpy(&frame_id_cstring, &data[ptr], sizeof(frame_id_cstring));
        std::string s((const char*)&frame_id_cstring, size_of_frameid);
        ID = s;
        std::cout << "Frame ID: " <<  ID << std::endl;
            
        ptr += size_of_frameid;
        std::memcpy(&height, &data[ptr], sizeof(height));
        std::cout << "Height: " <<  height << std::endl;

        ptr += sizeof(height);
        std::memcpy(&width, &data[ptr], sizeof(width));
        std::cout << "Width: " <<  width << std::endl;
        
        ptr += sizeof(width);
        std::memcpy(&encoding_bytes, &data[ptr], sizeof(encoding_bytes));
        //std::cout << "Size of encoding: " <<  encoding_bytes << std::endl;

        ptr += sizeof(encoding_bytes);        
        uint8_t *encoding_cstring = (uint8_t*)malloc(encoding_bytes);
        std::memcpy(&encoding_cstring, &data[ptr], sizeof(encoding_cstring));
        encoding = std::string((const char*)&encoding_cstring);
        std::cout << "Encoding: " <<  encoding << std::endl;

        ptr += encoding_bytes;
        std::memcpy(&is_bigendian, &data[ptr], sizeof(is_bigendian));
        std::cout << "Is Bigendian: " <<  is_bigendian << std::endl;

        ptr += sizeof(is_bigendian);
        std::memcpy(&step, &data[ptr], sizeof(step));
        std::cout << "Step: " <<  step << std::endl;

        ptr += sizeof(step);
        std::memcpy(&data_size, &data[ptr], sizeof(data_size));
        std::cout << "Data Size: " <<  data_size << std::endl;

        ptr += sizeof(data_size);
        std::cout << "Ptr: " <<  ptr << std::endl;

        uint8_t *data_tmp = (uint8_t*)malloc((size_t)data_size);
        std::memcpy(&data_tmp[0], &data[ptr], (size_t)(data_size));     

        // Save raw image
        cv::Mat img_mat = cv::Mat(height, width, CV_8UC1, data_tmp);
        std::string timestr = std::to_string(timestamp_sec) + "_" + std::to_string(timestamp_nanosec);
        std::string img_name = "/home/aru/repos/ximea_stuff/ecal_ximea/img/out/" + ID + "_" + timestr + ".tiff";
        cv::imwrite(img_name, img_mat);
        std::cout << "Saved Image" << std::endl;
        
        
        // Process image
        XI_IMG out_image;
        out_image.size = sizeof(XI_IMG);
        //std::cout << "Out image size: " << out_image.size << std::endl;

        out_image.bp = NULL;
        out_image.bp_size = 0;

        XI_CARE(xiProcPushImage(proc, data_tmp));
        XI_CARE(xiProcPullImage(proc, 0, &out_image));
        std::cout << "Ximea Processed Image" << std::endl;

        //std::cout << "image height: " << out_image.height << std::endl;
        //std::cout << "image width: " << out_image.width << std::endl;

        cv::Mat img_mat_rgb = cv::Mat(out_image.height, out_image.width, CV_8UC4, out_image.bp);
        std::cout << "copied image" << std::endl; 
        std::string img_name_rgb = "/home/aru/repos/ximea_stuff/ecal_ximea/img/rgb/" + ID + "_" + timestr + ".jpeg";
        cv::imwrite(img_name_rgb, img_mat_rgb);
        

        delete [] data_tmp;
        delete [] data;
        
        
    }

    //XI_CARE(xiProcClose(proc));

    std::cout<<std::endl;
    std::cout << "Done Processing Measurement" << std::endl;
    return 0;
}

void read_bin_file(std::string src_path, char* dest, int size){
    std::ifstream file_in(src_path, std::ios::in | std::ios::binary);

    file_in.read(dest, size);

    if (!file_in){
        std::cout << "Error Reading File" << std::endl;
    }

} */
