#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <QString>
#include <QVariant>
#include <QVector>
#include <QMetaEnum>
#include <QDebug>
#include <QDateTime>
#include <memory>
#include "base/singleton.hpp"
#include "base/meta.hpp"

class QXmlStreamReader;
class QXmlStreamWriter;

class Source
{
public:
    Source()
        : _parent(nullptr)
    {}
    // Interface for returning values
    virtual QString getText() { return QString(); }
    virtual int getValue() { return 0; }
    virtual int getRange() { return 100; }
    virtual bool getBoolean() { return false; }
    virtual int getTime() { return 0; }
    virtual QVariant getVariant(const QString& key)
    {
        Q_UNUSED(key);
        return QVariant();
    }

    // Parent - child relation
    void attach(Source* parent);
    Source* parent() const { return _parent; }

private:
    Source* _parent;
};

class MockSource : public Source
{
public:
    MockSource() {}
    MockSource(QXmlStreamReader& xml);
    QVariant getVariant(const QString& key) final;
    void setValue(const QString& key, const QVariant& value);

private:
    QHash<QString, QVariant> _values;
};

class MockSourceFactory : public SingletonMixin<MockSourceFactory>
{
public:
    MockSourceFactory();
    void loadXml(const QString& path);
    Source* getReference(const QString& name); // FIXME: should return const
private:
    static QString readName(QXmlStreamReader& xml);
    QHash<QString, MockSource> _sources;
};

class Converter : public Source
{
public:
    Converter() {}
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;
    const QString& arg() const { return m_text; }
    void setArg(const QString& arg);
    const QString& type() const { return m_type; }
    virtual ~Converter(){};

protected:
    QVariant askParent() { return askParent(m_text); }
    QVariant askParent(const QString& arg);
    // Called when argument changes
    virtual void parseArgument(){};

private:
    QString m_type;
    QString m_text;
};

class ConverterFactory : public SingletonMixin<ConverterFactory>
{
public:
    ConverterFactory();
    template<typename T>
    void registerObject();
    std::unique_ptr<Converter> createConverterByName(const QString& name);

private:
    // Constructor functions map
    using Constructor = Converter* (*)();
    QHash<QByteArray, Constructor> _constructors;
};

class ServiceName : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        Name,
        Number,
        Provider,
        Reference,
    };
    Q_ENUM(Arg);
    QString getText() final { return askParent().toString(); }

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    Arg m_type;
};

class ServiceTime : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        Duration,
        StartTime,
        EndTime,
    };
    Q_ENUM(Arg);
    QString getText() final { return askParent().toString(); }
    int getTime() final { return askParent().toInt(); }

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    Arg m_type;
};

class ServicePosition : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
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
    QString getText() final;
    int getTime() final;

protected:
    void parseArgument() final;

private:
    bool detailed;
    bool negate;
    bool showHours;
    bool showNoSeconds;
    Arg m_type;
};

class ServiceInfo : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        HasTeletext,
        IsMultichannel,
        IsCrypted,
        IsWidescreen,
        SubservicesAvailable,
        VideoWidth,
        VideoHeight,
        AudioPid,
        VideoPid,
        PcrPid,
        PmtPid,
        TxtPid,
        TsId,
        OnId,
        Sid
    };
    Q_ENUM(Arg);
    bool getBoolean() final;
    QString getText() final;
    int getValue() final;

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    Arg m_type;
};

class FrontendInfo : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        BER,
        SNR,
        AGC,
        SNRdB,
        NUMBER,
        TYPE,
        LOCK
    };
    Q_ENUM(Arg);
    bool getBoolean() final;
    QString getText() final;
    int getValue() final;

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    Arg m_type;
};

class EventName : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        Name,
        Description,
        ExtendedDescription,
        ID
    };
    Q_ENUM(Arg);
    QString getText() final { return askParent().toString(); }

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    Arg m_type;
};

class EventTime : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        StartTime,
        EndTime,
        Remaining,
        Duration,
        Progress,
    };
    Q_ENUM(Arg);
    int getTime() final
    {
        if (m_type != Progress) {
            return askParent().toInt();
        }
        return -1;
    }
    int getValue() final
    {
        if (m_type == Progress) {
            return askParent().toInt();
        }
        return -1;
    }

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    Arg m_type;
};

class TunerInfo : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        TunerUseMask,
    };
    Q_ENUM(Arg);
    QString argStr() const { return QMetaEnum::fromType<Arg>().valueToKey(m_type); }
    bool getBoolean() final { return askParent().toInt() > 0; }
    QString getText() final { return askParent().toString(); }
    int getValue() final { return askParent().toInt(); }

private:
    Arg m_type;
};

class ValueBitTest : public Converter
{
    Q_GADGET
public:
    bool getBoolean()
    {
        int value = parent()->getValue();
        return (value & _testValue) == _testValue;
    }

protected:
    void parseArgument() final { _testValue = arg().toInt(); }

private:
    int _testValue;
};

class ValueRange : public Converter
{
    Q_GADGET
public:
    bool getBoolean() final
    {
        int value = parent()->getValue();
        return lower <= value && value <= upper;
    }

protected:
    void parseArgument() final
    {
        auto list = arg().splitRef(",");
        lower = list[0].toInt();
        upper = list[1].toInt();
    }

private:
    int lower;
    int upper;
};

class ClockToText : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        Default,
        WithSeconds,
        InMinutes,
        Date,
        AsLenght,
        Format,
    };
    Q_ENUM(Arg);
    QString getText() final;

protected:
    void parseArgument() final;

private:
    int m_type;
    QString m_format;
};

class RemainingToText : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        Default,
        WithSeconds,
        NoSeconds,
        InSeconds,
    };
    Q_ENUM(Arg);
    QString getText() final;

protected:
    void parseArgument() final { m_type = strToEnum<Arg>(arg()); }

private:
    int m_type;
};

class ProgressToText : public Converter
{
    Q_GADGET
public:
    enum Arg
    {
        Default,
        InPercent
    };
    Q_ENUM(Arg);
    QString getText() final;

protected:
    void parseArgument() final;

private:
    int m_type;
};

#endif // CONVERTER_HPP
