/****************************************************************************
** Meta object code from reading C++ file 'basefetcher.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/basefetcher.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'basefetcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11BaseFetcherE_t {};
} // unnamed namespace

template <> constexpr inline auto BaseFetcher::qt_create_metaobjectdata<qt_meta_tag_ZN11BaseFetcherE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "BaseFetcher",
        "ratesFetched",
        "",
        "std::map<std::string,double>",
        "rates",
        "fetchError",
        "message"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'ratesFetched'
        QtMocHelpers::SignalData<void(std::map<std::string,double>)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'fetchError'
        QtMocHelpers::SignalData<void(QString)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<BaseFetcher, qt_meta_tag_ZN11BaseFetcherE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject BaseFetcher::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11BaseFetcherE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11BaseFetcherE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11BaseFetcherE_t>.metaTypes,
    nullptr
} };

void BaseFetcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BaseFetcher *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->ratesFetched((*reinterpret_cast<std::add_pointer_t<std::map<std::string,double>>>(_a[1]))); break;
        case 1: _t->fetchError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (BaseFetcher::*)(std::map<std::string,double> )>(_a, &BaseFetcher::ratesFetched, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (BaseFetcher::*)(QString )>(_a, &BaseFetcher::fetchError, 1))
            return;
    }
}

const QMetaObject *BaseFetcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseFetcher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11BaseFetcherE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BaseFetcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void BaseFetcher::ratesFetched(std::map<std::string,double> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void BaseFetcher::fetchError(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
