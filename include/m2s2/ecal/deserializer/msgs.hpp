namespace m2s2{ namespace ecal{ namespace deserializer{


    struct BaseMsg {
        std::string ID = "123456";

        uint32_t timestamp_sec;
        uint32_t timestamp_nanosec;
    };

    struct Image : BaseMsg {

        uint32_t height;
        uint32_t width;

        std::string encoding = "mono8";
        uint8_t is_bigendian;
        uint32_t step;

        uint64_t data_size;
        uint8_t* data;
    };

    struct ThermalRaw : BaseMsg {
        //change for new recording of boson messages to uint32_t
        uint16_t height;
        uint16_t width;

        uint64_t data_size;
        uint16_t* data;
    };

    struct AudioData : BaseMsg {
        
        uint64_t data_size;
        uint8_t* data;
    };

    struct EnviroData : BaseMsg {

        double temperature;
        double pressure;
        double humidity;
        
    };

    struct RadarFrame : BaseMsg {
        
        uint32_t frame_size;
        uint8_t* frame_data;
    };
    
}}} // namespace m2s2, ecal, deserializer