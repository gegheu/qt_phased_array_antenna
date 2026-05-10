#ifndef NAVIGAPROTOCOL_H
#define NAVIGAPROTOCOL_H

#include <array>
#include <cstddef>
#include <cstdint>

#include <QByteArray>

#include "iProtocol.h"

class NavigaProtocol : public IProtocol
{
    Q_OBJECT

public:
    static constexpr quint16 NAVIGA_UPDATA_INFO_HEAD = 0xAA55;
    static constexpr quint8 NAVIGA_UPDATA_INFO_LENGTH = 0x4C;
    static constexpr quint8 NAVIGA_UPDATA_INFO_BUFFER_LENGTH = 0x50;
    static constexpr quint32 SHORT_TYPE_NUM_BIT = 16;
    static constexpr quint32 SEC_HAS_NUM_MS = 1000;
    static constexpr quint32 MIN_HAS_NUM_SEC = 60;
    static constexpr quint32 HOUR_HAS_NUM_MIN = 60;
    static constexpr quint32 DAY_HAS_NUM_HOUR = 24;
    static constexpr qint32 BEIJING_TIME_ZONE = 8;
    static constexpr int VAILD_NAVIGA_UPDATA_INFO_NUM = 2;

    explicit NavigaProtocol(QObject* parent = nullptr);

    QByteArray buildCommand(const QByteArray& data, const QVariantMap& para = {}) override;
    void parseResponse(const QByteArray& data) override;

    // External usage interface.
    void parseFrame(QByteArray frame);

private:
    enum DATA_TYPE {
        UNSIGNED_CHAR_TYPE,
        UNSIGNED_SHORT_INT_TYPE,
        SHORT_INT_TYPE,
        UNSIGNED_INT_TYPE,
        INT_TYPE,
        FLOAT_TYPE,
    };

    enum NAVIGA_EXTER_DATA_INFO {
        WORK_MODE_SELF_TEST,
        WORK_STATION,
        DEVICE_TEMP,
        HDOP,
        INFO_MAX = 20,
    };

    enum RECV_INFO_TYPE {
        SET_RESP,
        QUERY_INFO,
        UPDATA_INFO,
        RECV_INFO_TYPE_MAX,
    };

    struct NavigaUtcTime {
        quint32 year;
        quint32 month;
        quint32 day;
        quint32 hour;
        quint32 minute;
        quint32 second;
        quint32 msecond;
    };

    struct WorkModeSelfTest {
        unsigned int work_mode : 8;
        unsigned int self_test : 24;
    };

    struct WorkStation {
        unsigned int pos_type : 8;
        unsigned int positionok : 8;
        unsigned int headingok : 8;
        unsigned int num_sv : 8;
    };

    struct NavigaUpdataInfo {
        quint16 head;
        quint8 length;
        quint32 time_cnt;
        quint32 resv;
        float x_angular_velocity;
        float y_angular_velocity;
        float z_angular_velocity;
        float x_acceleration;
        float y_acceleration;
        float z_acceleration;
        float north_speed;
        float sky_speed;
        float east_speed;
        float heigh;
        qint32 longitude;
        qint32 latitude;
        float roll;
        float yaw;
        float pitch;
        quint32 utc_high;
        quint16 utc_low;
        qint16 heave_displacement;
        quint8 crc;
    };

    struct StructMembersInfo {
        int offset;
        DATA_TYPE dt;
        int members_length;
    };

    struct NavigaCmdUpdataRespMapInfo {
        quint16 head;
        RECV_INFO_TYPE recv_type;
        void (NavigaProtocol::*resp_callback)(const QByteArray& buffer);
    };

private:
    static std::array<StructMembersInfo, 23> createMembersInfo();
    static std::array<NavigaCmdUpdataRespMapInfo, 1> createRespMapInfo();

    static quint16 readBigEndianU16(const QByteArray& buffer, int index);

    // Original C: naviga_updata_extern_info(struct naviga_updata_info *nui)
    void naviga_updata_extern_info(const NavigaUpdataInfo& nui);

    // Original C: get_year_mm_dd(struct naviga_utc_time *nut, unsigned int days)
    void get_year_mm_dd(NavigaUtcTime& nut, quint32 days);

    // Original C: naviga_updata_date_info(struct naviga_updata_info *nui)
    void naviga_updata_date_info(const NavigaUpdataInfo& nui);

    // Original C: naviga_info_updata(unsigned char *buffer, unsigned int len)
    void naviga_info_updata(const QByteArray& buffer);

    // Original C: naviga_updata_info_ack(unsigned char *buffer, unsigned int len)
    void naviga_updata_info_ack(const QByteArray& buffer);

private:
    NavigaUpdataInfo extern_nui_{};
    std::array<NavigaUpdataInfo, VAILD_NAVIGA_UPDATA_INFO_NUM> vaild_nui_{};
    int valid_nui_index_ = 0;

    std::array<StructMembersInfo, 23> naviga_updata_smi_{};
    std::array<NavigaCmdUpdataRespMapInfo, 1> ncurmi_{};
};

#endif // NAVIGAPROTOCOL_H
