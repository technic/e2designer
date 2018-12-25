#include "converter.hpp"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMetaType>
#include <QMetaObject>
#include <QFile>
#include <functional>

// Source

void Source::attach(Source* parent)
{
    _parent = parent;
}


// Converter

void Converter::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "convert");

    m_text.clear();
    m_type = xml.attributes().value("type").toString();
    xml.readNext();
    if (xml.isCharacters()) {
        m_text = xml.text().toString();
    }
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
    parseArgument();
}

void Converter::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("convert");
    xml.writeAttribute("type", m_type);
    if (!m_text.isEmpty())
        xml.writeCharacters(m_text);
    xml.writeEndElement();
}

void Converter::setArg(const QString& arg)
{
    m_text = arg;
    parseArgument();
}

QVariant Converter::askParent(const QString& arg)
{
    if (parent()) {
        return parent()->getVariant(arg);
    }
    return QVariant();
}

//std::unique_ptr<Converter> Converter::createConverterByName(const QString& name)
//{
//    auto id = QMetaType::type(name.toUtf8());
//    if (id == QMetaType::UnknownType) {
//        return nullptr;
//    }
//    return std::unique_ptr<Converter>(static_cast<Converter*>(QMetaType::create(id));
//}


// MockSource

MockSource::MockSource(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "entries");

    while (xml.readNextStartElement()) {
        if (xml.name() != "entry") {
            xml.skipCurrentElement();
            continue;
        }
        auto name = xml.attributes().value("name");
        auto value = xml.attributes().value("value");
        _values[name.toString()] = value.toString();
        xml.skipCurrentElement();
    }
}

QVariant MockSource::getVariant(const QString& key)
{
    auto it = _values.find(key);
    if (it != _values.end()) {
        return *it;
    }
    return QVariant();
}

void MockSource::setValue(const QString& key, const QVariant& value)
{
    _values[key] = value;
}

// MockSourceFactory

MockSourceFactory::MockSourceFactory()
{
    Q_INIT_RESOURCE(data);
    loadXml(":/sources.xml");
}

void MockSourceFactory::loadXml(const QString& path)
{
    _sources.clear();

    QFile file(path);
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok) {
        qInfo() << "Can't open source values file" << path;
        return;
    }

    QXmlStreamReader xml(&file);
    while (xml.readNextStartElement()) {
        if (xml.name() == "sources")
            break;
        xml.skipCurrentElement();
    }

    // we are inside sources
    while (xml.readNextStartElement()) {
        if (xml.name() != "source") {
            xml.skipCurrentElement();
            continue;
        }

        // we are inside source
        QVector<QString> names;
        MockSource source;

        while (xml.readNextStartElement()) {
            if (xml.name() == "name") {
                names.append(readName(xml));
            } else if (xml.name() == "entries") {
                source = MockSource(xml);
            } else {
                xml.skipCurrentElement();
                continue;
            }
        }

        // FIXME: not memory efficient
        for (auto& name: qAsConst(names)) {
            _sources[name] = source;
        }
    }
}

Source* MockSourceFactory::getReference(const QString& name)
{
    auto it = _sources.find(name);
    if (it != _sources.end()) {
        return &it.value();
    } else {
        return nullptr;
    }
}

QString MockSourceFactory::readName(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "name");
    QString name;
    while (xml.readNext() != QXmlStreamReader::EndElement) {
        if (xml.isCharacters()) {
            name = xml.text().toString();
        }
    }
    return name;
}


// ConverterFactory

ConverterFactory::ConverterFactory()
{
    registerObject<ServiceName>();
    registerObject<ServiceTime>();
    registerObject<ServicePosition>();
    registerObject<ServiceInfo>();
    registerObject<FrontendInfo>();
    registerObject<EventName>();
    registerObject<EventTime>();
    registerObject<TunerInfo>();
    registerObject<ValueBitTest>();
    registerObject<ValueRange>();
    registerObject<ClockToText>();
    registerObject<RemainingToText>();
    registerObject<ProgressToText>();
}

std::unique_ptr<Converter> ConverterFactory::createConverterByName(const QString& name)
{
    auto it = _constructors.find(name.toLatin1());
    if (it != _constructors.end()) {
        Constructor function = *it;
        return std::unique_ptr<Converter>((*function)());
    } else {
        return nullptr;
    }
}

template<typename T>
void ConverterFactory::registerObject()
{
    static_assert(std::is_base_of<Converter, T>::value, "Can only register Converter classes");
    const QMetaObject* mo = &T::staticMetaObject;
    _constructors[mo->className()] = []() -> Converter* { return new T(); };
}


void ClockToText::parseArgument()
{
    auto meta = QMetaEnum::fromType<Arg>();
    bool ok;
    m_type = meta.keyToValue(arg().toLatin1().data(), &ok);
    auto prefix = QStringLiteral("Format");
    if (!ok && arg().startsWith(prefix)) {
        m_format = arg().section(':', 1);
        m_type = Format;
    }
    m_format.replace("%a", "ddd").replace("%A", "dddd")
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

QString ClockToText::getText() {
    qint64 t = askParent("time").toInt();
    if (t == -1) {
        t = QDateTime::currentDateTime().toSecsSinceEpoch();
    }
    if (m_type == InMinutes) {
        return QString::number(t / 60);
    } else if (m_type == AsLenght) {
        return QString().sprintf("%d:%02d", t / 60, t % 60);
    }
    auto date = QDateTime::fromSecsSinceEpoch(t);
    switch (m_type) {
    case WithSeconds:
        return date.toString("hh:mm:ss");
    case Default:
        return date.toString("h:mm");
    case Date:
        return date.toString("dddd MMM dd, yyyy");
    case Format: {
        return date.toString(m_format);
    }
    default:
        return "???";
    }
}

void ServicePosition::parseArgument() {
    QVector<Arg> vec;
    for (const auto& s: arg().splitRef(',')) {
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

QString ServicePosition::getText() {
    int t = 0;
    switch (m_type) {
    case Length:
    case Position:
        t = askParent().toInt();
        break;
    case Remaining:
        t = askParent(enumToStr(Length)).toInt();
        t -= askParent(enumToStr(Position)).toInt();
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

int ServicePosition::getTime() {
    return askParent().toInt();
}

bool ServiceInfo::getBoolean() {
    switch (m_type) {
    case HasTeletext:
    case IsMultichannel:
    case IsCrypted:
    case IsWidescreen:
    case SubservicesAvailable:
        return askParent().toBool();
    default:
        return false;
    }
}

QString ServiceInfo::getText() {
    switch (m_type) {
    case VideoWidth:
    case VideoHeight:
        return askParent().toString();
    default:
        return QString();
    }
}

int ServiceInfo::getValue() {
    switch (m_type) {
    case VideoWidth:
    case VideoHeight:
        return askParent().toInt();
    default:
        return -1;
    }
}

bool FrontendInfo::getBoolean() {
    switch (m_type) {
    case LOCK:
        return askParent().toBool();
    default:
        return askParent().toInt() > 0;
    }
}

QString FrontendInfo::getText() {
    switch (m_type) {
    case BER:
        return askParent().toString();
    case AGC:
    case SNR:
        return askParent().toString() + "%";
    case SNRdB:
        return askParent().toString() + "%";
    case TYPE: {
        int tuner = askParent().toInt();
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

int FrontendInfo::getValue() {
    switch (m_type) {
    case AGC:
    case SNR:
    case BER:
    case TYPE:
    case NUMBER:
        return askParent().toInt();
    default:
        return 0;
    }
}

QString RemainingToText::getText() {
    int t = parent()->getTime();
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

QString ProgressToText::getText() {
    int value = parent()->getValue();
    int range = parent()->getRange();
    if (m_type == InPercent) {
        return QString("%1 %").arg(value * 100 / range);
    } else {
        return QString("%1 / %2").arg(value, range);
    }
}

void ProgressToText::parseArgument()
{
    m_type = strToEnum<Arg>(arg());
}
