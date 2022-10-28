#include "m2s2/ecal/deserializer/deserializer_ximea.hpp"


int main(int argc, char** argv){

    std::string meas_path = "/home/aru/ecal_meas/2022-10-19_17-16-00.841_measurement";
    std::string cam_context_path = "/home/aru/repos/ximea_stuff/ecal_ximea/img/cam_context.bin";
    std::string channel_name = "rt/image_raw";

    m2s2::ecal::deserializer::DeserializerXimea xi_deserializer(meas_path, channel_name, cam_context_path);

    xi_deserializer.process_all();

    return 0;
}

namespace m2s2{ namespace ecal{ namespace deserializer{

    DeserializerXimea::DeserializerXimea(std::string meas_path, std::string channel_name, std::string cam_context_path) :
        Deserializer(meas_path, channel_name),
        cam_context_path(cam_context_path)
  
    {
        this->camh = NULL;
        this->cam_context = NULL;
        this->cam_context = (char*)malloc(SIZE_OF_CONTEXT_BUFFER);
        this->read_bin_file(this->cam_context_path, this->cam_context, SIZE_OF_CONTEXT_BUFFER);

        std::string s(cam_context);
        std::cout << "GOT CAM CONTEXT: " << std::endl << cam_context << std::endl;
    
        std::cout << "Opening Ximea Offline Processing" << std::endl;
        XI_CARE(xiProcOpen(&this->proc));
        std::cout << "Ximea Offline Procesing Opened" << std::endl;
        int cam_context_len = (DWORD)strlen(cam_context);

        XI_CARE(xiProcSetParam(this->proc, XI_PRM_API_CONTEXT_LIST, this->cam_context, cam_context_len, xiTypeString));
        XI_IMG_FORMAT output_format = XI_RGB32;
        XI_CARE(xiProcSetParam(this->proc, XI_PRM_IMAGE_DATA_FORMAT, &output_format, sizeof(output_format), xiTypeInteger));
    }
    
    struct BaseMsg DeserializerXimea::deserialize_message(
        eCAL::eh5::HDF5Meas* meas_,
        long long ID)
    {   

        struct Image* msg = &this->msg;
        
        size_t entry_size;
        if (!meas_->GetEntryDataSize(ID, entry_size)){
            std::cout << "Error Getting Entry Data Size" << std::endl;
            return this->msg;
        }
        
        uint8_t *data = new uint8_t[entry_size];
        if (!meas_->GetEntryData(ID, data)){
            std::cout << "Problem getting entry data: " << ID << std::endl;
            return this->msg;
        }

        // COPY this->msg context to an Image struct
        //Timestamp
        int ptr = 0;
        std::memcpy(&(this->msg.timestamp_sec), &data[ptr], sizeof((this->msg.timestamp_sec)));
        std::cout << "Timestamp secs: " <<  (this->msg.timestamp_sec) << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.timestamp_sec);
        std::memcpy(&this->msg.timestamp_nanosec, &data[ptr], sizeof(this->msg.timestamp_nanosec));
        std::cout << "Timestamp nanosecs: " <<  this->msg.timestamp_nanosec << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.timestamp_nanosec);

        // Frame ID
        uint64_t size_of_frameid;
        std::memcpy(&size_of_frameid, &data[ptr], sizeof(size_of_frameid));
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(size_of_frameid);
        uint8_t *frame_id_cstring = (uint8_t*)malloc((size_t)size_of_frameid);
        std::memcpy(&frame_id_cstring, &data[ptr], sizeof(frame_id_cstring));
        std::string s((const char*)&frame_id_cstring, size_of_frameid);
        this->msg.ID = s;
        std::cout << "Frame ID: " <<  this->msg.ID << std::endl;

        ptr += size_of_frameid;
        std::memcpy(&(this->msg.height), &data[ptr], sizeof((this->msg.height)));
        std::cout << "Height: " <<  (this->msg.height) << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.height);
        std::memcpy(&(this->msg.width), &data[ptr], sizeof((this->msg.width)));
        std::cout << "Width: " <<  (this->msg.width) << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.width);

        // Encoding
        uint64_t size_of_encoding;
        std::memcpy(&size_of_encoding, &data[ptr], sizeof(size_of_encoding));
        //std::cout << "Size of Encoding String: " <<  size_of_encoding << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;
        
        ptr += sizeof(size_of_encoding);
        uint8_t *encoding_cstring = (uint8_t*)malloc(size_of_encoding);
        std::memcpy(&encoding_cstring, &data[ptr], sizeof(encoding_cstring));
        this->msg.encoding = std::string((const char*)&encoding_cstring);
        std::cout << "Encoding: " <<  this->msg.encoding << std::endl;

        ptr += size_of_encoding;
        std::memcpy(&(this->msg.is_bigendian), &data[ptr], sizeof((this->msg.is_bigendian)));
        std::cout << "Is Bigendian?: " <<  (this->msg.is_bigendian) << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.is_bigendian);
        std::memcpy(&(this->msg.step), &data[ptr], sizeof((this->msg.step)));
        std::cout << "Step: " <<  (this->msg.step) << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.step);
        std::memcpy(&(this->msg.data_size), &data[ptr], sizeof((this->msg.data_size)));
        std::cout << "Data size: " <<  (this->msg.data_size) << std::endl;
        //std::cout << "PTR: " <<  ptr << std::endl;

        ptr += sizeof(this->msg.data_size);
        this->msg.data = (uint8_t*)malloc((size_t)this->msg.data_size);
        std::memcpy(&this->msg.data[0], &data[ptr], (size_t)(this->msg.data_size));    
        //this->msg.data = data_tmp;
        std::cout << "Data COPIED " << std::endl;
        //std::cout << "DATA: " << this->msg.data[0] << std::endl;
        
        std::cout << "DONE DESERIALIZING" << std::endl;

        delete [] data;

        return this->msg;
    }

    void DeserializerXimea::process_message(struct BaseMsg* msg_){
        
        std::cout << std::endl << "Processing Ximea Image" << std::endl;
        struct Image* msg = (struct Image*)msg_;
        std::cout << "DATA: " << std::endl; 
        XI_IMG out_image;

        out_image.size = sizeof(XI_IMG);
        std::cout << "Out image size: " << out_image.size << std::endl; 
        out_image.bp = NULL;
        out_image.bp_size = 0;
       
        XI_CARE(xiProcPushImage(this->proc, this->msg.data));
        std::cout << "Proc: " << this->proc <<std::endl; 

        XI_CARE(xiProcPullImage(this->proc, 0, &out_image));
        std::cout << "Ximea Processed Image" << std::endl;

        std::cout << "image height: " << out_image.height << std::endl;
        std::cout << "image width: " << out_image.width << std::endl;

        cv::Mat img_mat_rgb = cv::Mat(out_image.height, out_image.width, CV_8UC4, out_image.bp);
        std::string img_name_rgb = "/home/aru/repos/ximea_stuff/ecal_ximea/img/rgb/" + this->msg.ID + ".jpeg";        
        cv::imwrite(img_name_rgb, img_mat_rgb);
        std::cout << "Image Saved" << std::endl;
    }

    void DeserializerXimea::read_bin_file(std::string src_path, char* dest, int size){
        std::ifstream file_in(src_path, std::ios::in | std::ios::binary);

        file_in.read(dest, size);

        if (!file_in){
            std::cout << "Error Reading File: " << src_path << std::endl;
        }
    }

}}} // namespace m2s2, ecal, deserializer