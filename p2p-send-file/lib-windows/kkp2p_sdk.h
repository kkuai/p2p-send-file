#ifndef _KKP2P_SDK_H_
#define _KKP2P_SDK_H_

#include <stdio.h>
#include <stdint.h>

#ifdef KKP2P_WINDLL
#ifdef KKP2P_EXPORTS
#define KKP2P_API __declspec(dllexport)
#else
#define KKP2P_API __declspec(dllimport)
#endif
#else
#define KKP2P_API
#endif


// error code
#define KKP2P_SUCCESS                             0
#define KKP2P_ERROR                              -1
#define KKP2P_ERROR_DOMAIN_PARAM_ERROR         -101
#define KKP2P_ERROR_ANALYSIS_DOMAIN_TIMEOUT    -102
#define KKP2P_ERROR_ANALYSIS_DOMAIN_FAILURE    -103
#define KKP2P_ERROR_DOMAIN_NO_IP               -104
#define KKP2P_ERROR_SYSTEM_INTERNAL        -105
#define KKP2P_ERROR_PARAMETER              -106
#define KKP2P_ERROR_CONN_DISCONNECT        -107
#define KKP2P_ERROR_OTHER_DISCONNECT       -108
#define KKP2P_ERROR_CONNECT_TIMEOUT        -109
#define KKP2P_ERROR_INVALID_PEER_ID        -110
#define KKP2P_ERROR_INVALID_PEER_KEY       -111
#define KKP2P_ERROR_LANMODE_MUSTUSE_P2P    -112
#define KKP2P_ERROR_BIND_IP                -113
#define KKP2P_ERROR_SOCKET_REUSE_ADDR      -114
#define KKP2P_ERROR_SOCKET_BIND            -115
#define KKP2P_ERROR_INVALID_DOMAIN         -116
#define KKP2P_ERROR_DUPLICATE_ID           -117
#define KKP2P_ERROR_OUT_OF_MEMORY          -118
#define KKP2P_ERROR_SOCKET_FD_OVERFLOW     -119
#define KKP2P_ERROR_START_THREAD_POOL      -120
#define KKP2P_ERROR_SMALL_BUFF             -121
#define KKP2P_ERROR_TIMEOUT                -122

#define KKP2P_TCP_CHANNEL 0
#define KKP2P_UDP_CHANNEL 1
#define KKP2P_WEBRTC_CHANNEL 2

typedef struct kkp2p_engine_s kkp2p_engine_t;
typedef struct kkp2p_channel_s kkp2p_channel_t;
typedef void (*kkp2p_connect_cb)(int result, const kkp2p_channel_t* channel, void* param);
typedef int (*kkp2p_write_log_cb)(char* buff, int len, void* userPara);


#pragma pack(1)
typedef struct kkp2p_engine_conf_s
{
    // NULL means only LAN search mode
    // login domain, such as login.p2p.com
    char* login_domain;

    uint16_t login_port;

    // 0 means not use LAN serach mode
    // used to connect or listen
    uint16_t lan_search_port;

    // 0:enable upnp
    // 1:disable upnp,
    char disable_upnp;

    // 0:disable engine thread pool(thread count is 1)
    // 1:enable engine thread pool(thread count is 4)
    char enable_engine_thread_pool;
} kkp2p_engine_conf_t;

typedef struct kkp2p_connect_ctx_s
{
    char peer_id[33];    // peer_id with a maximum length of 32 and ended with '\0'
    int channel_type;    // 0:tcp channel, 1:udp channel
    int connect_mode;    // 0:auto; 1:only p2p; 2:only relay
    int encrypt_data;    // encrypt data or not. 0:not encrypt data; 1:encrypt
    int timeout;         // milliseconds, timeout for channel creation
    int connect_desc;    // user defined parameters, transmitted to the dest peer
    kkp2p_connect_cb func;    // connect async or not. NULL means synchronous, otherwise asynchronous
    void* func_param;         // the param for func when func is not NULL
} kkp2p_connect_ctx_t;

typedef struct kkp2p_channel_s
{
    char peer_id[33];         // peer_id with a maximum length of 32 and ended with '\0'
    int channel_type;         // 0:tcp channel, 1:udp channel, 2:webrtc channel
    int  transmit_mode;       // 1:p2p; 2:relay
    int  encrypt_data;        // encrypt data or not. 0:not encrypt data; 1:encrypt
    uint32_t channel_id;      // the channel id
    int is_ipv6_p2p;          // 0:ipv4 p2p; 1:ipv6 p2p
    int connect_desc;         // user defined parameters
    int fd;                   // the channel's fd for app read and write data
} kkp2p_channel_t;
#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

// get sdk version
// Parameter:
//   version: version buffer
// Return:
//   sdk version
KKP2P_API void kkp2p_sdk_version(char* versionBuff, int buffLen);

// use user's callback func to write log
// level 0:no log, 1:error log, 2:normal log, 3:info log, 4:debug log
KKP2P_API void kkp2p_log_init(kkp2p_write_log_cb func, void* funcParam, int level);
KKP2P_API void kkp2p_log_destroy();
KKP2P_API void kkp2p_log_switch_level(int level);

// Parameter:
//   timeout: millisecond when login_domain is domain needed to be analysised
// Return:
//   valid engine pointer when success and NULL when error
KKP2P_API kkp2p_engine_t*  kkp2p_engine_init(const kkp2p_engine_conf_t* conf, int timeout);

KKP2P_API void kkp2p_engine_destroy(kkp2p_engine_t* engine);

KKP2P_API int kkp2p_error_no();

// Parameter:
//  szHosts:  char szHosts[ipCount][ipLen], get the domain's ip
// Return:
//  actually returned ip count
KKP2P_API int kkp2p_get_domainip(kkp2p_engine_t* engine, char* szHosts, int ipLen, int ipCount);

// return value: 0 is success, <0 is error
KKP2P_API int kkp2p_join_net(kkp2p_engine_t* engine, char* peerId, char* secret);

// return value: 0 is success, <0 is error
KKP2P_API int kkp2p_join_lan(kkp2p_engine_t* engine, char* peerId);

KKP2P_API int kkp2p_listen_fd(kkp2p_engine_t* engine);

// return value: >0 is success, 0 is timeout, <0 is error
KKP2P_API int kkp2p_accept(kkp2p_engine_t* engine, int timeout, kkp2p_channel_t* channel);

// when ctx->func is not null, it's asynchronous connect and channel can be NULL
// when ctx->func is null, it's synchronous connect and channel can't be NULL
// ctx->channel_type 0:create tcp channel,1:create udp channel
// return value: <0 is error, >=0 is success
KKP2P_API int kkp2p_connect(kkp2p_engine_t* engine, kkp2p_connect_ctx_t* ctx, kkp2p_channel_t* channel);
KKP2P_API int kkp2p_lan_search(kkp2p_engine_t* engine, kkp2p_connect_ctx_t* ctx, kkp2p_channel_t* channel);

// start a proxy to communicate with peer
// The data sent to the IP and port will be forwarded to ctx.peer_id
// ctx->timeout must > 0
// ctx->func and ctx.func_param must be NULL
// return value:<0 is error, >=0 is success, proxyId is an output parameter
KKP2P_API int kkp2p_start_proxy(kkp2p_engine_t* engine, char* ip, uint16_t port, kkp2p_connect_ctx_t* ctx, uint32_t* proxyId);
KKP2P_API void kkp2p_stop_proxy(kkp2p_engine_t* engine, uint32_t proxyId);

// return value: <0 is error, 0 is timeout, >0 is success read byte count
KKP2P_API int kkp2p_read(int fd, char* buff, int len, int timeout);

// return value: <0 is error, 0 is timeout, >0 is success write byte count
KKP2P_API int kkp2p_write(int fd, char* buff,int len, int timeout);

KKP2P_API void kkp2p_close_channel(kkp2p_engine_t* engine, uint32_t channelId);

KKP2P_API void kkp2p_close_fd(int fd);

// compute the SHA-1 digest
// SHA-1（Secure Hash Algorithm 1）
// input parameter: 
//    k secret
//   lk lenth of secret
//    d  data buffer
//   ld  length of data
//  out  output digest buffer, length must bigger than 20 bytes
//   lo   input and output parameter, the input buffer len of out and output digiest len
KKP2P_API void kkp2p_hmac_sha1(const uint8_t* k, size_t lk, const uint8_t* d, size_t ld,   
        uint8_t* out, size_t *lo);

//---------------------------------for webrtc-----------------------------------------
// return >= 0,success, <0 error
// Create local SDP, Implemented by the application
// output: localSdp
typedef int (*kkp2p_webrtc_create_sdp)(uint32_t channelId, const char* peerId, const void* remoteSdp
                                       ,const char* webApi, const char* webParam
                                       ,const void* hostCandiate, const void* reflexCandiate
                                       ,const void* session, void* localSdp);

// set create local sdp information callback function
KKP2P_API void kkp2p_webrtc_set_sdp_cb(kkp2p_engine_t* engine,  kkp2p_webrtc_create_sdp cb);

// set tls certificate sha256 fingerprint
// such as 02:3E:53:90:19:94:A5:71:0A,,,
KKP2P_API void kkp2p_webrtc_set_certfile_finger(kkp2p_engine_t* engine, char* finger, int fingerLen);

// for libkkrtc
typedef int (*kkp2p_webrtc_handle_offer)(uint64_t sessionId, int rtcType, const char* peerId
        ,const char* peerKey, const char* turnIp,uint16_t turnPort
        ,const char* turnRealm, const char* sdpBuff, int sdpLen);

typedef int (*kkp2p_webrtc_handle_candidate)(uint64_t sessionId, int rtcType, const char* peerId
        ,const char* peerKey, const char* candidateBuff
        ,int candidateLen);

KKP2P_API void kkp2p_webrtc_set_handle_offer(kkp2p_engine_t* engine, kkp2p_webrtc_handle_offer cb);
KKP2P_API void kkp2p_webrtc_set_handle_candidate(kkp2p_engine_t* engine, kkp2p_webrtc_handle_candidate cb);

KKP2P_API void kkp2p_webrtc_send_signal_candidate(kkp2p_engine_t* engine, uint64_t sessionId, int rtcType
        ,char* peerId, char* jsonBuff, int buffLen);

KKP2P_API void kkp2p_webrtc_send_signal_sdp(kkp2p_engine_t* engine,  uint64_t sessionId, int webType
        ,char* peerId, char*jsonBuff, int buffLen);

// transmitMode: 1 is p2p, 2 is relay
KKP2P_API void kkp2p_webrtc_report_stat(kkp2p_engine_t* engine, const char* peerId, int transmitMode
            ,uint64_t sendByte, uint64_t recvByte, int channelLife);

// get webrtc share token
// tokenPeriod: the validity period of the token,second
// tokenBuffer: token buffer
// bufferLen: the input or output token buffer len
// timeout: the function timeout,millisecond, suggest over 2000
// return: >= 0,success, <0 error
KKP2P_API int kkp2p_webrtc_get_shared_token(kkp2p_engine_t* engine, const char* peerId, int tokenPeriod
           ,char* tokenBuffer, int* bufferLen, int timeout);

#ifdef __cplusplus
}
#endif

#endif


