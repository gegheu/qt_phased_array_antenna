#include "NavigaProtocol.h"

#include <algorithm>
#include <cstring>

#include <QDebug>
#include <QString>

NavigaProtocol::NavigaProtocol(QObject* parent)
    : IProtocol(parent)
    , extern_nui_{}
    , vaild_nui_{}
    , valid_nui_index_(0)
    , naviga_updata_smi_(createMembersInfo())
    , ncurmi_(createRespMapInfo())
{
}

QByteArray NavigaProtocol::buildCommand(const QByteArray& data, const QVariantMap& para)
{
    Q_UNUSED(para);
    return data;
}

void NavigaProtocol::parseResponse(const QByteArray& data)
{
    parseFrame(data);
}

void NavigaProtocol::parseFrame(QByteArray frame)
{
    naviga_updata_info_ack(frame);
}

std::array<NavigaProtocol::StructMembersInfo, 23> NavigaProtocol::createMembersInfo()
{
    return {
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, head)), UNSIGNED_SHORT_INT_TYPE, static_cast<int>(sizeof(quint16)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, length)), UNSIGNED_CHAR_TYPE, static_cast<int>(sizeof(quint8)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, time_cnt)), UNSIGNED_INT_TYPE, static_cast<int>(sizeof(quint32)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, resv)), UNSIGNED_INT_TYPE, static_cast<int>(sizeof(quint32)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, x_angular_velocity)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, y_angular_velocity)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, z_angular_velocity)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, x_acceleration)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, y_acceleration)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, z_acceleration)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, north_speed)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, sky_speed)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, east_speed)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, heigh)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, longitude)), INT_TYPE, static_cast<int>(sizeof(qint32)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, latitude)), INT_TYPE, static_cast<int>(sizeof(qint32)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, roll)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, yaw)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, pitch)), FLOAT_TYPE, static_cast<int>(sizeof(float)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, utc_high)), UNSIGNED_INT_TYPE, static_cast<int>(sizeof(quint32)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, utc_low)), UNSIGNED_SHORT_INT_TYPE, static_cast<int>(sizeof(quint16)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, heave_displacement)), SHORT_INT_TYPE, static_cast<int>(sizeof(qint16)) },
        StructMembersInfo{ static_cast<int>(offsetof(NavigaUpdataInfo, crc)), UNSIGNED_CHAR_TYPE, static_cast<int>(sizeof(quint8)) },
    };
}

std::array<NavigaProtocol::NavigaCmdUpdataRespMapInfo, 1> NavigaProtocol::createRespMapInfo()
{
    return {
        NavigaCmdUpdataRespMapInfo{ NAVIGA_UPDATA_INFO_HEAD, UPDATA_INFO, &NavigaProtocol::naviga_info_updata }
    };
}

quint16 NavigaProtocol::readBigEndianU16(const QByteArray& buffer, int index)
{
    const auto* u8 = reinterpret_cast<const unsigned char*>(buffer.constData());
    return static_cast<quint16>((static_cast<quint16>(u8[index]) << 8) | static_cast<quint16>(u8[index + 1]));
}

void NavigaProtocol::naviga_updata_extern_info(const NavigaUpdataInfo& nui)
{
    NAVIGA_EXTER_DATA_INFO resv_index = INFO_MAX;
    WorkModeSelfTest wmst{};
    WorkStation ws{};
    float float_tmp = 0.0f;

    resv_index = static_cast<NAVIGA_EXTER_DATA_INFO>((nui.time_cnt % 200U) / 10U);

    switch (resv_index) {
    case WORK_MODE_SELF_TEST:
        std::memcpy(&wmst, &(nui.resv), sizeof(WorkModeSelfTest));
        qDebug() << "work mode:" << wmst.work_mode;
        qDebug() << "self test:" << wmst.self_test;
        break;
    case WORK_STATION:
        std::memcpy(&ws, &(nui.resv), sizeof(WorkStation));
        qDebug() << "pos type:" << ws.pos_type
                 << "position ok:" << ws.positionok
                 << "heading ok:" << ws.headingok
                 << "No sv:" << ws.num_sv;
        break;
    case DEVICE_TEMP:
        std::memcpy(&float_tmp, &(nui.resv), sizeof(float));
        qDebug() << "device tmp:" << float_tmp;
        break;
    case HDOP:
        std::memcpy(&float_tmp, &(nui.resv), sizeof(float));
        qDebug() << "HDOP:" << float_tmp;
        break;
    case INFO_MAX:
        qInfo() << "resv index err, please check";
        break;
    default:
        qDebug() << "resv index" << static_cast<int>(resv_index) << ", we no need.";
        break;
    }
}

void NavigaProtocol::get_year_mm_dd(NavigaUtcTime& nut, quint32 days)
{
    quint32 era = 0;
    quint32 doe = 0;
    quint32 yoe = 0;
    quint32 doy = 0;
    quint32 yp = 0;
    quint32 mp = 0;
    quint32 dp = 0;

    days += 719468U;
    era = days / 146097U;
    doe = days - era * 146097U;
    yoe = (doe - doe / 1460U + doe / 36524U - doe / 146096U) / 365U;
    doy = doe - (365U * yoe + yoe / 4U - yoe / 100U);
    yp = yoe + era * 400U;
    mp = (5U * doy + 2U) / 153U;
    dp = doy - (153U * mp + 2U) / 5U + 1U;
    const qint32 month_tmp = static_cast<qint32>(mp) + (mp < 10U ? 3 : -9);
    nut.month = static_cast<quint32>(month_tmp);
    nut.day = dp;
    nut.year = yp + (nut.month <= 2U);
}

void NavigaProtocol::naviga_updata_date_info(const NavigaUpdataInfo& nui)
{
    quint64 utc_ms_cnt = 0x1000000000ULL;
    NavigaUtcTime nut{};
    quint32 day_num = 0;

    qDebug() << QString("nui->utc_high: %1 nui->utc_low: %2").arg(nui.utc_high, 0, 16).arg(nui.utc_low, 0, 16);
    utc_ms_cnt = static_cast<quint64>(nui.utc_high) * (1ULL << SHORT_TYPE_NUM_BIT)
        + static_cast<quint64>(nui.utc_low)
        + static_cast<quint64>(BEIJING_TIME_ZONE) * HOUR_HAS_NUM_MIN * MIN_HAS_NUM_SEC * SEC_HAS_NUM_MS;

    qDebug() << QString("ms_cnt.%1 %2.").arg(utc_ms_cnt).arg(utc_ms_cnt, 0, 16);

    nut.msecond = static_cast<quint32>(utc_ms_cnt % SEC_HAS_NUM_MS);
    nut.second = static_cast<quint32>((utc_ms_cnt / SEC_HAS_NUM_MS) % MIN_HAS_NUM_SEC);
    nut.minute = static_cast<quint32>((utc_ms_cnt / SEC_HAS_NUM_MS / MIN_HAS_NUM_SEC) % HOUR_HAS_NUM_MIN);
    nut.hour = static_cast<quint32>((utc_ms_cnt / SEC_HAS_NUM_MS / MIN_HAS_NUM_SEC / HOUR_HAS_NUM_MIN) % DAY_HAS_NUM_HOUR);
    day_num = static_cast<quint32>(utc_ms_cnt / SEC_HAS_NUM_MS / MIN_HAS_NUM_SEC / HOUR_HAS_NUM_MIN / DAY_HAS_NUM_HOUR);

    qDebug() << "day_num." << day_num;
    get_year_mm_dd(nut, day_num);
    qDebug().nospace() << "UTC time: "
                       << QString("%1").arg(nut.year, 4, 10, QChar('0')) << "."
                       << QString("%1").arg(nut.month, 2, 10, QChar('0')) << "."
                       << QString("%1").arg(nut.day, 2, 10, QChar('0')) << "."
                       << QString("%1").arg(nut.hour, 2, 10, QChar('0')) << "."
                       << QString("%1").arg(nut.minute, 2, 10, QChar('0')) << "."
                       << QString("%1").arg(nut.second, 2, 10, QChar('0')) << "."
                       << QString("%1").arg(nut.msecond, 3, 10, QChar('0'));
}

void NavigaProtocol::naviga_info_updata(const QByteArray& buffer)
{
    NavigaUpdataInfo nui{};
    QByteArray send_buff(NAVIGA_UPDATA_INFO_BUFFER_LENGTH, 0);
    const int send_len = NAVIGA_UPDATA_INFO_BUFFER_LENGTH;

    quint16 u_short_tmp = 0;
    quint32 u_int_tmp = 0;
    qint32 int_tmp = 0;
    quint8 char_tmp = 0;
    int buffer_index = 0;
    float float_tmp = 0.0f;
    qint16 short_tmp = 0;

    for (int i = 0; i < static_cast<int>(naviga_updata_smi_.size()); ++i) {
        const StructMembersInfo& smi = naviga_updata_smi_[i];
        qDebug() << "index." << i << ", data type." << smi.dt << "length." << smi.members_length;

        if (buffer_index + smi.members_length > buffer.size()) {
            qCritical() << "buffer length is invalid, need" << (buffer_index + smi.members_length)
                        << "actual" << buffer.size();
            return;
        }

        const char* src = buffer.constData() + buffer_index;
        char* dst = reinterpret_cast<char*>(&nui) + smi.offset;

        switch (smi.dt) {
        case UNSIGNED_SHORT_INT_TYPE:
            std::memcpy(&u_short_tmp, src, sizeof(quint16));
            std::memcpy(dst, &u_short_tmp, smi.members_length);
            qDebug() << "u_short_tmp." << u_short_tmp;
            break;
        case UNSIGNED_CHAR_TYPE:
            std::memcpy(&char_tmp, src, sizeof(quint8));
            std::memcpy(dst, &char_tmp, smi.members_length);
            qDebug() << "char_tmp." << char_tmp;
            break;
        case UNSIGNED_INT_TYPE:
            std::memcpy(&u_int_tmp, src, sizeof(quint32));
            std::memcpy(dst, &u_int_tmp, smi.members_length);
            qDebug() << "u_int_tmp." << u_int_tmp;
            break;
        case FLOAT_TYPE:
            std::memcpy(&u_int_tmp, src, sizeof(quint32));
            std::memcpy(&float_tmp, &u_int_tmp, sizeof(quint32));
            std::memcpy(dst, &u_int_tmp, smi.members_length);
            qDebug() << "float_tmp." << float_tmp;
            break;
        case INT_TYPE:
            std::memcpy(&int_tmp, src, sizeof(qint32));
            std::memcpy(dst, &int_tmp, smi.members_length);
            qDebug() << "int_tmp." << int_tmp << QString::number(int_tmp, 16);
            break;
        case SHORT_INT_TYPE:
            std::memcpy(&short_tmp, src, sizeof(qint16));
            std::memcpy(dst, &short_tmp, smi.members_length);
            qDebug() << "short_tmp." << short_tmp << QString::number(short_tmp, 16);
            break;
        default:
            qCritical() << "data type err, data type." << smi.dt << ", please check";
            break;
        }

        buffer_index += smi.members_length;
    }

    qDebug() << "longitude" << (nui.longitude / 10000000.0f)
             << ", latitude" << (nui.latitude / 10000000.0f)
             << "heigh" << nui.heigh;
    qDebug() << "roll." << nui.roll << ", yaw." << nui.yaw << ", pitch." << nui.pitch;

    naviga_updata_extern_info(nui);
    naviga_updata_date_info(nui);

    extern_nui_ = nui;
    vaild_nui_[valid_nui_index_] = nui;
    valid_nui_index_ = (valid_nui_index_ + 1) % VAILD_NAVIGA_UPDATA_INFO_NUM;

    std::memcpy(send_buff.data(), buffer.constData(), std::min(send_len, buffer.size()));
    for (int i = 0; i < send_len; ++i) {
        const auto byteValue = static_cast<unsigned char>(send_buff.at(i));
        qDebug() << "index" << i << QString::number(byteValue, 16);
    }

    // Original C kept this call commented.
    // uartx_send_data(reinterpret_cast<unsigned char*>(send_buff.data()), send_len, UART_SIX);
}

void NavigaProtocol::naviga_updata_info_ack(const QByteArray& buffer)
{
    NavigaUpdataInfo nui{};

    if (buffer.isEmpty()) {
        qCritical() << "buffer is null, please check buffer.";
        return;
    }

    if (buffer.size() < 3) {
        qCritical() << "buffer len too small:" << buffer.size();
        return;
    }

    nui.head = readBigEndianU16(buffer, 0);
    nui.length = static_cast<quint8>(buffer.at(2));
    qDebug() << "length." << QString::number(nui.length, 16);

    for (int i = 0; i < static_cast<int>(ncurmi_.size()); ++i) {
        qDebug() << "index." << i;
        if (NAVIGA_UPDATA_INFO_HEAD == ncurmi_[i].head) {
            qDebug() << "info head." << QString::number(ncurmi_[i].head, 16)
                     << ", recv_info type" << QString::number(ncurmi_[i].recv_type, 16);
            auto callback = ncurmi_[i].resp_callback;
            if (callback != nullptr) {
                (this->*callback)(buffer);
            }
            break;
        }
    }
}
