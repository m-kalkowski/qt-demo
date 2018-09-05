#ifndef MCP3424_H
#define MCP3424_H

#include<stdint.h>

// For ADC device
#define RAW_MAX_14 8192
#define V_MAX 3.30
#define MCP3424_OK 0
#define MCP3424_ERR -1
#define MCP3424_WARN -2
#define MCP3424_ERR_LEN 256
#define DIRECTION_MAX 35



class mcp3424
{
    public:
    enum mcp3424_channel {
        MCP3424_CHANNEL_1,
        MCP3424_CHANNEL_2,
        MCP3424_CHANNEL_3,
        MCP3424_CHANNEL_4
    };

    enum mcp3424_conversion_mode {
        MCP3424_CONVERSION_MODE_ONE_SHOT,
        MCP3424_CONVERSION_MODE_CONTINUOUS
    };

    enum mcp3424_pga {
        MCP3424_PGA_1X,
        MCP3424_PGA_2X,
        MCP3424_PGA_4X,
        MCP3424_PGA_8X
    };

    enum mcp3424_resolution {
        MCP3424_RESOLUTION_12,
        MCP3424_RESOLUTION_14,
        MCP3424_RESOLUTION_16,
        MCP3424_RESOLUTION_18
    };

    int fd;
    uint8_t addr;
    uint8_t config;
    int err;
    char errstr[MCP3424_ERR_LEN];
     int res[4];
	float voltage[4];


    mcp3424();
    void init(uint8_t , enum mcp3424_resolution);
    void set_conversion_mode(enum mcp3424_conversion_mode);

    void set_channel(enum mcp3424_channel);
    void set_pga(enum mcp3424_pga ) ;
    void set_resolution(enum mcp3424_resolution) ;
    int get_raw( enum mcp3424_channel);
    void set_errstr( const char*,... );
    void print();




    virtual ~mcp3424();

    protected:

    private:
};

#endif // MCP3424_H
