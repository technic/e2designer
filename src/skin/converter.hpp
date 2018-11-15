#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <QString>
#include <QVariant>
#include <QVector>
#include <QMetaEnum>
#include <QDebug>
#include <QDateTime>

class QXmlStreamReader;
class QXmlStreamWriter;

QVariant tableFind(QVector<QString>);

class Converter
{
public:
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;
//    QVariant convert(QString source);

    const QString& arg() { return m_text; }

    virtual QString getText() { return QString(); }
    virtual int getValue() { return 0; }
    virtual bool getBoolean() { return false; }
    virtual int getTime() { return 0; }
protected:
    Converter *parent;
private:
    QString m_type;
    QString m_text;
};


class ServiceName : public Converter {
    Q_GADGET
public:
    enum Arg {
        Name,
        Provider,
        Reference,
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return QMetaEnum::fromType<Arg>().valueToKey(m_type);
    }
    QString getText(const QString &source) {
        return tableFind({source, argStr()}).toString();
    }
private:
    Arg m_type;
};


class ServiceTime : public Converter {
    Q_GADGET
public:
    enum Arg {
        Duration,
        StartTime,
        EndTime,
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return QMetaEnum::fromType<Arg>().valueToKey(m_type);
    }
    QString getText(const QString &source) {
        return tableFind({source, argStr()}).toString();
    }
    int getTime(const QString &source) {
        return tableFind({source, argStr()}).toInt();
    }
private:
    Arg m_type;
};


class ServicePosition : public Converter {
    Q_GADGET
public:
    enum Arg {
        Length,
        Position,
        Remaining,
        Gauge,
        Negate,
        Detailed,
        ShowHours,
        ShowNoSeconds,
    };
    Q_ENUM(Arg);
    static constexpr int pts = 90000;

    ServicePosition()
        : Converter()
    {
        QVector<Arg> vec;
        for (auto s: arg().splitRef(',')) {
            auto meta = QMetaEnum::fromType<Arg>();
            bool ok;
            int k = meta.keyToValue(s.toLatin1().data(), &ok);
            if (ok) {
                vec.push_back(Arg(k));
            } else {
                qWarning() << "Unknown parameter" << s;
            }
        }
        if (!vec.isEmpty()) {
            m_type = vec[0];
        } else {
            m_type = Length;
            qWarning() << "No type specified";
        }
        detailed = vec.indexOf(Detailed) >= 0;
        negate = vec.indexOf(Negate) >= 0;
        showHours = vec.indexOf(ShowHours) >= 0;
        showNoSeconds = vec.indexOf(ShowNoSeconds) >= 0;
    }
    QString argStr() const {
        return argStr(m_type);
    }
    QString argStr(Arg type) const {
        return QMetaEnum::fromType<Arg>().valueToKey(type);
    }
    QString getText(const QString &source) {
        int t = 0;
        switch (m_type) {
        case Length:
        case Position:
            t = tableFind({source, argStr()}).toInt();
            break;
        case Remaining:
            t = tableFind({source, argStr(Length)}).toInt();
            t -= tableFind({source, argStr(Position)}).toInt();
            break;
        default:
            qWarning() << "!!!";
        }
        if (!detailed)
            t /= pts;
        if (negate)
            t = -t;

        QString str;

        if (!detailed) {
            if (showHours) {
                if (showNoSeconds) {
                    return str.sprintf("%d:%02d", t / 3600, (t % 3600) / 60);
                } else {
                    return str.sprintf("%d:%02d:%02d", t / 3600, (t % 3600) / 60, t % 60);
                }
            } else {
                if (showNoSeconds) {
                    return str.sprintf("%d", t / 60);
                } else {
                    return str.sprintf("%d:%02d", t / 60, t % 60);
                }
            }
        } else {
            int ms = t * 1000 / pts;
            t /= pts;
            if (showHours) {
                return str.sprintf("%d:%02d:%02d:%03d", t / 3600, (t % 3600) / 60, t % 60, ms % 1000);
            } else {
                return str.sprintf("%d:%02d:%03d", t / 60, t % 60, ms % 1000);
            }
        }
        return "???";
    }
    int getTime(const QString &source) {
        return tableFind({source, argStr()}).toInt();
    }

private:
    bool detailed;
    bool negate;
    bool showHours;
    bool showNoSeconds;
    Arg m_type;
};


class ServiceInfo : public Converter {
    Q_GADGET
public:
    enum Arg {
        HasTeletext,
        IsMultichannel, IsCrypted, IsWidescreen,
        SubservicesAvailable,
        VideoWidth, VideoHeight,
        AudioPid, VideoPid, PcrPid, PmtPid, TxtPid,
        TsId, OnId,
        Sid
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return QMetaEnum::fromType<Arg>().valueToKey(m_type);
    }
    bool getBoolean(const QString &source) {
        switch (m_type) {
        case HasTeletext:
        case IsMultichannel:
        case IsCrypted:
        case IsWidescreen:
        case SubservicesAvailable:
            return tableFind({source, argStr()}).toBool();
        default:
            return false;
        }
    }
    QString getText(const QString &source) {
        switch (m_type) {
        case VideoWidth:
        case VideoHeight:
            return tableFind({source, argStr()}).toString();
        default:
            return QString();
        }
    }
    int getValue(const QString &source) {
        switch (m_type) {
        case VideoWidth:
        case VideoHeight:
            return tableFind({source, argStr()}).toInt();
        default:
            return -1;
        }
    }
private:
    Arg m_type;
};


class FrontendInfo : public Converter {
    Q_GADGET
public:
    enum Arg {
        BER, SNR, AGC, SNRdB, NUMBER, TYPE, LOCK
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return argStr(m_type);
    }
    QString argStr(Arg type) const {
        return QMetaEnum::fromType<Arg>().valueToKey(type);
    }
    bool getBoolean(const QString &source) {
        switch (m_type) {
        case LOCK:
            return tableFind({source, argStr()}).toBool();
        default:
            return tableFind({source, argStr()}).toInt() > 0;
        }
    }
    QString getText(const QString &source) {
        switch (m_type) {
        case BER:
            return tableFind({source, argStr()}).toString();
        case AGC:
        case SNR:
            return tableFind({source, argStr()}).toString() + "%";
        case SNRdB:
            return tableFind({source, argStr(SNR)}).toString() + "%";
        case TYPE: {
            int tuner = tableFind({source, argStr()}).toInt();
            switch (tuner) {
            case 0: return "DVB-S";
            case 1: return "DVB-C";
            case 2: return "DVB-T";
            default: return "DVB-?";
            }
        }
        default:
            return QString();
        }
    }
    int getValue(const QString &source) {
        switch (m_type) {
        case AGC:
        case SNR:
        case BER:
        case TYPE:
        case NUMBER:
            return tableFind({source, argStr()}).toInt();
        default:
            return 0;
        }
    }
private:
    Arg m_type;
};


class EventName : public Converter {
    Q_GADGET
public:
    enum Arg {
        Name,
        Description,
        ExtendedDescription,
        ID
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return QMetaEnum::fromType<Arg>().valueToKey(m_type);
    }
    QString getText(const QString &source) {
        return tableFind({source, argStr()}).toString();
    }
private:
    Arg m_type;
};


class EventTime : public Converter {
    Q_GADGET
public:
    enum Arg {
        StartTime,
        EndTime,
        Remaining,
        Duration,
        Progress,
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return QMetaEnum::fromType<Arg>().valueToKey(m_type);
    }
    int getTime(const QString &source) {
        if (m_type != Progress) {
            return tableFind({source, argStr()}).toInt();
        }
        return -1;
    }
    int getValue(const QString &source) {
        if (m_type == Progress) {
            return tableFind({source, argStr()}).toInt();
        }
        return -1;
    }
private:
    Arg m_type;
};


class TunerInfo : public Converter {
    Q_GADGET
public:
    enum Arg {
        TunerUseMask,
    };
    Q_ENUM(Arg);
    QString argStr() const {
        return QMetaEnum::fromType<Arg>().valueToKey(m_type);
    }
    bool getBoolean(const QString &source) {
        return tableFind({source, argStr()}).toInt() > 0;
    }
    QString getText(const QString &source) {
        return tableFind({source, argStr()}).toString();
    }
    int getValue(const QString &source) {
        return tableFind({source, argStr()}).toInt();
    }
private:
    Arg m_type;
};


class ValueBitTest : public Converter {
    Q_GADGET
public:
    ValueBitTest() {
        testValue = arg().toInt();
    }
    int value();
    bool getBoolean(const QString &source) {
        return (value() & testValue) == testValue;
    }
private:
    int testValue;
};


class ValueRange : public Converter {
public:
    ValueRange() {
        auto list = arg().splitRef(",");
        lower = list[0].toInt();
        upper = list[1].toInt();
    }
    int value();
    bool getBoolean(const QString &source) {
        return lower <= value() && value() <= upper;
    }
private:
    int lower;
    int upper;
};


class ClockToText : public Converter {
    Q_GADGET
public:
    enum Arg {
        Default,
        WithSeconds,
        InMinutes,
        Date,
        AsLenght,
        Format,
    };
    Q_ENUM(Arg);
    ClockToText()
        : Converter()
    {
        auto meta = QMetaEnum::fromType<Arg>();
        bool ok;
        m_type = meta.keyToValue(arg().toLatin1().data(), &ok);
        auto prefix = QStringLiteral("Format");
        if (!ok && arg().startsWith(prefix)) {
            m_format = arg().section(':', 2);
            m_type = Format;
        }
        m_format
            .replace("%a", "ddd").replace("%A", "dddd")
            .replace("%b", "MMM").replace("%B", "MMMM")
            .replace("%c", "ddd MMM d HH:mm:ss yyyy")
            .replace("%d", "dd").replace("%D", "MM/dd/yy")
            .replace("%e", "dd")
            .replace("%F", "yyyy-MM-dd").replace("%g", "yy").replace("%G", "yyyy")
            .replace("%H", "HH").replace("%h", "MMM")
            .replace("%I", "hh")
            .replace("%j", "DDD")
            .replace("%k", "HH").replace("%l", "hh")
            .replace("%m", "MM").replace("%M", "mm")
            .replace("%n", "\n").replace("%p", "a")
            .replace("%P", "a").replace("%r", "hh:mm:ss a")
            .replace("%R", "HH:mm").replace("%S", "ss")
            .replace("%t", "\t").replace("%T", "HH:mm:ss")
            .replace("%V", "ww")
            .replace("%X", "HH:mm:ss").replace("%x", "MM/dd/yy")
            .replace("%y", "yy").replace("%Y", "yyyy")
            .replace("%Z", "z").replace("%z", "Z")
            .replace("%%", "%");
    }
    QString getText(const QString &source) {
        qint64 t = tableFind({source, "time"}).toInt();
        if (t == -1) {
            t = QDateTime::currentDateTime().toSecsSinceEpoch();
        }
        if (m_type == InMinutes) {
            return QString::number(t / 60);
        } else if (m_type == AsLenght) {
            return QString().sprintf("%d:%02d", t / 60, t % 60);
        }
        auto date = QDateTime::currentDateTime();
        switch (m_type) {
        case WithSeconds:
            return date.toString("hh:mm:ss");
        case Default:
            return date.toString();
        case Date:
            return date.toString("dddd MMM dd, yyyy");
        case Format: {
            return date.toString(m_format);
        }
        default:
            return "???";
        }
    }
private:
    int m_type;
    QString m_format;
};


class RemainingToText : public Converter {
    Q_GADGET
public:
    enum Arg {
        Default,
        WithSeconds,
        NoSeconds,
        InSeconds,
    };
    Q_ENUM(Arg);
    QString getText(const QString &source) {
        int t = 10;
        switch (m_type) {
        case WithSeconds:
            return QString().sprintf("%d:%02d:%02d", t / 3600, (t % 3600) / 60, t % 60);
        case NoSeconds:
            return QString().sprintf("%d:%02d", t / 3600, (t % 3600) / 60);
        case InSeconds:
            return QString::number(t);
        case Default:
        default:
            return QString("%1 min").arg(t / 60);
        }
    }
private:
    int m_type;
};


class ProgressToText : public Converter {
    Q_GADGET
public:
    enum Arg {
        Default,
        InPercent
    };
    Q_ENUM(Arg);
    QString getText(const QString &source) {
        if (m_type == InPercent) {
            return QString("%1 %").arg(m_value * 100 / m_range);
        } else {
            return QString("%1 / %2").arg(m_value, m_range);
        }
    }
private:
    int m_value;
    int m_range;
    int m_type;
};


#endif // CONVERTER_HPP
